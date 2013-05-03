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
// ggHTranslationMatrix3.C -- ggHTranslationMatrix3 class member functions
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
#include <ggHTranslationMatrix3.h>

//-----------------------------------------------------------------------------
// ggHTranslationMatrix3
//

//
// Constructors
//

ggHTranslationMatrix3::ggHTranslationMatrix3(const ggVector3& v)
 : ggHRigidBodyMatrix3()
{
    e[0][3] = v.x();
    e[1][3] = v.y();
    e[2][3] = v.z();
}

ggHTranslationMatrix3::ggHTranslationMatrix3(double tx, double ty, double tz)
 : ggHRigidBodyMatrix3()
{
    e[0][3] = tx;
    e[1][3] = ty;
    e[2][3] = tz;
}

//
// mathematical properties
//

ggHMatrix3 ggHTranslationMatrix3::inverse() const {
    return ggHTranslationMatrix3(-e[0][3], -e[1][3], -e[2][3]);
}

//
// operator*
//

// Matrix * Point
ggPoint3 operator*(const ggHTranslationMatrix3 &m, const ggPoint3 &p) {
     return ggPoint3( p.x() + m.e[0][3],
                      p.y() + m.e[1][3],
                      p.z() + m.e[2][3]);
}

// Matrix * Vector
ggVector3 operator*(const ggHTranslationMatrix3 &, const ggVector3 &in) {
    return in;
}

// Matrix * Ray
ggRay3 operator*(const ggHTranslationMatrix3 &m, const ggRay3 &in) {
    return ggRay3(m*in.origin(), m*in.direction());
}

// Matrix * ONB
ggONB3 operator*(const ggHTranslationMatrix3 &, const ggONB3 &in) {
    return in;
}

// Matrix * Frame
ggFrame3 operator*(const ggHTranslationMatrix3 &m, const ggFrame3 &in) {
    return ggFrame3(m * in.origin(), in.basis());
}

// Matrix * Matrix
//ggHTranslationMatrix3
//operator*(const ggHTranslationMatrix3 &m1, const ggHTranslationMatrix3 &m2) {
//    return ggHTranslationMatrix3(m1.e[0][3] + m2.e[0][3], 
//				 m1.e[1][3] + m2.e[1][3], 
//				 m1.e[2][3] + m2.e[2][3]);
//}
