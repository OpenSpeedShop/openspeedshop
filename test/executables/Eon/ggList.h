//////////////////////////////// -*- C++ -*- //////////////////////////////
//
// AUTHOR
//    Peter Shirley, Cornell University, shirley@graphics.cornell.edu 
//
// COPYRIGHT
//    Copyright (c) 1995  Peter Shirley. All rights reserved.
//
//    Permission to use, copy, modify, and distribute this software for any
//    purpose without fee is hereby granted, provided that this entire
//    notice is included in all copies of any software which is or includes
//    a copy or modification of this software and in all copies of the
//    a copy or modification of this software and in all copies of the
//    supporting documentation for such software.
//
// DISCLAIMER
//    Neither the Peter Shirley nor Cornell nor any of their
//    employees, makes any warranty, express or implied, or assumes any
//    liability or responsibility for the accuracy, completeness, or
//    usefulness of any information, apparatus, product, or process
//    disclosed, or represents that its use would not infringe
//    privately-owned rights.  
//
///////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////
////////////////                  ///////////////// 
////////////////     ggList.h     /////////////////
////////////////                  ///////////////// 
///////////////////////////////////////////////////

#ifndef GGLIST_H
#define GGLIST_H

#include <iostream.h>
#include <ggBoolean.h>
#include <assert.h>

template <class T> class ggList;
template <class T> class ggListNode;
template <class T> class ggListIterator;


/////////////////////////////////
///////      ggList      ////////
///////   declarations   //////// 
/////////////////////////////////

// here for NCC.  -- Ken 5/3/94
template <class T>
void operator >> (T object, ggList<T>& l);

template <class T> 
class ggList {

friend class ggListIterator<T>;
friend class ggListNode<T>; 
friend ostream& operator << (ostream& os, ggList<T>& l);
friend istream& operator >> (istream& is, ggList<T>& l);
friend void operator >> (T object, ggList<T>& l);

public:

//
// CONSTRUCTORS
//

  ggList() 
  { head = tail = NULL;
    listLength = iterCount = 0; }

  ggList(ggList<T>*); 

  ggList(const ggList<T>&);

//
// DESTRUCTORS
//

  ~ggList() 
  { ReleaseMemory(); }
  
//
// OPERATORS
//

  ggList<T> operator+(ggList<T>&);

  ggList<T>& operator+=(ggList<T>&);

  ggList<T>& operator=(const ggList<T>&);

  ggBoolean operator==(ggList<T>&);

  ggBoolean operator!=(ggList<T>& rhs) 
  { return (ggBoolean) !(*this == rhs); }

//
// FUNCTIONS
//

  int length() const 
  { return listLength; }

  int iteratorCount() const 
  { return iterCount; }

  ggBoolean empty() const 
  { return (ggBoolean) (head == NULL); }

  ggBoolean Prepend(const T&);  

  ggBoolean Append(const T&);  

  ggBoolean RemoveFirst(const T&);  

  ggBoolean RemoveAll(const T&);  

  void print(ostream&);

  
private:

  ggBoolean Append(ggListNode<T>*);
  
  ggBoolean Prepend(ggListNode<T>*);

  void ReleaseMemory();

//
// VARIABLES
//

  ggListNode<T> *head, *tail; 

  int listLength;

  int iterCount;
};

/////////////////////////////////
///////    ggListNode    ////////
///////   declarations   //////// 
/////////////////////////////////

template <class T>
class ggListNode {

// All ostream related operators have to be friends
  friend ostream& operator << (ostream&, ggListNode<T>&);

  friend class ggList<T>;
  friend class ggListIterator<T>;

private:

//
// CONSTRUCTORS
//

// Two constructors were created so that the option of passing either 
// objects or pointers could be used in creating the next and previous 
// pointers for the newly created Node.

  ggListNode(ggListNode<T>* oldNode,
	     ggListNode<T>* beforeIt,
	     ggListNode<T>* afterIt);

  ggListNode(const T&  dataField, 
	     ggListNode<T>* beforeIt, 
	     ggListNode<T>* afterIt);

  ggListNode(const T&  dataField,
	     ggListNode<T>& beforeIt,
	     ggListNode<T>& afterIt);


//
// DESTRUCTORS
//

  ~ggListNode() {}


//
// OPERATORS
//

  ggListNode<T>& operator=(ggListNode<T>&);
  ggBoolean operator==(ggListNode<T>& rhs) { 
    return ( (&data == &(rhs.data)) && (deleteMe == rhs.deleteMe) );
  }

//
// FUNCTIONS
//

// This function is used to remove an item from the list, once no more iterators
// are pointing to it. It rearranges all the pointers for the surrounding nodes
// and then deletes itself

  void print(ostream&);

  ggBoolean RemoveFromList(ggList<T>*);

  T getData()  { return data; }


//
// VARIABLES
//

  T data; 
  ggListNode<T> *next, *previous;
  ggBoolean deleteMe;  // This is to mark if a node is to be deleted, once
                       // all iterators have left this node

  int iterCount;  // This keeps track of how many iterators are pointing to
                  // this node  
};


/////////////////////////////////
///////  ggListIterator  ////////
///////   declarations   //////// 
/////////////////////////////////

template <class T> 
class ggListIterator {

  friend class ggList<T>;
  friend class ggListNode<T>;
  friend ostream& operator << ( ostream&, ggList<T>&);
  friend ostream& operator << ( ostream&, ggListNode<T>&);

public:

//
// CONSTRUCTORS
//

  ggListIterator(ggList<T>*); 

  ggListIterator(ggList<T>&);

//
// DESTRUCTORS
//

  ~ggListIterator(); 

//
// FUNCTIONS
//

  T item() const;

  ggList<T>* getList() const {return list;}

  ggBoolean find(const T&, ggListNode<T>* startingPoint);

  ggBoolean Find(const T&, ggListNode<T>* startingPoint);

  ggBoolean Next();

  ggBoolean Previous();

  void SetToHead();

  void SetToTail();

  ggBoolean Delete();

private:

  void SetCurrent(ggListNode<T>*);
  
  ggListIterator(){}       // Cannot construct an iterator with no args
  
//
// VARIABLES
//

  ggList<T>* list;

  ggListNode<T>* current;
};

// Here for NCC, remove if you get multiply defined stuff. -- Ken 5/3/94
//#if defined(_WIN_32)
//#include <ggList.cpp>
//#endif
#endif
