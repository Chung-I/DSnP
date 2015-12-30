/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
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
   int MILOA[5];
   int lineNo = 1;   //record line number, later store in CirGate data member _lineNo
   if(!ifs.is_open()) {
      ifs.close(); 
      cout<<"Cannot open design \""<<fileName<<"\"!!"<<endl;
      cirMgr = 0;
      return false;  
   }
   string s;
   ifs>>s;
   if(myStrNCmp(s,"aag",3))  //reading "aag" 
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
   _maxGateId = MILOA[0];
   _indexList.push_back(0);   //construct const0 gate
   _totalList.push_back(new ConstGate());   //construct const0 gate
   for(int i=0; i< MILOA[1]; i++) {
      ifs>>temp;
      if(temp/2 != 0) {  //if it is not a const gate
         _indexList.push_back(temp/2);
         _totalList.push_back(new PiGate(temp/2,lineNo++));
         _piList.push_back(_totalList.back());
      }
   }
   for(int i=0; i< MILOA[3]; i++) {
      ifs>>temp;
      _indexList.push_back(++MILOA[0]);
      _totalList.push_back(new PoGate(MILOA[0], lineNo++));
      _totalList.back() -> addFanInIdList(temp);
      _poList.push_back(_totalList.back());
   }   
   for(int i=0; i< MILOA[4]; i++) {
      ifs>>temp;
      _indexList.push_back(temp/2);
      _totalList.push_back(new AigGate(temp/2,lineNo++));
      _aigList.push_back(_totalList.back());
      ifs>>temp;
      _totalList.back() -> addFanInIdList(temp);   //store the raw faninId(inverted or not is also stored) in FanInIdList
      ifs>>temp;
      _totalList.back()-> addFanInIdList(temp);
      
   }
   for(int i=0;i<_poList.size();i++) {
      if(myGetGate((_poList[i] ->getFanInId(0))/2 ) ) {
         myGetGate((_poList[i] ->getFanInId(0))/2 ) 
         -> addFanOutIdList(_poList[i] -> getGateId()*2+(_poList[i] -> getFanInId(0)%2));
      }
   }
   for(int i=0;i<_aigList.size();i++) {
      for(int j=0;j<(int)_aigList[i] -> getFanInSize();j++)  {
         if(myGetGate((_aigList[i] ->getFanInId(j))/2 ) ) {
            myGetGate((_aigList[i] ->getFanInId(j))/2 ) 
            -> addFanOutIdList(_aigList[i] -> getGateId()*2+(_aigList[i] -> getFanInId(j)%2));
         }
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
   /*for(int i=0;i<(int)_poList.size();i++) { DFVisit(_poList[i]); }
   for(int i=1;i<_totalList.size();i++) {
      if(!_totalList[i] -> visited()) { 
      if(_totalList[i] -> )_unusedGates.push_back(_totalList[i] -> getGateId()); }
   }*/
   for(int i=1;i<(int)_totalList.size();i++) { //start from 1 because 0 is const0 gate
      if(_totalList[i] -> getTypeStr() != "PO") {
         if(!_totalList[i] -> getFanOutSize())   _unusedGates.push_back(_totalList[i] -> getGateId());
      }
   }
   
   for(int i=0;i<(int)_totalList.size();i++) {
         for(int j=0;j<(int)_totalList[i]->getFanInSize();j++) {
            if(!myGetGate( (_totalList[i] -> getFanInId(j) )/2)) {
               _fltGates.push_back(_totalList[i] -> getGateId());   
               break;
            }
          }
   }
   resetVisit();
   _aigList.clear();
   for(int i=0 ;i<_poList.size();i++) {
      DFAig(_poList[i]);
   }
   resetVisit();
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
}

void
CirMgr::printNetlist() const
{
   cout << endl;
   for (unsigned i = 0, n = _dfsList.size(); i < n; ++i) {
      cout << "[" << i << "] ";
      _dfsList[i]->printGate();
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
}

void
CirMgr::printFECPairs() const
{
}

void
CirMgr::writeAag(ostream& outfile) const
{
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
}

