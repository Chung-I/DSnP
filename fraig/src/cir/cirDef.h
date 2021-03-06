/****************************************************************************
  FileName     [ cirDef.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic data or var for cir package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_DEF_H
#define CIR_DEF_H

#include <vector>
#include "myHashMap.h"

using namespace std;

// TODO: define your own typedef or enum
class CirGate;
class CirMgr;
class SatSolver;
#define FANIN_SIZE  2
typedef vector<unsigned>   FecGroup; 
typedef vector<CirGate*>   GateList; 
enum GateType {
   CIR,
   PI,
   PO,
   AIG,
   UNDEF,
   CONST,
   DUMMY
};
#endif // CIR_DEF_H
