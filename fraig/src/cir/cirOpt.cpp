/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
   //if(!_unusedGates.size()) return;  //if there's no unusedGate, no gate should be removed

   dfsVisit();
   for(int un=1;un<(int)_totalList.size();un++) {
      if(_totalList[un] && 
         _totalList[un]->getGateType()!=PI &&
         !(_totalList[un]->visited()) ) {
         for(int in=0;in<(int)_totalList[un]->getFanInSize();in++) {
            CirGate* fanin = getGate(_totalList[un]->getFanInGate(in)->getGateId()); 
            if(fanin) {
               fanin -> eraseFanOutGate(_totalList[un]);
            }
         }
         cout<<"Sweeping: "<<_totalList[un]->getTypeStr()
            <<"("<<_totalList[un]->getGateId()<<")"<<" removed..."<<endl;
         eraseGate(_totalList[un]);
      }
   }
   for(int i=0;i<(int)_piList.size();i++) {  //update unused pi gates
      if(!(getGate(_piList[i]) -> getFanOutSize())) 
      addunusedGate(getGate(_piList[i]));
   }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
   //preOrderTraverse(); 
   for(int i=0;i<(int)_dfsList.size();i++) {
      if(getGate(_dfsList[i]) -> getGateType() != CONST &&
        getGate(_dfsList[i]) -> getGateType() != PI &&
         getGate(_dfsList[i]) -> getGateType() != UNDEF)
      if(getGate(_dfsList[i]) -> gateOptimize()) {
         eraseGate(getGate(_dfsList[i]));
      }
   } 
   //if(_updateDfsListFlag)  
   constructDfsList();
   //sweep(); 
   
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
