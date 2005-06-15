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




#ifndef GGRGBPIXEL_H
#define GGRGBPIXEL_H



#include <iostream.h>
#include <ggMacros.h>
#include <ggRGBE.h>



/******************************************************************************
* ggRGBPixel
*/

template <class T>
class ggRGBPixel {

public:
    
    /*
     * Constructors/Destructors
     */

    ggRGBPixel() { tolerance = 0; }
    ggRGBPixel(T r, T g, T b) {e[0] = r; e[1] = g; e[2] = b; tolerance = 0;}
    
    /*
     * Conversions
     */

    ggRGBPixel(const ggRGBPixel<char> &p);
    ggRGBPixel(const ggRGBPixel<unsigned char> &p);
    ggRGBPixel(const ggRGBPixel<short> &p);
    ggRGBPixel(const ggRGBPixel<unsigned short> &p);
    ggRGBPixel(const ggRGBPixel<int> &p);
    ggRGBPixel(const ggRGBPixel<unsigned int> &p);
    ggRGBPixel(const ggRGBPixel<long> &p);
    ggRGBPixel(const ggRGBPixel<float> &p);
    ggRGBPixel(const ggRGBPixel<double> &p);

    /*
     * Methods
     */

    void Set(T r, T g, T b) { e[0] = r; e[1] = g; e[2] = b; }

    ggRGBE ConvertToRGBE();
    ggRGBPixel<T> ConvertFromRGBE(ggRGBE& a);

    T r() const { return e[0]; }
    T g() const { return e[1]; }
    T b() const { return e[2]; }

    T &r() { return e[0]; }
    T &g() { return e[1]; }
    T &b() { return e[2]; }

    /*
     * Standard ggPixel interface
     */

        T tol() const { return (T) tolerance; }

     T e[3];
     unsigned char tolerance;
};



/***************************************
* Declarations
*/

/*
 * Non-friends
 */

template <class T1, class T2>
ggBoolean operator==(const ggRGBPixel<T1> &t1, const ggRGBPixel<T2> &t2);

template <class T1, class T2>
ggBoolean operator!=(const ggRGBPixel<T1> &t1, const ggRGBPixel<T2> &t2);


template <class T>
ostream &operator<<(ostream &os, const ggRGBPixel<T> &t);

template <class T>
istream &operator>>(istream &is, ggRGBPixel<T> &t);



// Addition/Subtraction/Multiplication
template <class T1, class T2>
    ggRGBPixel<T1> operator/(const ggRGBPixel<T1> &v1, const ggRGBPixel<T2> &v2);
template <class T1, class T2>
    ggRGBPixel<T1> operator+(const ggRGBPixel<T1> &v1, const ggRGBPixel<T2> &v2);
template <class T1, class T2>
    ggRGBPixel<T1> &operator+=(ggRGBPixel<T1> &v1, const ggRGBPixel<T2> &v2);
template <class T1, class T2>
    ggRGBPixel<T1> operator-(const ggRGBPixel<T1> &v1, const ggRGBPixel<T2> &v2);
template <class T1, class T2>
    ggRGBPixel<T1> &operator-=(ggRGBPixel<T1> &v1, const ggRGBPixel<T2> &v2);
template <class T1, class T2>
    ggRGBPixel<T1> operator*(const ggRGBPixel<T1> &v1, const ggRGBPixel<T2> &v2);
template <class T1, class T2>
    ggRGBPixel<T1> &operator*=(ggRGBPixel<T1> &v1, const ggRGBPixel<T2> &v2);

// Scalar Product
template <class T> ggRGBPixel<T> operator*(int t, const ggRGBPixel<T> &p);
template <class T> ggRGBPixel<T> operator*(float t, const ggRGBPixel<T> &p);
template <class T> ggRGBPixel<T> operator*(double t, const ggRGBPixel<T> &p);
template <class T> ggRGBPixel<T> operator*(const ggRGBPixel<T> &p, int t);
template <class T> ggRGBPixel<T> operator*(const ggRGBPixel<T> &p, float t);
template <class T> ggRGBPixel<T> operator*(const ggRGBPixel<T> &p, double t);
template <class T> ggRGBPixel<T> operator/(const ggRGBPixel<T> &p, int t);
template <class T> ggRGBPixel<T> operator/(const ggRGBPixel<T> &p, float t);
template <class T> ggRGBPixel<T> operator/(const ggRGBPixel<T> &p, double t);
template <class T> ggRGBPixel<T> &operator*=(ggRGBPixel<T> &p, int t);
template <class T> ggRGBPixel<T> &operator*=(ggRGBPixel<T> &p, float t);
template <class T> ggRGBPixel<T> &operator*=(ggRGBPixel<T> &p, double t);
template <class T> ggRGBPixel<T> &operator/=(ggRGBPixel<T> &p, int t);
template <class T> ggRGBPixel<T> &operator/=(ggRGBPixel<T> &p, float t);
template <class T> ggRGBPixel<T> &operator/=(ggRGBPixel<T> &p, double t);



/***************************************
* Inlines
*/

/*
 * Methods
 */

/*
 * Non-friends
 */

template <class T1, class T2>
inline ggRGBPixel<T1>
operator/(const ggRGBPixel<T1> &v1, const ggRGBPixel<T2> &v2) {
    return ggRGBPixel<T1>((T1) (v1.r()  / v2.r() ), (T1) (v1.g()  / v2.g() ),
     (T1) (v1.b()  / v2.b() ));
}
// Addition/subtraction/multiplication
template <class T1, class T2>
inline ggRGBPixel<T1>
operator+(const ggRGBPixel<T1> &v1, const ggRGBPixel<T2> &v2) {
    return ggRGBPixel<T1>((T1) (v1.r()  + v2.r() ), (T1) (v1.g()  + v2.g() ),
     (T1) (v1.b()  + v2.b() ));
}
template <class T1, class T2>
inline ggRGBPixel<T1> &
operator+=(ggRGBPixel<T1> &v1, const ggRGBPixel<T2> &v2) {
    v1.r()  += v2.r() ;
    v1.g()  += v2.g() ;
    v1.b()  += v2.b() ;
    return v1;
}

template <class T1, class T2>
inline ggRGBPixel<T1>
operator-(const ggRGBPixel<T1> &v1, const ggRGBPixel<T2> &v2) {
    return ggRGBPixel<T1>((T1) (v1.r()  - v2.r() ), (T1) (v1.g()  - v2.g() ),
     (T1) (v1.b()  - v2.b() ));
}
template <class T1, class T2>
inline ggRGBPixel<T1> &operator-=(ggRGBPixel<T1> &v1, const ggRGBPixel<T2> &v2) {
    v1.r()  -= v2.r() ;
    v1.g()  -= v2.g() ;
    v1.b()  -= v2.b() ;
    return v1;
}
template <class T1, class T2>
inline ggRGBPixel<T1>
operator*(const ggRGBPixel<T1> &v1, const ggRGBPixel<T2> &v2) {
    return ggRGBPixel<T1>((T1) (v1.r()  * v2.r() ), (T1) (v1.g()  * v2.g() ),
     (T1) (v1.b()  * v2.b() ));
}
template <class T1, class T2>
inline ggRGBPixel<T1> &operator*=(ggRGBPixel<T1> &v1, const ggRGBPixel<T2> &v2) {
    v1.r()  *= v2.r() ;
    v1.g()  *= v2.g() ;
    v1.b()  *= v2.b() ;
    return v1;
}

// Scalar Product
template <class T>
    inline ggRGBPixel<T> operator*(int t, const ggRGBPixel<T> &v) {
    return ggRGBPixel<T>(t*v.r() , t*v.g() , t*v.b() ); }
template <class T>
    inline ggRGBPixel<T> operator*(float t, const ggRGBPixel<T> &v) {
    return ggRGBPixel<T>((T) (t*v.r() ), (T) (t*v.g() ), (T) (t*v.b() )); }
template <class T>
    inline ggRGBPixel<T> operator*(double t, const ggRGBPixel<T> &v) {
    return ggRGBPixel<T>((T) (t*v.r() ), (T) (t*v.g() ), (T) (t*v.b() )); }
template <class T>
    inline ggRGBPixel<T> operator*(const ggRGBPixel<T> &v, int t) {
    return ggRGBPixel<T>(t*v.r() , t*v.g() , t*v.b() ); }
template <class T>
    inline ggRGBPixel<T> operator*(const ggRGBPixel<T> &v, float t) {
    return ggRGBPixel<T>((T) (t*v.r() ), (T) (t*v.g() ), (T) (t*v.b() )); }
template <class T>
    inline ggRGBPixel<T> operator*(const ggRGBPixel<T> &v, double t) {
    return ggRGBPixel<T>((T) (t*v.r() ), (T) (t*v.g() ), (T) (t*v.b() )); }
template <class T>
    inline ggRGBPixel<T> operator/(const ggRGBPixel<T> &v, int t) {
    return ggRGBPixel<T>(v.r() /t, v.g() /t, v.b() /t); }
template <class T>
    inline ggRGBPixel<T> operator/(const ggRGBPixel<T> &v, float t) {
    return ggRGBPixel<T>((T) (v.r() /t), (T) (v.g() /t), (T) (v.b() /t)); }
template <class T>
    inline ggRGBPixel<T> operator/(const ggRGBPixel<T> &v, double t) {
    return ggRGBPixel<T>((T) (v.r() /t), (T) (v.g() /t), (T) (v.b() /t)); }
template <class T>
inline ggRGBPixel<T> &operator*=(ggRGBPixel<T> &v, int t) {
    v.r()  *= t;
    v.g()  *= t;
    v.b()  *= t;
    return v;
}
template <class T>
inline ggRGBPixel<T> &operator*=(ggRGBPixel<T> &v, float t) {
    v.r()  *= t;
    v.g()  *= t;
    v.b()  *= t;
    return v;
}
template <class T>
inline ggRGBPixel<T> &operator*=(ggRGBPixel<T> &v, double t) {
    v.r()  *= t;
    v.g()  *= t;
    v.b()  *= t;
    return v;
}
template <class T>
inline ggRGBPixel<T> &operator/=(ggRGBPixel<T> &v, int t) {
    v.r()  /= t;
    v.g()  /= t;
    v.b()  /= t;
    return v;
}
template <class T>
inline ggRGBPixel<T> &operator/=(ggRGBPixel<T> &v, float t) {
    v.r()  /= t;
    v.g()  /= t;
    v.b()  /= t;
    return v;
}
template <class T>
inline ggRGBPixel<T> &operator/=(ggRGBPixel<T> &v, double t) {
    v.r()  /= t;
    v.g()  /= t;
    v.b()  /= t;
    return v;
}

//#if defined _WIN32
#include "ggRGBPixel.cc"
//#endif

#endif
