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
#include <limits.h>
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
   for(int i=0;i<500;i++) {
      generateSignal();
      for_each_po_simulate();
//      detectFecGrps();
   }
}

void
CirMgr::fileSim(ifstream& patternFile)
{
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void
CirMgr::generateSignal() {
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
void
CirMgr::detectFecGrps() {
   for(HashMap<SimValue,FecGroup>::iterator it = _fecGrps.begin()
   ;it !=_fecGrps.end();++it) {
      HashMap<SimValue, FecGroup> newFecGrps;
      for(int gt = 0;gt < ((*it).second).size();gt++) {
         FecGroup grp;
         CirGate* gate = ((*it).second)[gt];
         if( newFecGrps.check( gate->getSimValue(),grp )) {
            grp.push_back(gate);
         } else {
            FecGroup newGrp;
            newGrp.push_back(gate);
            newFecGrps.forceInsert( gate -> getSimValue(),newGrp);
         }
      }
//      CollectValidFecGrp(newFecGrps, fecGrp, fecGrps);
   }
}
void
CirMgr::CollectValidFecGrp(newFecGrps, fecGrp, fecGrps) {

   for(HashMap<SimValue,FecGroup>::iterator it = newFecGrps.begin()
   ;it != newFecGrps.end();++it) {
 
}
