/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
   HashMap<HashKey,size_t >* hash = new HashMap<HashKey,size_t >(1.6*_dfsList.size());
   for(int i=0; i<(int) _dfsList.size(); i++) {
      if(getGate(_dfsList[i]) -> getGateType() == AIG)
      gateStrash(getGate(_dfsList[i]),hash);
   } 
   constructDfsList();
   delete hash;
}

void
CirMgr::fraig()
{
   SatSolver solver;
   solver.initialize();
   genProofModel(solver);
   prove(solver);
   constructDfsList();
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
void
CirMgr::fraigMerge(size_t grp,size_t gt) {
   cout<<"Fraig: "<<_fecGrps[grp][0]/2<<" merging "
   <<(_fecGrps[grp][gt]%2 ? "!" : "")<<_fecGrps[grp][gt]/2<<"..."<<endl;
   CirGate* toBeMerged = getGate(_fecGrps[grp][gt]/2);
   CirGate* mergeGate = getGate(_fecGrps[grp][0]/2);
   toBeMerged -> cutoffFanInWiring();
      for(int to=0;to<(int)toBeMerged -> getFanOutSize();to++) {
      mergeGate -> addFanOutList(toBeMerged -> getFanOutGate(to),(toBeMerged->getFanOutGateV(to)->isInv()!= _fecGrps[grp][gt]%2));
      toBeMerged -> getFanOutGate(to) -> addFanInList(mergeGate,
         (toBeMerged->getFanOutGateV(to)->isInv()!=_fecGrps[grp][gt]%2));
      }
   toBeMerged -> cutoffFanOutWiring();
   eraseGate(toBeMerged);
}
void
CirMgr::genProofModel(SatSolver& s)
{
   // Allocate and record variables; No Var ID for POs
   Var v = s.newVar();
   getGate(0)->setVar(v);
   for (size_t i = 0, n = _dfsList.size(); i < n; ++i) {
      Var v = s.newVar();
      getGate(_dfsList[i])->setVar(v);
   }   
   // Hard code the model construction here...
   s.addAigCNF(getGate(0)->getVar(),getGate(0)->getVar(),true,getGate(0)->getVar(),false);
   for (size_t i = 0, n = _aigList.size(); i < n; ++i) {
   s.addAigCNF( getGate(_aigList[i])->getVar(),
               getGate(_aigList[i])->getFanInGate(0)->getVar(), 
               getGate(_aigList[i])->getFanInGateV(0)->isInv(),
               getGate(_aigList[i])->getFanInGate(1)->getVar(),
               getGate(_aigList[i])->getFanInGateV(1)->isInv());
   }
}
void
CirMgr::prove(SatSolver& solver) {
   for(size_t gr=0; gr<_fecGrps.size();++gr) {
      cout<<"proving fecGrp: "<<_fecGrps[gr][0]<<endl;
      for(size_t gt=1;gt<_fecGrps[gr].size();++gt) {
         Var newV = solver.newVar();
         solver.addXorCNF(newV, getGate(_fecGrps[gr][0]/2)->getVar(), false, 
         getGate(_fecGrps[gr][gt]/2)->getVar(), _fecGrps[gr][gt]%2);
         solver.assumeRelease();  // Clear assumptions
         solver.assumeProperty(newV, true);  // k = 1
         if(!solver.assumpSolve()) {
            fraigMerge(gr,gt);
         }
      }
   }
  _fecGrps.clear();

}
