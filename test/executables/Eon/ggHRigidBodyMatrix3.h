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
// ggHRigidBodyMatrix3.h -- ggHRigidBodyMatrix3 class definition
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

#ifndef GGHRIGIDBODYMATRIX3_H
#define GGHRIGIDBODYMATRIX3_H

#include <ggHAffineMatrix3.h>

//-----------------------------------------------------------------------------
// ggHRigidBodyMatrix3
//

class ggHRigidBodyMatrix3 : public ggHAffineMatrix3 {

public:

    // constructors
    ggHRigidBodyMatrix3() : ggHAffineMatrix3() {}
    ggHRigidBodyMatrix3(const ggHMatrix3& m) : ggHAffineMatrix3(m) {
// roatates about axis by radians
#ifdef GGSAFE
	assert (isRigidBody());
#endif
    }
    ggHRigidBodyMatrix3(const ggVector3& v, double radians);
};

//    friend ggHRigidBodyMatrix3
//    operator*(const ggHRigidBodyMatrix3& m1, const ggHRigidBodyMatrix3& m2);

//
// Friends
//
ggPoint3  operator*(const ggHRigidBodyMatrix3 &m, const ggPoint3 &v);
ggVector3 operator*(const ggHRigidBodyMatrix3 &m, const ggVector3 &v);

//ggHRigidBodyMatrix3
//operator*(const ggHRigidBodyMatrix3& m1, const ggHRigidBodyMatrix3& m2);

//
// non-Friends
//
ggRay3    operator*(const ggHRigidBodyMatrix3 &m, const ggRay3 &v);
ggONB3    operator*(const ggHRigidBodyMatrix3 &m, const ggONB3 &v);
ggFrame3  operator*(const ggHRigidBodyMatrix3 &m, const ggFrame3 &v);


//-----------------------------------------------------------------------------
//  class ggHCanonicalToFrameMatrix3
//
class ggHCanonicalToFrameMatrix3 : public ggHRigidBodyMatrix3 {
public:
    ggHCanonicalToFrameMatrix3() : ggHRigidBodyMatrix3() {}
    ggHCanonicalToFrameMatrix3(const ggFrame3& f);
};

//
// Friends
//
ggPoint3  operator*(const ggHCanonicalToFrameMatrix3 &m, const ggPoint3 &v);
ggVector3 operator*(const ggHCanonicalToFrameMatrix3 &m, const ggVector3 &v);

//
// non-Friends
//
ggRay3    operator*(const ggHCanonicalToFrameMatrix3 &m, const ggRay3 &v);
ggONB3    operator*(const ggHCanonicalToFrameMatrix3 &m, const ggONB3 &v);
ggFrame3  operator*(const ggHCanonicalToFrameMatrix3 &m, const ggFrame3 &v);


//-----------------------------------------------------------------------------
//  class ggHFrameToCanonicalMatrix3
//
class ggHFrameToCanonicalMatrix3 : public ggHRigidBodyMatrix3 {
public:
    ggHFrameToCanonicalMatrix3() : ggHRigidBodyMatrix3() {}
    ggHFrameToCanonicalMatrix3(const ggFrame3& f);
};

//
// Friends
//
ggPoint3  operator*(const ggHFrameToCanonicalMatrix3 &m, const ggPoint3 &v);
ggVector3 operator*(const ggHFrameToCanonicalMatrix3 &m, const ggVector3 &v);

//
// non-Friends
//
ggRay3    operator*(const ggHFrameToCanonicalMatrix3 &m, const ggRay3 &v);
ggONB3    operator*(const ggHFrameToCanonicalMatrix3 &m, const ggONB3 &v);
ggFrame3  operator*(const ggHFrameToCanonicalMatrix3 &m, const ggFrame3 &v);

#endif
