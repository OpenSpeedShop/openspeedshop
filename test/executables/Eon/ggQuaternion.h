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



// Header file information for the ggQuaternion class
// Bob Cross - 10/17/93

#ifndef GGQUATERNION_H
#define GGQUATERNION_H

#include <math.h>
#include <stdlib.h>
#include <iostream.h>
#include <assert.h>

#include <ggMacros.h>
#include <ggVector3.h>
#include <ggHMatrix3.h>
#include <ggONB3.h>

const int X = 0;
const int Y = 1;
const int Z = 2;
const int W = 3;

class ggQuaternion {

  friend istream &operator>>(istream &is, ggQuaternion &t);
  friend ostream &operator<<(ostream &os, const ggQuaternion &t);

public:
  ggQuaternion();

  ggQuaternion(const ggHMatrix3 &HM);
  ggQuaternion(double qx, double qy, double qz, double qw);
  ggQuaternion(const ggVector3 axis, double theta);
  ggQuaternion(const ggONB3 &uvw);

  ~ggQuaternion() {}
  
  const ggQuaternion& operator=(const ggQuaternion&);

  ggBoolean operator==(const ggQuaternion&) const;

  ggBoolean operator!=(const ggQuaternion&) const;

  // Handy [] operator
  double& operator[](int index) { return q[index]; }
  double operator[](int index) const{ return q[index]; }

  // More weenie data access
  double x() const { return q[X]; }
  double y() const { return q[Y]; }
  double z() const { return q[Z]; }
  double w() const { return q[W]; }
  double& x() { return q[X]; }
  double& y() { return q[Y]; }
  double& z() { return q[Z]; }
  double& w() { return q[W]; }

  double length() const { return q[X]*q[X]+q[Y]*q[Y]+q[Z]*q[Z]+q[W]*q[W]; }

  // Quaternion arithmetic:

  // Multiply
  ggQuaternion operator*(const ggQuaternion&) const;

  // Multiply quat*vector (with transparent matrix multiply)
  ggVector3 operator*(const ggVector3&);

  // Addition
  ggQuaternion operator+(const ggQuaternion&) const;

  // Conjugate
  const ggQuaternion& Conjugate(void);

  // return Conjugate
  ggQuaternion getConjugate(void) const;

  // Invert
  const ggQuaternion& Invert(void);

  // return Inverse
  ggQuaternion getInverse(void) const;

  // Conversions for data access
  ggHMatrix3 getHMatrix3(void);

  ggVector3 getVector3(void) const;
  double getAngle(void) const;

  ggONB3 getONB3(void);

private:
  double q[4];			// The quaternion
				// XYZ is the 3-component vector - sin(t/2) 
				// W is the scalar - cos(t/2)
  ggBoolean matrix_computed;	// Pre-computation flag for the quaternion
  ggHMatrix3 M;			// The basis/rotation matrix equivalent
				// to this quaternion.
protected:
};

// Obviously, some of this stuff can be inlined.
ggQuaternion operator*(double, const ggQuaternion&);

istream &operator>>(istream &is, ggQuaternion &t);

ostream &operator<<(ostream &os, const ggQuaternion &t);

// Interpolation function - spherical as quaternions lie on unit 3-sphere
ggQuaternion ggSlerp(const ggQuaternion&, const ggQuaternion&, double t);

double ggDot(const ggQuaternion&, const ggQuaternion&);

// Handy Identity quaternion

const ggQuaternion ggQIdentity(0,0,0,1);

#endif
