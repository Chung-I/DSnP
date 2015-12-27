/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ()" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
friend class iterator;
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
   public:
      friend class HashSet<Data>;
      iterator(vector<Data>* d,size_t i,vector<Data>* endBucket):
      _currBucket(d), _id(i), _endBucket(endBucket) {};
      ~iterator() {};
   iterator& operator ++() {
      bool flag = false;
      while(1) {
         if(_currBucket == _endBucket)   break;
         if(!(_currBucket -> size()) || 
         (!flag && _id == _currBucket -> size()-1)) {
            _currBucket = _currBucket+1; 
            _id = 0; 
            flag = true;
         }
         else { if(!flag) {++_id; } break; }
      }
      return *(this);
   }
    
   iterator operator ++(int) {
      iterator temp = *(this);
      ++(*this);
      return temp;
   }
   iterator& operator --() {
      bool flag = false;
      while(1) {
         if(_currBucket = _endBucket)   break;
         if(!(_currBucket -> size())) {
            _currBucket = _currBucket-1; 
            _id = _currBucket -> size()-1; 
            flag = true;
         }
         else if(!flag && _id == 0) {
            _currBucket = _currBucket-1; 
            _id = _currBucket -> size()-1; 
            flag = true;
          }
          else { if(!flag) {--_id; } break; }
      }
      return *(this);
   }
   iterator operator --(int) {
      iterator temp = *(this);
      --(*this);
      return temp;
   }
   iterator& operator =(const iterator it) {
      _currBucket = it._currBucket;
      _id = it._id;
   }
   bool operator ==(const iterator it) const {
      if(_currBucket == it._currBucket && _id == it._id )   return true;
      return false;
   }
   bool operator !=(const iterator it) const {
     return !((*this) == it);
   }
   Data& operator *() { return (*_currBucket)[_id]; }
   private:
      vector<Data>*   _currBucket;
      size_t          _id;
      vector<Data>*   _endBucket;
   };

   void init(size_t b) { 
      _numBuckets = b; 
      _buckets = new vector<Data>[b];
   }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const { 
      if(empty())   return end();
      if(!_buckets -> size()) return ++iterator(_buckets,0,_buckets+_numBuckets);
      return iterator(_buckets,0,_buckets+_numBuckets); 
   }
   // Pass the end
   iterator end() const { 
      return iterator(_buckets+_numBuckets,0,_buckets+_numBuckets);
   }
   // return true if no valid data
   bool empty() const {  
      return !size();
   }
   // number of valid data
   size_t size() const { 
      size_t s = 0; 
      for(int i=0;i<(int)_numBuckets;i++)   s += _buckets[i].size();
      return s; //except dummy node
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const { 
      if(search(d) > -1)   return true;
      return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const { 
      int num = bucketNum(d);
      int i = search(d);
      if(i != -1) {
         _buckets[num][i] = d;
         return true;
      }
      return false;
   }

   // update the entry in hash that is equal to d
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) { 
      int num = bucketNum(d);
      int i = search(d);
      if(i != -1) {
         _buckets[num][i] = d;
         return true;
      }
      _buckets[num].push_back(d);
      return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) { 
      size_t num = bucketNum(d);
      int i = search(d);
      if(i != -1)   return false;

      _buckets[num].push_back(d);
      return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return false otherwise (i.e. nothing is removed)
   bool remove(const Data& d) {
      int num = bucketNum(d);
      int i = search(d);
      if(i == -1)   return false;
      _buckets[num].erase(_buckets[num].begin()+i);
      return true;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
   int search(const Data& d) const {
      int num = bucketNum(d);
      for(int i=0; i<(int)_buckets[num].size();i++) {
         if(_buckets[num][i] == d)   return i;
      }
      return -1;
   } 
};

#endif // MY_HASH_SET_H
