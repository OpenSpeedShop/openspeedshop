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



#ifndef GGSOLIDNOISE2_H
#define GGSOLIDNOISE2_H


#include <math.h>
#include <ggMacros.h>
#include <ggVector2.h>
#include <ggPoint2.h>
#include <ggRanNum.h>

class ggSolidNoise2 {
public:
    int n;
    ggVector2 grad[256];
    int phi[256];
    ggSolidNoise2();
    double noise(const ggPoint2&);
    ggVector2 vectorNoise(const ggPoint2&) ;
    ggVector2 vectorTurbulence(
                                   const ggPoint2&, int) ;
    double turbulence(const ggPoint2&, int) ;
    double dturbulence(const ggPoint2&, int, double) ;
    double omega(double) ;
    ggVector2 gamma(int, int);
    int intGamma(int, int) ;
    double knot(int, int, ggVector2&) ;
    ggVector2 vectorKnot( int, int, ggVector2&) ;
};


inline double ggSolidNoise2::omega(double t) 
{
   t = (t > 0.0)? t : -t;
   return (t < 1.0)?  (2*t*t*t - 3*t*t + 1) : 0.0;
}

#define tabs(x) (((x)<0)?-(x):(x))

inline ggVector2 ggSolidNoise2::gamma(int i, int j)
{
   int idx;
   idx = phi[tabs((long)j)%n];
   idx = phi[tabs((long)(i + idx)) % n];
   return grad[idx];
}

inline double ggSolidNoise2::knot(int i, int j, ggVector2& v)
{
  return ( omega(v.x()) * omega(v.y()) * (ggDot(gamma(i,j),  v)) );
}

inline ggVector2 ggSolidNoise2::vectorKnot( int i, int j, ggVector2& v) {
    return ( omega(v.x()) * omega(v.y())  * gamma(i,j) );
}

inline int ggSolidNoise2::intGamma(int i, int j)
{
   int idx;
   idx = phi[tabs((long)j)%n];
   idx = phi[tabs((long)(i + idx)) % n];
   return idx;
}


#endif
