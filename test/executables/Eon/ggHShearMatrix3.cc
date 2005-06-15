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
// ggHShearMatrix3.C -- shear matrix class member functions
//
// Author:  Greg Vogl, November 24, 1993
//
// Copyright 1993 by Greg Vogl
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
#include <ggHShearMatrix3.h>

//-----------------------------------------------------------------------------
// ggHXYShearMatrix3
//

//
// Constructors
//

ggHXYShearMatrix3::ggHXYShearMatrix3(double x, double y)
: ggHAffineMatrix3()
{
    e[0][2] = x;
    e[1][2] = y;
}


//
// Friends
//


// Matrix * point
ggPoint3 operator*(const ggHXYShearMatrix3 &m, const ggPoint3 &p) {
    return ggPoint3
      (
       m.e[0][2] * p.z() + p.x(),
       m.e[1][2] * p.z() + p.y(),
       p.z()
       );
}

// Matrix * vec
ggVector3 operator*(const ggHXYShearMatrix3 &m, const ggVector3 &p) {
    return ggVector3
      (
       p.z() * m.e[0][2] + p.x(),
       p.z() * m.e[1][2] + p.y(),
       p.z()
       );
}

//-----------------------------------------------------------------------------
// ggHYZShearMatrix3
//

//
// Constructors
//

ggHYZShearMatrix3::ggHYZShearMatrix3(double y, double z)
: ggHAffineMatrix3()
{
    e[1][0] = y;
    e[2][0] = z;
}


//
// Friends
//


// Matrix * point
ggPoint3 operator*(const ggHYZShearMatrix3 &m, const ggPoint3 &p) {
    return ggPoint3
      (
       p.x(),
       p.x() * m.e[1][0] + p.y(),
       p.x() * m.e[2][0] + p.z()
       );
}

// Matrix * vec
ggVector3 operator*(const ggHYZShearMatrix3 &m, const ggVector3 &p) {
    return ggVector3
      (
       p.x(),
       p.x() * m.e[1][0] + p.y(),
       p.x() * m.e[2][0] + p.z()
       );
}

//-----------------------------------------------------------------------------
// ggHZXShearMatrix3
//

//
// Constructors
//

ggHZXShearMatrix3::ggHZXShearMatrix3(double z, double x) 
: ggHAffineMatrix3()
{
    e[2][1] = z;
    e[0][1] = x;
}


//
// Friends
//


// Matrix * point
ggPoint3 operator*(const ggHZXShearMatrix3 &m, const ggPoint3 &p) {
    return ggPoint3
      (
       p.y() * m.e[0][1] + p.x(),
       p.y(),
       p.y() * m.e[2][1] + p.z()
       );
}

// Matrix * vec
ggVector3 operator*(const ggHZXShearMatrix3 &m, const ggVector3 &p) {
    return ggVector3
      (
       p.y() * m.e[0][1] + p.x(),
       p.y(),
       p.y() * m.e[2][1] + p.z()
       );
}
