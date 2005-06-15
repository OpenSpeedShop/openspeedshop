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
// ggHScaleMatrix3.C -- ggHScaleMatrix3 class member functions
//
// Author:    Peter Shirley, October 1993
// Modified:  Greg Vogl, November 24, 1993
//
// Copyright 1993 by Peter Shirley and Greg Vogl
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
//

#include <iostream.h>
#include <math.h>
#include <ggHScaleMatrix3.h>

//-----------------------------------------------------------------------------
// ggHScaleMatrix3
//

//
// Constructors
//

ggHScaleMatrix3::ggHScaleMatrix3(double sx, double sy, double sz)
 : ggHAffineMatrix3()
{
    e[0][0] = sx;
    e[1][1] = sy;
    e[2][2] = sz;
}

ggHScaleMatrix3::ggHScaleMatrix3(const ggVector3& v)
 : ggHAffineMatrix3()
{
    e[0][0] = v.x();
    e[1][1] = v.y();
    e[2][2] = v.z();
}

//
// Mathematical properties
//

double ggHScaleMatrix3::determinant() const {
    return e[0][0] * e[1][1] * e[2][2];
}

ggHMatrix3 ggHScaleMatrix3::inverse() const {
    // avoid dividing by 0
    for (int i=0; i<3; i++)
      if (e[i][i] == 0)
	return ggHZeroMatrix3;

    return ggHScaleMatrix3(1/e[0][0], 1/e[1][1], 1/e[2][2]);
}

//
// Friends
//

// Matrix * Point
ggPoint3 operator*(const ggHScaleMatrix3 &m, const ggPoint3 &in) {
    return ggPoint3(m.e[0][0] * in.x(),
                    m.e[1][1] * in.y(),
                    m.e[2][2] * in.z());
}

// Matrix * Vector
ggVector3 operator*(const ggHScaleMatrix3 &m, const ggVector3 &in) {
    return ggVector3(m.e[0][0] * in.x(),
                     m.e[1][1] * in.y(),
                     m.e[2][2] * in.z());
}

// Matrix * Ray
ggRay3 operator*(const ggHScaleMatrix3 &m, const ggRay3 &in) {
    return ggRay3(m*in.origin(), m*in.direction());
}

// Matrix * ONB
ggONB3 operator*(const ggHScaleMatrix3 &, const ggONB3 &in) {
    return in;
}

// Matrix * Frame
ggFrame3 operator*(const ggHScaleMatrix3 &m, const ggFrame3 &in) {
    return ggFrame3(m * in.origin(), in.basis());
}

// Matrix * Matrix
//ggHScaleMatrix3 
//operator*(const ggHScaleMatrix3 &m1, const ggHScaleMatrix3 &m2) {
//    return ggHScaleMatrix3(m1.e[0][0] * m2.e[0][0],
//			   m1.e[1][1] * m2.e[1][1],
//			   m1.e[2][2] * m2.e[2][2]);
//}
