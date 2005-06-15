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



/******************************************************************************
* ggRanInteger
*/

/*
 * Constructors
 */

template <class T> ggRanInteger<T>::ggRanInteger() {
    *this = ggRanInteger<T>(0, 1); }
template <class T> ggRanInteger<T>::ggRanInteger(T high) {
    *this = ggRanInteger<T>(0, high); }
template <class T> ggRanInteger<T>::ggRanInteger(T lo, T hi) {
    if (lo >= hi) {
        cerr << "ggRanInteger(): invalid range." << endl;
        exit(1);
    }
    low = lo;
    range = hi - lo + 1;
}



/******************************************************************************
* ggRanReal
*/

/*
 * Constructors
 */

template <class T> ggRanReal<T>::ggRanReal() {
    *this = ggRanReal<T>(0, 1); }
template <class T> ggRanReal<T>::ggRanReal(T high) {
    *this = ggRanReal<T>(0, high); }
template <class T> ggRanReal<T>::ggRanReal(T lo, T hi) {
    if (lo >= hi) {
        cerr << "ggRanReal(): invalid range." << endl;
        exit(1);
    }
    low = lo;
    factor = (hi - lo)/RAND_MAX;
}
