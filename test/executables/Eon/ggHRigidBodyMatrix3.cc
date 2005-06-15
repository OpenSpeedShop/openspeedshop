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
// ggHRigidBodyMatrix3.C -- ggHRigidBodyMatrix3 class member functions
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
#include <ggHRigidBodyMatrix3.h>
#include <ggHRotationMatrix3.h>
#include <ggHTranslationMatrix3.h>

//-----------------------------------------------------------------------------
// ggHRigidBodyMatrix3
//

// Matrix * Point
ggPoint3 operator*(const ggHRigidBodyMatrix3 &m, const ggPoint3 &p) {
    return ggPoint3(
       m.e[0][0] * p.x() + m.e[0][1] * p.y() + m.e[0][2] * p.z() + m.e[0][3],
       m.e[1][0] * p.x() + m.e[1][1] * p.y() + m.e[1][2] * p.z() + m.e[1][3],
       m.e[2][0] * p.x() + m.e[2][1] * p.y() + m.e[2][2] * p.z() + m.e[2][3]);
}

// Matrix * Vector
ggVector3
operator*(const ggHRigidBodyMatrix3 &m, const ggVector3 &p) {
    return ggVector3(
         m.e[0][0] * p.x() + m.e[0][1] * p.y() + m.e[0][2] * p.z(),
         m.e[1][0] * p.x() + m.e[1][1] * p.y() + m.e[1][2] * p.z(),
         m.e[2][0] * p.x() + m.e[2][1] * p.y() + m.e[2][2] * p.z());
}

// Matrix * Ray
ggRay3 operator*(const ggHRigidBodyMatrix3 &m, const ggRay3 &in) {
    return ggRay3(m * in.origin(), m * in.direction());
}

// Matrix * ONB
ggONB3
operator*(const ggHRigidBodyMatrix3 &m, const ggONB3 &in) {
    return ggONB3(m * in.u(), m * in.v(), m * in.w());
}

// Matrix * Frame
ggFrame3 operator*(const ggHRigidBodyMatrix3 &m, const ggFrame3 &in) {
    return ggFrame3(m * in.origin(), m * in.basis());
}

// Matrix * Matrix
//ggHRigidBodyMatrix3
//operator*(const ggHRigidBodyMatrix3& m1, const ggHRigidBodyMatrix3& m2) {
//    ggHRigidBodyMatrix3 m(ggHIdentityMatrix3);
//
//    for (int r = 0; r < 3; r++) {
//        for (int c = 0; c < 4; c++) {
//            m.e[r][c] = 0;
//            for (int i = 0; i < 4; i++)
//                m.e[r][c] += m1.e[r][i] * m2.e[i][c];
//        }
//    }
//    return m;
//}

//-----------------------------------------------------------------------------
// ggHFrameToCanonicalMatrix3
//

//
// Constructors
//

ggHFrameToCanonicalMatrix3::ggHFrameToCanonicalMatrix3(const ggFrame3& f) {

      ggHBasisToCanonicalRotationMatrix3 rot(f.basis());
      ggHTranslationMatrix3 t(+f.origin().x(), 
                              +f.origin().y(), 
                              +f.origin().z());
      ggHRigidBodyMatrix3 m = t * rot; 

      for (int r = 0; r < 4; r++)
          for (int c = 0; c < 4; c++)
              e[r][c] = m.e[r][c];
}

// Matrix * Point
ggPoint3 operator*(const ggHFrameToCanonicalMatrix3 &m, const ggPoint3 &p) {
    return ggPoint3(
       m.e[0][0] * p.x() + m.e[0][1] * p.y() + m.e[0][2] * p.z() + m.e[0][3],
       m.e[1][0] * p.x() + m.e[1][1] * p.y() + m.e[1][2] * p.z() + m.e[1][3],
       m.e[2][0] * p.x() + m.e[2][1] * p.y() + m.e[2][2] * p.z() + m.e[2][3] );
}

// Matrix * Vector
ggVector3 operator*(const ggHFrameToCanonicalMatrix3 &m, const ggVector3 &p) {
    return ggVector3(
       m.e[0][0] * p.x() + m.e[0][1] * p.y() + m.e[0][2] * p.z(),
       m.e[1][0] * p.x() + m.e[1][1] * p.y() + m.e[1][2] * p.z(),
       m.e[2][0] * p.x() + m.e[2][1] * p.y() + m.e[2][2] * p.z());
}

// Matrix * Ray
ggRay3 operator*(const ggHFrameToCanonicalMatrix3 &m, const ggRay3 &in) {
    ggRay3 r;
    r.Set( m * in.origin(), m * in.direction());
    return r;
}

// Matrix * ONB
ggONB3
operator*(const ggHFrameToCanonicalMatrix3 &m, const ggONB3 &in) {
    return ggONB3(m * in.u(), m * in.v(), m * in.w());
}

// Matrix * Frame
ggFrame3 operator*(const ggHFrameToCanonicalMatrix3 &m, const ggFrame3 &in) {
    return ggFrame3(m * in.origin(), m * in.basis());
}

//-----------------------------------------------------------------------------
// ggHCanonicalToFrameMatrix3
//

//
// Constructors
//

ggHCanonicalToFrameMatrix3::ggHCanonicalToFrameMatrix3(const ggFrame3& f) {

      ggHCanonicalToBasisRotationMatrix3 rot(f.basis());
      ggHTranslationMatrix3 t(-f.origin().x(), 
                              -f.origin().y(), 
                              -f.origin().z());
      ggHRigidBodyMatrix3 m = rot * t;

      for (int r = 0; r < 4; r++)
          for (int c = 0; c < 4; c++)
              e[r][c] = m.e[r][c];
}

// Matrix * Point
ggPoint3
operator*(const ggHCanonicalToFrameMatrix3 &m, const ggPoint3 &p) {
    return ggPoint3(
       m.e[0][0] * p.x() + m.e[0][1] * p.y() + m.e[0][2] * p.z() + m.e[0][3],
       m.e[1][0] * p.x() + m.e[1][1] * p.y() + m.e[1][2] * p.z() + m.e[1][3],
       m.e[2][0] * p.x() + m.e[2][1] * p.y() + m.e[2][2] * p.z() + m.e[2][3] );
}

// Matrix * Vector
ggVector3
operator*(const ggHCanonicalToFrameMatrix3 &m, const ggVector3 &p) {
    return ggVector3(
       m.e[0][0] * p.x() + m.e[0][1] * p.y() + m.e[0][2] * p.z(),
       m.e[1][0] * p.x() + m.e[1][1] * p.y() + m.e[1][2] * p.z(),
       m.e[2][0] * p.x() + m.e[2][1] * p.y() + m.e[2][2] * p.z() );
}

// Matrix * Ray
ggRay3 operator*(const ggHCanonicalToFrameMatrix3 &m, const ggRay3 &in) {
    ggRay3 r;
    r.Set( m * in.origin(), m * in.direction());
    return r;
}

// Matrix * ONB
ggONB3 operator*(const ggHCanonicalToFrameMatrix3 &m, const ggONB3 &in) {
    return ggONB3(m * in.u(), m * in.v(), m * in.w());
}

// Matrix * Frame
ggFrame3 operator*(const ggHCanonicalToFrameMatrix3 &m, const ggFrame3 &in) {
    return ggFrame3(m * in.origin(), m * in.basis());
}



//-----------------------------------------------------------------------------
// ggHRotateAboutAxisMatrix3
//

ggHRigidBodyMatrix3::ggHRigidBodyMatrix3(const ggVector3& axis, double angle) {
        ggONB3 uvw;
        uvw.InitFromW(axis);
        ggHCanonicalToBasisRotationMatrix3 Mto(uvw);
        ggHBasisToCanonicalRotationMatrix3 Mfrom(uvw);
        ggHZRotationMatrix3 Mr(angle);
        *this = Mfrom * Mr * Mto;
}
