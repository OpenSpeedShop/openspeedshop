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
// ggVector3.h-- declarations for class ggVector3
//
// Author:    Pete Shirley
//            10/30/93
// Modified:  Kwansik Kim
// Modified:  Pete Shirley 4/16/94 to speed inlining (replaced x() with e[0])
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



#ifndef GGVECTOR3_H
#define GGVECTOR3_H



#include <math.h>
#include <stdlib.h>
#include <iostream.h>
#include <ggMacros.h>

#ifdef GGSAFE
#include <stdio.h>
#endif


// Class ggVector3

class ggVector3  {

public:
    
    // Constructors/Destructors

    ggVector3(ggBoolean) {}
    ggVector3() {e[0] = 1.0;  e[1] = e[2] = 0.0; }
    ggVector3(double e0, double e1, double e2) {
                   e[0] = e0; e[1] = e1; e[2] = e2; }

    // Conversions


    // Access Operators/Methods
    inline void Set(double x, double y, double z) {e[0] = x; e[1] = y; e[2] = z;}
    inline double x() const { return e[0]; }
    inline double y() const { return e[1]; }
    inline double z() const { return e[2]; }
    inline double &x() { return e[0]; }
    inline double &y() { return e[1]; }
    inline double &z() { return e[2]; }
    inline void SetX(double a) { x() = a; }
    inline void SetY(double a) { y() = a; }
    inline void SetZ(double a) { z() = a; }

    inline ggVector3(const ggVector3 &v) {
         e[0] = v.e[0];
         e[1] = v.e[1];
         e[2] = v.e[2];
    }
    


    inline double tolerance() const { return tol; }
    inline double& tolerance() { return tol; }



    // Operators

    inline const ggVector3& operator+() const { return *this; }
    inline ggVector3 operator-() const { return ggVector3(-e[0], -e[1], -e[2]); }
    inline double operator[](int i) const;
    inline double& operator[](int i) ;

    inline ggVector3& operator+=(const ggVector3 &v2);
    inline ggVector3& operator-=(const ggVector3 &v2);
    inline ggVector3& operator*=(const double t);
    inline ggVector3& operator/=(const double t);


    
    // Miscellaneous Methods

    inline double length() const { return sqrt(e[0]*e[0] + e[1]*e[1] + e[2]*e[2]); }
    inline double squaredLength() const { return e[0]*e[0] + e[1]*e[1] + e[2]*e[2]; }
//    inline ggVector3 normal() const;
//    inline void Normalize();
    inline void MakeUnitVector();
    ggVector3 Perturb(double down, double around) const;
     
    inline double maxComponent() const { return ggMax(e[0],e[1],e[2]);}
    inline double minComponent() const { return ggMin(e[0],e[1],e[2]);}

   inline double maxAbsComponent() const { return e[indexOfMaxAbsComponent()]; }
   inline double minAbsComponent() const { return e[indexOfMinAbsComponent()]; }
    inline  int indexOfMinComponent() const { 
	return (e[0]< e[1] && e[0]< e[2]) ? 0 : (e[1] < e[2] ? 1 : 2);
    }

    inline  int indexOfMinAbsComponent() const {
	if (ggAbs(e[0]) < ggAbs(e[1]) && ggAbs(e[0]) < ggAbs(e[2]))
	    return 0;
	else if (ggAbs(e[1]) < ggAbs(e[2]))
	    return 1;
	else
	    return 2;
    }

    inline  int indexOfMaxComponent() const {
	return (e[0]> e[1] && e[0]> e[2]) ? 0 : (e[1] > e[2] ? 1 : 2);
    }

    inline  int indexOfMaxAbsComponent() const {
	if (ggAbs(e[0]) > ggAbs(e[1]) && ggAbs(e[0]) > ggAbs(e[2]))
	    return 0;
	else if (ggAbs(e[1]) > ggAbs(e[2]))
	    return 1;
	else
	    return 2;
    }


    double e[3];
private:
    static double tol;
};

const ggVector3 ggXAxis(1.0,0.0,0.0);   // obsolete
const ggVector3 ggYAxis(0.0,1.0,0.0);
const ggVector3 ggZAxis(0.0,0.0,1.0);

const ggVector3 ggXAxis3(1.0,0.0,0.0);
const ggVector3 ggYAxis3(0.0,1.0,0.0);
const ggVector3 ggZAxis3(0.0,0.0,1.0);





//  Operators

ggBoolean operator==(const ggVector3 &t1, const ggVector3 &t2);

ggBoolean operator!=(const ggVector3 &t1, const ggVector3 &t2);

istream &operator>>(istream &is, ggVector3 &t);

ostream &operator<<(ostream &os, const ggVector3 &t);

// Addition/Subtraction

ggVector3 operator+(const ggVector3 &v1, const ggVector3 &v2);
ggVector3 operator-(const ggVector3 &v1, const ggVector3 &v2);

// Scalar Product

ggVector3 operator*(double t, const ggVector3 &v);
ggVector3 operator*(const ggVector3 &v, double t);
ggVector3 operator/(const ggVector3 &v, double t);

// Dot Product
//double dot(const ggVector3 &v1, const ggVector3 &v2);
inline double ggDot(const ggVector3 &v1, const ggVector3 &v2);

// Cross Product
//ggVector3 cross(const ggVector3 &v1, const ggVector3 &v2);
inline ggVector3 ggCross(const ggVector3 &v1, const ggVector3 &v2);


// inlines

inline ggVector3 ggUnitVector(const ggVector3& v) {
    double k = 1.0 / sqrt(v.e[0]*v.e[0] + v.e[1]*v.e[1] + v.e[2]*v.e[2]);

    return ggVector3(v.e[0]*k, v.e[1]*k, v.e[2]*k);
}



inline void ggVector3::MakeUnitVector() {
    double k = 1.0 / sqrt(e[0]*e[0] + e[1]*e[1] + e[2]*e[2]);

#ifdef GGSAFE
    if (k <= 0.0) {
	fprintf(stderr, "Vector Length is zero\n");
	exit(1);
    }
#endif

    e[0] *= k; e[1] *= k; e[2] *= k;
}



// Addition/subtraction
inline ggVector3 operator+(const ggVector3 &v1, const ggVector3 &v2) {
    return ggVector3( v1.e[0] + v2.e[0], 
                      v1.e[1] + v2.e[1],
                      v1.e[2] + v2.e[2]);
}

inline ggVector3 operator-(const ggVector3 &v1, const ggVector3 &v2) {
    return ggVector3( v1.e[0] - v2.e[0],
                      v1.e[1] - v2.e[1],
                      v1.e[2] - v2.e[2]);
}

// Scalar Product
inline ggVector3 operator*(double t, const ggVector3 &v) {
    return ggVector3(t*v.e[0], t*v.e[1], t*v.e[2]); 
}

inline ggVector3 operator*(const ggVector3 &v, double t) {
    return ggVector3(t*v.e[0], t*v.e[1], t*v.e[2]); 
}

inline ggVector3 operator/(const ggVector3 &v, double t) {
    double k = 1.0/t;
    return ggVector3(k*v.e[0], k*v.e[1], k*v.e[2]); 
}


// Dot product

inline double ggDot(const ggVector3 &v1, const ggVector3 &v2) {
    return v1.e[0] *v2.e[0] + v1.e[1] *v2.e[1]  + v1.e[2] *v2.e[2];
}

// Cross products


inline ggVector3 ggCross(const ggVector3 &v1, const ggVector3 &v2) {
    return ggVector3( (v1.e[1]*v2.e[2] - v1.e[2]*v2.e[1]),
                      (-(v1.e[0]*v2.e[2] - v1.e[2]*v2.e[0])),
                      (v1.e[0]*v2.e[1] - v1.e[1]*v2.e[0]));
}


// Operators

inline double& ggVector3::operator[](int i) { 

#ifdef GGSAFE
    if ( i < 0 || i > 2 )
    {
	fprintf(stderr, "ggVector3: out of range %d\n", i);
	exit(1);
    }
#endif

    return e[i]; 
}
inline double ggVector3::operator[](int i) const { 

#ifdef GGSAFE
    if ( i < 0 || i > 2 )
    {
	fprintf(stderr, "ggVector3: out of range %d\n", i);
	exit(1);
    }
#endif

    return e[i]; 
}

inline ggVector3& ggVector3::operator+=(const ggVector3 &v){
    e[0]  += v.e[0];
    e[1]  += v.e[1];
    e[2]  += v.e[2];
    return *this;
}

inline ggVector3& ggVector3::operator-=(const ggVector3& v) {
    e[0]  -= v.e[0];
    e[1]  -= v.e[1];
    e[2]  -= v.e[2];
    return *this;
}

inline ggVector3& ggVector3::operator*=(const double t) {
    e[0]  *= t;
    e[1]  *= t;
    e[2]  *= t;
    return *this;
}

inline ggVector3& ggVector3::operator/=(const double t) {
    double k = 1.0/t;

#ifdef GGSAFE
    if (ggAbs(t) <= 0.0) {
	fprintf(stderr, "In ggVector3 :Divide by zero\n");
	exit(1);
    }
#endif
    e[0]  *= k;
    e[1]  *= k;
    e[2]  *= k;
    return *this;
}


#endif
