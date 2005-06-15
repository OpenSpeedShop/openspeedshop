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
// ggHMatrix3.C -- ggHMatrix3 class member functions
//
// Authors:  Peter Shirley, October 1993 and Greg Vogl, November 24, 1993
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
#include <ggHMatrix3.h>

//-----------------------------------------------------------------------------
// ggHMatrix3
//

//
// Constructors
//

// default constructor is identity matrix
ggHMatrix3::ggHMatrix3() {
    for (int r = 0; r < 4; r++)
      for (int c = 0; c < 4; c++)
	e[r][c] = r == c;
}

// copy constructor
ggHMatrix3::ggHMatrix3(const ggHMatrix3& m) {
    for (int r = 0; r < 4; r++)
      for (int c = 0; c < 4; c++)
	e[r][c] = m.e[r][c];
}

// initialization constructor
ggHMatrix3::ggHMatrix3(InitializationType it) {
    if (it == Z) {
      for (int r = 0; r < 4; r++)
	for (int c = 0; c < 4; c++)
	  e[r][c] = 0;
    } else {
      for (int r = 0; r < 4; r++) 
	for (int c = 0; c < 4; c++)
	  e[r][c] = r == c;
    }
    switch (it) {
      case RX: e[0][0] = -1; break;
      case RY: e[1][1] = -1; break;
      case RZ: e[2][2] = -1; break;
      default: break;
    }
}

//
// Type identification functions
//

ggBoolean ggHMatrix3::isZero() const {
    return ggHMatrix3Equal (*this, ggHZeroMatrix3, ggEpsilon);
}

ggBoolean ggHMatrix3::isIdentity() const {
    return ggHMatrix3Equal (*this, ggHIdentityMatrix3, ggEpsilon);
}

ggBoolean ggHMatrix3::isAffine() const { 
    for (int c=0; c<3; c++)
      if (e[3][c] != 0)
	return ggFalse;
    // some other check needed here

    return ggBoolean (e[3][3] == 1);
}

ggBoolean ggHMatrix3::isScale() const { 
    for (int r=0; r<4; r++)
      for (int c=0; c<4; c++)
	if (e[r][c] != 0 && r != c)
	  return ggFalse;

    return ggBoolean (e[3][3] == 1);
}

ggBoolean ggHMatrix3::isRigidBody() const { 
    for (int c=0; c<3; c++)
      if (e[3][c] != 0)
	return ggFalse;
    if (e[3][3] != 1)
      return ggFalse;

    return isOrthogonal();
}

ggBoolean ggHMatrix3::isTranslation() const { 
    for (int r=0; r<4; r++)
      for (int c=0; c<3; c++)
	if (e[r][c] != (r==c ? 1 : 0))
	  return ggFalse;

    return ggBoolean (e[3][3] == 1);
}

ggBoolean ggHMatrix3::isRotation() const { 
    for (int c=0; c<3; c++)
      if (e[3][c] != 0)
	return ggFalse;
    for (int r=0; r<3; r++)
      if (e[r][3] != 0)
	return ggFalse;
    if (e[3][3] != 1)
      return ggFalse;

    return isOrthogonal();
}

ggHMatrix3::MatrixType ggHMatrix3::type() const { 
    if (ggHMatrix3Equal (*this, ggHZeroMatrix3, ggEpsilon))
      return ZERO;
    if (ggHMatrix3Equal (*this, ggHIdentityMatrix3, ggEpsilon))
      return IDENTITY;
    if (isTranslation())
      return TRANSLATION;
    if (isRotation())
      return ROTATION;
    if (isRigidBody())
      return RIGID_BODY;
    if (isScale())
      return SCALE;
    if (isAffine())
      return AFFINE;

    return MATRIX;
}

//
// mathematical properties
//

ggBoolean ggHMatrix3::isOrthogonal() const { 
    ggVector3 rows[3], cols[3];

    // check that rows and cols have length 1 and are mutually perpendicular 
    for (int r=0; r<3; r++)
      for (int c=0; c<3; c++) {
	  rows[r].e[c] = e[r][c];
	  cols[c].e[r] = e[r][c];
      }
    int kd;			// kronecker delta
    for (int i=0; i<3; i++)
      for (int j=0; j<3; j++) {
	  kd = i==j;
	  if (!ggEqual (ggDot(rows[i], rows[j]), kd, ggEpsilon) || 
	      !ggEqual (ggDot(cols[i], cols[j]), kd, ggEpsilon))
	    return ggFalse;
      }

    return ggTrue;
}

double ggHMatrix3::determinant() const {
    return
      e[0][0] *
      (e[1][1] * (e[2][2] * e[3][3] - e[2][3] * e[3][2]) +
       e[1][2] * (e[2][3] * e[3][1] - e[2][1] * e[3][3]) +
       e[1][3] * (e[2][1] * e[3][2] - e[2][2] * e[3][1]))
      - e[0][1] *
      (e[1][0] * (e[2][2] * e[3][3] - e[2][3] * e[3][2]) +
       e[1][2] * (e[2][3] * e[3][0] - e[2][0] * e[3][3]) +
       e[1][3] * (e[2][0] * e[3][2] - e[2][2] * e[3][0]))
      + e[0][2] *
      (e[1][0] * (e[2][1] * e[3][3] - e[2][3] * e[3][1]) +
       e[1][1] * (e[2][3] * e[3][0] - e[2][0] * e[3][3]) +
       e[1][3] * (e[2][0] * e[3][1] - e[2][1] * e[3][0]))
      - e[0][3] *
      (e[1][0] * (e[2][1] * e[3][2] - e[2][2] * e[3][1]) +
       e[1][1] * (e[2][2] * e[3][0] - e[2][0] * e[3][2]) +
       e[1][2] * (e[2][0] * e[3][1] - e[2][1] * e[3][0]));
}

ggHMatrix3 ggHMatrix3::transpose() const {
    ggHMatrix3 m;

    for (int r=0; r<4; r++)
      for (int c=0; c<4; c++)
	m.e[r][c] = e[c][r];

    return m;
}

ggHMatrix3 ggHMatrix3::inverse() const {
    double d = determinant();

    // Should GGSAFE be used here?
    // The cost of this check to avoid dividing by zero is negligible.
    // But the user might want to know that the matrix has no inverse.
#ifdef GGSAFE
    assert (d != 0);
#endif
    if (d == 0)
      return ggHZeroMatrix3;

    // put big calculation here: 18 e components for each of 16 m.e components!
    // To save time, Maple was used to do the calculation
    // copy and paste was used to get the numbers from Maple
    // search and replace was used to format from Maple output into C++ code
    double 
         a1 = e[1][2],
	 a2 = e[0][3],
	 a3 = e[2][1],
	 a4 = e[3][0],
	 a5 = e[1][3],
	 a6 = e[0][2],
	 a7 = e[2][2],
	 a8 = e[1][1],
	 a9 = e[2][3],
	a10 = e[0][1],
	a11 = e[3][1],
	a12 = e[2][0],
	a13 = e[3][2],
	a14 = e[3][3],
	a15 = e[1][0],
	a16 = e[0][0];

    ggHMatrix3 m;
    m.e[0][0] = a8*a7*a14 - a8*a9*a13 - a3*a1*a14 + a3*a5*a13 + a11*a1*a9 - a11*a5*a7;
    m.e[0][1] = -(a10*a7*a14 - a10*a9*a13 - a3*a6*a14 + a3*a2*a13 + a11*a6*a9 - a11*a2*a7);
    m.e[0][2] = a10*a1*a14 - a10*a5*a13 - a8*a6*a14 + a8*a2*a13 + a11*a6*a5 - a11*a2*a1;
    m.e[0][3] = -(a10*a1*a9 - a10*a5*a7 - a8*a6*a9 + a8*a2*a7 + a3*a6*a5 - a3*a2*a1);
    m.e[1][0] = -(a15*a7*a14 - a15*a9*a13 - a12*a1*a14 + a12*a5*a13 + a4*a1*a9 - a4*a5*a7);
    m.e[1][1] = a16*a7*a14 - a16*a9*a13 - a12*a6*a14 + a12*a2*a13 + a4*a6*a9 - a4*a2*a7;
    m.e[1][2] = -(a16*a1*a14 - a16*a5*a13 - a15*a6*a14 + a15*a2*a13 + a4*a6*a5 - a4*a2*a1);
    m.e[1][3] = a16*a1*a9 - a16*a5*a7 - a15*a6*a9 + a15*a2*a7 + a12*a6*a5 - a12*a2*a1;
    m.e[2][0] = a15*a3*a14 - a15*a9*a11 - a12*a8*a14 + a12*a5*a11 + a4*a8*a9 - a4*a5*a3;
    m.e[2][1] = -(a16*a3*a14 - a16*a9*a11 - a12*a10*a14 + a12*a2*a11 + a4*a10*a9 - a4*a2*a3);
    m.e[2][2] = a16*a8*a14 - a16*a5*a11 - a15*a10*a14 + a15*a2*a11 + a4*a10*a5 - a4*a2*a8;
    m.e[2][3] = -(a16*a8*a9 - a16*a5*a3 - a15*a10*a9 + a15*a2*a3 + a12*a10*a5 - a12*a2*a8);
    m.e[3][0] = -(a15*a3*a13 - a15*a7*a11 - a12*a8*a13 + a12*a1*a11 + a4*a8*a7 - a4*a1*a3);
    m.e[3][1] = a16*a3*a13 - a16*a7*a11 - a12*a10*a13 + a12*a6*a11 + a4*a10*a7 - a4*a6*a3;
    m.e[3][2] = -(a16*a8*a13 - a16*a1*a11 - a15*a10*a13 + a15*a6*a11 + a4*a10*a1 - a4*a6*a8);
    m.e[3][3] = a16*a8*a7 - a16*a1*a3 - a15*a10*a7 + a15*a6*a3 + a12*a10*a1 - a12*a6*a8;

    // divide each matrix element by the determinant
    d = 1/d;			// in case * is faster than /
    for (int r=0; r<4; r++)
      for (int c=0; c<4; c++)
	m.e[r][c] *= d;

    return m;
}

//
// operator*
//

// Matrix * Point
ggPoint3 operator*(const ggHMatrix3 &m, const ggPoint3 &p) {
ggHPoint3 q(
         m.e[0][0] * p.x() + m.e[0][1] * p.y() + m.e[0][2] * p.z() + m.e[0][3],
         m.e[1][0] * p.x() + m.e[1][1] * p.y() + m.e[1][2] * p.z() + m.e[1][3],
         m.e[2][0] * p.x() + m.e[2][1] * p.y() + m.e[2][2] * p.z() + m.e[2][3],
         m.e[3][0] * p.x() + m.e[3][1] * p.y() + m.e[3][2] * p.z() + m.e[3][3]
             );

    q = q.Homogenize();
    return ggPoint3(q.x(), q.y(), q.z());
}

// Matrix * HPoint
ggHPoint3 operator*(const ggHMatrix3 &m, const ggHPoint3 &p) {

 return ggHPoint3(
         m.e[0][0] * p.x() + m.e[0][1] * p.y() + m.e[0][2] * p.z() + m.e[0][3],
         m.e[1][0] * p.x() + m.e[1][1] * p.y() + m.e[1][2] * p.z() + m.e[1][3],
         m.e[2][0] * p.x() + m.e[2][1] * p.y() + m.e[2][2] * p.z() + m.e[2][3],
         m.e[3][0] * p.x() + m.e[3][1] * p.y() + m.e[3][2] * p.z() + m.e[3][3]
             );
}

// Matrix * Vector
ggVector3 operator*(const ggHMatrix3 &m, const ggVector3 &p) {
   return ggVector3(
         m.e[0][0] * p.x() + m.e[0][1] * p.y() + m.e[0][2] * p.z(),
         m.e[1][0] * p.x() + m.e[1][1] * p.y() + m.e[1][2] * p.z(),
         m.e[2][0] * p.x() + m.e[2][1] * p.y() + m.e[2][2] * p.z()
     );
}

// Matrix * Ray
ggRay3 operator*(const ggHMatrix3 &m, const ggRay3 &in) {
    ggRay3 r;
    r.Set( m * in.origin(), m * in.direction());
    return r;
}

// Matrix * ONB
ggONB3 operator*(const ggHMatrix3 &m, const ggONB3 &in) {
    ggONB3 uvw;
    uvw.InitFromUV(m * in.u(), m * in.v());
    return uvw;
}

// Matrix * Frame
ggFrame3 operator*(const ggHMatrix3 &m, const ggFrame3 &in) {
    
    return ggFrame3(m * in.origin(), m * in.basis());
}


// Matrix * Matrix
ggHMatrix3 operator*(const ggHMatrix3 &m1, const ggHMatrix3 &m2) {
    ggHMatrix3 m;

    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++) {
            m.e[r][c] = 0;
            for (int i = 0; i < 4; i++)
                m.e[r][c] += m1.e[r][i] * m2.e[i][c];
        }

    return m;
}

//
// operator==
//

ggBoolean  operator==(const ggHMatrix3 &m1, const ggHMatrix3 &m2) {
    for (int r=0; r<4; r++)
      for (int c=0; c<4; c++)
	if (m1.e[r][c] != m2.e[r][c])
	  return ggFalse;
    return ggTrue;
}

ggBoolean  
ggHMatrix3Equal (const ggHMatrix3 &m1, const ggHMatrix3 &m2, double tol) {
    for (int r=0; r<4; r++)
      for (int c=0; c<4; c++)
	if (!ggEqual(m1.e[r][c], m2.e[r][c], tol))
	  return ggFalse;
    return ggTrue;
}

//
// I/O
//

ostream & operator<<(ostream &os, const ggHMatrix3 &m) {

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++)
	  os << m.e[r][c] << " ";
	os << endl;
    }
    return os;
}

istream & operator>>(istream &is, ggHMatrix3 &m) {

    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
	  is >> m.e[r][c];
    return is;
}

// pretty-print
void ggHMatrix3::print(ostream &os) const {
    for (int r = 0; r < 4; r++) {
        os << (r == 0 ? "(" : " ");
        os << "(";
        for (int c = 0; c < 4; c++) {
            if (c != 0)
                os << ", ";
            os << e[r][c];
        }
        os << ")";
        if (r == 3)
            os << ")";
        os << endl;
    }
}
