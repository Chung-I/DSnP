/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
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
   int* MILOA = new int[5];
   int lineNo = 2;   //record line number, later store in CirGate data member _lineNo
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
    _totalList.push_back(new ConstGate);
    for(int i=1;i<MILOA[0]+MILOA[3]+5;i++)  _totalList.push_back(0);
  _maxGateId = MILOA[0];
   for(int i=0;i<MILOA[1];i++) {
      ifs>>temp;
      if(temp/2 != 0) {
         _totalList[temp/2] = new PiGate(temp/2,lineNo++);
         _piList.push_back(temp/2);
      }
   }
   int* poGateWiring = new int[MILOA[3]];
   for(int i=0; i< MILOA[3]; i++) {
      ifs>>temp;
      _totalList[MILOA[0]+i+1] = new PoGate(MILOA[0]+i+1, lineNo++);
      poGateWiring[i] = temp;
      _poList.push_back(MILOA[0]+i+1);
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
       _aigList.push_back(newAigId);
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
            getGate(_aigList[row]) -> addFanInList(fanins[col],phase);
            fanins[col] -> addFanOutList(getGate(_aigList[row]),phase);
         } 
         else {
            undefId = andGateWiring[row][col+1]/2;
            _totalList[undefId] = new UndefGate(undefId,0);
            //if not constructed yet -> initialize undef gate, lineNo is 0
            _totalList[undefId] -> addFanOutList(getGate(_aigList[row]),phase);
            _totalList[curGateId] -> addFanInList(_totalList[undefId],phase);
            addfltGate(getGate(_aigList[row]));
         }         
      }
      fanins.clear();
   }

   for(int row=0;row<(int)_poList.size();row++) {
      CirGate* fanin = getGate(poGateWiring[row]/2);
      int phase = poGateWiring[row]%2;
      if(fanin) {
         getGate(_poList[row]) -> addFanInList(fanin,phase);
         fanin -> addFanOutList(getGate(_poList[row]),phase);
      }
      else {
            undefId = poGateWiring[row]/2;
           _totalList[undefId] = new UndefGate(undefId,0);
           //if not constructed yet -> initialize undef gate, lineNo is 0
           _totalList[undefId] -> addFanOutList(getGate(_poList[row]),phase);
           getGate(_poList[row]) -> addFanInList(_totalList[undefId],phase);
           addfltGate(getGate(_poList[row]));
      }         
   }

   string i = "i";
   string o = "o"; 
   string c = "c";
   int inputCount = 0;
   int outputCount = 0;
   while(ifs>>s) {
      if(!myStrNCmp(s,c,1))   break;
      if(inputCount >= (int)_piList.size() && outputCount >= (int)_poList.size()) break;
      if( !(myStrNCmp(s,i,1)) ) {
         string gateNum = s.substr(1);
         stringstream ss;
         ss<<gateNum;
         int intGateNum;
         ss>>intGateNum;
         ifs>>s;
         getGate(_piList[intGateNum]) -> setGateName(s);
         inputCount++;
      }
      else if( !(myStrNCmp(s,o,1)) ) {
         string gateNum = s.substr(1);
         stringstream ss;
         ss<<gateNum;
         int intGateNum;
         ss>>intGateNum;
         ifs>>s;
         getGate(_poList[intGateNum]) -> setGateName(s);
         outputCount++;
      }
   }
   constructDfsList();
   
   for(int i=1;i<(int)_totalList.size();i++) { //start from 1 because 0 is const0 gate
    if(_totalList[i] && _totalList[i] -> getGateType()!=PO &&
          _totalList[i] -> getGateType() != CONST &&
          !_totalList[i] -> getFanOutSize()) 
         _unusedGates.push_back(_totalList[i]->getGateId());
   }

  // std::sort(_piList.begin(),_piList.end());
  // std::sort(_poList.begin(),_poList.end());
  // std::sort(_aigList.begin(),_aigList.end());
   _fecGates.push_back(0);
   for(int i=0;i<(int)_dfsList.size();i++) {
      if(getGate(_dfsList[i])-> getGateType() == AIG)
      _fecGates.push_back(2*_dfsList[i]);
   }
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
   for(int i=0; i< (int)_totalList.size(); i++ ) {
      if(_totalList[i]) {
         if(_totalList[i] -> getTypeStr() == "AIG")   numOfAig++;
         else if(_totalList[i] -> getTypeStr() == "PI")   numOfPi++;
         else if(_totalList[i] -> getTypeStr() == "PO")   numOfPo++;
      }
   }
   cout<<endl<<"Circuit Statistics"<<endl;
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
   cout << endl;
   for (unsigned i = 0, n = (int)_dfsList.size(); i < n; ++i) {
      cout << "[" << i << "] ";
      getGate(_dfsList[i])->printGate();
     cout<<endl; 
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(int i=0 ; i<(int)_piList.size() ;i++)   
   cout<<" "<<_piList[i]; 
   
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(int i=0 ; i<(int)_poList.size() ;i++)   
   cout<<" "<<_poList[i]; 

   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   if(_fltGates.size()) {  
      cout<<"Gates with floating fanin(s): ";
      for(int i = 0;i<(int)_fltGates.size();i++)   
      cout<<_fltGates[i]<<" ";
   }
   if(_unusedGates.size()) {  
      cout<<endl<<"Gates defined but not used  : ";
      for(int i = 0;i<(int)_unusedGates.size();i++)   
      cout<<_unusedGates[i]<<" ";
   }
}

void
CirMgr::printFECPairs() const
{
   for(int grp = 0;grp<(int)_fecGrps.size();++grp) {
      cout<<"["<<grp<<"]";
      for(int gt=0;gt<_fecGrps[grp].size();++gt) {
         cout<<" "<<(_fecGrps[grp][gt]%2 ? "!" : "")
         <<_fecGrps[grp][gt]/2;
      }
      cout<<endl;
   }
   
}

void
CirMgr::writeAag(ostream& outfile) const
{
   outfile<<"aag "<<_maxGateId<<" "<<_piList.size()<<" 0 "<<_poList.size()<<" "<<_aigList.size()<<endl;
   for(int i=0; i< (int)_piList.size(); i++)   outfile<<2*(_piList[i])<<endl;
   for(int i=0; i< (int)_poList.size(); i++)   
   outfile<<(getGate(_poList[i]) -> getFanInGateV(0) -> getWiringId())<<endl;

   for(int i=0; i< (int)_aigList.size(); i++) {
      outfile<<2*_aigList[i]<<" ";
      for(int j=0;j<(int) getGate(_aigList[i]) -> getFanInSize();j++) {
         outfile<<getGate(_aigList[i]) -> getFanInGateV(j)->getWiringId();
         if(j+1 < (int)getGate(_aigList[i]) -> getFanInSize())   outfile<<" ";
      }
      outfile<<endl;
   }
   for(int i=0;i<(int)_piList.size();i++) {
      if(!getGate(_piList[i]) -> getGateName().empty()) {
         outfile<<"i"<<i<<" "<<getGate(_piList[i]) -> getGateName()<<endl;
      }
   }
   for(int i=0;i<(int)_poList.size();i++) {
      if(!getGate(_poList[i]) -> getGateName().empty()) {
         outfile<<"o"<<i<<" "<<getGate(_poList[i]) -> getGateName()<<endl;
      }
   }
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
}

void
CirMgr::resetVisit() const
{
   for(int i=0;i<(int)_totalList.size();i++) {
      if(_totalList[i]!=0)   _totalList[i] -> resetGateVisit();
   }
}
void
CirMgr::DFS(CirGate* thisGate,DFSFunc dfsFunc) 
{
   if(thisGate -> getFanInSize()) {
      if(thisGate->getFanInSize()==2 &&
      thisGate -> getFanInGate(0)->getGateType()!=AIG &&
      thisGate -> getFanInGate(1)->getGateType()==AIG) {
      thisGate->swapFanIn();
      }
      
      for(int i=0;i<(int)thisGate -> getFanInSize();i++) {
         CirGate* fanin = thisGate -> getFanInGate(i);
         if(dfsFunc == DFSConstruct && fanin -> getTypeStr() == "UNDEF") { 
            continue;
         }
         if(!(fanin -> visited()) )  { 
            DFS(fanin,dfsFunc);
         }
      }
   }
   if(!thisGate -> visited()) {
      if(dfsFunc == DFSVisit) {thisGate -> visitGate();}
      else if(dfsFunc == DFSConstruct)  {   
         thisGate -> visitGate();
         _dfsList.push_back(thisGate->getGateId());
      }
   }
}

void
CirMgr::updateDfsList() 
{
   resetVisit();
   for(int i=0;i<(int)_poList.size();i++) {
      DFS(getGate(_poList[i]),DFSVisit);
   }
   for(int i=0;i<(int)_dfsList.size();i++) {
      if(!(getGate(_dfsList[i])->visited())) {
         _dfsList.erase(_dfsList.begin()+i); 
         --i;
      }

   }
   
}
void
CirMgr::constructDfsList() 
{
   _dfsList.clear();
   resetVisit();
   for(int i=0;i<(int)_poList.size();i++) {
      DFS(getGate(_poList[i]),DFSConstruct);
   }
   _updateDfsListFlag = false;
}

void
CirMgr::dfsVisit() 
{
   resetVisit();
   for(int i=0;i<(int)_poList.size();i++) {
      DFS(getGate(_poList[i]),DFSVisit);
   }
}
void
CirMgr::eraseGate(CirGate* toBeErased) 
{
   _totalList[toBeErased -> getGateId()] = 0;
   unsigned int eraseId = toBeErased -> getGateId();
   if(toBeErased -> getGateType() == PI) {
      for(int i=0;i<(int)_piList.size();i++) {
         if(_piList[i] == eraseId) _piList.erase(_piList.begin()+i);
      }
   }
   else if(toBeErased -> getGateType() == PO) {
      for(int i=0;i<(int)_poList.size();i++) {
         if(_poList[i] == eraseId) _poList.erase(_poList.begin()+i);
      }
   }
   else if(toBeErased -> getGateType() == AIG) {
      for(int i=0;i<(int)_aigList.size();i++) {
         if(_aigList[i] == eraseId) _aigList.erase(_aigList.begin()+i);
      }
   }
      for(int i=0;i<(int)_unusedGates.size();i++) {
         if(_unusedGates[i] == eraseId) _unusedGates.erase(_unusedGates.begin()+i);
      }
      for(int i=0;i<(int)_fltGates.size();i++) {
         if(_fltGates[i] == eraseId) _fltGates.erase(_fltGates.begin()+i);
      }
    delete toBeErased;     
}
void
CirMgr::preOrderTraverse()
{
   _updateDfsListFlag = false;
   resetVisit();
   for(int i=0;i<(int)_poList.size();i++) {
      atomicPreOrderTraverse(getGate(_poList[i]));
   }
}
void
CirMgr::atomicPreOrderTraverse(CirGate* thisGate)
{
   
   if(!thisGate -> visited()) {
      thisGate -> visitGate();
      thisGate -> gateOptimize();
   }
   if(thisGate -> getFanInSize()) {  //if thisGate has any fanin
      for(int i=0;i<(int)thisGate -> getFanInSize();i++) {
         if(!(thisGate ->getFanInGate(i) -> visited()) )  { 
            atomicPreOrderTraverse(thisGate -> getFanInGate(i));
         }
      }
   }
}
void 
CirMgr::addfltGate(CirGate* fltGate) 
{
   for(int flt=0;flt<(int)_fltGates.size();flt++) {
      if(fltGate -> getGateId() == _fltGates[flt]) return;
   }
   _fltGates.push_back(fltGate -> getGateId());
}
void 
CirMgr::addunusedGate(CirGate* unusedGate) 
{
   for(int unused=0;unused<(int)_unusedGates.size();unused++) {
      if(unusedGate -> getGateId() == _unusedGates[unused]) return;
   }
   _unusedGates.push_back(unusedGate -> getGateId());
}

bool
CirMgr::checkInv(CirGate* mergeGate,CirGate* toBeMerged){
   bool g1f1 = mergeGate -> getFanInGateV(0) -> isInv();
   bool g1f2 = mergeGate -> getFanInGateV(1) -> isInv();
   bool g2f1 = toBeMerged -> getFanInGateV(0) -> isInv();
   bool g2f2 = toBeMerged -> getFanInGateV(1) -> isInv();
   if(g1f1==g2f1 && g1f2==g2f2) return 0;
   return 1;   
}

void
CirMgr::merge(CirGate* mergeGate,CirGate* toBeMerged ) {
   int inv = checkInv(mergeGate,toBeMerged);
   if(inv == 1) {
      return;
   }
   else {
      cout<<"Strashing: "<<mergeGate ->getGateId()<<" merging "
      <<toBeMerged->getGateId()<<"..."<<endl;
      toBeMerged -> cutoffFanInWiring();
      toBeMerged -> cutoffFanOutWiring();
         for(int to=0;to<(int)toBeMerged -> getFanOutSize();to++) {
         mergeGate -> addFanOutListV(toBeMerged -> getFanOutGateV(to));
         toBeMerged -> getFanOutGate(to) -> addFanInList(mergeGate,
            toBeMerged->getFanOutGateV(to)->isInv());
         }
      eraseGate(toBeMerged);
   }
}



void
CirMgr::gateStrash(CirGate* toBeMerged,HashMap<HashKey,size_t >* hash)
{
   size_t inId0 = toBeMerged -> getFanInGate(0) -> getGateId();
   size_t inId1 = toBeMerged -> getFanInGate(1) -> getGateId();
   if(inId0 > inId1) {
      toBeMerged -> swapFanIn();
   }
   HashKey k((size_t)(toBeMerged->getFanInGate(0)),
      (size_t)(toBeMerged->getFanInGate(1))); 
   size_t mergeGate;
   if (hash -> check(k, mergeGate)) {
      merge((CirGate*)mergeGate,toBeMerged);
   }
   else hash -> forceInsert(k, (size_t)toBeMerged);
}
