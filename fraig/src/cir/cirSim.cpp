/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits.h>
#include <bitset>
#include "myHashMap.h"
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/
extern CirMgr *cirMgr;
/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
   for(size_t i=0;i<_noSimFlag.size();i++) _noSimFlag[i] = 0;
   int failTime = 0;
   int maxFail = round(30*log(_fecGates.size()));
   int simulatedPattern = 0;
   cout<<"MAX_FAIL: "<<maxFail<<endl;
   while(failTime<maxFail && _fecGrps.size()){
      loadRandSignal();
      for_each_po_simulate();
      collectFaninSignal();
      collectFanoutSignal();
      if(!detectFecGrps()) ++failTime;
      simulatedPattern+=32;
      cout << "Total #FEC Group = "<<_fecGrps.size()<<'\r';
   }
   cout << endl;
   printPatternSimulated(simulatedPattern);
   updateFecPair();
   if(_simLog)  writesimLogfile();
}

void
CirMgr::fileSim(ifstream& patternFile)
{
   for(size_t i=0;i<_noSimFlag.size();i++) _noSimFlag[i] = false;

   int simulatedPattern = 0;
   vector<string> fileSimLines;
   vector<size_t> filePattern;
   vector<string> fileSimbackup;
   if(readSimFile(patternFile,fileSimLines)) {   //if no invalid input
      fileSimbackup = fileSimLines;
      simulatedPattern+=fileSimLines.size();
      while(fileSimLines.size()) {
         packSim2size_t(fileSimLines,filePattern);
         loadSignal(filePattern);
         filePattern.clear();
         for_each_po_simulate();
         collectFanoutSignal();
         detectFecGrps();
      cout << "Total #FEC Group = "<<_fecGrps.size()<<'\r';
      }
   if(_simLog) writesimLogfile(fileSimbackup);
   printPatternSimulated(simulatedPattern);
   //updateFecPair();
   } else {
      printPatternSimulated(simulatedPattern);
   }
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void
CirMgr::printPatternSimulated(int numOfPattern) const {
   cout<<numOfPattern<<" patterns simulated."<<endl;
}

void
CirMgr::writesimLogfile(vector<string>& fileSimLines) {
   vector<string> fanoutStr;
   packSize_t2Str(_fanoutSignal,fanoutStr,_poList); 
   for(int line=0;line<fileSimLines.size();++line) {
      (*_simLog) << fileSimLines[line]<<" ";
      (*_simLog) << fanoutStr[line]<<endl;
   }   
}

void
CirMgr::writesimLogfile() {
   vector<string> faninStr;
   packSize_t2Str(_faninSignal,faninStr,_piList); 
   vector<string> fanoutStr;
   packSize_t2Str(_fanoutSignal,fanoutStr,_poList); 
   for(int line=0;line<faninStr.size();++line) {
      (*_simLog) << faninStr[line]<<" ";
      (*_simLog) << fanoutStr[line]<<endl;
   }   
}

void
CirMgr::packSize_t2Str(vector<size_t>& filePattern,vector<string>& outputStr,
vector<unsigned>& list)const {
   vector<string> fileString;
   string line;
   int numOfLine = 32*(filePattern.size()/list.size());
   for(int pat=0;pat<filePattern.size();++pat) {
      bitset<32> b(filePattern[pat]);
      fileString.push_back(b.to_string());
   }
   for(int l = 0;l<numOfLine;++l) {
      for(int po=0;po<list.size();++po) {
         line.push_back(fileString[list.size()*(l/32)+po][31-l%32]);
      }
      outputStr.push_back(line);
      line.clear();
   }
}

void
CirMgr::packSim2size_t(vector<string>& rawPatternStr,vector<size_t>& filePattern) const{
   string packedStr;
   size_t numOfbit = ( rawPatternStr.size()>=32? 32 :rawPatternStr.size() );
   for(int pi = 0;pi<_piList.size();++pi) {
      packedStr.clear();
      for(int line=numOfbit-1;line>-1;--line) {
         packedStr.push_back(rawPatternStr[line][pi]);
      }
      bitset<32> b(packedStr);
      filePattern.push_back(b.to_ulong());
   }
   rawPatternStr.erase(rawPatternStr.end()-numOfbit,rawPatternStr.end());
}

void
CirMgr::collectFaninSignal() {
   for(int i=0;i<_piList.size();++i) {  
      _faninSignal.push_back(getGate(_piList[i])->getsimValue());
   }
}

void
CirMgr::collectFanoutSignal() {
   for(int i=0;i<_poList.size();++i) {  
      _fanoutSignal.push_back(getGate(_poList[i])->getsimValue());
   }
}

bool
CirMgr::readSimFile(ifstream& patternFile,vector<string>& fileSimLines) const
{
//return true if no invalid input
   string simLine;
   while(patternFile>>simLine) {
      if(simLine.size() != _piList.size()) {
         cout<<"Error: Pattern("<<simLine<<") length("<<simLine.size()<<") does not match the number of inputs("<<_piList.size()<<") in a circuit!!"<<endl;
         return false;
      }
      std::size_t found = simLine.find_first_not_of("01");
      if (found!=std::string::npos) {
         cout << "Error: Pattern("<<simLine<<") contains a non-0/1 character(\'"
         <<simLine[found]<<"\')."<<endl;
         return false;
      }
      fileSimLines.push_back(simLine);           
   }
   return true;
}


void
CirMgr::loadSignal(vector<size_t>& signals) {
   for(int pi=0;pi<(int)_piList.size();pi++) {
      ((PiGate*)getGate(_piList[pi])) -> receiveSignal(signals[pi]);
   }
}

void
CirMgr::loadRandSignal() {
   for(int pi=0;pi<(int)_piList.size();pi++) {
      ((PiGate*)getGate(_piList[pi])) -> receiveSignal(rnGen(INT_MAX));
   }
}

void
CirMgr::for_each_po_simulate() {
   for(int i=0;i<(int)_dfsList.size();i++) {
      getGate(_dfsList[i])->simulate();
   }
}
bool
CirMgr::detectFecGrps() {
   bool failflag = false; //if theres's splitting, set to true
   bool pairflag = false; //set to true if there's any pairs in the loop
   int originalSize = _fecGrps.size();
cout<<_fecGrps.size();
   int inFailTime = 0;
   for(int gr=0;gr<(int)_fecGrps.size();++gr) {
      //if(_noSimFlag[gr]>10)   continue;
      HashMap<SimValue, FecGroup> newFecGrps(100+(_fecGrps[gr].size())/10);
      for(int gt = 0;gt < (int)_fecGrps[gr].size();gt++) {
         FecGroup grp;
         unsigned gate = _fecGrps[gr][gt]/2;
         if( newFecGrps.check( getGate(gate)->getSimValue(),grp )) {
            grp.push_back(2*gate);
            newFecGrps.replaceInsert(getGate(gate)->getSimValue(),grp);
            pairflag = true;
         }
         else if(  newFecGrps.check( ~(getGate(gate)->getSimValue()),grp )) { //IFEC
            grp.push_back(2*gate+1); //inverted
            newFecGrps.replaceInsert(~(getGate(gate)->getSimValue()),grp);
            pairflag = true;
         } else {
            FecGroup newGrp;
            newGrp.push_back(2*gate);
            newFecGrps.forceInsert( getGate(gate) -> getSimValue(),newGrp);
         }
      }
      if(newFecGrps.size()==1) { 
         ++_noSimFlag[gr];
         ++inFailTime;
         continue; 
      }
      else if(!pairflag){
         _fecGrps.erase(_fecGrps.begin()+gr);
         _noSimFlag.erase(_noSimFlag.begin()+gr);
         --gr;
      } else {
        failflag = true;
        pairflag = false;
        CollectValidFecGrp(newFecGrps,gr);
      }
   }

   if(_fecGrps.size() < 2*originalSize || 
   2*inFailTime > _fecGrps.size()) failflag = false;  

   return failflag;
}
void
CirMgr::CollectValidFecGrp(HashMap<SimValue, FecGroup>& newFecGrps,int grpId) {
   bool first = true;
   for(HashMap<SimValue,FecGroup>::iterator it = newFecGrps.begin()
   ;it != newFecGrps.end();++it) {
      if((*it).second.size()>1 && first) {
         _fecGrps[grpId].clear();
         _fecGrps[grpId] = (*it).second;
         _fecGrps[grpId].assign((*it).second.begin(),(*it).second.end());
         _noSimFlag[grpId] = 0;
         first = false;
      }
      else if((*it).second.size()>1){ 
         _fecGrps.push_back((*it).second); 
         _noSimFlag.push_back(0);
      }
   }
}
void
CirMgr::updateFecPair() {
   for(int gr=0;gr<(int)_fecGrps.size();++gr) {
      for(int gt=0;gt<(int)_fecGrps[gr].size();++gt)
      getGate(_fecGrps[gr][gt]/2)->gateUpdateFecPair(gr);
   }
}
