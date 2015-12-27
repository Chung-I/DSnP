/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) { 
      _data.reserve(_data.size()? 1 : _data.size()*2);
      int t = _data.size()+1;
      _data.push_back(d);
      while(t > 1) {
         int p = t/2;
         if(!(d < _data[p-1])) { break; }
         Data temp = _data[t-1];
         _data[t-1] = _data[p-1];
         _data[p-1] = temp;
         t = p;
      }
      _data[t-1] = d;
   }
   void delMin() { 
      delData(0);
   }
   void delData(size_t i) { 
      ++i;
      int n = _data.size();
      int t = 2*i;
      while( t < n ) {
         if(!(_data[t-1] < _data[t]))   ++t;
         if(_data[n-1] < _data[t-1]) { break; }
         _data[i-1] = _data[t-1];
         i = t;
         t = 2*i;
      }
      _data[i-1] = _data[n-1];
      _data.pop_back();
   }

private:
   // DO NOT add or change data members
   std::vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
