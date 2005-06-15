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
// ggPoint3.h-- declarations for class ggPoint3
//
// Author:    Pete Shirley
//            10/30/93
// Modified:  Kwansik Kim
// Modified:  Peter Shirley 4/16/94 to speed inlining x() -> e[0] etc
//


#ifndef GGPOINT3_H
#define GGPOINT3_H



#include <stdlib.h>
#include <ggVector3.h>
#include <ggPoint2.h>
#include <ggMacros.h>

#ifdef GGSAFE
#include <stdio.h>
#endif


//  Class ggPoint3


class ggPoint3  {
    
public:
    
    // Constructors/Destructors

    ggPoint3(double e0, double e1, double e2) {
                e[0] = e0; e[1] = e1; e[2] = e2; }
    ggPoint3() { e[0] = e[1] = e[2] = 0.0; }
    ggPoint3(ggBoolean) {}


    // Access Operators/Methods
    
    inline void Set(double a, double b, double c) {e[0] = a; e[1] = b; e[2] = c;}
    inline double x() const { return e[0]; }
    inline double y() const { return e[1]; }
    inline double z() const { return e[2]; }
    inline double &x() { return e[0]; }
    inline double &y() { return e[1]; }
    inline double &z() { return e[2]; }

    ggPoint3(const ggPoint3 &p) {
         e[0] = p.e[0];
         e[1] = p.e[1];
         e[2] = p.e[2];
    }

    inline void SetX(double a) { x() = a; }
    inline void SetY(double a) { y() = a; }
    inline void SetZ(double a) { z() = a; }
    
    double tolerance() const { return tol; }
    double& tolerance() { return tol; }


    // Operators

    inline ggPoint3 operator-() const { return ggPoint3(-x(), -y(), -z()); }
    inline double operator[](int i) const;
    inline double& operator[](int i) ;
    inline ggPoint3& operator+=(const ggVector3 &v);
    inline ggPoint3& operator-=(const ggVector3 &v);


    // Methods

    ggPoint2 project(int i) const ;

    double e[3];
     
    inline double maxComponent() const { return e[indexOfMaxComponent()];}
    inline double minComponent() const { return e[indexOfMinComponent()];}
    inline double maxAbsComponent() const { return e[indexOfMaxAbsComponent()]; }
    inline double minAbsComponent() const { return e[indexOfMinAbsComponent()]; }
    inline int indexOfMinComponent() const { 
        return (e[0]< e[1] && e[0]< e[2]) ? 0 : (e[1] < e[2] ? 1 : 2);
    }
    inline int indexOfMinAbsComponent() const  {
         if (ggAbs(e[0]) < ggAbs(e[1]) && ggAbs(e[0]) < ggAbs(e[2]))
            return 0;
        else if (ggAbs(e[1]) < ggAbs(e[2]))
            return 1;
        else
            return 2;
    }
    inline int indexOfMaxComponent() const {
         return (e[0]> e[1] && e[0]> e[2]) ? 0 : (e[1] > e[2] ? 1 : 2);
    }
    inline int indexOfMaxAbsComponent() const {
         if (ggAbs(e[0]) > ggAbs(e[1]) && ggAbs(e[0]) > ggAbs(e[2]))
            return 0;
        else if (ggAbs(e[1]) > ggAbs(e[2]))
            return 1;
        else
            return 2;
    }
private:
     static double tol;
};


const ggPoint3 ggOrigin(0.0,0.0,0.0);  // obsolete
const ggPoint3 ggOrigin3(0.0,0.0,0.0);  // obsolete

inline ggPoint3 ggWeightedAverage(const ggPoint3& p0, const ggPoint3& p1,
                         double alpha, double beta) {
#ifdef GGSAFE
   assert(ggEqual(alpha+beta, 1.0));
#endif
    return ggPoint3(alpha*p0.e[0]+ beta*p1.e[0],
                    alpha*p0.e[1]+ beta*p1.e[1],
                    alpha*p0.e[2]+ beta*p1.e[2]);
}

inline ggPoint3 ggWeightedAverage(const ggPoint3& p0, const ggPoint3& p1,
                         const ggPoint3& p2, double alpha,
                         double beta, double gamma) {
#ifdef GGSAFE
   assert(ggEqual(alpha+beta+gamma, 1.0) );
#endif
    return ggPoint3(alpha*p0.e[0]+ beta*p1.e[0]+ gamma*p2.e[0],
                    alpha*p0.e[1]+ beta*p1.e[1]+ gamma*p2.e[1],
                    alpha*p0.e[2]+ beta*p1.e[2]+ gamma*p2.e[2]);
}


//============================
// Operator declaration
//===========================

// Subtraction

ggVector3 operator-(const ggPoint3 &p1, const ggPoint3 &p2);

// Addition/subtraction of point and vector

ggPoint3 operator+(const ggPoint3 &p, const ggVector3 &v);
ggPoint3 operator+(const ggVector3 &p, const ggPoint3 &v);
ggPoint3 operator-(const ggPoint3 &p, const ggVector3 &v);

ggBoolean operator==(const ggPoint3 &t1, const ggPoint3 &t2);

ggBoolean operator!=(const ggPoint3 &t1, const ggPoint3 &t2);

ostream &operator<<(ostream &os, const ggPoint3 &t);

istream &operator>>(istream &is, ggPoint3 &t);


// inlines

inline ggVector3 operator-(const ggPoint3 &p1, const ggPoint3 &p2) {
    return ggVector3( p1.e[0] - p2.e[0], 
                      p1.e[1] - p2.e[1],
                      p1.e[2] - p2.e[2]);
}

// Addition/subtraction of point and vector

inline ggPoint3 operator+(const ggPoint3 &p, const ggVector3 &v) {
    return ggPoint3( p.e[0] + v.e[0],
                     p.e[1] + v.e[1],
                     p.e[2] + v.e[2]);
}

inline ggPoint3 operator+(const ggVector3 &v, const ggPoint3 &p) {
    return ggPoint3( p.e[0] + v.e[0],
                     p.e[1] + v.e[1],
                     p.e[2] + v.e[2]);
}

inline ggPoint3 operator-(const ggPoint3 &p, const ggVector3 &v) {
    return ggPoint3( p.e[0] - v.e[0],
                     p.e[1] - v.e[1],
                     p.e[2] - v.e[2]);
}


// [] operator

inline double& ggPoint3::operator[](int i) { 

#ifdef GGSAFE
    if ( i < 0 || i > 2 ) 
    {
	fprintf(stderr, "ggPoint3 : out of range :%d\n", i);
	exit(1);
    }
#endif

    return e[i]; 
}
inline double ggPoint3::operator[](int i) const { 

#ifdef GGSAFE
    if ( i < 0 || i > 2 ) 
    {
	fprintf(stderr, "ggPoint3 : out of range :%d\n", i);
	exit(1);
    }
#endif

    return e[i]; 
}

inline ggPoint3& ggPoint3::operator+=(const ggVector3 &v) {
    e[0]  += v.e[0];
    e[1]  += v.e[1];
    e[2]  += v.e[2];
    return *this;
}

inline ggPoint3& ggPoint3::operator-=(const ggVector3 &v) {
    e[0]  -= v.e[0];
    e[1]  -= v.e[1];
    e[2]  -= v.e[2];
    return *this;
}


#endif

