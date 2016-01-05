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
   
   for(int i=0;i<_totalList.size();i++) {
      if(!_totalList[i].visited()) {
         for(int gateNum=0;gateNum<getFanInSize();gateNum++) {
            _totalList.getFanInGate(gateNum) -> deleteFanOutGate(_totalList[gateNum]);
         }
         for(int gateNum=0;gateNum<getFanOutSize();gateNum++) {
            _totalList.getFanOutGate(gateNum) -> deleteFanInGate(_totalList[gateNum]);
         }
         cout<<"Sweeping: "<<_totalList[i]->getTypeStr()
         <<"("<<_totalList[i]->getGateId()<<")"<<" removed..."<<endl;
         _totalList.erase(_totalList.begin()+i);
      }
   }
   updateLists();
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{

}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
