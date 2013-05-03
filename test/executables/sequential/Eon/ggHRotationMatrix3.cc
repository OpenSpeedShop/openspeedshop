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
// ggHRotationMatrix3.C -- rotation matrix class member functions
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
#include <ggHRotationMatrix3.h>

//-----------------------------------------------------------------------------
// ggHRotationMatrix3
//

//
// Constructors
//

ggHXRotationMatrix3::ggHXRotationMatrix3(double rad)
 : ggHRotationMatrix3()
{
    e[1][1] = cos(rad);
    e[1][2] = -sin(rad);
    e[2][1] = -e[1][2];
    e[2][2] = e[1][1];
}

ggHYRotationMatrix3::ggHYRotationMatrix3(double rad)
 : ggHRotationMatrix3()
{
    e[0][0] = cos(rad);
    e[0][2] = sin(rad);
    e[2][0] = -e[0][2];
    e[2][2] = e[0][0];
}

ggHZRotationMatrix3::ggHZRotationMatrix3(double rad)
 : ggHRotationMatrix3()
{
    e[0][0] = cos(rad);
    e[0][1] = -sin(rad);
    e[1][0] = -e[0][1];
    e[1][1] = e[0][0];
}

ggHCanonicalToBasisRotationMatrix3::ggHCanonicalToBasisRotationMatrix3(
              const ggONB3& uvw) : ggHRotationMatrix3()
{
     e[0][0] = uvw.u().x();
     e[0][1] = uvw.u().y();
     e[0][2] = uvw.u().z();

     e[1][0] = uvw.v().x();
     e[1][1] = uvw.v().y();
     e[1][2] = uvw.v().z();

     e[2][0] = uvw.w().x();
     e[2][1] = uvw.w().y();
     e[2][2] = uvw.w().z();
}

ggHBasisToCanonicalRotationMatrix3::ggHBasisToCanonicalRotationMatrix3(
                const ggONB3& uvw) : ggHRotationMatrix3()
{
     e[0][0] = uvw.u().x();
     e[1][0] = uvw.u().y();
     e[2][0] = uvw.u().z();

     e[0][1] = uvw.v().x();
     e[1][1] = uvw.v().y();
     e[2][1] = uvw.v().z();

     e[0][2] = uvw.w().x();
     e[1][2] = uvw.w().y();
     e[2][2] = uvw.w().z();
}

//
// operator*
//

// Matrix * Point
ggPoint3
operator*(const ggHRotationMatrix3 &m, const ggPoint3 &in) {
    return ggPoint3(
           m.e[0][0] * in.x() + m.e[0][1] * in.y() + m.e[0][2] * in.z(),
           m.e[1][0] * in.x() + m.e[1][1] * in.y() + m.e[1][2] * in.z(),
           m.e[2][0] * in.x() + m.e[2][1] * in.y() + m.e[2][2] * in.z());
}

// Matrix * Vector
ggVector3
operator*(const ggHRotationMatrix3 &m, const ggVector3 &in) {
    return ggVector3(
           m.e[0][0] * in.x() + m.e[0][1] * in.y() + m.e[0][2] * in.z(),
           m.e[1][0] * in.x() + m.e[1][1] * in.y() + m.e[1][2] * in.z(),
           m.e[2][0] * in.x() + m.e[2][1] * in.y() + m.e[2][2] * in.z());
}

// Matrix * Ray
ggRay3
operator*(const ggHRotationMatrix3 &m, const ggRay3 &in) {
    return ggRay3(m * in.origin(), m * in.direction());
}

// Matrix * ONB
ggONB3
operator*(const ggHRotationMatrix3 &m, const ggONB3 &in) {
    return ggONB3(m * in.u(), m * in.v(), m * in.w());
}

// Matrix * Frame
ggFrame3
operator*(const ggHRotationMatrix3 &m, const ggFrame3 &in) {
    return ggFrame3(m * in.origin(), m * in.basis());
}

// Matrix * Matrix
//ggHRotationMatrix3 
//operator*(const ggHRotationMatrix3& m1, const ggHRotationMatrix3& m2) {
//    ggHRotationMatrix3 m(ggHIdentityMatrix3);
//
//    for (int r = 0; r < 3; r++) {
//        for (int c = 0; c < 3; c++) {
//            m.e[r][c] = 0;
//            for (int i = 0; i < 3; i++)
//                m.e[r][c] += m1.e[r][i] * m2.e[i][c];
//        }
//    }
//    return m;
//}
