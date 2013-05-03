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



// Bob's set of splines.  Originally meant for the Frame Interpolation
// using Quaternions, may be expanded to deal with everything else.

#include <ggHPoint3.h>

#include <ggSpline.h>

// Catmull-Rom splines
// For Frame Interpolation, we need an interpolator for ggPoint3's and
// ggONB3's (these are interpolated by multiplying an interpolated 
// quaternion).

double ggDot(const ggHPoint3 &p0, const ggHPoint3 &p1)
{ return p0.e[X]*p1.e[X]+p0.e[Y]*p1.e[Y]+p0.e[Z]*p1.e[Z]+p0.e[W]*p1.e[W]; }

ggPoint3 ggCRSpline(const ggPoint3& p0, const ggPoint3& p1,
		    const ggPoint3& p2, const ggPoint3& p3, double t)
{
  register int i;
  // Use ggHPoint3 so that we can re-use matrix code
  ggHPoint3 tvec(t*t*t,t*t,t,1);
  ggHPoint3 pvec;
  ggPoint3 presult;

  for (i=X;i<=Z;i++){
    pvec = ggHPoint3(p0.e[i],p1.e[i],p2.e[i],p3.e[i]);
    presult.e[i]= ggDot(tvec,(CRM * pvec));
  }

  return presult;
}

// A spherical space construction of a Catmull-Rom spline using the
// Bezier-esque combinations of linear interpolations to produce
// cubic behavior.
ggQuaternion ggCRSpline(const ggQuaternion& q0, const ggQuaternion& q1,
			const ggQuaternion& q2, const ggQuaternion& q3,
			double t)
{
  ggQuaternion qalpha, qbeta, qA, qB, qC, qD, qE;
  double one_sixth = 1.0/6.0;
  qalpha = ggSlerp(q1, (q2 * q0.getInverse()) * q1, one_sixth);
  qbeta = ggSlerp(q2, (q1 * q3.getInverse()) * q2, one_sixth);
  qA = ggSlerp(q1,qalpha,t);
  qB = ggSlerp(qalpha,qbeta,t);
  qC = ggSlerp(qbeta,q2,t);
  qD = ggSlerp(qA,qB,t);
  qE = ggSlerp(qB,qC,t);
  return ggSlerp(qD,qE,t);
}

// This splines whole frames of reference; thus requiring the above two.
ggFrame3 ggCRSpline(const ggFrame3& f0, const ggFrame3& f1, 
		    const ggFrame3& f2, const ggFrame3& f3, double t)
{
  ggFrame3 result;
  ggQuaternion q0(f0.basis()), q1(f1.basis());
  ggQuaternion q2(f2.basis()), q3(f3.basis()), qresult;

  result.SetOrigin(ggCRSpline(f0.origin(), f1.origin(),
			      f2.origin(), f3.origin(), t));
  qresult = ggCRSpline(q0, q1, q2, q3, t);
  result.SetBasis(qresult.getONB3());
  return result;
}

