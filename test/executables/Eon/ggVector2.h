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
// ggVector2.h-- declarations for class ggVector2
//
// Author:    Pete Shirley
//            10/30/93
// Modified:  Kwansik Kim
// Modified:  Pete Shirley 4/16/94 to speed inlining
//
//
// Copyright 1993 by Peter Shirley, Kwansik Kim
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
//

#ifndef GGVECTOR2_H
#define GGVECTOR2_H



#include <math.h>
#include <stdlib.h>
#include <iostream.h>
#include <ggMacros.h>

#ifdef GGSAFE
#include <stdio.h>
#endif



// Class ggVector2

class ggVector2 {

public:
    
    // Constructors/Destructors

    ggVector2() { e[0] = 1.0; e[0] = 0.0; }
    ggVector2(double e0, double e1) { e[0] = e0; e[1] = e1; }
    
    
    // Access Operators/Methods
    inline void Set(double x, double y) {e[0] = x; e[1] = y; }
    inline double x() const { return e[0]; }
    inline double y() const { return e[1]; }
    inline double u() const { return e[0]; }
    inline double v() const { return e[1]; }
    inline double &x() { return e[0]; }
    inline double &y() { return e[1]; }
    inline double &u() { return e[0]; }
    inline double &v() { return e[1]; }
    inline void SetX(double a) { x() = a; }
    inline void SetY(double a) { y() = a; }
    inline void SetU(double a) { x() = a; }
    inline void SetV(double a) { y() = a; }

    ggVector2(const ggVector2& v) { e[0] = v.e[0]; e[1] = v.e[1]; }

    inline double tolerance() const { return tol; }
    inline double& tolerance() { return tol; }


    // Operators
    inline double operator[](int i) const;
    inline ggVector2 operator-() const { return ggVector2(-x() , -y() ); }
    inline ggVector2 operator+() const { return ggVector2(x() , y() ); }
    inline ggVector2& operator+=(const ggVector2 &v);
    inline ggVector2& operator-=(const ggVector2 &v);
    inline ggVector2& operator*=(double t);
    inline ggVector2& operator/=(double t);

    
     // Miscellaneous Methods

    inline double length() const { return sqrt(x() *x()  + y() *y() ); }
    inline double squaredLength() const { return x() *x()  + y() *y()  ; }
    inline void MakeUnitVector();

    double e[2];
private:
    static double tol;

};


const ggVector2 ggXAxis2(1.0,0.0);
const ggVector2 ggYAxis2(0.0,1.0);



// Operators

ggBoolean operator==(const ggVector2 &t1, const ggVector2 &t2);

ggBoolean operator!=(const ggVector2 &t1, const ggVector2 &t2);

istream &operator>>(istream &is, ggVector2 &t);

ostream &operator<<(ostream &os, const ggVector2 &t);

// Addition/Subtraction

ggVector2 operator+(const ggVector2 &v1, const ggVector2 &v2);
ggVector2 operator-(const ggVector2 &v1, const ggVector2 &v2);

// Scalar Product

ggVector2 operator*(double t, const ggVector2 &v);
ggVector2 operator*(const ggVector2 &v, double t);
ggVector2 operator/(const ggVector2 &v, double t);


// Dot Product

//double dot(const ggVector2 &v1, const ggVector2 &v2);
inline double ggDot(const ggVector2 &v1, const ggVector2 &v2);

// inlines
inline ggVector2 ggUnitVector(const ggVector2& v) {
    double l = v.length();

#ifdef GGSAFE
    if (l <= 0.0) {
	fprintf(stderr, "Vector Length is zero\n");
	exit(1);
    }
#endif

    return ggVector2(v.x() /l, v.y() /l);
}



inline void ggVector2::MakeUnitVector() {
    double l = length();

#ifdef GGSAFE
    if (l <= 0.0) {
	fprintf(stderr, "Vector Length is zero\n");
	exit(1);
    }
#endif

    e[0] /= l; e[1] /= l;
}

// Addition/subtraction

inline ggVector2 operator+(const ggVector2 &v1, const ggVector2 &v2) {
    return ggVector2(v1.x()  + v2.x(), v1.y()  + v2.y());
}

inline ggVector2 operator-(const ggVector2 &v1, const ggVector2 &v2) {
    return ggVector2(v1.x()  - v2.x() , v1.y()  - v2.y() );
}

// Scalar Product

inline ggVector2 operator*(double t, const ggVector2 &v) {
    return ggVector2( t*v.x() , t*v.y() );
}

inline ggVector2 operator*(const ggVector2 &v, double t) {
    return ggVector2( t*v.x() , t*v.y() );
}

inline ggVector2 operator/(const ggVector2 &v, double t) {

#ifdef GGSAFE
    if (ggAbs(t) <= 0.0) {
	fprintf(stderr, "In ggVector3 :Divide by zero\n");
	exit(1);
    }
#endif
 
    return ggVector2( v.x()/t , v.y()/t );
}

// Dot product

//inline double dot(const ggVector2 &v1, const ggVector2 &v2) {
//    return v1.x() *v2.x()  + v1.y() *v2.y();
//}

inline double ggDot(const ggVector2 &v1, const ggVector2 &v2) {
    return v1.x() *v2.x()  + v1.y() *v2.y();
}


// Operators

inline double ggVector2::operator[](int i) const { 

#ifdef GGSAFE
    if ( i < 0 || i > 1 )
    {
	fprintf(stderr, "ggVector3: out of range %d\n", i);
	exit(1);
    }
#endif

    return e[i]; 
}


inline ggVector2& ggVector2::operator+=(const ggVector2 &v) {
    e[0]  += v.x() ;
    e[1]  += v.y() ;
    return *this;
}

inline ggVector2& ggVector2::operator-=(const ggVector2 &v) {
    e[0]  -= v.x() ;
    e[1]  -= v.y() ;
    return *this;
}

inline ggVector2& ggVector2::operator*=(double t){
    e[0]  *= t;
    e[1]  *= t;
    return *this;
}

inline ggVector2& ggVector2::operator/=(double t) {
    e[0]  /= t;
    e[1]  /= t;
    return *this;
}

#endif
