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
// ggHShearMatrix3.h -- shear matrix class definitions
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

#ifndef GGHSHEARMATRIX3_H
#define GGHSHEARMATRIX3_H

#include <ggHAffineMatrix3.h>

//-----------------------------------------------------------------------------
// ggHXYShearMatrix3
//

class ggHXYShearMatrix3 : public ggHAffineMatrix3 {
  public:
    ggHXYShearMatrix3() : ggHAffineMatrix3() {}
    ggHXYShearMatrix3(double x, double y);
};

//
// Friends
//

ggPoint3  operator*(const ggHXYShearMatrix3 &m, const ggPoint3 &p);
ggVector3 operator*(const ggHXYShearMatrix3 &m, const ggVector3 &p);

//-----------------------------------------------------------------------------
// ggHYZShearMatrix3
//

class ggHYZShearMatrix3 : public ggHAffineMatrix3 {
  public:
    ggHYZShearMatrix3() : ggHAffineMatrix3() {}
    ggHYZShearMatrix3(double y, double z);
};

//
// Friends
//

ggPoint3  operator*(const ggHYZShearMatrix3 &m, const ggPoint3 &p);
ggVector3 operator*(const ggHYZShearMatrix3 &m, const ggVector3 &p);

//-----------------------------------------------------------------------------
// ggHZXShearMatrix3
//

class ggHZXShearMatrix3 : public ggHAffineMatrix3 {
  public:
    ggHZXShearMatrix3() : ggHAffineMatrix3() {}
    ggHZXShearMatrix3(double z, double x);
};

//
// Friends
//

ggPoint3  operator*(const ggHZXShearMatrix3 &m, const ggPoint3 &p);
ggVector3 operator*(const ggHZXShearMatrix3 &m, const ggVector3 &p);

#endif
