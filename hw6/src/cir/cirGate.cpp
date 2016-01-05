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
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

size_t CirGate::_classVisit = 0;
// TODO: Implement memeber functions for class(es) in cirGate.h

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
   cout<<"=================================================="<<endl;
}

void
CirGate::reportFloatGates() const
{
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
