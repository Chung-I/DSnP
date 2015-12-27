/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr(){}
   ~CirMgr() {}

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const { 
      resetVisit();
      bool exist = false;
      int gateNum;
      for(int i=0; i<(int)_indexList.size() ; i++) {
         if(_indexList[i] == gid) {exist = true; gateNum = i; break;}
      }
      if(exist) {
         if(_totalList[gateNum] != 0) {return _totalList[gateNum];}
         else {return 0;}
      }
      return 0;
   }
   CirGate* myGetGate(unsigned gid) const { 
      bool exist = false;
      int gateNum;
      for(int i=0; i<(int)_indexList.size() ; i++) {
         if(_indexList[i] == gid) {exist = true; gateNum = i; break;}
      }
      if(exist) {
         if(_totalList[gateNum] != 0) {return _totalList[gateNum];}
         else {return 0;}
      }
      return 0;
   }
   
   void resetVisit() const;
   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;
private:
   GateList   _totalList;
   IdList     _indexList;
   IdList     _fltGates;
   IdList     _unusedGates;
   GateList   _piList;
   GateList   _poList;
   mutable GateList   _aigList;
   int        _maxGateId;
   void DFS(CirGate*) const;
   void DFVisit(CirGate*) const;
   void DFAig(CirGate*) const;
   mutable int   dfsNum;
};

#endif // CIR_MGR_H
