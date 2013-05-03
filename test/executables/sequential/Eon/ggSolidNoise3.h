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



#ifndef GGSOLIDNOISE3_H
#define GGSOLIDNOISE3_H


#include <math.h>
#include <ggMacros.h>
#include <ggVector3.h>
#include <ggPoint3.h>
#include <ggRanNum.h>

class ggSolidNoise3 {
public:
    int n;
    ggVector3 grad[256];
    int phi[256];
    ggSolidNoise3();
    double noise(const ggPoint3&) const;
    ggVector3 vectorNoise(const ggPoint3&) const;
    ggVector3 vectorTurbulence( const ggPoint3&, int) const ;
    double turbulence(const ggPoint3&, int) const;
    double dturbulence(const ggPoint3&, int, double) const;
    double omega(double) const;
    ggVector3 gamma(int, int, int) const;
    int intGamma(int, int) const;
    double knot(int, int, int, ggVector3&) const;
    ggVector3 vectorKnot(int, int, int, ggVector3&) const;
};


inline double ggSolidNoise3::omega(double t) const {
   t = (t > 0.0)? t : -t;
   return (t < 1.0)?  ((2*t - 3)*t*t  + 1) : 0.0;
}

#define tabs(x) (((x)<0)?-(x):(x))
inline ggVector3 ggSolidNoise3::gamma(int i, int j, int k) const
{
   int idx;
   idx = phi[tabs((long)k)%n];
   idx = phi[tabs((long)(j + idx)) % n];
   idx = phi[tabs((long)(i + idx)) % n];
   return grad[idx];
}

inline double ggSolidNoise3::knot(int i, int j, int k, ggVector3& v) const {
  return ( omega(v.x()) * omega(v.y()) * omega(v.z()) * (ggDot(gamma(i,j,k),v)) );
}

inline ggVector3 ggSolidNoise3::vectorKnot( int i, int j, int k, ggVector3& v)
const {
    return ( omega(v.x()) * omega(v.y()) * omega(v.z()) * gamma(i,j,k) );
}

inline int ggSolidNoise3::intGamma(int i, int j) const {
   int idx;
   idx = phi[tabs((long)(j))%n];
   idx = phi[tabs((long)(i + idx)) % n];
   return idx;
}


#endif
