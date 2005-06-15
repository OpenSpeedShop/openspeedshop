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




#ifndef GGRGBFPIXEL_H
#define GGRGBFPIXEL_H



#include <iostream.h>
#include <ggBoolean.h>
#include <ggMacros.h>
#include <ggRGBE.h>



/******************************************************************************
* ggRGBFPixel
*/

class ggRGBFPixel {

public:
    
    /*
     * Constructors/Destructors
     */

    ggRGBFPixel() {}
    ggRGBFPixel(float r, float g, float b) {e[0] = r; e[1] = g; e[2] = b; }
    
    /*
     * Conversions
     */

    /*
     * Methods
     */

    void Set(float r, float g, float b) { e[0] = r; e[1] = g; e[2] = b; }

    ggRGBE ConvertToRGBE();
    ggRGBFPixel ConvertFromRGBE(ggRGBE& a);

    float r() const { return e[0]; }
    float g() const { return e[1]; }
    float b() const { return e[2]; }

    float &r() { return e[0]; }
    float &g() { return e[1]; }
    float &b() { return e[2]; }

    /*
     * Standard ggPixel interface
     */

        static float &tol() { return tolerance; }

     float e[3];
     static float tolerance;
};

const ggRGBFPixel ggFBlack(0, 0, 0);
const ggRGBFPixel ggFWhite(1, 1, 1);
const ggRGBFPixel ggFRed(1, 0, 0);
const ggRGBFPixel ggFGreen(0, 1, 0);
const ggRGBFPixel ggFBlue(0, 0, 1);


/***************************************
* Declarations
*/

/*
 * Non-friends
 */

ggBoolean operator==(const ggRGBFPixel &t1, const ggRGBFPixel &t2);

ggBoolean operator!=(const ggRGBFPixel &t1, const ggRGBFPixel &t2);


ostream &operator<<(ostream &os, const ggRGBFPixel &t);

istream &operator>>(istream &is, ggRGBFPixel &t);



// Addition/Subtraction/Multiplication
    ggRGBFPixel operator/(const ggRGBFPixel &v1, const ggRGBFPixel &v2);
    ggRGBFPixel operator+(const ggRGBFPixel &v1, const ggRGBFPixel &v2);
    ggRGBFPixel &operator+=(ggRGBFPixel &v1, const ggRGBFPixel &v2);
    ggRGBFPixel operator-(const ggRGBFPixel &v1, const ggRGBFPixel &v2);
    ggRGBFPixel &operator-=(ggRGBFPixel &v1, const ggRGBFPixel &v2);
    ggRGBFPixel operator*(const ggRGBFPixel &v1, const ggRGBFPixel &v2);
    ggRGBFPixel &operator*=(ggRGBFPixel &v1, const ggRGBFPixel &v2);

// Scalar Product
ggRGBFPixel operator*(int t, const ggRGBFPixel &p);
ggRGBFPixel operator*(float t, const ggRGBFPixel &p);
ggRGBFPixel operator*(double t, const ggRGBFPixel &p);
ggRGBFPixel operator*(const ggRGBFPixel &p, int t);
ggRGBFPixel operator*(const ggRGBFPixel &p, float t);
ggRGBFPixel operator*(const ggRGBFPixel &p, double t);
ggRGBFPixel operator/(const ggRGBFPixel &p, int t);
ggRGBFPixel operator/(const ggRGBFPixel &p, float t);
ggRGBFPixel operator/(const ggRGBFPixel &p, double t);
ggRGBFPixel &operator*=(ggRGBFPixel &p, int t);
ggRGBFPixel &operator*=(ggRGBFPixel &p, float t);
ggRGBFPixel &operator*=(ggRGBFPixel &p, double t);
ggRGBFPixel &operator/=(ggRGBFPixel &p, int t);
ggRGBFPixel &operator/=(ggRGBFPixel &p, float t);
ggRGBFPixel &operator/=(ggRGBFPixel &p, double t);



/***************************************
* Inlines
*/

/*
 * Methods
 */

/*
 * Non-friends
 */

inline ggRGBFPixel
operator/(const ggRGBFPixel &v1, const ggRGBFPixel &v2) {
    return ggRGBFPixel( (v1.r()  / v2.r() ),  (v1.g()  / v2.g() ),
      (v1.b()  / v2.b() ));
}
// Addition/subtraction/multiplication
inline ggRGBFPixel
operator+(const ggRGBFPixel &v1, const ggRGBFPixel &v2) {
    return ggRGBFPixel( (v1.r()  + v2.r() ),  (v1.g()  + v2.g() ),
      (v1.b()  + v2.b() ));
}
inline ggRGBFPixel &
operator+=(ggRGBFPixel &v1, const ggRGBFPixel &v2) {
    v1.r()  += v2.r() ;
    v1.g()  += v2.g() ;
    v1.b()  += v2.b() ;
    return v1;
}

inline ggRGBFPixel
operator-(const ggRGBFPixel &v1, const ggRGBFPixel &v2) {
    return ggRGBFPixel( (v1.r()  - v2.r() ),  (v1.g()  - v2.g() ),
      (v1.b()  - v2.b() ));
}
inline ggRGBFPixel &operator-=(ggRGBFPixel &v1, const ggRGBFPixel &v2) {
    v1.r()  -= v2.r() ;
    v1.g()  -= v2.g() ;
    v1.b()  -= v2.b() ;
    return v1;
}
inline ggRGBFPixel
operator*(const ggRGBFPixel &v1, const ggRGBFPixel &v2) {
    return ggRGBFPixel( (v1.r()  * v2.r() ),  (v1.g()  * v2.g() ),
      (v1.b()  * v2.b() ));
}
inline ggRGBFPixel &operator*=(ggRGBFPixel &v1, const ggRGBFPixel &v2) {
    v1.r()  *= v2.r() ;
    v1.g()  *= v2.g() ;
    v1.b()  *= v2.b() ;
    return v1;
}

// Scalar Product
    inline ggRGBFPixel operator*(int t, const ggRGBFPixel &v) {
    return ggRGBFPixel(t*v.r() , t*v.g() , t*v.b() ); }
    inline ggRGBFPixel operator*(float t, const ggRGBFPixel &v) {
    return ggRGBFPixel((t*v.r() ), (t*v.g() ), (t*v.b() )); }
    inline ggRGBFPixel operator*(double t, const ggRGBFPixel &v) {
    return ggRGBFPixel((t*v.r() ), (t*v.g() ), (t*v.b() )); }
    inline ggRGBFPixel operator*(const ggRGBFPixel &v, int t) {
    return ggRGBFPixel(t*v.r() , t*v.g() , t*v.b() ); }
    inline ggRGBFPixel operator*(const ggRGBFPixel &v, float t) {
    return ggRGBFPixel((t*v.r() ), (t*v.g() ), (t*v.b() )); }
    inline ggRGBFPixel operator*(const ggRGBFPixel &v, double t) {
    return ggRGBFPixel((t*v.r() ), (t*v.g() ), (t*v.b() )); }
    inline ggRGBFPixel operator/(const ggRGBFPixel &v, int t) {
    return ggRGBFPixel(v.r() /t, v.g() /t, v.b() /t); }
    inline ggRGBFPixel operator/(const ggRGBFPixel &v, float t) {
    return ggRGBFPixel((v.r() /t), (v.g() /t), (v.b() /t)); }
    inline ggRGBFPixel operator/(const ggRGBFPixel &v, double t) {
    return ggRGBFPixel((v.r() /t), (v.g() /t), (v.b() /t)); }
inline ggRGBFPixel &operator*=(ggRGBFPixel &v, int t) {
    v.r()  *= t;
    v.g()  *= t;
    v.b()  *= t;
    return v;
}
inline ggRGBFPixel &operator*=(ggRGBFPixel &v, float t) {
    v.r()  *= t;
    v.g()  *= t;
    v.b()  *= t;
    return v;
}
inline ggRGBFPixel &operator*=(ggRGBFPixel &v, double t) {
    v.r()  *= t;
    v.g()  *= t;
    v.b()  *= t;
    return v;
}
inline ggRGBFPixel &operator/=(ggRGBFPixel &v, int t) {
    v.r()  /= t;
    v.g()  /= t;
    v.b()  /= t;
    return v;
}
inline ggRGBFPixel &operator/=(ggRGBFPixel &v, float t) {
    v.r()  /= t;
    v.g()  /= t;
    v.b()  /= t;
    return v;
}
inline ggRGBFPixel &operator/=(ggRGBFPixel &v, double t) {
    v.r()  /= t;
    v.g()  /= t;
    v.b()  /= t;
    return v;
}



#endif
