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
// ggHPoint3.h-- declarations for class ggHPoint3
//
// Author:    Pete Shirley
//            10/30/93
// Modified:  Kwansik Kim
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

#ifndef GGHPOINT3_H
#define GGHPOINT3_H



#include <iostream.h>
#include <ggPoint3.h>



//  Class ggHPoint3


class ggHPoint3 {

public:
    
    // Constructors/Destructors

    ggHPoint3() { e[0] = e[1] = 0.0; e[2] = e[3] = 0.0; } // default
    ggHPoint3(double e0, double e1, double e2) { e[0] = e0; e[1] = e1;
                                                 e[2] = e2; e[3] = 1;}
    ggHPoint3(double e0, double e1, double e2, double e3)
                                { e[0] = e0; e[1] = e1; e[2] = e2; e[3] = e3;}

    // Conversions

    ggHPoint3(const ggHPoint3 &p) {
                x() = p.x();
                y() = p.y();
                z() = p.z();
                h() = p.h(); }

    // To and from Point3.

    ggHPoint3(const ggPoint3 &p) {
                x() = p.x();
                y() = p.y();
                z() = p.z();
                h() = 1.0; }

    inline operator ggPoint3();

    //  Access Operators/Methods
    inline void Set(double e0, double e1, double e2);
    inline void Set(double e0, double e1, double e2, double e3);



    inline double x() const { return e[0]; }
    inline double y() const { return e[1]; }
    inline double z() const { return e[2]; }
    inline double w() const { return e[3]; }
    inline double h() const { return e[3]; }
    inline double &x() { return e[0]; }
    inline double &y() { return e[1]; }
    inline double &z() { return e[2]; }
    inline double &w() { return e[3]; }
    inline double &h() { return e[3]; }

    inline void SetX(double a) { x() = a; }
    inline void SetY(double a) { y() = a; }
    inline void SetZ(double a) { z() = a; }
    inline void SetW(double a) { w() = a; }
    inline void SetH(double a) { h() = a; }

    // Operators
    inline double operator[](int i) const;
    inline ggHPoint3& operator+=(const ggVector3 &v);
    inline ggHPoint3& operator-=(const ggVector3 &v);
    inline ggHPoint3& operator*=(double t);
    inline ggHPoint3& operator/=(double t);

    
    // Miscellaneous Methods

    ggHPoint3 Homogenize() const;

    double e[4];

};

const ggHPoint3 ggHOrigin3(0.0,0.0,0.0, 1.0); 


// Subtraction

ggVector3 operator-(const ggHPoint3 &p1, const ggHPoint3 &p2);

// Addition/subtraction of point and vector

ggHPoint3 operator+(const ggHPoint3 &p, const ggVector3 &v);
ggHPoint3 operator+(const ggVector3 &p, const ggHPoint3 &v);
ggHPoint3 operator-(const ggHPoint3 &p, const ggVector3 &v);

// Scalar product/quotient
ggHPoint3 operator*(double t, const ggHPoint3 &p);
ggHPoint3 operator*(const ggHPoint3 &p, double t);
ggHPoint3 operator/(const ggHPoint3 &p, double t);

ostream &operator<<(ostream &os, const ggHPoint3 &t) ;

istream &operator>>(istream &is, ggHPoint3 &t) ;


// Equality/inequality

ggBoolean operator==(const ggHPoint3 &p1, const ggHPoint3 &p2);

ggBoolean operator!=(const ggHPoint3 &p1, const ggHPoint3 &p2);


// Conversion

inline ggHPoint3::operator ggPoint3() { 

#ifdef GGSAFE
    if (ggAbs(e[3]) <= 0.0) {
	fprintf(stderr, "Invalid homogeneous coordinate: h =%f\n", e[3]);
	exit(1);
	}
#endif
    return ggPoint3(e[0]/e[3], e[1]/e[3], e[2]/e[3]);
}


// Access methods

inline void ggHPoint3::Set(double e0, double e1, double e2) {
    e[0] = e0; e[1] = e1; e[2] = e2; e[3] = 1;
}

inline void ggHPoint3::Set(double e0, double e1, double e2, double e3) {
     e[0] = e0; e[1] = e1; e[2] = e2; e[3] = e3;
}



// Miscellaneous Methods

inline ggHPoint3 ggHPoint3::Homogenize() const {

#ifdef GGSAFE
    if (ggAbs(e[3]) <= 0.0) {
	fprintf(stderr, "Invalid homogeneous coordinate: h =%f\n", e[3]);
	exit(1);
	}
#endif

    return ggHPoint3(e[0]/e[3], e[1]/e[3], e[2]/e[3]);
}




//  Operators

// Subtraction
inline ggVector3 operator-(const ggHPoint3 &p1, const ggHPoint3 &p2) {

#ifdef GGSAFE
    if (ggAbs(p1.e[3]) <= 0.0 || ggAbs(p2.e[3]) <= 0.0 ) {
	fprintf(stderr, "Invalid homogeneous coordinate");
	exit(1);
	}
#endif

    return ggVector3(
          (p1.e[0]/p1.e[3] - p2.e[0]/p2.e[3]),
          (p1.e[1]/p1.e[3] - p2.e[1]/p2.e[3]),
          (p1.e[2]/p1.e[3] - p2.e[2]/p2.e[3]));
}

// Addition/subtraction of point and vector
inline ggHPoint3 operator+(const ggHPoint3 &p, const ggVector3 &v) {

#ifdef GGSAFE
    if (ggAbs(p.e[3]) <= 0.0) {
	fprintf(stderr, "Invalid homogeneous coordinate");
	exit(1);
	}
#endif

    return ggHPoint3(
         (p.e[0]/p.e[3] + v.e[0]),
         (p.e[1]/p.e[3] + v.e[1]),
         (p.e[2]/p.e[3] + v.e[2]));
}

inline ggHPoint3 operator+(const ggVector3 &v, const ggHPoint3 &p) {

#ifdef GGSAFE
    if (ggAbs(p.e[3]) <= 0.0) {
	fprintf(stderr, "Invalid homogeneous coordinate");
	exit(1);
	}
#endif

    return ggHPoint3(
          (p.e[0]/p.e[3] + v.e[0]),
          (p.e[1]/p.e[3] + v.e[1]),
          (p.e[2]/p.e[3] + v.e[2]));
}

inline ggHPoint3 operator-(const ggHPoint3 &p, const ggVector3 &v) {

#ifdef GGSAFE
    if (ggAbs(p.e[3]) <= 0.0) {
	fprintf(stderr, "Invalid homogeneous coordinate");
	exit(1);
	}
#endif

    return ggHPoint3(
      (p.e[0]/p.e[3] - v.e[0]),
      (p.e[1]/p.e[3] - v.e[1]),
      (p.e[2]/p.e[3] - v.e[2]));
}

// Scalar product/quotient
inline ggHPoint3 operator*(double t, const ggHPoint3 &p) {

#ifdef GGSAFE
    if (ggAbs(t) <= 0.0) {
	fprintf(stderr, "ggHPoint3::devide by zero\n");
	exit(1);
    }
#endif

    ggHPoint3 np = p; np.e[3] /=  t; return np;
}

inline ggHPoint3 operator*(const ggHPoint3 &p, double t) {

#ifdef GGSAFE
    if (ggAbs(t) <= 0.0) {
	fprintf(stderr, "ggHPoint3::devide by zero\n");
	exit(1);
    }
#endif

    ggHPoint3 np = p; np.e[3] /= t; return np;
}

inline ggHPoint3 operator/(const ggHPoint3 &p, double t) {
    ggHPoint3 np = p; np.e[3] *= t; return np;
}

// [] operator

inline double ggHPoint3::operator[](int i) const { 

#ifdef GGSAFE
    if ( i < 0 || i > 3 ) 
    {
	fprintf(stderr, "ggHPoint3 : out of range :%d\n", i);
	exit(1);
    }
#endif

    return e[i]; 
}


inline ggHPoint3& ggHPoint3::operator+=(const ggVector3 &v) {

#ifdef GGSAFE
    if (ggAbs(e[3]) <= 0.0) {
	fprintf(stderr, "Invalid homogeneous coordinate");
	exit(1);
	}
#endif

    e[0] +=  (e[3]*v.e[0]);
    e[1] +=  (e[3]*v.e[1]);
    e[2] +=  (e[3]*v.e[2]);
    return *this;
}

inline ggHPoint3& ggHPoint3::operator-=(const ggVector3 &v) {

#ifdef GGSAFE
    if (ggAbs(e[3]) <= 0.0)  {
	fprintf(stderr, "Invalid homogeneous coordinate");
	exit(1);
	}
#endif

    e[0] -=  (e[3]*v.e[0]);
    e[1] -=  (e[3]*v.e[1]);
    e[2] -=  (e[3]*v.e[2]);
    return *this;
}


inline ggHPoint3& ggHPoint3::operator*=(double t) {

#ifdef GGSAFE
    if (ggAbs(t) < 0.0) {
	fprintf(stderr, "ggHPoint3:divide by zero\n");
	exit(1);
    }
#endif

    e[3] /=  t; return *this;
}

inline ggHPoint3& ggHPoint3::operator/=(double t) {
    e[3] *=  t; return *this;
}

#endif

