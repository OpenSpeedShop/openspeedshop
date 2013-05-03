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



// modified 5/4/94 by P Shirley to make NCCable
///////////////////////////////////////////////////
////////////////                  ///////////////// 
////////////////     ggBST.h      /////////////////
////////////////                  ///////////////// 
///////////////////////////////////////////////////

#ifndef GGBST_H
#define GGBST_H

#include <iostream.h>
#include <ggString.h>
#include <ggTrain.h>


template <class T> class ggBST;
template <class T> class ggBSTNode;

/////////////////////////////////
///////      ggBST       ////////
///////   declarations   //////// 
/////////////////////////////////

template <class T>
class ggBST {


public:

  ggBST();

  ~ggBST();

  ggBoolean empty() const { return (numItems == 0); }

  int numberOfItems() const { return numItems; }

  ggBoolean Insert(const ggString& Key, T *Data) 
        { return InsertIntoSubtree(Key, Data, root); }

  ggBoolean Delete(const ggString&);

  void fillTrain(ggTrain<T*>& ) const;

  T* find(const ggString&);

// private:

// Recursive functions
  void DestroySubtree(ggBSTNode<T>* Root);

  ggBoolean InsertIntoSubtree(const ggString&, T*, ggBSTNode<T>*&);

  ggBSTNode<T>* root;
  int numItems;

};

/////////////////////////////////
///////    ggBSTNode     ////////
///////   declarations   //////// 
/////////////////////////////////

template <class T>
class ggBSTNode {
public:

  ggBSTNode(const ggString&, T&);
  ~ggBSTNode() {}

  T* getData() const { return data; }
  ggString getKey() const { return key; }

  void SetKey(const ggString& Key) { key = Key; }
  void SetData(T& Data) { *data = Data; }

  void fillTrain(ggTrain<T*>& ) const;

  ggBSTNode<T>* left;
  ggBSTNode<T>* right;
  ggString key;
  T* data;
};
//#if defined( _WIN32 )
#include "ggBST.cc"
//#endif

#endif
