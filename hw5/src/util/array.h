/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   Array() : _data(0), _size(0), _capacity(0) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { 
         T* _temp = _node;
         return (*_temp); 
      }
      T& operator * () { return (*_node); }
      iterator& operator ++ () { 
         _node = _node + 1;
         return (*this);
      }
      iterator operator ++ (int) { 
         iterator temp(*this);
         ++(*this);
         return temp;           
      }
      iterator& operator -- () { 
         _node = _node - 1;
         return (*this); 
      }
      iterator operator -- (int) {
         iterator temp(*this);
         --(*this);
         return temp;           
      }
      iterator operator + (int i) const { return iterator(_node + i); }
      iterator& operator += (int i) { _node = _node + i; return (*this); }

      iterator& operator = (const iterator& i) { _node = i._node; return (*this); }

      bool operator != (const iterator& i) const { 
         if(_node != i._node)      return true;
         return false;
      }
      bool operator == (const iterator& i) const { return !( (*this) != i); }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { 
      if(_capacity == 0)   return 0;
      return iterator(_data);       
   }
   iterator end() const { 
      if(_capacity == 0)   return 0;
      return iterator(_data+_size); 
   }
   bool empty() const { return !(_size); }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return _data[i]; }
   const T& operator [] (size_t i) const { return _data[i]; }

   void push_back(const T& x) {
      if(_capacity == 0)   reserve(1);
      if(_size == _capacity)     reserve(_capacity*2);
      *(_data+_size) = x;
      _size++;
   }
   void pop_front() {
      erase(begin());
   }
   void pop_back() {
      erase(--end());
   }

   bool erase(iterator pos) { 
      if(empty())   return false;
      while(pos != --end() ) {
         *pos = *(pos+1); 
         ++pos;
      }
      *(--end()) = T();
      _size--;
      return true;
   }
   bool erase(const T& x) { 
      if(empty())   return false;
      iterator eraser = begin();
      while(!(*eraser == const_cast<T&>(x) ) && eraser != end())   ++eraser;
      if(eraser == end())   return false;
      return erase(eraser); 
   }

   void clear() {
      for(int i = 0; i < (int)_size; i++)   _data[i] = T();
      _size = 0;
   }

   // This is done. DO NOT change this one.
   void sort() const { if (!empty()) ::sort(_data, _data+_size); }

   // Nice to have, but not required in this homework...
   void reserve(size_t n) { 
   if( n <= _capacity)   return;
   T* reallocData = new T[n];
   _capacity = n;
   myMemcpy(reallocData, _data);  
   delete[] _data;
   _data = reallocData;
}
   void resize(size_t n) {
      if(n <= _size) { for(int i=n ; i < (int)_size ; i++)   (_data+i) = T(); }
      else {
            reserve(n);
            for(int i = _size; i < (int)n ; i++)   _data[i] = T();
      }
      _size = n;
   }

private:
   T*           _data;
   size_t       _size;       // number of valid elements
   size_t       _capacity;   // max number of elements

   // [OPTIONAL TODO] Helper functions; called by public member functions
   void myMemcpy(T* newLoc,T* oldLoc) { for(int i = 0; i < (int)_size; i++)   *(newLoc+i) = *(oldLoc + i); }
};

#endif // ARRAY_H
