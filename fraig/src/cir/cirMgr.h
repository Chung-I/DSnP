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
#include "sat.h"
extern CirMgr *cirMgr;
enum DFSFunc {
   DFSGate,
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
   
   vector<vector<unsigned> >* getFecGrps() const;

private:
   ofstream   *_simLog;
   vector<CirGate*>   _totalList;   
   vector<unsigned>   _fecGates;   
  vector<unsigned>    _fltGates;
  vector<unsigned>    _unusedGates;
  vector<unsigned>    _piList;
  vector<unsigned>    _poList;
  vector<unsigned>    _aigList;
  vector<unsigned>    _dfsList;
  vector<vector<unsigned> > _fecGrps;
  bool                _updateDfsListFlag;
  mutable int         dfsNum;
  int                 _maxGateId;
  vector<int>        _noSimFlag;
  vector<size_t>      _faninSignal;
  vector<size_t>      _fanoutSignal;
  int                 _failTime;
    

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
   void getGateDfsList(CirGate*&,vector<unsigned>&,vector<unsigned>&,int&) const ;
   
   // Private Member functions about simulation
   void loadSignal(vector<size_t>&);
   void loadRandSignal();
   void for_each_po_simulate();
   bool detectFecGrps();
   void CollectValidFecGrp(HashMap<SimValue, FecGroup>&,int );
   void updateFecPair();
   void printPatternSimulated(int ) const;
   bool readSimFile(ifstream&,vector<string>&) const;
   void packSim2size_t(vector<string>&,vector<size_t>&) const;
   void collectFaninSignal();
   void collectFanoutSignal();
   void packSize_t2Str(vector<size_t>&,vector<string>&,vector<unsigned>&) const;
   void writesimLogfile(vector<string>&); 
   void writesimLogfile(); 

   // Member functions about fraig
   void genProofModel(SatSolver&); 
   void prove(SatSolver&);
   void fraigMerge(size_t ,size_t);
};
#endif // CIR_MGR_H
