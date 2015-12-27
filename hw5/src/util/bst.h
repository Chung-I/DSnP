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

template <class T> class BSTree;
#define LEFT  1
#define RIGHT  2
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
   BSTreeNode(const T& d, BSTreeNode<T>* l = 0, BSTreeNode<T>* r = 0):
      _data(d), _left(l), _right(r) {}

   T   _data;
   BSTreeNode<T>*   _left;
   BSTreeNode<T>*   _right;
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
public:
   BSTree(): _root(0), _tail(0) {}
   ~BSTree() { clear(); delete _tail; delete _root; }

   class iterator { 
      friend class BSTree;

   public:
      class oneTrace
      {
         friend class iterator;

         oneTrace(BSTreeNode<T>* node, int direc):
            _traceNode(node), _direction(direc) {}

         BSTreeNode<T>* _traceNode;
         int _direction;
      };

      iterator(BSTreeNode<T>* node): _node(node) {}
      iterator(const iterator& i): _node(i._node), _trace(i._trace) {}
      ~iterator() {}
      
      const T& operator * () const {
         T temp = _node -> _data;
         return  temp;
      }
      T& operator * () { return _node -> _data; }
      iterator& operator ++ () {
         iterator temp = *(this);
         if(_node -> _right == NULL) {
            while(1) {
               int direc = backToParent();
               if(direc == LEFT)  return *(this); 
               if(direc == RIGHT)   continue;
               if(direc == 0)   *(this) = temp;   this -> moveToChild(RIGHT);   return *(this);
            }
         }
         this -> moveToChild(RIGHT);
         this -> toMin();
         return *(this);
      }
      iterator operator ++ (int) {
         iterator temp = *(this);
         ++(*this);
         return temp;
      } 
      iterator& operator -- () {
         iterator temp = *(this);
         if(_node -> _left == NULL) {
            while(1) {
               int direc = backToParent();
               if(direc == LEFT)  continue; 
               if(direc == RIGHT)   return *(this);
               if(direc == 0)   *(this) = temp; return *(this);
            }
         }
         this -> moveToChild(LEFT);
         this -> toMax();
         return *(this);
      }
      iterator operator -- (int) {
         iterator temp = *(this);
         --(*this);
         return temp;
      }
      iterator& operator = (const iterator& i) {
        _node = i._node;
        _trace = i._trace; 
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
      void moveToChild (int direc) {
         if(direc == LEFT) { 
               _trace.push_back(new oneTrace(_node,LEFT));
               _node = _node -> _left;
               return ;
         }
         else if(direc == RIGHT) {
               _trace.push_back(new oneTrace(_node,RIGHT));
               _node = _node -> _right;
               return ;
         }
      }
      BSTreeNode<T>*& getChild (int direc) {
         if(direc == LEFT) return _node -> _left;
         if(direc == RIGHT) return _node -> _right;
         return _node;
      }
      int backToParent () { //return 0 if no parent(root); return 1 if left child itself; return 2 if right child itself
         oneTrace* temp;
         temp = pop_trace();
         if(temp == 0 ) return 0;
         _node = temp -> _traceNode;
         return temp -> _direction;
      }
      BSTreeNode<T>* getParent() const {
         if(_trace.empty()) return NULL;
         return _trace.back() -> _traceNode;
      }
      int countChild() const {
         if(_node -> _left == NULL && _node -> _right == NULL)   return 0;
         if(_node -> _left != NULL && _node -> _right == NULL)   return LEFT;
         if(_node -> _left == NULL && _node -> _right != NULL)   return RIGHT;
         if(_node -> _left != NULL && _node -> _right != NULL)   return 3;
         return 0; 
         
      }
   private: 
      BSTreeNode<T>*   _node;
      void toMin () {
         while(getChild(LEFT) != NULL) moveToChild(LEFT);
      }
      void toMax () {
         while(getChild(RIGHT) != NULL)   moveToChild(RIGHT);
      }
      vector<oneTrace*> _trace;
      oneTrace* pop_trace() { 
         if(_trace.empty() ) return NULL;  
         oneTrace* temp = _trace.back(); 
         _trace.pop_back();
         return temp;
      }
   };

   iterator begin() const {
      if(_root == NULL)   return end();
      iterator findBegin(_root);
      while(findBegin.getChild(LEFT) != NULL) { findBegin.moveToChild(LEFT); }
      return findBegin;
   }
   iterator end() const {
      if(_root == NULL)   return iterator(_root);
      iterator findEnd(_root);
      while(findEnd.getChild(RIGHT) != NULL) { findEnd.moveToChild(RIGHT); }
      return findEnd;
   }
   void pop_front() {
      erase(begin());
   }
   void pop_back() {
      erase(--end());
   }
   bool erase(iterator pos) {
      size_t size = this -> size();
      if(empty()) {return false; }
         if(_tail != NULL) {
         iterator cleanTail(_root);
         cleanTail.toMax();
         int direc = cleanTail.backToParent();
         delete _tail;
         cleanTail.getChild(direc) = 0;
         _tail = 0;
      }
      if(size > 1) {
         if(!traverse(pos))   return false;
         eraseNode(pos);
      }
      else {
         if(pos._node == _root) { 
            delete _root;   
            _root = 0;
         }
         else { return false; }
      }

      if(!empty()) {
         iterator addTail(_root);
         addTail.toMax();
         _tail = new BSTreeNode<T>(T("DUMMY!"),NULL,NULL);
         addTail._node -> _right = _tail;
      }
      return true;
   }
   bool erase(const T& x) {
      if(_tail != NULL) {
         iterator cleanTail(_root);
         cleanTail.toMax();
         int direc = cleanTail.backToParent();
         cleanTail.getChild(direc) = 0;
         delete _tail;
         _tail = 0;
      }

      if(empty()) { return false; }
      if(size() > 1) {
         iterator eraser(_root);
         traverse(x, eraser);
         if(eraser._node == NULL)   return false;
         eraseNode(eraser);
      }
      else {
      if(x == _root -> _data) { delete _root; _root = 0; }
      else { return false; }
      }
      if(_root != NULL) {
         iterator addTail(_root);
         addTail.toMax();
         _tail = new BSTreeNode<T>(T("DUMMY!"),NULL,NULL);
         addTail._node -> _right = _tail;
      }
      return true;
   }
   
   bool empty() const {
      if (_root == NULL) return true;
      return false;      
   }
   void insert(const T& x) {
      if(_tail != NULL) {
         iterator cleanTail(_root);
         cleanTail.toMax();
         int direc = cleanTail.backToParent();
         delete _tail;
         cleanTail.getChild(direc) = 0;
         _tail = 0;
      }
      BSTreeNode<T>* newNode = new BSTreeNode<T>(x,NULL,NULL);
      if(_root == NULL) {
         _root = newNode; 
      }
      else {
         iterator toInsert(_root);
         traverse(x, toInsert);
         if(toInsert._node == NULL) {
            int direc = toInsert.backToParent();

             if(direc == LEFT) toInsert._node -> _left = newNode;
            else if(direc == RIGHT) toInsert._node -> _right = newNode;
         }
         else {
            if(toInsert.getChild(LEFT) == NULL) { toInsert._node -> _left = newNode; }
            else if(toInsert.getChild(RIGHT) == NULL) { toInsert._node -> _right = newNode; }
            toInsert++;
            if(toInsert.getChild(LEFT) == NULL) { toInsert._node -> _left = newNode; }
            else if(toInsert.getChild(RIGHT) == NULL) { toInsert._node -> _right = newNode; }
         }
      }
      iterator addTail(_root);
      addTail.toMax();
      _tail = new BSTreeNode<T>(T("DUMMY!"),NULL,NULL);
      addTail._node -> _right = _tail;
   }
   void sort() {}
   void print() {}
   void clear() { 
      if(!empty()) {
         clearNode(_root); 
         _root = 0; 
         _tail = 0; 
      }
   }
   size_t size() {
      size_t count = 0;
      countSize(_root,count); 
      --count;
      return count;
   }
private:
   BSTreeNode<T>*   _root;
   BSTreeNode<T>*   _tail;
   const BSTreeNode<T>* getRoot() const {
      return _root;
   }
   void traverse (T x, iterator& insert) {
      if(insert._node == NULL || x == *insert) return ;
      if(x < *insert) {
         insert.moveToChild(LEFT);
         traverse(x, insert);
      }
      else {
         insert.moveToChild(RIGHT);
         traverse(x, insert);
      }
   }
   bool traverse(iterator& target) {
      iterator traversal(_root);
      while(traversal._node != NULL) {
         if(*target < *traversal) {
            traversal.moveToChild(LEFT);
         }
         else if(!( (*target < *traversal) || (*target == *traversal)) ) {
            traversal.moveToChild(RIGHT);
         }
         else if(*target == *traversal) {
            traversal.toMin();
            while(!(*target == *traversal))   ++traversal;
            while(*target == *traversal && target != traversal)   ++traversal;
            if(*target == *traversal && target == traversal)   return true;
            return false;
         } 
      }
      return false;
   }

   void eraseNode(iterator& eraser) {
      int child = eraser.countChild();
      if(child == 0) {
         delete eraser._node;
         eraser._node = 0;
         int direc = eraser.backToParent();
         if(direc)   eraser.getChild(direc) = 0;
      }
      else if(child == LEFT || child == RIGHT) {
         iterator deleteNode = eraser;
         eraser.moveToChild(child);
         if(child == LEFT)   eraser.toMax();
         else if(child == RIGHT)   eraser.toMin();
         *deleteNode = *eraser;
         eraseNode(eraser);
      }
      else if(child == 3) {
         iterator deleteNode = eraser; 
         ++eraser; //go to successor
         if(eraser.countChild() == 0) {
            *deleteNode = *eraser;
            delete eraser._node;
            int direc = eraser.backToParent();
            eraser.getChild(direc) = 0;
         }
         else if(eraser.countChild() == RIGHT) {
            *deleteNode = *eraser;
            eraseNode(eraser);
         }         
      }     
   }          
   void clearNode(BSTreeNode<T>* deleteNode) {
      if(deleteNode -> _left != NULL )   clearNode(deleteNode -> _left);
      if(deleteNode -> _right != NULL )   clearNode(deleteNode -> _right);
      delete deleteNode;
      deleteNode = 0;
   }
   void countSize(BSTreeNode<T>* countNode, size_t& size) {
      if(countNode -> _left != NULL )   countSize(countNode -> _left,size);
      if(countNode -> _right != NULL)   countSize(countNode -> _right, size);
      size++;
   }
};

#endif // BST_H
