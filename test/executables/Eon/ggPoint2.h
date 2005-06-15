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
// ggPoint2.h-- declarations for class ggPoint2
//
// Author:    Pete Shirley
//            10/30/93
// Modified:  Kwansik Kim
//

#ifndef GGPOINT2_H
#define GGPOINT2_H

#ifdef GGSAFE
#include <stdio.h>
#endif


#include <ggVector2.h>


// Class ggPoint2

class ggPoint2 {
    
public:
    
    //  Constructors/Destructors

    ggPoint2() { e[0] = e[1] = 0.0; }
    ggPoint2(double e0, double e1) {e[0] = e0; e[1] = e1;}
    ggPoint2(ggBoolean) {}

    // Acess methods/operator
    inline void Set(double x, double y) {e[0] = x; e[1] = y;}
    inline double x() const { return e[0]; }
    inline double y() const { return e[1]; }
    inline double u() const { return e[0]; }
    inline double v() const { return e[1]; }
    inline double &x() { return e[0]; }
    inline double &y() { return e[1]; }
    inline double &u() { return e[0]; }
    inline double &v() { return e[1]; }

    ggPoint2(const ggPoint2& p) { e[0]= p.e[0]; e[1] = p.e[1]; }

    inline void SetX(double a) { x() = a; }
    inline void SetY(double a) { y() = a; }
    inline void SetU(double a) { x() = a; }
    inline void SetV(double a) { y() = a; }

    inline double tolerance() const { return tol; }
    inline double& tolerance() { return tol; }



     // Operators
    inline double operator[](int i) const;
    inline double& operator[](int i) ;
    inline ggPoint2 operator-() { return ggPoint2(-e[0], -e[1]); }
    inline ggPoint2& operator+=(const ggVector2 &v);
    inline ggPoint2& operator-=(const ggVector2 &v);

     double e[2];
  private:
    static double  tol;
};


const ggPoint2 ggOrigin2(0.0,0.0);  // obsolete





//
// Operators
//

ggBoolean operator==(const ggPoint2 &t1, const ggPoint2 &t2);

ggBoolean operator!=(const ggPoint2 &t1, const ggPoint2 &t2);

ostream &operator<<(ostream &os, const ggPoint2 &t);

istream &operator>>(istream &is, ggPoint2 &t);


// Subtraction

ggVector2 operator-(const ggPoint2 &p1, const ggPoint2 &p2);


// Addition/subtraction of point and vector

ggPoint2 operator+(const ggPoint2 &p, const ggVector2 &v);

ggPoint2 operator+(const ggVector2 &p, const ggPoint2 &v);

ggPoint2 operator-(const ggPoint2 &p, const ggVector2 &v);


// Inlines

// Subtraction

inline ggVector2 operator-(const ggPoint2 &p1, const ggPoint2 &p2) {
    return ggVector2( p1.e[0] - p2.e[0], p1.e[1] - p2.e[1]);
}

// Addition/subtraction of point and vector

inline ggPoint2 operator+(const ggPoint2 &p, const ggVector2 &v) {
    return ggPoint2( p.e[0] + v.e[0], p.e[1] + v.e[1]);
}

inline ggPoint2 operator+(const ggVector2 &v, const ggPoint2 &p) {
    return ggPoint2( p.e[0] + v.e[0], p.e[1] + v.e[1]);
}

inline ggPoint2 operator-(const ggPoint2 &p, const ggVector2 &v) {
    return ggPoint2( p.e[0] - v.e[0], p.e[1] - v.e[1]);
}

// [] operator

inline double& ggPoint2::operator[](int i) { 

#ifdef GGSAFE
    if (i < 0 || i > 1) 
    {
	fprintf(stderr, "ggPoint3 : out of range :%d\n", i);
	exit(1);
    }
#endif

    return e[i]; 
}

inline double ggPoint2::operator[](int i) const { 

#ifdef GGSAFE
    if (i < 0 || i > 1) 
    {
	fprintf(stderr, "ggPoint3 : out of range :%d\n", i);
	exit(1);
    }
#endif

    return e[i]; 
}


inline ggPoint2& ggPoint2::operator+=(const ggVector2 &v) {
    e[0]  += v.e[0];
    e[1]  += v.e[1];
    return *this;
}

inline ggPoint2& ggPoint2::operator-=(const ggVector2 &v) {
    e[0]  -= v.e[0];
    e[1]  -= v.e[1];
    return *this;
}




#endif

