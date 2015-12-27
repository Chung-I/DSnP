/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { 
         T temp = _node -> _data;
         return temp; 
      }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { 
         _node = _node -> _next;
         return *(this); 
      }
      iterator operator ++ (int) { 
         iterator temp = *(this);
         _node = _node -> _next;
         return temp; 
      }
      iterator& operator -- () { 
         _node = _node -> _prev;
         return *(this); 
      }
      iterator operator -- (int) { 
         iterator temp = (*this);
         _node = _node -> _prev;
         return temp; 
      }

      iterator& operator = (const iterator& i) { 
         _node = i._node;
         return *(this); 
      }
      bool operator != (const iterator& i) const { 
         if(_node != i._node)   return true;
         return false; 
      }
      bool operator == (const iterator& i) const { 
         if(_node == i._node)   return true;
         return false;   
      }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { 
      if(empty())   return end();
      return iterator(_head -> _next); 
   }
   iterator end() const { return iterator(_head); }
   bool empty() const { 
      if(_head -> _prev == _head && _head -> _next == _head)   return true;
      return false; 
   }
   size_t size() const {  
      if(empty())   return 0;
      int size = 0;
      iterator traversal = begin();
      while(traversal != end()) {
         size++;
         traversal++;
      }
      return (size_t)size; 
   }

   void push_back(const T& x) {
      iterator beforeEnd = --end();
      DListNode<T>* newElement = new DListNode<T>(x,beforeEnd._node,end()._node);
      beforeEnd._node -> _next = newElement;
      _head -> _prev = newElement; 
   }
   void pop_front() {
      iterator temp = begin();
      iterator newBegin = ++begin();
      _head -> _next = newBegin._node;
      newBegin._node -> _prev = _head;
      delete temp._node;
   }
   void pop_back() { 
      iterator temp = --end();
      iterator newEnd = --(--end());
      _head -> _prev = newEnd._node;
      newEnd._node -> _next = _head;
      delete temp._node;
   }

   // return false if nothing to erase
   bool erase(iterator pos) { 
      if(empty())   return false;
      DListNode<T>* temp = pos._node;
      temp -> _prev -> _next = temp -> _next;
      temp -> _next -> _prev = temp -> _prev;
      delete temp;
      return true; 
   }
   bool erase(const T& x) { 
      if(empty())   return false;
      iterator traversal = begin();
      while(!(traversal._node -> _data == const_cast<T&>(x) ) ) {
         if(traversal == end()) return false;
         traversal++;
      }
      erase(traversal);
      return true; 
   }

   void clear() { 
      iterator traversal = begin();
      DListNode<T>* temp = traversal._node;
      while(traversal != end()) {
         temp = traversal._node;
         traversal++;
         delete temp;
      }
      _head -> _next = _head -> _prev = _head;
   }  // delete all nodes except for the dummy node

   void sort() const { 
      if(empty() || size() == 1)   return;
      iterator traversal = begin();
      iterator upperBound = --end();
      while(traversal != upperBound) {
         for(;traversal!= upperBound ; traversal++) {
         if(!(*traversal < traversal._node -> _next -> _data) )  
            swap(traversal._node, const_cast<DListNode<T>*>(traversal._node -> _next));
         }
      traversal = begin();
      --upperBound;
      }
   }

private:
   DListNode<T>*  _head;  // = dummy node if list is empty

   // [OPTIONAL TODO] helper functions; called by public member functions
   void swap(DListNode<T>* a,DListNode<T>* b) const {
      T temp = a -> _data;
      a -> _data = b -> _data;
      b -> _data = temp;
   }
};

#endif // DLIST_H
