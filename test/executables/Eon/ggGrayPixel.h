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




#ifndef GGGRAYPIXEL_H
#define GGGRAYPIXEL_H



#include <iostream.h>
#include <ggMacros.h>



/******************************************************************************
* ggGrayPixel
*/

template <class T>
class ggGrayPixel {

public:
    
    /*
     * Constructors/Destructors
     */

    ggGrayPixel() {tolerance = 1E-8;}
    ggGrayPixel(T i) { e[0] = i; tolerance = 1E-8;}
    
    /*
     * Conversions
     */

    ggGrayPixel(const ggGrayPixel<char> &p);
    ggGrayPixel(const ggGrayPixel<unsigned char> &p);
    ggGrayPixel(const ggGrayPixel<short> &p);
    ggGrayPixel(const ggGrayPixel<unsigned short> &p);
    ggGrayPixel(const ggGrayPixel<int> &p);
    ggGrayPixel(const ggGrayPixel<unsigned int> &p);
    ggGrayPixel(const ggGrayPixel<long> &p);
    ggGrayPixel(const ggGrayPixel<float> &p);
    ggGrayPixel(const ggGrayPixel<double> &p);

    /*
     * Methods
     */

    void Set(T i) { e[0] = i; }

    T intensity() const { return e[0]; }
    T &intensity() { return e[0]; }

        static T &tol() { return (T) tolerance; }




     T e[1];
     static double tolerance;

};



/***************************************
* Declarations
*/

/*
 * Non-friends
 */

// Addition/Subtraction/Multiplication

template <class T1, class T2>
ggGrayPixel<T1>
operator+(const ggGrayPixel<T1> &v1, const ggGrayPixel<T2> &v2);

template <class T1, class T2>
ggGrayPixel<T1> &
operator+=(ggGrayPixel<T1> &v1, const ggGrayPixel<T2> &v2);

template <class T1, class T2>
ggGrayPixel<T1>
operator-(const ggGrayPixel<T1> &v1, const ggGrayPixel<T2> &v2);

template <class T1, class T2>
ggGrayPixel<T1> &
operator-=(ggGrayPixel<T1> &v1, const ggGrayPixel<T2> &v2);

template <class T1, class T2>
ggGrayPixel<T1>
operator*(const ggGrayPixel<T1> &v1, const ggGrayPixel<T2> &v2);

template <class T1, class T2>
ggGrayPixel<T1> &
operator*=(ggGrayPixel<T1> &v1, const ggGrayPixel<T2> &v2);

// Scalar Product

template <class T> ggGrayPixel<T> operator*(int t, const ggGrayPixel<T> &p);
template <class T> ggGrayPixel<T> operator*(float t, const ggGrayPixel<T> &p);
template <class T> ggGrayPixel<T> operator*(double t, const ggGrayPixel<T> &p);
template <class T> ggGrayPixel<T> operator*(const ggGrayPixel<T> &p, int t);
template <class T> ggGrayPixel<T> operator*(const ggGrayPixel<T> &p, float t);
template <class T> ggGrayPixel<T> operator*(const ggGrayPixel<T> &p, double t);
template <class T> ggGrayPixel<T> operator/(const ggGrayPixel<T> &p, int t);
template <class T> ggGrayPixel<T> operator/(const ggGrayPixel<T> &p, float t);
template <class T> ggGrayPixel<T> operator/(const ggGrayPixel<T> &p, double t);
template <class T> ggGrayPixel<T> &operator*=(ggGrayPixel<T> &p, int t);
template <class T> ggGrayPixel<T> &operator*=(ggGrayPixel<T> &p, float t);
template <class T> ggGrayPixel<T> &operator*=(ggGrayPixel<T> &p, double t);
template <class T> ggGrayPixel<T> &operator/=(ggGrayPixel<T> &p, int t);
template <class T> ggGrayPixel<T> &operator/=(ggGrayPixel<T> &p, float t);
template <class T> ggGrayPixel<T> &operator/=(ggGrayPixel<T> &p, double t);



/***************************************
* Inlines
*/

/*
 * Methods
 */

/*
 * Non-friends
 */

// Addition/subtraction/multiplication

template <class T1, class T2>
inline ggGrayPixel<T1>
operator+(const ggGrayPixel<T1> &v1, const ggGrayPixel<T2> &v2) {
    return ggGrayPixel<T1>((T1) (v1.e[0] + v2.e[0]));
}
template <class T1, class T2>
inline ggGrayPixel<T1> &
operator+=(ggGrayPixel<T1> &v1, const ggGrayPixel<T2> &v2) {
    v1.e[0] += v2.e[0];
    return v1;
}

template <class T1, class T2>
inline ggGrayPixel<T1>
operator-(const ggGrayPixel<T1> &v1, const ggGrayPixel<T2> &v2) {
    return ggGrayPixel<T1>((T1) (v1.e[0] - v2.e[0]));
}
template <class T1, class T2>
inline ggGrayPixel<T1> &
operator-=(ggGrayPixel<T1> &v1, const ggGrayPixel<T2> &v2) {
    v1.e[0] -= v2.e[0];
    return v1;
}

template <class T1, class T2>
inline ggGrayPixel<T1>
operator*(const ggGrayPixel<T1> &v1, const ggGrayPixel<T2> &v2) {
    return ggGrayPixel<T1>((T1) (v1.e[0]*v2.e[0]));
}
template <class T1, class T2>
inline ggGrayPixel<T1> &
operator*=(ggGrayPixel<T1> &v1, const ggGrayPixel<T2> &v2) {
    v1.e[0] *= v2.e[0];
    return v1;
}

// Scalar Product

template <class T>
inline ggGrayPixel<T>
operator*(int t, const ggGrayPixel<T> &v) {
    return ggGrayPixel<T>(t*v.e[0]);
}
template <class T>
inline ggGrayPixel<T>
operator*(float t, const ggGrayPixel<T> &v) {
    return ggGrayPixel<T>((T) (t*v.e[0]));
}
template <class T>
inline ggGrayPixel<T>
operator*(double t, const ggGrayPixel<T> &v) {
    return ggGrayPixel<T>((T) (t*v.e[0]));
}

template <class T>
inline ggGrayPixel<T>
operator*(const ggGrayPixel<T> &v, int t) {
    return ggGrayPixel<T>(t*v.e[0]);
}
template <class T>
inline ggGrayPixel<T>
operator*(const ggGrayPixel<T> &v, float t) {
    return ggGrayPixel<T>((T) (t*v.e[0]));
}
template <class T>
inline ggGrayPixel<T>
operator*(const ggGrayPixel<T> &v, double t) {
    return ggGrayPixel<T>((T) (t*v.e[0]));
}
template <class T>
inline ggGrayPixel<T> operator/(const ggGrayPixel<T> &v, int t) {
    return ggGrayPixel<T>(v.e[0]/t);
}
template <class T>
inline ggGrayPixel<T>
operator/(const ggGrayPixel<T> &v, float t) {
    return ggGrayPixel<T>((T) (v.e[0]/t));
}
template <class T>
inline ggGrayPixel<T>
operator/(const ggGrayPixel<T> &v, double t) {
    return ggGrayPixel<T>((T) (v.e[0]/t));
}
template <class T>
inline ggGrayPixel<T> &
operator*=(ggGrayPixel<T> &v, int t) {
    v.e[0] *= t;
    return v;
}
template <class T>
inline ggGrayPixel<T> &
operator*=(ggGrayPixel<T> &v, float t) {
    v.e[0] *= t;
    return v;
}
template <class T>
inline ggGrayPixel<T> &
operator*=(ggGrayPixel<T> &v, double t) {
    v.e[0] *= t;
    return v;
}
template <class T>
inline ggGrayPixel<T> &
operator/=(ggGrayPixel<T> &v, int t) {
    v.e[0] /= t;
    return v;
}
template <class T>
inline ggGrayPixel<T> &
operator/=(ggGrayPixel<T> &v, float t) {
    v.e[0] /= t;
    return v;
}
template <class T>
inline ggGrayPixel<T> &
operator/=(ggGrayPixel<T> &v, double t) {
    v.e[0] /= t;
    return v;
}

template <class T1, class T2>
ggBoolean operator==(const ggGrayPixel<T1> &t1, const ggGrayPixel<T2> &t2);

template <class T1, class T2>
ggBoolean operator!=(const ggGrayPixel<T1> &t1, const ggGrayPixel<T2> &t2);

template <class T>
ostream &operator<<(ostream &os, const ggGrayPixel<T> &t);

template <class T>
istream &operator>>(istream &is, ggGrayPixel<T> &t);

//#if defined( _WIN32 )
//#  include "ggGrayPixel.cpp"
//#endif

#endif
