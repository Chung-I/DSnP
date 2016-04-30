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
#include <algorithm>
#include <iostream>
#include "cirDef.h"
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------


class CirGate
{
friend class cirMgr;
friend class GateV;
class GateV {
#define NEG 0x1
public:   
   GateV(CirGate* g, bool phase,unsigned id):
   _gateV(size_t(g) + phase),_gateId(id) { }
   void setGateV(CirGate* g,bool phase,unsigned id) {
      _gateV = size_t(g)+phase;
      _gateId = id;
   }
   void setInv(bool phase) {_gateV = size_t((_gateV & ~size_t(NEG))+phase);}
   size_t getWiringId() { return 2*_gateId+isInv(); }
   CirGate* gate() const {
   return (CirGate*)(_gateV & ~size_t(NEG)); }
   bool isInv() const { return (_gateV & NEG); }
   unsigned getGateId() const {return _gateId;}
   
private:
   mutable size_t     _gateV;
   mutable unsigned   _gateId;
};
public:
   // Basic access methods
   virtual bool isAig() const { return false; }

   // Printing functions
   CirGate(unsigned id,int No): _gateID(id),_lineNo(No),_visit(_classVisit),_wiringGate(new GateV(0,0,0)),_invWiringGate(new GateV(0,0,0)),_simValue(0),_SimValue(SimValue(0)) {
      _wiringGate -> setGateV(this,0,id);
      _invWiringGate -> setGateV(this,1,id);
   }
   CirGate(unsigned id): _gateID(id), _visit(_classVisit),_simValue(0), _SimValue(SimValue(0)){}
   virtual ~CirGate() {}
   // Basic access methods
   virtual string getTypeStr() const {return "cirGate";};
   virtual GateType getGateType() const {return CIR;};
   //virtual void setLineNo(int no) {_lineNo = no; }
   unsigned getLineNo() const { return _lineNo; }
   //void setGateId(int Id) {_gateID = Id; }
   unsigned getGateId() const {return _gateID;}
   virtual bool visited() const {
      if(_visit != _classVisit)  return true;
      return false;
   }
   void resetGateVisit() {_visit = _classVisit; }
   CirGate* getFanInGate(int i) const {return _faninList[i] -> gate();}
   GateV* getFanInGateV(int i) const {return _faninList[i];}
   CirGate* getFanOutGate(int i) const {return _fanoutList[i] -> gate();}
   GateV* getFanOutGateV(int i) const {return _fanoutList[i];}
   void eraseFanInGate(CirGate* eraseGate) {
      for(int i=0;i<(int)_faninList.size();i++) {
         if(_faninList[i]->gate()==eraseGate) {
            _faninList.erase(_faninList.begin()+i);
         }
      }
   }
   void eraseFanOutGate(CirGate* eraseGate) {
      for(int i=0;i<(int)_fanoutList.size();i++) {
         if(_fanoutList[i]->gate()==eraseGate) 
         _fanoutList.erase(_fanoutList.begin()+i);
      }
   }
   unsigned getFanInSize()const {return _faninList.size();}
   unsigned getFanOutSize()const {return _fanoutList.size();}
   void setGateName(string name) {_gateName = name;}
   string getGateName() const {return _gateName;}
   void visitGate() {++_visit;};
   // Printing functions
   virtual void printGate() const {cout<<"cirGate"; return;}
   virtual void printGateName() const {
      if(!_gateName.empty())   
      cout<<" ("<<_gateName<<")";
   }
   void reportGate() const;
   void reportFloatGates() const;
   void reportFanin(int level) const;
   void atomicReportFanin(int level,int callLevel) const;
   void atomicReportFanout(int level,int callLevel) const;
   void reportFanout(int level) const;
   bool gateOptimize();
   void replaceByFanin(int in);
   void replaceByConst0(bool in);
   void cutoffFanInWiring();
   void cutoffFanOutWiring();
   void reWireFanIn();
   void reWireFanOut();
   void swapFanIn(){
      if(getFanInSize()==2) {
         swap(_faninList[0],_faninList[1]);
      }
   }
   GateV* getWiringGate(bool phase) {
      if(phase) return _invWiringGate;
      else return _wiringGate;
   }
   void addFanInList(CirGate* inGate,bool phase) {_faninList.push_back(inGate -> getWiringGate(phase)); }
   void addFanInListV(GateV* inGateV) {_faninList.push_back(inGateV); }
   void addFanOutList(CirGate* outGate,bool phase) {_fanoutList.push_back(outGate -> getWiringGate(phase)); }
   void addFanOutListV(GateV* outGateV) {_fanoutList.push_back(outGateV); }
   virtual size_t simulate() = 0;
   string getSimValueString() const;
   size_t getsimValue() const {return _simValue; }
   SimValue getSimValue() const {return _SimValue;}
   void gateUpdateFecPair(size_t);
   void gatePrintFecPair() const;
   size_t getFECs() const { return _grp; };
   static size_t    _classVisit;
   Var getVar() const { return _var; }
   void setVar(const Var& v) { _var = v; } 
private:

protected:
   //IdList           _faninIdList; 
   vector<GateV*>    _faninList;
   //IdList           _fanoutIdList;
   vector<GateV*>    _fanoutList;
   unsigned         _gateID;
   string           _gateName;
   int              _lineNo;
   mutable size_t   _visit;
   GateV*           _wiringGate;
   GateV*           _invWiringGate;
   int              _simValue;
   SimValue         _SimValue;
   size_t           _grp; //which fec grp you're in
   Var              _var;
};

class AigGate: public CirGate
{
   friend class GateV;
public:
   AigGate(unsigned id, int No): CirGate(id,No) {}
   AigGate(unsigned id): CirGate(id) {}
   virtual string getTypeStr() const { return "AIG";}
   virtual void printGate() const {
      cout<<"AIG "<<_gateID;
      for(int i=0;i<(int)_faninList.size();i++) {
         cout<<(_faninList[i]->gate()->getGateType()==UNDEF?"*":"")
            <<" "<<(_faninList[i]->isInv()?"!":"")
            <<_faninList[i] -> gate() -> getGateId();
            if(!getGateName().empty())  
            cout<<" ("<<getGateName()<<")";
      }
   }
   virtual bool isAig() const { return true; }
   virtual size_t simulate() {
 //     _simValue = 
 //     ((getFanInGateV(0)->isInv()? 0xFFFFFFFF : 0)^
 //     getFanInGate(0) -> getsimValue()) &
 //     ((getFanInGateV(1)->isInv()? 0xFFFFFFFF : 0)^
 //     getFanInGate(1) -> getsimValue() );
 //     _SimValue.set(_simValue);
       size_t sim0;
       size_t sim1;
       if(getFanInGateV(0)->isInv()) sim0 =~(getFanInGate(0) -> getsimValue());
       else {sim0 = getFanInGate(0) -> getsimValue();}
       if(getFanInGateV(1)->isInv()) sim1 =~(getFanInGate(1) -> getsimValue());
       else {sim1 = getFanInGate(1) -> getsimValue();}
       _simValue = sim0 & sim1;
       _SimValue.set(_simValue);
       return _simValue;
   }
   virtual GateType getGateType() const {return AIG;}
protected:
};





class PiGate: public CirGate
{
public:
   PiGate(unsigned id, int No):CirGate(id,No) {}
   virtual string getTypeStr() const { return "PI";}
   virtual void printGate() const { 
      cout<<"PI  "<<_gateID;
      if(!getGateName().empty())  
      cout<<" ("<<getGateName()<<")";
   }
   void receiveSignal(int pattern) {
      _simValue = pattern;
      _SimValue.set(_simValue);
   }
   virtual size_t simulate() {
      return _simValue;
   }
   virtual GateType getGateType() const {return PI;} 
protected:
};



class PoGate: public CirGate
{
public:
   PoGate(unsigned id,int No):CirGate(id,No) {}
   virtual string getTypeStr() const { return "PO";}
   virtual void printGate() const {
      cout<<"PO  "<<_gateID;
      for(int i=0;i<(int)_faninList.size();i++) {
         cout<<(_faninList[i]->gate()->getGateType()==UNDEF?"*":"")
            <<" "<<(_faninList[i]->isInv()?"!":"")
            <<_faninList[i] -> gate() -> getGateId();
            if(!getGateName().empty())  
            cout<<" ("<<getGateName()<<")";
      }
   } 
   virtual GateType getGateType()const {return PO;}
   virtual size_t simulate() {
      if(getFanInGateV(0)->isInv()) { 
         _simValue = ~(getFanInGate(0) -> getsimValue()); 
      } else {
         _simValue = getFanInGate(0) -> getsimValue(); 
      }
      _SimValue.set(_simValue);
      return _simValue;
   }
private:
protected:
};



class ConstGate: public CirGate
{
public:
   ConstGate(): CirGate(0,0){_SimValue.set(0);}
   virtual string getTypeStr() const {return "CONST";}
   virtual void printGate() const { 
      _visit++;
      cout<<"CONST0"; 
   }
   virtual GateType getGateType() const {return CONST;} 
   virtual size_t simulate() { return 0; }
protected:
};



class UndefGate: public CirGate
{
public:
   UndefGate(unsigned id,int No): CirGate(id,No){ _SimValue.set(0); }
   virtual string getTypeStr() const {return "UNDEF";}
   virtual void printGate() const { 
      _visit++;
      cout<<"UNDEF"; 
   }
   virtual GateType getGateType() const {return UNDEF;}
   virtual size_t simulate() { return 0; }
protected:
};
#endif // CIR_GATE_H
