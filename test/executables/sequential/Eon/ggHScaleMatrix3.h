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
// ggHScaleMatrix3.h -- ggHScaleMatrix3 class definition
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

#ifndef GGHSCALEMATRIX3_H
#define GGHSCALEMATRIX3_H

#include <ggHAffineMatrix3.h>

//-----------------------------------------------------------------------------
// ggHScaleMatrix3
//

class ggHScaleMatrix3 : public ggHAffineMatrix3 {
  public:
    // constructors
    ggHScaleMatrix3() : ggHAffineMatrix3() {}
    ggHScaleMatrix3(const ggHMatrix3& m) : ggHAffineMatrix3(m) { 
#ifdef GGSAFE
	assert (isScale());
#endif
    }
    ggHScaleMatrix3(double x, double y, double z);
    ggHScaleMatrix3(const ggVector3& v);

    // mathematical properties
    ggHMatrix3 inverse() const;
    double determinant() const;

    // scale factors
    double x() const { return e[0][0]; }
    double y() const { return e[1][1]; }
    double z() const { return e[2][2]; }

//    friend ggHScaleMatrix3 
//    operator*(const ggHScaleMatrix3 &m1, const ggHScaleMatrix3 &m2);
};

ggRay3    operator*(const ggHScaleMatrix3 &m, const ggRay3 &r);
ggPoint3  operator*(const ggHScaleMatrix3 &m, const ggPoint3 &v);
ggVector3 operator*(const ggHScaleMatrix3 &m, const ggVector3 &v);
ggONB3    operator*(const ggHScaleMatrix3 &m, const ggONB3 &v);
ggFrame3  operator*(const ggHScaleMatrix3 &m, const ggFrame3 &v);

//ggHScaleMatrix3 
//operator*(const ggHScaleMatrix3 &m1, const ggHScaleMatrix3 &m2);

#endif
