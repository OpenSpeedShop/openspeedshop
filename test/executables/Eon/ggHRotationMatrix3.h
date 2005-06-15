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
// ggHRotationMatrix3.h -- rotation matrix class definitions
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

#ifndef GGHROTATIONMATRIX3_H
#define GGHROTATIONMATRIX3_H

#include <ggHRigidBodyMatrix3.h>

//-----------------------------------------------------------------------------
// ggHRotationMatrix3
//

class ggHRotationMatrix3 : public ggHRigidBodyMatrix3 {
public:
    ggHRotationMatrix3() : ggHRigidBodyMatrix3() {}
    ggHRotationMatrix3(const ggHMatrix3& m) : ggHRigidBodyMatrix3 (m) {
#ifdef GGSAFE
	assert (isRotation());
#endif
    }

//    friend ggHRotationMatrix3 
//    operator*(const ggHRotationMatrix3& m1, const ggHRotationMatrix3& m2);
};

ggPoint3  operator*(const ggHRotationMatrix3 &m, const ggPoint3 &p);
ggVector3 operator*(const ggHRotationMatrix3 &m, const ggVector3 &v);
ggRay3    operator*(const ggHRotationMatrix3 &m, const ggRay3 &r);
ggONB3    operator*(const ggHRotationMatrix3 &m, const ggONB3 &o);
ggFrame3  operator*(const ggHRotationMatrix3 &m, const ggFrame3 &f);

//ggHRotationMatrix3 
//operator*(const ggHRotationMatrix3& m1, const ggHRotationMatrix3& m2);


//-----------------------------------------------------------------------------
// ggHXRotationMatrix3
//

class ggHXRotationMatrix3 : public ggHRotationMatrix3 {
public:
    ggHXRotationMatrix3() : ggHRotationMatrix3() {}
    ggHXRotationMatrix3(double theta);
};

//-----------------------------------------------------------------------------
// ggHYRotationMatrix3
//

class ggHYRotationMatrix3 : public ggHRotationMatrix3 {
public:
    ggHYRotationMatrix3() : ggHRotationMatrix3() {}
    ggHYRotationMatrix3(double theta);
};

//-----------------------------------------------------------------------------
// ggHZRotationMatrix3
//

class ggHZRotationMatrix3 : public ggHRotationMatrix3 {
public:
    ggHZRotationMatrix3() : ggHRotationMatrix3() {}
    ggHZRotationMatrix3(double theta);
};

//-----------------------------------------------------------------------------
// ggHCanonicalToBasisRotationMatrix3
//

class ggHCanonicalToBasisRotationMatrix3 : public ggHRotationMatrix3 {
public:
    ggHCanonicalToBasisRotationMatrix3() : ggHRotationMatrix3() {}
    ggHCanonicalToBasisRotationMatrix3(const ggONB3& uvw);
};

//-----------------------------------------------------------------------------
// ggHBasisToCanonicalRotationMatrix3
//

class ggHBasisToCanonicalRotationMatrix3 : public ggHRotationMatrix3 {
public:
    ggHBasisToCanonicalRotationMatrix3() : ggHRotationMatrix3() {}
    ggHBasisToCanonicalRotationMatrix3(const ggONB3& uvw);
};

#endif

