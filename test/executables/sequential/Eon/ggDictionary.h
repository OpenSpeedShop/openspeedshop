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
////////////////  ggDictionary.h  /////////////////
////////////////                  ///////////////// 
///////////////////////////////////////////////////

#ifndef GGDICTIONARY_H
#define GGDICTIONARY_H

#include <iostream.h>
#include <ggString.h>
#include <ggBST.h>
#include <ggTrain.h>


template <class T> class ggBST;

template <class T>
class ggDictionary {

public:

// sam sam sam 10/26/94 this line was originally the last line in class; 
//             g++ does not like that 
  ggBST<T> bst;

  int numberOfEntries() const {return bst.numberOfItems(); }

  ggBoolean Add(const ggString& Key, T* Data) {
     return bst.Insert(Key, Data);
  }

  ggBoolean Delete(const ggString& key) {
      return bst.Delete(key);
  }

  T* lookUp(const ggString& Key) { return bst.find(Key); }

  void fillTrain(ggTrain<T*>& train) const { bst.fillTrain(train);}

};

#endif

