/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>
using namespace std;
static const size_t MASK = -2;
template <class T> class BSTree;
// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;
   BSTreeNode(const T& d, size_t l = 0, size_t r = 0):
      _data(d), _left(l), _right(r) {}
   BSTreeNode<T>* getLeftPtr ()    { return (BSTreeNode<T>*)(_left  & MASK); }
   BSTreeNode<T>* getRightPtr ()   { return (BSTreeNode<T>*)(_right & MASK); }
   BSTreeNode<T>* getLeftChild ()  { return (hasLeftChild()  ? 
      (BSTreeNode<T>*)(_left) : 0);  }
   BSTreeNode<T>* getRightChild () { return (hasRightChild() ? (BSTreeNode<T>*)(_right) : 0); }
   void setLeftChild  ( BSTreeNode<T>* l ) {  _left = (size_t)l; }
   void setRightChild ( BSTreeNode<T>* r ) { _right = (size_t)r; }
   void setLeftThread  ( BSTreeNode<T>* l ) {  _left = (size_t)l; setLeftFlag(true); }
   void setRightThread ( BSTreeNode<T>* r ) { _right = (size_t)r; setRightFlag(true);}
   bool hasLeftChild ()  { return !(_left & 1);  }
   bool hasRightChild () { return !(_right & 1); }
   void setLeftFlag (bool l)  { _left  |= l; }   // if l = true, set thread flag
   void setRightFlag (bool r) { _right |= r; }   // if r = true, set thread flag
   const T& getData () {return _data; }
   void setData(T d) { _data = d; }
   bool isLeaf() { return (!hasLeftChild()) && (!hasRightChild()); }
   T        _data;
   size_t   _left;
   size_t   _right;
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
public:
   BSTree() {
      _head = new BSTreeNode<T>(T("dummyz"));
      _head ->setLeftThread(_head);
      _head ->setRightThread(_head);
   }
   ~BSTree() { clear(); delete _head; }

   class iterator { 
      friend class BSTree;

   public:

      iterator(BSTreeNode<T>* node): _node(node) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {}
      
      const T& operator * () const {
        return _node -> _data;
      }
      T& operator * () { return _node -> _data; }
      iterator& operator ++ () {
         if(!_node -> hasRightChild()) _node = _node -> getRightPtr();
         else {
            _node = _node -> getRightPtr();
            while(_node -> hasLeftChild()) {
               _node = _node -> getLeftPtr();
            }
         }
         return *(this);
      }
      iterator operator ++ (int) {
         iterator rev = *(this);
         ++(*this);
         return rev;
      } 
      iterator& operator -- () {
         if(!_node -> hasLeftChild()) _node = _node -> getLeftPtr();
         else {
            _node = _node -> getLeftPtr();
            while(_node -> hasRightChild()) {
               _node = _node -> getRightPtr();
            }
         }
         return *(this);
      }
      iterator operator -- (int) {
         iterator rev = *(this);
         --(*this);
         return rev;
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
      BSTreeNode<T>*   _node;
      //iterator parent() {
      //   if(this -> _node -> isLeaf()) { 
      //      if(this -> _node -> isLeft)
      //   }
      //}
   };

   iterator begin() const {
      BSTreeNode<T>* b = _head;
      if(empty()) return end();
      else {
         b  = b -> getLeftPtr();
         while(b -> hasLeftChild()) { b = b -> getLeftPtr();}
      }
      return iterator(b);
   }
   iterator end() const {
      return iterator(_head);
   }
   void pop_front() {
   }
   void pop_back() {
   }
   bool erase(iterator pos) {
      if(pos._node == _head) return false;
      iterator it = pos;
      BSTreeNode<T>* curr = it._node;
      iterator itit = it;
      BSTreeNode<T>* prev = (--it)._node;
      BSTreeNode<T>* next = (++itit)._node;
      if(pos._node -> isLeaf()) {
         if( curr -> getRightPtr() -> getLeftPtr() == curr ) {
         next -> setLeftThread(prev); }
         else { prev -> setRightThread(next); }
         delete curr;
      }
    //  else if(pos._node -> hasLeftChild() && !(pos._node -> hasRightChild()) ) {
    //     next -> setLeftChild(curr -> getLeftChild());
    //     prev -> setRightThread(next);
    //     delete curr;
    //  }
    //  else if(pos._node -> hasRightChild() && !(pos._node -> hasLeftChild()) ) {
    //     if(prev != _head) prev -> setRightChild(curr -> getRightChild());
    //     next -> setLeftThread(prev);
    //     delete curr;
    //  }
      else if(pos._node -> hasLeftChild()) {
         pos._node -> setData(prev -> getData());
         erase(iterator(prev));
      }
      else {
         pos._node -> setData(next -> getData());
         erase(iterator(next));
      }
      return true;
   }
   bool erase(const T& x) {
      iterator it(0);
      int era = find(x,it);
      if(era!=0) { return false; }
      return erase(it);   
   }
   
   bool empty() const {
      return(_head -> getLeftPtr() == _head);
   }
   void insert(const T& x) {
      iterator it(0);
      BSTreeNode<T>* newNode = new BSTreeNode<T>(x);
      int dir = find(x,it);
      iterator pos = it;
      iterator pospos = pos;
      BSTreeNode<T>* prev = (--pos)._node;
      BSTreeNode<T>* next = (++pospos)._node;
      if(dir == 0) {
         if (!(it._node -> hasLeftChild())) { dir = -1; }
         else if(!(it._node -> hasRightChild())) { dir = 1; }
         else if(!(prev -> hasRightChild())) { 
            it = prev;
            dir = 1;
         } else if(!(next -> hasLeftChild())) { 
            it = next;
            dir = -1;
         }
      }
      else if(dir == 1) {
         newNode -> setLeftThread(it._node);
         newNode -> setRightThread(it._node -> getRightPtr());
         it._node -> setRightChild(newNode); 
      }else if(dir == -1 ) {
         newNode -> setRightThread(it._node);
         newNode -> setLeftThread(it._node -> getLeftPtr());
         it._node -> setLeftChild(newNode);
      }
   }
   void sort() {}
   void print() {}
   void clear() { 
   }
   size_t size() {
      size_t s = 0;
      for(iterator b = this -> begin(), end = this -> end() ; b != end;++b) 
         ++s;
      return s;
   }
private:
   BSTreeNode<T>*   _head;
   int find(const T& val, iterator& it) {
      // return 0 if data is found, and set 'it' to the node whose data is val
      // else set 'it' to the node that can be inserted to
      // -1 for left, 1 for right
      BSTreeNode<T>* _current = _head;
      int output;
      if(empty()) {
         it = iterator(_head);
         return -1;
      }
      _current = _current -> getLeftPtr();
      while(1) {
         if(val < _current -> getData()) {
            if(!_current -> hasLeftChild()) {
               output = -1; 
               break;
            }
            _current = _current -> getLeftPtr();
         }
         else if(val == _current -> getData()) {
            output = 0; 
            break;
         }
         else {
            if(!_current -> hasRightChild()) {
               output = 1; 
               break;
            }
            _current = _current -> getRightPtr();
         }
      }
      it = iterator(_current);
      return output;
   }



};

#endif // BST_H
