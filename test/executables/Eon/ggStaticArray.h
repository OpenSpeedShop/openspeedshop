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
// ggStaticArray.h-- declarations for class ggStaticArray
//
// Author:    Pete Shirley
//            10/9/93
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

#ifndef GGSTATICARRAY
#define GGSTATICARRAY


#include <ggBoolean.h>
#include <stdlib.h>
#include <assert.h>


// a ggStaticArray stores data in a random access structure with a
// static size.  The array can be used as either 1, 2, or 3 dimensional.
// the dimensionality is set at construction time and fetches are
// assumed to have the right dimensionality.
// The array does NOT grow on demand, but can be resized.


template <class T>
class ggStaticArray { 
public:
    ggStaticArray() { arraySize = 0;  data = 0;};
    int length() const { return arraySize; }
    ggBoolean empty() const { return length() == 0; }
    ggStaticArray(int);
    ggStaticArray(int, int);
    ggStaticArray(int, int, int);
    ~ggStaticArray();
    ggStaticArray(const ggStaticArray<T>&);
    ggStaticArray<T>& operator=(const ggStaticArray<T>&);
    ggBoolean Resize(int newSize);
    ggBoolean Resize(int newx, int newy);
    ggBoolean Resize(int newx, int newy, int newz);


    T operator() (int i) const { assert(i >= 0 && i < length()); 
                                    return data[i]; }
    T& operator() (int i)      { assert(i >= 0 && i < length()); 
                                    return data[i]; }
    T operator[] (int i) const { assert(i >= 0 && i < length()); 
                                    return data[i]; }
    T& operator[] (int i)      { assert(i >= 0 && i < length()); 
                                    return data[i]; }
 // 2-d arrays
    T operator() (int i, int j) const { assert (i >= 0 && j >= 0 );
                                        assert (i < nx && j < ny);
                                        return data[i*ny + j]; }
    T& operator() (int i, int j)   {    assert (i >= 0 && j >= 0 );
                                        assert (i < nx && j < ny);
                                        return data[i*ny + j]; }
 // 3-d arrays
    T operator() (int i, int j, int k) const {
                                      assert (i >= 0 && j >= 0 && k >= 0 );
                                      assert (i < nx && j < ny && k < nz);
                                      return data[(j+i*ny)*nz + k]; }
    T& operator() (int i, int j, int k)      { 
                                      assert (i >= 0 && j >= 0 && k >= 0 );
                                      assert (i < nx && j < ny && k < nz);
                                      return data[(j+i*ny)*nz + k]; }

#if 0
#    ifdef GGSAFE 
    T operator() (int i) const { assert(i >= 0 && i < length()); 
                                    return data[i]; }
    T& operator() (int i)      { assert(i >= 0 && i < length()); 
                                    return data[i]; }
    T operator[] (int i) const { assert(i >= 0 && i < length()); 
                                    return data[i]; }
    T& operator[] (int i)      { assert(i >= 0 && i < length()); 
                                    return data[i]; }
 // 2-d arrays
    T operator() (int i, int j) const { assert (i >= 0 && j >= 0 );
                                        assert (i < nx && j < ny);
                                        return data[i*ny + j]; }
    T& operator() (int i, int j)   {    assert (i >= 0 && j >= 0 );
                                        assert (i < nx && j < ny);
                                        return data[i*ny + j]; }
 // 3-d arrays
    T operator() (int i, int j, int k) const {
                                      assert (i >= 0 && j >= 0 && k >= 0 );
                                      assert (i < nx && j < ny && k < nz);
                                      return data[(j+i*ny)*nz + k]; }
    T& operator() (int i, int j, int k)      { 
                                      assert (i >= 0 && j >= 0 && k >= 0 );
                                      assert (i < nx && j < ny && k < nz);
                                      return data[(j+i*ny)*nz + k]; }
#    else
 // 1-d arrays
    T operator[] (int i) const { return data[i]; }
    T& operator[] (int i)      { return data[i]; }
    T operator() (int i) const { return data[i]; }
    T& operator() (int i)      { return data[i]; }
 // 2-d arrays
    T operator() (int i, int j) const { return data[i*ny + j]; }
    T& operator() (int i, int j)      { return data[i*ny + j]; }
 // 3-d arrays
    T operator() (int i, int j, int k) const { return data[(j+i*ny)*nz + k]; }
    T& operator() (int i, int j, int k)      { return data[(j+i*ny)*nz + k]; }
#    endif
#endif

private:
    T *data;
    int arraySize;
    int nx, ny, nz;
};

//#if defined( _WIN32 )
#include "ggStaticArray.cc"
//#endif
#endif
