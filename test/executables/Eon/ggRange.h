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



/* $Id$ */



#ifndef GGRANGE_H
#define GGRANGE_H



#include <iostream.h>
#include <ggBoolean.h>



/******************************************************************************
* Declarations
*/

template <class T>
class ggRange {

public:

    ggRange() {}
    ggRange(T x0, T x1) { xx0 = x0; xx1 = x1; }

    T x0() const { return xx0; }
    T x1() const { return xx1; }
    T &X0() { return xx0; }
    T &X1() { return xx1; }

protected:
    
    T xx0, xx1;
};



/*
 * Non-friends
 */

template <class T1, class T2, class T3>
T3
ggScaleIntegralToIntegral(const T1 x, const ggRange<T2> &domain,
 const ggRange<T3> &range);

template <class T1, class T2, class T3>
T3
ggScaleIntegralToReal(const T1 x, const ggRange<T2> &domain,
 const ggRange<T3> &range);

template <class T1, class T2, class T3>
T3
ggScaleRealToIntegral(const T1 x, const ggRange<T2> &domain,
 const ggRange<T3> &range);

template <class T1, class T2, class T3>
T3
ggScaleRealToReal(const T1 x, const ggRange<T2> &domain,
 const ggRange<T3> &range);

// char to ...

char ggScale(const char x, const ggRange<char> domain,
 const ggRange<char> range);

unsigned char ggScale(const char x, const ggRange<char> domain,
 const ggRange<unsigned char> range);

short ggScale(const char x, const ggRange<char> domain,
 const ggRange<short> range);

int ggScale(const char x, const ggRange<char> domain,
 const ggRange<int> range);

unsigned int ggScale(const char x, const ggRange<char> domain,
 const ggRange<unsigned int> range);

long ggScale(const char x, const ggRange<char> domain,
 const ggRange<long> range);

float ggScale(const char x, const ggRange<char> domain,
 const ggRange<float> range);

double ggScale(const char x, const ggRange<char> domain,
 const ggRange<double> range);

// unsigned char to ...

char ggScale(const unsigned char x, const ggRange<unsigned char> domain,
 const ggRange<char> range);

unsigned char ggScale(const unsigned char x, const ggRange<unsigned char> domain,
 const ggRange<unsigned char> range);

short ggScale(const unsigned char x, const ggRange<unsigned char> domain,
 const ggRange<short> range);

int ggScale(const unsigned char x, const ggRange<unsigned char> domain,
 const ggRange<int> range);

unsigned int ggScale(const unsigned char x, const ggRange<unsigned char> domain,
 const ggRange<unsigned int> range);

long ggScale(const unsigned char x, const ggRange<unsigned char> domain,
 const ggRange<long> range);

float ggScale(const unsigned char x, const ggRange<unsigned char> domain,
 const ggRange<float> range);

double ggScale(const unsigned char x, const ggRange<unsigned char> domain,
 const ggRange<double> range);

// short to ...

char ggScale(const short x, const ggRange<short> domain,
 const ggRange<char> range);

unsigned char ggScale(const short x, const ggRange<short> domain,
 const ggRange<unsigned char> range);

short ggScale(const short x, const ggRange<short> domain,
 const ggRange<short> range);

int ggScale(const short x, const ggRange<short> domain,
 const ggRange<int> range);

unsigned int ggScale(const short x, const ggRange<short> domain,
 const ggRange<unsigned int> range);

long ggScale(const short x, const ggRange<short> domain,
 const ggRange<long> range);

float ggScale(const short x, const ggRange<short> domain,
 const ggRange<float> range);

double ggScale(const short x, const ggRange<short> domain,
 const ggRange<double> range);

// unsigned short to ...

char ggScale(unsigned short x,
 const ggRange<unsigned short> domain, const ggRange<char> range);

unsigned char ggScale(unsigned short x,
 const ggRange<unsigned short> domain, const ggRange<unsigned char> range);

unsigned short ggScale(unsigned short x,
 const ggRange<unsigned short> domain, const ggRange<unsigned short> range);

short ggScale(unsigned short x,
 const ggRange<unsigned short> domain, const ggRange<short> range);

int ggScale(unsigned short x,
 const ggRange<unsigned short> domain, const ggRange<int> range);

unsigned int ggScale(unsigned short x,
 const ggRange<unsigned short> domain, const ggRange<unsigned int> range);

long ggScale(unsigned short x,
 const ggRange<unsigned short> domain, const ggRange<long> range);

float ggScale(unsigned short x,
 const ggRange<unsigned short> domain, const ggRange<float> range);

double ggScale(unsigned short x,
 const ggRange<unsigned short> domain, const ggRange<double> range);

// int to ...

char ggScale(const int x, const ggRange<int> domain,
 const ggRange<char> range);

unsigned char ggScale(const int x, const ggRange<int> domain,
 const ggRange<unsigned char> range);

short ggScale(const int x, const ggRange<int> domain,
 const ggRange<short> range);

int ggScale(const int x, const ggRange<int> domain,
 const ggRange<int> range);

unsigned int ggScale(const int x, const ggRange<int> domain,
 const ggRange<unsigned int> range);

long ggScale(const int x, const ggRange<int> domain,
 const ggRange<long> range);

float ggScale(const int x, const ggRange<int> domain,
 const ggRange<float> range);

double ggScale(const int x, const ggRange<int> domain,
 const ggRange<double> range);

// unsigned int to ...

char ggScale(const unsigned int x, const ggRange<unsigned int> domain,
 const ggRange<char> range);

unsigned char ggScale(const unsigned int x, const ggRange<unsigned int> domain,
 const ggRange<unsigned char> range);

short ggScale(const unsigned int x, const ggRange<unsigned int> domain,
 const ggRange<short> range);

int ggScale(const unsigned int x, const ggRange<unsigned int> domain,
 const ggRange<int> range);

unsigned int ggScale(const unsigned int x, const ggRange<unsigned int> domain,
 const ggRange<unsigned int> range);

long ggScale(const unsigned int x, const ggRange<unsigned int> domain,
 const ggRange<long> range);

float ggScale(const unsigned int x, const ggRange<unsigned int> domain,
 const ggRange<float> range);

double ggScale(const unsigned int x, const ggRange<unsigned int> domain,
 const ggRange<double> range);

// long to ...

char ggScale(const long x, const ggRange<long> domain,
 const ggRange<char> range);

unsigned char ggScale(const long x, const ggRange<long> domain,
 const ggRange<unsigned char> range);

short ggScale(const long x, const ggRange<long> domain,
 const ggRange<short> range);

unsigned short ggScale(const long x, const ggRange<long> domain,
 const ggRange<unsigned short> range);

int ggScale(const long x, const ggRange<long> domain,
 const ggRange<int> range);

unsigned int ggScale(const long x, const ggRange<long> domain,
 const ggRange<unsigned int> range);

long ggScale(const long x, const ggRange<long> domain,
 const ggRange<long> range);

float ggScale(const long x, const ggRange<long> domain,
 const ggRange<float> range);

double ggScale(const long x, const ggRange<long> domain,
 const ggRange<double> range);

// float to ...

char ggScale(const float x, const ggRange<float> domain,
 const ggRange<char> range);

unsigned char ggScale(const float x, const ggRange<float> domain,
 const ggRange<unsigned char> range);

short ggScale(const float x, const ggRange<float> domain,
 const ggRange<short> range);

int ggScale(const float x, const ggRange<float> domain,
 const ggRange<int> range);

unsigned int ggScale(const float x, const ggRange<float> domain,
 const ggRange<unsigned int> range);

long ggScale(const float x, const ggRange<float> domain,
 const ggRange<long> range);

float ggScale(const float x, const ggRange<float> domain,
 const ggRange<float> range);

double ggScale(const float x, const ggRange<float> domain,
 const ggRange<double> range);

// double to ...

char ggScale(const double x, const ggRange<double> domain,
 const ggRange<char> range);

unsigned char ggScale(const double x, const ggRange<double> domain,
 const ggRange<unsigned char> range);

short ggScale(const double x, const ggRange<double> domain,
 const ggRange<short> range);

unsigned short ggScale(const double x, const ggRange<double> domain,
 const ggRange<unsigned short> range);

int ggScale(const double x, const ggRange<double> domain,
 const ggRange<int> range);

unsigned int ggScale(const double x, const ggRange<double> domain,
 const ggRange<unsigned int> range);

long ggScale(const double x, const ggRange<double> domain,
 const ggRange<long> range);

float ggScale(const double x, const ggRange<double> domain,
 const ggRange<float> range);

double ggScale(const double x, const ggRange<double> domain,
 const ggRange<double> range);

template <class T>
ostream &
operator<<(ostream &os, const ggRange<T> &r);



/******************************************************************************
* Inlines
*/



/*
 * Non-friends
 */

// char to ...

inline char
ggScale(const char x, const ggRange<char> domain,
 const ggRange<char> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline unsigned char
ggScale(const char x, const ggRange<char> domain,
 const ggRange<unsigned char> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline short
ggScale(const char x, const ggRange<char> domain,
 const ggRange<short> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline int
ggScale(const char x, const ggRange<char> domain,
 const ggRange<int> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline long
ggScale(const char x, const ggRange<char> domain,
 const ggRange<long> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline float
ggScale(const char x, const ggRange<char> domain,
 const ggRange<float> range) {
    return ggScaleIntegralToReal(x, domain, range);
}

inline double
ggScale(const char x, const ggRange<char> domain,
 const ggRange<double> range) {
    return ggScaleIntegralToReal(x, domain, range);
}

// unsigned char to ...

inline char
ggScale(const unsigned char x, const ggRange<unsigned char> domain,
 const ggRange<char> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline unsigned char
ggScale(const unsigned char x, const ggRange<unsigned char> domain,
 const ggRange<unsigned char> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline short
ggScale(const unsigned char x, const ggRange<unsigned char> domain,
 const ggRange<short> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline int
ggScale(const unsigned char x, const ggRange<unsigned char> domain,
 const ggRange<int> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline long
ggScale(const unsigned char x, const ggRange<unsigned char> domain,
 const ggRange<long> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline float
ggScale(const unsigned char x, const ggRange<unsigned char> domain,
 const ggRange<float> range) {
    return ggScaleIntegralToReal(x, domain, range);
}

inline double
ggScale(const unsigned char x, const ggRange<unsigned char> domain,
 const ggRange<double> range) {
    return ggScaleIntegralToReal(x, domain, range);
}

// short to ...

inline char
ggScale(const short x, const ggRange<short> domain,
 const ggRange<char> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline unsigned char
ggScale(const short x, const ggRange<short> domain,
 const ggRange<unsigned char> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline short
ggScale(const short x, const ggRange<short> domain,
 const ggRange<short> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline int
ggScale(const short x, const ggRange<short> domain,
 const ggRange<int> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline long
ggScale(const short x, const ggRange<short> domain,
 const ggRange<long> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline float
ggScale(const short x, const ggRange<short> domain,
 const ggRange<float> range) {
    return ggScaleIntegralToReal(x, domain, range);
}

inline double
ggScale(const short x, const ggRange<short> domain,
 const ggRange<double> range) {
    return ggScaleIntegralToReal(x, domain, range);
}

// unsigned short to ...

inline char
ggScale(unsigned short x, const ggRange<unsigned short> domain,
 const ggRange<char> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline unsigned char
ggScale(unsigned short x, const ggRange<unsigned short> domain,
 const ggRange<unsigned char> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline short
ggScale(unsigned short x, const ggRange<unsigned short> domain,
 const ggRange<short> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline unsigned short
ggScale(unsigned short x, const ggRange<unsigned short> domain,
 const ggRange<unsigned short> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline int
ggScale(unsigned short x, const ggRange<unsigned short> domain,
 const ggRange<int> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline long
ggScale(unsigned short x, const ggRange<unsigned short> domain,
 const ggRange<long> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline float
ggScale(unsigned short x, const ggRange<unsigned short> domain,
 const ggRange<float> range) {
    return ggScaleIntegralToReal(x, domain, range);
}

inline double
ggScale(unsigned short x, const ggRange<unsigned short> domain,
 const ggRange<double> range) {
    return ggScaleIntegralToReal(x, domain, range);
}

// int to ...

inline char
ggScale(const int x, const ggRange<int> domain,
 const ggRange<char> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline unsigned char
ggScale(const int x, const ggRange<int> domain,
 const ggRange<unsigned char> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline short
ggScale(const int x, const ggRange<int> domain,
 const ggRange<short> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline int
ggScale(const int x, const ggRange<int> domain,
 const ggRange<int> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline long
ggScale(const int x, const ggRange<int> domain,
 const ggRange<long> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline float
ggScale(const int x, const ggRange<int> domain,
 const ggRange<float> range) {
    return ggScaleIntegralToReal(x, domain, range);
}

inline double
ggScale(const int x, const ggRange<int> domain,
 const ggRange<double> range) {
    return ggScaleIntegralToReal(x, domain, range);
}

// long to ...

inline char
ggScale(const long x, const ggRange<long> domain,
 const ggRange<char> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline unsigned char
ggScale(const long x, const ggRange<long> domain,
 const ggRange<unsigned char> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline short
ggScale(const long x, const ggRange<long> domain,
 const ggRange<short> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline unsigned short
ggScale(const long x, const ggRange<long> domain,
 const ggRange<unsigned short> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline int
ggScale(const long x, const ggRange<long> domain,
 const ggRange<int> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline long
ggScale(const long x, const ggRange<long> domain,
 const ggRange<long> range) {
    return ggScaleIntegralToIntegral(x, domain, range);
}

inline float
ggScale(const long x, const ggRange<long> domain,
 const ggRange<float> range) {
    return ggScaleIntegralToReal(x, domain, range);
}

inline double
ggScale(const long x, const ggRange<long> domain,
 const ggRange<double> range) {
    return ggScaleIntegralToReal(x, domain, range);
}

// float to ...

inline char
ggScale(const float x, const ggRange<float> domain,
 const ggRange<char> range) {
    return ggScaleRealToIntegral(x, domain, range);
}

inline unsigned char
ggScale(const float x, const ggRange<float> domain,
 const ggRange<unsigned char> range) {
    return ggScaleRealToIntegral(x, domain, range);
}

inline short
ggScale(const float x, const ggRange<float> domain,
 const ggRange<short> range) {
    return ggScaleRealToIntegral(x, domain, range);
}

inline int
ggScale(const float x, const ggRange<float> domain,
 const ggRange<int> range) {
    return ggScaleRealToIntegral(x, domain, range);
}

inline long
ggScale(const float x, const ggRange<float> domain,
 const ggRange<long> range) {
    return ggScaleRealToIntegral(x, domain, range);
}

inline float
ggScale(const float x, const ggRange<float> domain,
 const ggRange<float> range) {
    return ggScaleRealToReal(x, domain, range);
}

inline double
ggScale(const float x, const ggRange<float> domain,
 const ggRange<double> range) {
    return ggScaleRealToReal(x, domain, range);
}

// double to ...

inline char
ggScale(const double x, const ggRange<double> domain,
 const ggRange<char> range) {
    return ggScaleRealToIntegral(x, domain, range);
}

inline unsigned char
ggScale(const double x, const ggRange<double> domain,
 const ggRange<unsigned char> range) {
    return ggScaleRealToIntegral(x, domain, range);
}

inline short
ggScale(const double x, const ggRange<double> domain,
 const ggRange<short> range) {
    return ggScaleRealToIntegral(x, domain, range);
}

inline unsigned short
ggScale(const double x, const ggRange<double> domain,
 const ggRange<unsigned short> range) {
    return ggScaleRealToIntegral(x, domain, range);
}

inline int
ggScale(const double x, const ggRange<double> domain,
 const ggRange<int> range) {
    return ggScaleRealToIntegral(x, domain, range);
}

inline long
ggScale(const double x, const ggRange<double> domain,
 const ggRange<long> range) {
    return ggScaleRealToIntegral(x, domain, range);
}

inline float
ggScale(const double x, const ggRange<double> domain,
 const ggRange<float> range) {
    return ggScaleRealToReal(x, domain, range);
}

inline double
ggScale(const double x, const ggRange<double> domain,
 const ggRange<double> range) {
    return ggScaleRealToReal(x, domain, range);
}

//#if defined( _WIN32 )
//#include "ggRange.cpp"
//#endif

#endif



/* vi:set ts=4 sw=4: */
