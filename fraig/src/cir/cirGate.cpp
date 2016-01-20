/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include <bitset>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

size_t CirGate::_classVisit = 0;
/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
   int width = 0;
   stringstream widthCount;                          //print width adjustment
   widthCount<<_gateID;                              //print width adjustment 
   string countWStr; 
   widthCount>>countWStr;
   width+=countWStr.size();
   widthCount.str("");
   widthCount.clear();
   widthCount<<_lineNo;
   widthCount>>countWStr;
   width+=countWStr.size();
   width += (getTypeStr().size()+_gateName.size());
   if(!_gateName.size())   { width = width-2; }
   cout<<"=================================================="<<endl;
   cout<<"= "<<getTypeStr()<<"("<<_gateID<<")"
      <<(_gateName.size() ? "\"": "")<<_gateName<<(_gateName.size() ? "\"": "")
      <<", "<<"line "<<_lineNo<<setw(37-width)<<"="<<endl;
   cout<<"= FECs:";
   gatePrintFecPair();
   cout<<setw(41-2*(_FECs.size()))<<"="<<endl;
   cout<<"= Value: "<<this->getSimValueString()<<" ="<<endl;
   cout<<"=================================================="<<endl;
}

void
CirGate::reportFanin(int level) const
{
   cirMgr -> resetVisit();
   assert (level >= 0);
   atomicReportFanin(level,level);
}
void
CirGate::atomicReportFanin(int level,int callLevel) const
{
   if(level <0)   return;
   --level;
   cout<<getTypeStr()<<" "<<_gateID;
   if(visited() && level > -1 && _faninList.size() )   cout<<" (*)";
   cout<<endl;
   if(!visited()) {
      ++_visit;
      if(level > -1) {
         for(int i=0; i< (int)_faninList.size(); i++) {
            for(int j =0; j < (callLevel-level); j++) { cout<<"  ";}
            cout<<(_faninList[i] -> isInv()  ? "!": "");
            _faninList[i] -> gate() -> atomicReportFanin(level,callLevel);
         }
      }
   }
}

void
CirGate::reportFanout(int level) const
{
   cirMgr -> resetVisit();
   assert (level >= 0);
   atomicReportFanout(level,level);
}
void
CirGate::atomicReportFanout(int level,int callLevel) const
{
   if(level <0)   return;
   --level;
   cout<<getTypeStr()<<" "<<_gateID;
   if(visited() && level > -1 && _fanoutList.size())   cout<<" (*)";
   cout<<endl;
   if(!visited()) {
      ++_visit;
      if(level > -1) {
         for(int i=0; i< (int)_fanoutList.size(); i++) {
            for(int j =0; j < (callLevel-level); j++) { cout<<"  ";}
               cout<<(_fanoutList[i] -> isInv() ? "!": "");
               _fanoutList[i] -> gate() -> atomicReportFanout(level,callLevel);
         }
      }
   }
}
void
CirGate::replaceByFanin(int in)
{
   cout<<"Simplifying: "<<getFanInGate(in)->getGateId()<<" merging "
      <<this -> getGateId()<<"..."<<endl;
   for(int out=0;out<(int)getFanOutSize();out++) {
      getFanOutGate(out) ->eraseFanInGate(this); //unwiring fanout and thisgate
      bool newphase = ((!getFanOutGateV(out) -> isInv()) != (!getFanInGateV(in)->isInv()));
      getFanOutGate(out) -> addFanInList(getFanInGate(in),newphase);
      getFanInGate(in) -> addFanOutList(getFanOutGate(out),newphase);
   }
   for(int i=0;i<(int)getFanInSize();i++) {
      getFanInGate(i) -> eraseFanOutGate(this);
   }
}
void
CirGate::replaceByConst0(bool in)
{
   cout<<"Simplifying: 0 merging "
      <<this -> getGateId()<<"..."<<endl;
   for(int out=0;out<(int)getFanOutSize();out++) {
      getFanOutGate(out) ->eraseFanInGate(this); 
      bool newphase = ((!in) != (!getFanOutGateV(out)->isInv()));
      getFanOutGate(out) -> addFanInList(cirMgr -> getGate(0),newphase);
      cirMgr -> getGate(0) -> addFanOutList(getFanOutGate(out),newphase); //const add fanin
   }
   for(int i=0;i<(int)getFanInSize();i++) {
      getFanInGate(i) -> eraseFanOutGate(this);
   }
}
bool  
CirGate::gateOptimize() //return true if optimization has been performed 
{
     
     int* faninSignal = new int[FANIN_SIZE];
   bool gateUpdateDfsList = false;
   if(getFanInSize()==2) {
      for(int in=0;in < (int)getFanInSize();in++) {
         faninSignal[in] = 2;   // not a const gate
         if(getFanInGate(in) -> getGateType()==CONST) {
            if(getFanInGateV(in) -> isInv()) { faninSignal[in] = 1; }   // const 1
            else { faninSignal[in] = 0; }  // const 0
         }
      }
      if(faninSignal[0]*faninSignal[1] == 0 ) {
         gateUpdateDfsList = true;
         replaceByConst0(0);
      }
      else if(faninSignal[0]*faninSignal[1] == 1) {
         gateUpdateDfsList = true;
         replaceByFanin(0);
      }
      else if(faninSignal[0]*faninSignal[1] == 2){
         gateUpdateDfsList = true;
         if(faninSignal[0] == 1) replaceByFanin(1);
         else { replaceByFanin(0); }
      }
      else if(getFanInGate(0) == getFanInGate(1)) {
           gateUpdateDfsList = true;
           if(getFanInGateV(0) -> isInv() != getFanInGateV(1) -> isInv()) {
              replaceByConst0(0);
           }
           else {replaceByFanin(0);}
      }
  }
   if(gateUpdateDfsList) {
     return true;
   }
   return false; 
}

void CirGate::cutoffFanOutWiring () {
   for(int out=0;out<(int)getFanOutSize();out++) {
      getFanOutGate(out) ->eraseFanInGate(this); //unwiring fanout and thisgate
   }
}
void CirGate::cutoffFanInWiring () {
   for(int in=0;in<(int)getFanInSize();in++) {
      getFanInGate(in) -> eraseFanOutGate(this);
   }
}

void CirGate::reWireFanIn () {
   for(int in=0;in<(int)getFanInSize();in++) {
      getFanInGate(in) -> eraseFanOutGate(this);
   }
}

void CirGate::reWireFanOut () {
   for(int in=0;in<(int)getFanInSize();in++) {
      getFanInGate(in) -> eraseFanOutGate(this);
   }
}

string
CirGate::getSimValueString() const {
  bitset<32> sim(_simValue);
  string simString = sim.to_string();
  for(int i=4;i<simString.size();i+=5) {
     simString.insert(i,1,'_');
  }
  return simString;
}

void
CirGate::gatePrintFecPair() const {
   for(int i=0;i<(int)_FECs.size();i++) cout<<" "<<(_FECs[i]%2 ?"!" : "")<<_FECs[i]/2;
}

void
CirGate::gateUpdateFecPair(FecGroup& grp) {
   _FECs = grp;
   for(int i=0;i<(int)_FECs.size();i++) {
      if((_FECs[i]/2) == getGateId()) { _FECs.erase(_FECs.begin()+i); break; }
   }
}
