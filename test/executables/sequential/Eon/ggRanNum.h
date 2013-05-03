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



#ifndef GGRANNUM_H
#define GGRANNUM_H

#include <stdlib.h>
#include <time.h>

#include <myrand.h>

#ifdef RAND_MAX
#undef RAND_MAX
#endif

#define RAND_MAX 32767

/******************************************************************************
* ggRanNumP
*/

class ggRanNumP {

public:
    
    static void Seed(unsigned int seed);
    static void Randomize();
};



/******************************************************************************
* ggRanInteger
*/

template <class T>
class ggRanInteger : public ggRanNumP {
    
public:

    ggRanInteger();                         // integers in the range [0, 1]
    ggRanInteger(T high);                   // integers in the range [0, high]
    ggRanInteger(T low, T high);            // integers in the range [low, high]

    inline T operator()();
    inline T operator()(T offset);

private:
    
    T low, range;
};

/*
 * Operators
 */

template <class T> inline T ggRanInteger<T>::operator()() {
    return (T) (long(low) + my_rand() % long(range));
}

template <class T> inline T ggRanInteger<T>::operator()(T offset) {
    return (T) (long(offset) + long(low) + my_rand() % long(range));
}



/******************************************************************************
* ggRanReal
*/

template <class T>
class ggRanReal : public ggRanNumP {
    
public:

    ggRanReal();                            // reals in the range [0, 1)
    ggRanReal(T high);                      // reals in the range [0, high)
    ggRanReal(T low, T high);               // reals in the range [low, high)

    inline T operator()();
    inline T operator()(T offset);

private:
    
    T low, factor;
};

/*
 * Operators
 */

template <class T> inline T ggRanReal<T>::operator()() {
    return my_rand() * factor + low; }
template <class T> inline T ggRanReal<T>::operator()(T offset) {
    return my_rand() * factor + low + offset; }

// never returns 1.0 -- [0.0,1.0)
inline double ggCanonicalRandom() {
   return my_rand() * (0.9999999 / double(RAND_MAX));
}

inline void ggReseed() {
   my_srand((unsigned int) time(0));
}

inline double ggRandomDouble(double a, double b) {
   return a + my_rand() * (b - a) *  (0.9999999 / double(RAND_MAX));
}

// returns an int in [a,b] inclusive
inline int ggRandomInteger(int a, int b) {
   return a + int((b - a + 1) * ggCanonicalRandom());
}

//#if defined( _WIN32 )
#include "ggRanNum.cc"
//#endif
#include <ggPermute.h>

#endif
