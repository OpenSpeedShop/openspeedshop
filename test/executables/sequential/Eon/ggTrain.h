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




//
// ggTrain.h-- declarations for class ggTrain
//
// Author:    Pete Shirley
//            9/19/93
// Modified:
//
//
// Copyright 1993 by Peter Shirley
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
//

#ifndef GGDATATRAIN
#define GGDATATRAIN

#include <stdlib.h>
#include <assert.h>
#include <ggBoolean.h>
#include <ggRanNum.h>

// a ggTrain stores data in an ordered random access structure with
// no delete items and an append op.



template <class T>
class ggTrain { 
public:
    ggBoolean Append(T item); // always tagged to end
    void Permute() { ggPermute(data, nData); }
    int length() const { return nData; }
    ggBoolean empty() const { return length() == 0; }
    ggTrain();
    ggTrain(int);
    ~ggTrain();
    ggTrain(const ggTrain<T>&);
    ggTrain<T>& operator=(const ggTrain<T>&);
    void Clear() { nData = 0; }

#    ifdef GGSAFE 
       T operator[] (int i) const { assert(i >= 0 && i < length()); 
                                    return data[i]; }
#    else
        T operator[] (int i) const { return data[i]; }
#    endif

private:
    T *data;
    int nData;
    int arraySize;
};

template <class T>
T ggAverage(const ggTrain<T>& t);

// #if defined( _WIN32 )
#include "ggTrain.cc"
// #endif


#endif
