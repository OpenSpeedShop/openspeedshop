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



#ifndef GGSPLINE_H
#define GGSPLINE_H

// Bob's set of splines.  Originally meant for the Frame Interpolation
// using Quaternions, may be expanded to deal with everything else.

#include <ggPoint3.h>
#include <ggHMatrix3.h>
#include <ggFrame3.h>

#include <ggQuaternion.h>

// Catmull-Rom splines
// For Frame Interpolation, we need an interpolator for ggPoint3's and
// ggONB3's (these are interpolated by multiplying an interpolated 
// quaternion).

ggPoint3 ggCRSpline(const ggPoint3& p0, const ggPoint3& p1,
		    const ggPoint3& p2, const ggPoint3& p3, double t);

ggQuaternion ggCRSpline(const ggQuaternion& q0, const ggQuaternion& q1,
			const ggQuaternion& q2, const ggQuaternion& q3,
			double t);

ggFrame3 ggCRSpline(const ggFrame3& f0, const ggFrame3& f1, 
		    const ggFrame3& f2, const ggFrame3& f3, double t);

inline ggHMatrix3 makeCRHM(void)
{
  ggHMatrix3 r;
  register int i,j;
  r.e[0][0] = -1.0; r.e[0][1] =  3.0; r.e[0][2] = -3.0; r.e[0][3] =  1.0;
  r.e[1][0] =  2.0; r.e[1][1] = -5.0; r.e[1][2] =  4.0; r.e[1][3] = -1.0;
  r.e[2][0] = -1.0; r.e[2][1] =  0.0; r.e[2][2] =  1.0; r.e[2][3] =  0.0;
  r.e[3][0] =  0.0; r.e[3][1] =  2.0; r.e[3][2] =  0.0; r.e[3][3] =  0.0;
  for (i=0;i<4;i++)
    for (j=0;j<4;j++)
      r.e[i][j] *= 0.5;
  return r;
}

// This matrix is used for all flat space Catmull-Rom splines
const ggHMatrix3 CRM(makeCRHM());

#endif
