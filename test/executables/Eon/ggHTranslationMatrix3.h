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
// ggHTranslationMatrix3.h -- ggHTranslationMatrix3 class definition
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

#ifndef GGHTRANSLATIONMATRIX3_H
#define GGHTRANSLATIONMATRIX3_H

#include <ggHRigidBodyMatrix3.h>

//-----------------------------------------------------------------------------
// ggHTranslationMatrix3
//

class ggHTranslationMatrix3 : public ggHRigidBodyMatrix3 {

  public:
    // constructors
    ggHTranslationMatrix3() : ggHRigidBodyMatrix3() {}
    ggHTranslationMatrix3(const ggHMatrix3& m) : ggHRigidBodyMatrix3(m) {
#ifdef GGSAFE
	assert(isTranslation());
#endif
    }
    ggHTranslationMatrix3(const ggVector3& v);
    ggHTranslationMatrix3(double x, double y, double z);

    double x() const { return e[0][3]; }
    double y() const { return e[1][3]; }
    double z() const { return e[2][3]; }

    ggHMatrix3 inverse() const;
    double determinant() const { return 1; }

//    friend ggHTranslationMatrix3
//    operator*(const ggHTranslationMatrix3 &m1, const ggHTranslationMatrix3 &m2);
};

ggPoint3  operator*(const ggHTranslationMatrix3 &m, const ggPoint3 &v);
ggVector3 operator*(const ggHTranslationMatrix3 &m, const ggVector3 &v);
ggRay3    operator*(const ggHTranslationMatrix3 &m, const ggRay3 &r);
ggONB3    operator*(const ggHTranslationMatrix3 &m, const ggONB3 &v);
ggFrame3  operator*(const ggHTranslationMatrix3 &m, const ggFrame3 &v);

//ggHTranslationMatrix3
//operator*(const ggHTranslationMatrix3 &m1, const ggHTranslationMatrix3 &m2);

#endif
