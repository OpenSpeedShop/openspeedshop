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
// ggHMatrix3.h -- ggHMatrix3 class definition
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

#ifndef GGHMATRIX3_H
#define GGHMATRIX3_H

#include <iostream.h>
#include <ggBoolean.h>
#include <ggONB3.h>
#include <ggRay3.h>
#include <ggPoint3.h>
#include <ggBox3.h>
#include <ggHPoint3.h>
#include <ggFrame3.h>

//-----------------------------------------------------------------------------
// ggHMatrix3
//

class ggHMatrix3 {
public:

    enum InitializationType {Z, I, RX, RY, RZ};

    // constructors
    ggHMatrix3();		     // default is identity matrix
    ggHMatrix3(const ggHMatrix3& m); // copy
    ggHMatrix3(InitializationType);  // initialization type

    enum MatrixType {MATRIX, ZERO, IDENTITY, 
		     AFFINE, SCALE, RIGID_BODY, TRANSLATION, ROTATION};

    // type identification functions (at least useful for debugging)
    ggBoolean isZero() const;
    ggBoolean isIdentity() const;
    ggBoolean isAffine() const;	     // note: this might be difficult to check
    ggBoolean isScale() const;
    ggBoolean isRigidBody() const;
    ggBoolean isTranslation() const;
    ggBoolean isRotation() const;
    MatrixType type() const;

    // mathematical properties
    ggBoolean  isOrthogonal() const; // is upper-left 3x3 submatrix orthogonal?
    double     determinant() const;  // determinant of entire 4x4 matrix
    ggHMatrix3 transpose() const;    // swap rows for columns
    ggHMatrix3 inverse() const;	     // m * m.inverse() == Identity

    // operator*
    friend ggPoint3  operator*(const ggHMatrix3 &m, const ggPoint3 &v);
    friend ggHPoint3 operator*(const ggHMatrix3 &m, const ggHPoint3 &v);
    friend ggVector3 operator*(const ggHMatrix3 &m, const ggVector3 &v);
    friend ggRay3    operator*(const ggHMatrix3 &m, const ggRay3 &v);

    // operator== (exact)
    friend ggBoolean operator==(const ggHMatrix3 &m1, const ggHMatrix3 &m2);
    // approximate equality
    friend ggBoolean 
    ggHMatrix3Equal (const ggHMatrix3 &m1, const ggHMatrix3 &m2, double tol);

    // I/O
    friend ostream & operator<<(ostream &os, const ggHMatrix3 &m); // output
    friend istream & operator>>(istream &is, ggHMatrix3 &m);	   // input
    void print(ostream &os) const; // pretty-print

    // safe read-only data access
    inline double data(int r, int c) const {
#ifdef GGSAFE
	assert (r >= 0 && r <= 3);
	assert (c >= 0 && c <= 3);
#endif
	return e[r][c];
    }

    // safe data access
    inline double& Data(int r, int c) {
#ifdef GGSAFE
	assert (r >= 0 && r <= 3);
	assert (c >= 0 && c <= 3);
#endif
	return e[r][c];
    }

    // data
    double e[4][4];
};

//
// Friends
//

ggPoint3   operator*(const ggHMatrix3 &m, const ggPoint3 &p);
ggHPoint3  operator*(const ggHMatrix3 &m, const ggHPoint3 &p);
ggVector3  operator*(const ggHMatrix3 &m, const ggVector3 &v);
ggRay3     operator*(const ggHMatrix3 &m, const ggRay3 &r);
ggONB3     operator*(const ggHMatrix3 &m, const ggONB3 &v);
ggFrame3   operator*(const ggHMatrix3 &m, const ggFrame3 &v);

ggHMatrix3 operator*(const ggHMatrix3 &m1, const ggHMatrix3 &m2);

ggBoolean  operator==(const ggHMatrix3 &m1, const ggHMatrix3 &m2);
ggBoolean  
ggHMatrix3Equal (const ggHMatrix3 &m1, const ggHMatrix3 &m2, double tol);

ostream &  operator<<(ostream &os, const ggHMatrix3 &m);
istream &  operator>>(istream &is, const ggHMatrix3 &m);

//
// constant matrices
//

// zero and identity

const ggHMatrix3 ggHZeroMatrix3     (ggHMatrix3::Z);
const ggHMatrix3 ggHIdentityMatrix3 (ggHMatrix3::I);

// reflection in x, y, and z directions

const ggHMatrix3 ggHXReflectionMatrix3 (ggHMatrix3::RX);
const ggHMatrix3 ggHYReflectionMatrix3 (ggHMatrix3::RY);
const ggHMatrix3 ggHZReflectionMatrix3 (ggHMatrix3::RZ);

#endif
