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





#ifndef GGMACROS_H
#define GGMACROS_H

#include<ggBoolean.h>
#include<ggConstants.h>
#include<assert.h>

template<class T1, class T2>
inline T1 ggBilinearInterpolate(T1 c00, T1 c01,
                                T1 c10, T1 c11, 
                                T2 beta, T2 gamma) {
  T2 r1 = 1 - beta;
  T2 r2 = 1 - gamma;

  return (((r1 * r2) * c00) + 
	  ((beta * r2) * c10) + 
	  ((r1 * gamma) * c01) + 
	  ((beta * gamma) * c11));
}


// ***************************************************************************
//  Min/Max

template <class T1,class T2>
inline T1 ggMin(T1 t1, T2 t2){ return t1 < t2 ? t1 : t2;}

template <class T1,class T2, class T3>
inline T1 ggMin(T1 t1, T2 t2, T3 t3) {
  return t1 < t2 ? (t1 < t3 ? t1 : t3) : (t2 < t3 ? t2 : t3);
}

template <class T1,class T2>
inline T1 ggMax(T1 t1, T2 t2){ return t1 > t2 ? t1 : t2;}

template <class T1,class T2, class T3>
inline T1 ggMax(T1 t1, T2 t2, T3 t3) {
  return t1 > t2 ? (t1 > t3 ? t1 : t3) : (t2 > t3 ? t2 : t3);
} 

template <class T1, class T2>
inline void ggMinMax(T1& min, T2& max) {
  if(min > max) ggSwap(min, max);
}

template <class T1, class T2, class T3>
inline void ggMinMax(T1& min, T2& med, T3& max) {
  if(min < med){
    if(min < max){
      ggMinMax(med, max);
    } else {
      ggSwap(min, max);
      ggSwap(med, max);
    }
  } else {
    if(med < max){
      ggSwap(min, med);
      ggMinMax(med, max);
    } else {
      ggSwap(min, max);
    }
  }
}

template <class T1, class T2>
inline void ggMinMax(T1 x1, T2 x2, T1& min, T1& max ) {
    if (x1 < x2) { min = x1; max = x2; }
    else         { min = x2; max = x1; }
}

template <class T1, class T2, class T3>
inline void ggMinMax(T1 x1, T2 x2, T3 x3, T1& min, T1& max ) {
    if (x1 < x2) {
        if (x1 < x3) {
             min = x1;
             max = ggMax(x2, x3);
        }
        else {
             min = x3;
             max = x2;
        }
    }
    else {
        if (x2 < x3) {
             min = x2;
             max = ggMax(x1, x3);
        }
        else {
             min = x3;
             max = x1;
        }
    }
}


// ***************************************************************************
//  Sign/Abs

template <class T>
inline int ggSign(T t) {return t > 0 ? 1 : t < 0 ? -1 : 0;}

template <class T>
inline T ggAbs(T t){return t < 0 ? -t : t;}

#ifdef NEED_EXPLICIT_SPECIALIZATION
inline double ggAbs(const double t){return t < 0 ? -t : t;}
#endif


// ***************************************************************************
//  Swaps and Transfers

template <class T1, class T2>
inline void ggSwap(T1& t1, T2& t2) {T1 t3 = t1;t1 = t2;t2 = t3;}


// ***************************************************************************
//  Squares and Cubes

template <class T>
inline T ggSqr(T x) { return x*x; }

template <class T>
inline T ggCube(T x) { return x * x * x; }


// ***************************************************************************
//  Conversions

template <class T>
inline T ggDegreesToRadians(T d) {return d * ((2.0 * ggPi)/360.0);}

template <class T>
inline T ggRadiansToDegrees(T r) {return r * (360.0 /(2.0*ggPi));}

// ***************************************************************************
//  Equality


template <class T1>
inline ggBoolean
ggIsZero(T1 n) {return ggAbs(double(n)) < ggTinyEpsilon;}

template <class T1, class T2>
inline ggBoolean
ggIsZero(T1 n,T2 tol) {return ggAbs(double(n)) <= tol;}

#ifdef NEED_EXPLICIT_SPECIALIZATION
inline ggBoolean ggIsZero(double n,const double tol) {return ggAbs(double(n)) <= tol;}
#endif

template <class T1, class T2>
inline ggBoolean
ggEqual(T1 n, T2 m) {return ggIsZero(n - m);}

template <class T1, class T2, class T3>
inline ggBoolean
ggEqual(T1 n, T2 m, T3 tol)  {return ggIsZero(n - m, tol);}


// ***************************************************************************
//  Interpolation -- Linear


template<class T1, class T2>
inline T1 ggLinearInterpolate(T1 c1, T2 c2, T2 t)
{
  return (c1 + t * (c2 - c1));
}

template <class T1, class T2>
inline T1 ggTriangleInterpolate(T1 c0,T1 c1,T1 c2,T2 beta, T2 gamma)
{
#ifdef GGSAFE
  assert((beta + gamma) <= 1.0);
#endif
  return (c0 + (beta * (c1 - c0)) + (gamma * (c2 - c0)));
}

template<class T1, class T2>
inline T1 ggTrilinearInterpolate(T1 p000, T1 p001, T1 p010, T1 p011,
				   T1 p100, T1 p101, T1 p110, T1 p111,
				   T2 u, T2 v, T2 w)
{
  T2 utemp = 1.0 - u;
  T2 vtemp = 1.0 - v;
  T2 wtemp = 1.0 - w;

  return (((utemp * vtemp * wtemp) * p000) +
	  ((utemp * vtemp * w) * p001) +
	  ((utemp * v * wtemp) * p010) + 
	  ((utemp * v * w) * p011) +
	  ((u * vtemp * wtemp) * p100) +
	  ((u * vtemp * w) * p101) +
	  ((u * v * wtemp) * p110) +
	  ((u * v * w) * p111));
}

// ***************************************************************************
//  Interpolation -- Perlin

template<class T1, class T2>
inline T1 ggCubicInterpolate(T1 c0, T1 c1, T2 t)
{
  double s = t * t * (3 - 2 * t);

  return (c0 + s * (c1 - c0));
}


template<class T1, class T2>
inline T1 ggBicubicInterpolate(T1 p00,T1 p01,T1 p10,T1 p11,T2 beta, T2 gamma)
{
  T2 beta1 = beta * beta * (3 - 2 * beta);
  T2 gamma1 = gamma * gamma * (3 - 2 * gamma);
  T2 beta2 = (1.0 - beta1);
  T2 gamma2 = (1.0 - gamma1);

  return (((beta2 * gamma2) * p00) + 
	  ((beta2 * gamma1) * p01) + 
	  ((beta1 * gamma2) * p10) + 
	  ((beta1 * gamma1) * p11));
}

template<class T1, class T2>
inline T1 ggTricubicInterpolate(T1 p000, T1 p001, T1 p010, T1 p011,
				 T1 p100, T1 p101, T1 p110, T1 p111,
				 T2 u, T2 v, T2 w)
{
  T2 u1 = u * u * (3 - 2 * u);
  T2 v1 = v * v * (3 - 2 * v);
  T2 w1 = w * w * (3 - 2 * w);

  T2 u2 = 1.0 - u1;
  T2 v2 = 1.0 - v1;
  T2 w2 = 1.0 - w1;

  return (((u2 * v2 * w2) * p000) +
	  ((u2 * v2 * w1) * p001) +
	  ((u2 * v1 * w2) * p010) + 
	  ((u2 * v1 * w1) * p011) +
	  ((u1 * v2 * w2) * p100) +
	  ((u1 * v2 * w1) * p101) +
	  ((u1 * v1 * w2) * p110) +
	  ((u1 * v1 * w1) * p111));
}





// ***************************************************************************
#endif

