/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;
class CirGate;
//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
friend class cirMgr;
public:
   CirGate(unsigned id,int No): _gateID(id),_lineNo(No), _visit(_classVisit) {}
   virtual ~CirGate() {}
   // Basic access methods
   virtual string getTypeStr() const  = 0;
   unsigned getLineNo() const { return _lineNo; }
   unsigned getGateId() const {return _gateID;}
   virtual bool visited() const {
      if(_visit != _classVisit)  return true;
      return false;
   }
   void resetGateVisit() {_visit = _classVisit; }
   unsigned getFanInId(int i) const {return _faninIdList[i];}
   unsigned getFanInSize()const {return _faninIdList.size();}
   unsigned getFanOutSize()const {return _fanoutIdList.size();}
   void setGateName(string name) {_gateName = name;}
   string getGateName() const {return _gateName;}
   void visitGate() {++_visit;};
   // Printing functions
   virtual void printGate() const = 0;
   void printGateName() const {if(!_gateName.empty())   cout<<"("<<_gateName<<")";}
   void reportGate() const;
   void reportFloatGates() const;
   void reportFanin(int level) const;
   void atomicReportFanin(int level,int callLevel) const;
   void atomicReportFanout(int level,int callLevel) const;
   void reportFanout(int level) const;
   void addFanInIdList(unsigned id) {_faninIdList.push_back(id);}
   void addFanOutIdList(unsigned id) {_fanoutIdList.push_back(id);}
   static size_t    _classVisit;
private:

protected:
   IdList           _faninIdList; 
   IdList           _fanoutIdList;
   string           _gateName;
   unsigned         _gateID;
   mutable size_t   _visit;
   int              _lineNo;
};

class AigGate: public CirGate
{
   friend class AigGateV;
public:
   AigGate(unsigned id, int No): CirGate(id,No) {}
   virtual string getTypeStr() const { return "AIG";}
   virtual void printGate() const {
      _visit++;
      cout<<" AIG "<<_gateID;
   };
private:
protected:

};
/*class AigGateV {
#define NEG 0x1
public:   
   AigGateV(AigGate* g, size_t phase):
   _gateV(size_t(g) + phase) { }
   AigGate* gate() const {
   return (AigGate*)(_gateV & ~size_t(NEG)); }
   bool isInv() const { return (_gateV & NEG); }
private:
   size_t     _gateV;
};
*/


class PiGate: public CirGate
{
public:
   PiGate(unsigned id, int No):CirGate(id,No) {}
   virtual string getTypeStr() const { return "PI";}
   virtual void printGate() const { 
      ++_visit;
      cout<<" PI "<<_gateID;
   };
private:
};



class PoGate: public CirGate
{
public:
   PoGate(unsigned id,int No):CirGate(id,No) {}
   virtual string getTypeStr() const { return "PO";}
   virtual void printGate() const {
      _visit++;
      cout<<" PO "<<_gateID;
   };
private:
};



class ConstGate: public CirGate
{
public:
   ConstGate(): CirGate(0,0){}
   virtual string getTypeStr() const {return "CONST0";}
   virtual void printGate() const { 
      _visit++;
      cout<<"CONST0"; 
   }
};
#endif // CIR_GATE_H
