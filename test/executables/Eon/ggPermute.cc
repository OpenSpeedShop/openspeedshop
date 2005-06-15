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



#include <stdlib.h>
#include <iostream.h>
#include <ggRanNum.h>


// permute code for array of T.  Needs to be rewritten in terms of rand ints

template <class T>
void ggPermute(T* a, int n)
{
    int i, j;
    T temp;

    for (i = n-1; i > 0; i--)
    {
       ggRanInteger<int> rl(0, i);
       j = rl();
       temp = a[i];
       a[i] = a[j];
       a[j] = temp;
    }
}
