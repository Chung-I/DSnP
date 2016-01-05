/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <sstream>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
   CirParseError err;
   ifstream ifs(fileName.c_str());
   int temp;
   int* MILOA = new int[5];
   int lineNo = 1;   //record line number, later store in CirGate data member _lineNo
   string s;
   if(!ifs.is_open()) {
      ifs.close(); 
      cout<<"Cannot open design \""<<fileName<<"\"!!"<<endl;
      cirMgr = 0;
      return false;  
   }
   ifs>>s;
   if(myStrNCmp(s,"aag",3))   
   {   
      err = EXTRA_SPACE;   
      cout<<"error... couldn't find aag... exiting... "<<s<<endl;
      return false;
   }
   for(int i=0;i<5;i++) {  
      if(!(ifs>>MILOA[i])) {  
         err = EXTRA_SPACE;   
         cout<<"error in MILOA... exiting..."<<endl;
         return false;
      }
   }
  _totalList.reserve(MILOA[0]+MILOA[3]);
  _totalList.push_back(new ConstGate());
    for(int i=0;i<MILOA[0]+MILOA[3];i++)  _totalList.push_back(0);
  _maxGateId = MILOA[0];
   for(int i=0;i<MILOA[1];i++) {
      ifs>>temp;
      if(temp/2 != 0) {
         _totalList[temp/2] = new PiGate(temp/2,lineNo++);
         _piList.push_back(_totalList[temp/2]);
      }
   }
   int* poGateWiring = new int[MILOA[3]];
   for(int i=0; i< MILOA[3]; i++) {
      ifs>>temp;
      _totalList[MILOA[0]+i+1] = new PoGate(MILOA[0]+i+1, lineNo++);
      poGateWiring[i] = temp;
      _poList.push_back(_totalList[MILOA[0]+i+1]);
   }  
   unsigned** andGateWiring = new unsigned*[MILOA[4]];
   for(int row=0;row<MILOA[4];row++) andGateWiring[row] = new unsigned[4];

   for(int row=0;row<MILOA[4];row++) {
      for(int col=0;col<3;col++) { 
         ifs>>andGateWiring[row][col];
      } // storing wiring index into array
    }
    int newAigId = 0;
    for(int Row=0;Row<MILOA[4];Row++) {
       newAigId = andGateWiring[Row][0];
       newAigId/=2;
       _totalList[newAigId] = new AigGate(newAigId,lineNo++);
       _aigList.push_back(_totalList[newAigId]);
    }
   vector<CirGate*> fanins; 
   size_t undefId = 0;
   for(int row=0;row<MILOA[4];row++) {
      fanins.push_back(getGate(andGateWiring[row][1]/2));
      fanins.push_back(getGate(andGateWiring[row][2]/2));
      for(int col=0;col<2;col++) {
         int curGateId = andGateWiring[row][0]/2;
         int phase = andGateWiring[row][col+1]%2;
         if(fanins[col]) {
            _aigList[row] -> addFanInList(fanins[col],phase);
            fanins[col] -> addFanOutList(_aigList[row],phase);
         } 
         else {
            undefId = andGateWiring[row][col+1]/2;
            _totalList[undefId] = new UndefGate(undefId,0);
            //if not constructed yet -> initialize undef gate, lineNo is 0
            _totalList[undefId] -> addFanOutList(_aigList[row],phase);
            _totalList[curGateId] -> addFanInList(_totalList[undefId],phase);
            addfltGate(_aigList[row]);
         }         
      }
      fanins.clear();
   }

   for(int row=0;row<_poList.size();row++) {
      CirGate* fanin = getGate(poGateWiring[row]/2);
      int phase = poGateWiring[row]%2;
      if(fanin) {
         _poList[row] -> addFanInList(fanin,phase);
         fanin -> addFanOutList(_poList[row],phase);
      }
      else {
            undefId = poGateWiring[row]/2;
           _totalList[undefId] = new UndefGate(undefId,0);
           //if not constructed yet -> initialize undef gate, lineNo is 0
           _totalList[undefId] -> addFanOutList(_poList[row],phase);
           _poList[row] -> addFanInList(_totalList[undefId],phase);
           addfltGate(_poList[row]);
      }         
   }

   string i = "i";
   string o = "o"; 
   string c = "c";
   int inputCount = 0;
   int outputCount = 0;
   while(ifs>>s) {
      if(!myStrNCmp(s,c,1))   break;
      if(inputCount >= _piList.size() && outputCount >= _poList.size() )   break;
      if( !(myStrNCmp(s,i,1)) ) {
         string gateNum = s.substr(1);
         stringstream ss;
         ss<<gateNum;
         int intGateNum;
         ss>>intGateNum;
         ifs>>s;
         _piList[intGateNum] -> setGateName(s);
         inputCount++;
      }
      else if( !(myStrNCmp(s,o,1)) ) {
         string gateNum = s.substr(1);
         stringstream ss;
         ss<<gateNum;
         int intGateNum;
         ss>>intGateNum;
         ifs>>s;
         _poList[intGateNum] -> setGateName(s);
         outputCount++;
      }
   }
   //for(int i=0;i<(int)_poList.size();i++) { DFS(_poList[i],DFSVisit); }


   for(int i=1;i<(int)_totalList.size();i++) { //start from 1 because 0 is const0 gate
    if(_totalList[i] && _totalList[i] -> getTypeStr()!="PO" && !_totalList[i] -> getFanOutSize()) 
         _unusedGates.push_back(_totalList[i]);
   }

      updateDfsList();

   for(int i=0;i<MILOA[4];i++) delete [] andGateWiring[i];
   delete [] andGateWiring;
   delete [] MILOA;
   delete [] poGateWiring;
   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   if(cirMgr == 0) {   
      cout<<"Error: circuit has not been read!!"<<endl;   
      return;
   }
   int numOfPi = 0;
   int numOfPo = 0;
   int numOfAig = 0;
   for(int i=0; i< _totalList.size(); i++ ) {
      if(_totalList[i]) {
         if(_totalList[i] -> getTypeStr() == "AIG")   numOfAig++;
         else if(_totalList[i] -> getTypeStr() == "PI")   numOfPi++;
         else if(_totalList[i] -> getTypeStr() == "PO")   numOfPo++;
      }
   }
   cout<<"Circuit Statistics"<<endl;
   cout<<"=================="<<endl;
   cout<<"  PI"<<setw(12)<<right<<numOfPi<<endl;
   cout<<"  PO"<<setw(12)<<right<<numOfPo<<endl;
   cout<<"  AIG"<<setw(11)<<right<<numOfAig<<endl;
   cout<<"------------------"<<endl;
   cout<<"  Total"<<setw(9)<<right<<numOfPi+numOfPo+numOfAig<<endl;
}

void
CirMgr::printNetlist() const
{
   cout<<endl;
   dfsNum = 0;
   resetVisit();
     // cout<<"printing netList... "<<endl;
   for(int i=0; i< (int)_poList.size(); i++) {
      //cout<<"entering poList loop... "<<endl;
      this -> DFS(_poList[i], DFSPrint);
   }
      //cout<<"finishing printing netList... "<<endl;
}

void
CirMgr::printPIs() const
{
   cout << "POs of the circuit:";
   for(int i=0 ; i<(int)_piList.size() ;i++)   cout<<" "<<_piList[i] -> getGateId()<<" "; 
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(int i=0 ; i<(int)_poList.size() ;i++)   cout<<" "<<_poList[i] -> getGateId()<<" "; 
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   if(_fltGates.size()) {  
      cout<<"Gates with floating fanin(s): ";
      for(int i = 0;i<_fltGates.size();i++)   cout<<_fltGates[i]->getGateId()<<" ";
   }
   if(_unusedGates.size()) {  
      cout<<endl<<"Gates defined but not used  : ";
      for(int i = 0;i<_unusedGates.size();i++)   cout<<_unusedGates[i]->getGateId()<<" ";
   }
}

void
CirMgr::writeAag(ostream& outfile) const
{
   outfile<<"aag "<<_maxGateId<<" "<<_piList.size()<<" 0 "<<_poList.size()<<" "<<_aigList.size()<<endl;
   for(int i=0; i< _piList.size(); i++)   outfile<<2*(_piList[i] -> getGateId())<<endl;
   for(int i=0; i< _poList.size(); i++)   outfile<<(_poList[i] -> getFanInGate(0)-> getGateId())<<endl;
   for(int i=0; i< _aigList.size(); i++) {
      outfile<<2*(_aigList[i] -> getGateId())<<" ";
      for(int j=0;j<_aigList[i] -> getFanInSize();j++) {
         outfile<<_aigList[i] -> getFanInGate(j)->getGateId();
         if(j+1 < _aigList[i] -> getFanInSize())   outfile<<" ";
      }
      outfile<<endl;
   }
   for(int i=0;i<(int)_piList.size();i++) {
      if(!_piList[i] -> getGateName().empty()) {
         outfile<<"i"<<i<<" "<<_piList[i] -> getGateName()<<endl;
      }
   }
   for(int i=0;i<(int)_poList.size();i++) {
      if(!_poList[i] -> getGateName().empty()) {
         outfile<<"o"<<i<<" "<<_poList[i] -> getGateName()<<endl;
      }
   }
}
void
CirMgr::resetVisit() const
{
   for(int i=0;i<_totalList.size();i++) {
      if(_totalList[i]!=0)   _totalList[i] -> resetGateVisit();
   }
}

/*void
CirMgr::DFAig(CirGate* po) const
{
   if(po -> getFanInSize()) {
      for(int i=0;i<(int)po -> getFanInSize();i++) {
         CirGate* currentGate = myGetGate((po -> getFanInId(i) )/2);
         if(!currentGate) { 
            continue;
         }
         if(! ( myGetGate( (po -> getFanInId(i) )/2) -> visited()) )  { 
            DFAig(myGetGate( (po -> getFanInId(i))/2));
         }
      }
   }
   if(!po -> visited()) {
      po -> visitGate();
      if(po -> getTypeStr() == "AIG")   _aigList.push_back(po);
   }
}

void
CirMgr::DFVisit(CirGate* po) const
{
   if(po -> getFanInSize()) {
      for(int i=0;i<(int)po -> getFanInSize();i++) {
         CirGate* currentGate = myGetGate((po -> getFanInId(i) )/2);
         if(!currentGate) { 
            continue;
         }
         if(! ( myGetGate( (po -> getFanInId(i) )/2) -> visited()) )  { 
            DFVisit(myGetGate( (po -> getFanInId(i))/2));
         }
      }
   }
   if(!po -> visited())   po -> visitGate();
}*/

void
CirMgr::DFS(CirGate* po,DFSFunc dfsFunc) const
{
   if(po -> getFanInSize()) {
      for(int i=0;i<(int)po -> getFanInSize();i++) {
         CirGate* currentGate = po -> getFanInGate(i);
         if(currentGate -> getTypeStr() == "UNDEF") { 
            continue;
         }
         if(!(currentGate -> visited()) )  { 
            DFS(currentGate,dfsFunc);
         }
      }
   }
   if(!po -> visited()) {
      if(dfsFunc == DFSPrint) {
         cout<<"["<<dfsNum++<<"]";
         po -> printGate();
         po -> visitGate();
         cout<<" ";
         for(int i=0;i<(int)po -> getFanInSize();i++) {
            GateV* VisitingGate = po -> getFanInGateV(i);
            cout<<(VisitingGate ->gate() -> getTypeStr()=="UNDEF"?"*":"")<<(VisitingGate->isInv()?"!":"")<<VisitingGate -> gate()-> getGateId()
            <<((i== (po -> getFanInSize()-1))?"":" ");
         }
         po -> printGateName();
         cout<<endl;
      }
      else if(dfsFunc == DFSVisit) { po -> visitGate(); }
      else if(dfsFunc == DFSUpdate) { po -> visitGate(); _dfsList.push_back(po); }
   }
}
void
CirMgr::updateDfsList() {
   for(int i=0;i<_poList.size();i++) {
      DFS(_poList[i],DFSUpdate);
   }
}
