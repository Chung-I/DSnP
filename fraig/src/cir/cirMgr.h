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

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"
#include "cirGate.h"
#include "myHashMap.h"
extern CirMgr *cirMgr;
enum DFSFunc {
   DFSAig,
   DFSVisit,
   DFSPrint,
   DFSConstruct
};

class CirMgr
{
public:
   CirMgr() {}
     ~CirMgr() {} 

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const { 
      if(gid < _totalList.size())  return _totalList[gid];
      return 0; 
   }
   void resetVisit() const;
   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;

private:
   ofstream   *_simLog;
   vector<CirGate*>   _totalList;   
   vector<CirGate*>   _fecGates;   
  vector<unsigned>    _fltGates;
  vector<unsigned>    _unusedGates;
  vector<unsigned>    _piList;
  vector<unsigned>    _poList;
  vector<unsigned>    _aigList;
  vector<unsigned>    _dfsList;
  HashMap<SimValue,FecGroup> _fecGrps;
   bool _updateDfsListFlag;
   mutable int   dfsNum;
   int        _maxGateId;

   void DFS(CirGate*, DFSFunc dfsFunc); 
   void updateDfsList();
   void constructDfsList();
   void dfsVisit();
   void preOrderTraverse();
   void atomicPreOrderTraverse(CirGate*);
   void addfltGate(CirGate* fltGate);
   void addunusedGate(CirGate* unusedGate);
   void eraseGate(CirGate*);
   void merge(CirGate* ,CirGate*);
   void gateStrash(CirGate* ,HashMap<HashKey, size_t >*);
   bool checkInv(CirGate*,CirGate*);
   void generateSignal();
   void for_each_po_simulate();
   void detectFecGrps();
};

#endif // CIR_MGR_H
