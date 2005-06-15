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



// Code for ggQuaternion

#include <ggQuaternion.h>

// Default is the identity quaternion with an identity matrix
ggQuaternion::ggQuaternion()
{
  register int i,j;
  q[X] = q[Y] = q[Z] = 0.0; q[W] = 1.0;
  matrix_computed = ggFalse;
  for (i=X;i<=W;i++)
    for (j=X;j<=W;j++)
      M.e[i][j] = ((i==j)?1.0:0.0);
}

// Conversion from matrix to quaternion
// mattoquat from Shoemake
ggQuaternion::ggQuaternion(const ggHMatrix3 &HM)
{
  double tr, s;
  int i,j,k;

  static int nxt[]={Y,Z,X};

  M = HM;			// Use the nicely provided matrix
  matrix_computed = ggTrue;

  tr = HM.e[X][X] + HM.e[Y][Y] + HM.e[Z][Z];
  if (tr>0.0){
    s = sqrt(tr + 1.0);
    q[W] = s*0.5;
    s = 0.5/s;
    q[X] = (HM.e[Z][Y] - HM.e[Y][Z]) * s;
    q[Y] = (HM.e[X][Z] - HM.e[Z][X]) * s;
    q[Z] = (HM.e[Y][X] - HM.e[X][Y]) * s;
  }else{
    i = X;
    if (HM.e[Y][Y]>HM.e[X][X]) i=Y;
    if (HM.e[Z][Z]>HM.e[i][i]) i=Z;
    j = nxt[i]; k = nxt[j];
    s = sqrt((HM.e[i][i] - (HM.e[j][j]+HM.e[k][k])) + 1.0);
    q[i] = s*0.5;
    s = 0.5/s;
    q[W] = (HM.e[k][j] - HM.e[j][k]) * s;
    q[j] = (HM.e[j][i] + HM.e[i][j]) * s;
    q[k] = (HM.e[k][i] + HM.e[i][k]) * s;
  }
}

// Semi-standard initializer.  Note that we don't compute the equivalent 
// matrix until it becomes relevant.
ggQuaternion::ggQuaternion(double qx, double qy, double qz, double qw)
{
  q[X] = qx; q[Y] = qy; q[Z] = qz; q[W] = qw;
  matrix_computed = ggFalse;
}

// This may occasionally be useful; specify the quaternion according to 
// a rotation about a unit 3-vector.
ggQuaternion::ggQuaternion(const ggVector3 axis, double theta)
{
  double st2 = sin(theta/2.0);
  double ct2 = cos(theta/2.0);

  q[X] = axis.e[X]/st2; q[Y] = axis.e[Y]/st2; q[Z] = axis.e[Z]/st2;
  q[W] = ct2;
  matrix_computed = ggFalse;
}

// Construct a quaternion from an ONB3
ggQuaternion::ggQuaternion(const ggONB3 &uvw)
{
  ggHMatrix3 M;
  ggVector3 u = uvw.u(), v = uvw.v(), w = uvw.w();
  register int i;
  for (i=X;i<=Z;i++){
    M.e[i][X] = u.e[i]; M.e[i][Y] = v.e[i]; M.e[i][Z] = w.e[i];
  }
  for (i=0;i<3;i++)
    M.e[3][i] = M.e[i][3] = 0.0;
  M.e[3][3] = 1.0;
  *this = ggQuaternion(M);
}

// Standard set equal
const ggQuaternion& ggQuaternion::operator=(const ggQuaternion &qin)
{ q[X] = qin.q[X]; q[Y] = qin.q[Y]; q[Z] = qin.q[Z]; q[W] = qin.q[W];
  // Only copy the matrix store if something good is there.
  if ((matrix_computed = qin.matrix_computed) == ggTrue)
    M = qin.M;
  return *this;
}

// Test for equality
ggBoolean ggQuaternion::operator==(const ggQuaternion& qin) const
{
  if ((fabs(q[X]-qin[X])<ggEpsilon) &&
      (fabs(q[Y]-qin[Y])<ggEpsilon) &&
      (fabs(q[Z]-qin[Z])<ggEpsilon) &&
      (fabs(q[W]-qin[W])<ggEpsilon))
    return ggTrue;
  else return ggFalse;
}

// Inequality
ggBoolean ggQuaternion::operator!=(const ggQuaternion& qin) const
{
  return !(*this == qin);
}

// Dot product for quaternions
double ggDot(const ggQuaternion& q1, const ggQuaternion& q2)
{ return q1[X]*q2[X]+q1[Y]*q2[Y]+q1[Z]*q2[Z]+q1[W]*q2[W]; }

ggQuaternion ggQuaternion::operator*(const ggQuaternion& q2) const
{
  return
    ggQuaternion((q[W]*q2[X]+q[X]*q2[W]+q[Y]*q2[Z]-q[Z]*q2[Y]),
		 (q[W]*q2[Y]+q[Y]*q2[W]+q[Z]*q2[X]-q[X]*q2[Z]),
		 (q[W]*q2[Z]+q[Z]*q2[W]+q[X]*q2[Y]-q[Y]*q2[X]),
		 (q[W]*q2[W]-q[X]*q2[X]-q[Y]*q2[Y]-q[Z]*q2[Z]));
}

ggVector3 ggQuaternion::operator*(const ggVector3& v)
{
  // Check to see if we have a current matrix
  if (matrix_computed == ggFalse){
    M = getHMatrix3();
    // From now on, the matrix will be remembered
  }
  return M*v;
}

ggQuaternion ggQuaternion::operator+(const ggQuaternion& q2) const
{
  return ggQuaternion(q[X]+q2[X],q[Y]+q2[Y],q[Z]+q2[Z],q[W]+q2[W]);
}

// Left vs. Right hand rule
const ggQuaternion& ggQuaternion::Conjugate(void)
{
  q[X] = -q[X]; q[Y] = -q[Y]; q[Z] = -q[Z];
  return *this;
}

ggQuaternion ggQuaternion::getConjugate(void) const
{
  return ggQuaternion(-q[X],-q[Y],-q[Z],q[W]);
}

const ggQuaternion& ggQuaternion::Invert(void)
{
  double norminv;
  norminv = 1.0/length();
  q[X] = -q[X] * norminv;
  q[Y] = -q[Y] * norminv;
  q[Z] = -q[Z] * norminv;
  q[W] = q[W] * norminv;
  return *this;
}

ggQuaternion ggQuaternion::getInverse(void) const
{
  double norminv;
  norminv = 1.0/length();
  return ggQuaternion(-q[X] * norminv,-q[Y] * norminv,-q[Z] * norminv,
		      q[W] * norminv);
}

// Note that this is non-const; we may have to compute some values
ggHMatrix3 ggQuaternion::getHMatrix3(void) 
{
  double s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;
  register int i;
  // Only do the work that must be done
  if (matrix_computed == ggFalse){
    // Start with part of identity matrix
    for (i=X;i<W;i++){
      M.e[i][W] = 0.0;
      M.e[W][i] = 0.0;
    }
    M.e[W][W] = 1.0;

    s = 2.0/length();

    xs = q[X]*s;  ys = q[Y]*s;  zs = q[Z]*s;
    wx = q[W]*xs; wy = q[W]*ys; wz = q[W]*zs;
    xx = q[X]*xs; xy = q[X]*ys; xz = q[X]*zs;
    yy = q[Y]*ys; yz = q[Y]*zs; zz = q[Z]*zs;

    M.e[X][X] = 1.0 - (yy + zz);
    M.e[X][Y] = xy - wz;
    M.e[X][Z] = xz + wy;

    M.e[Y][X] = xy + wz;
    M.e[Y][Y] = 1.0 - (xx + zz);
    M.e[Y][Z] = yz - wx;

    M.e[Z][X] = xz - wy;
    M.e[Z][Y] = yz + wx;
    M.e[Z][Z] = 1.0 - (xx + yy);

    matrix_computed = ggTrue;
  }
  return M;
}

// Scalar part holds cos(theta/2)
// Others are scaled by 1/(sin(theta/2))
ggVector3 ggQuaternion::getVector3(void) const
{
  double st2, act2;
  act2 = acos(q[W]);
  st2 = sin(act2);
  return ggVector3(q[X]*st2,q[Y]*st2,q[Z]*st2);
}

double ggQuaternion::getAngle(void) const
{
  return acos(q[W])*2.0;
}

// Get an orthonormal basis corresponding to this quaternion (rotated from
// XYZ)
ggONB3 ggQuaternion::getONB3(void)
{
  return ggONB3(*this * ggXAxis,*this * ggYAxis,*this * ggZAxis);
}

ggQuaternion operator*(double d, const ggQuaternion& qin)
{
  return ggQuaternion(d*qin[X],d*qin[Y],d*qin[Z],d*qin[W]);
}

ostream &operator<<(ostream &os, const ggQuaternion &qin)
{ 
  os << qin[X] << " " << qin[Y] << " " << qin[Z] << " " << qin[W];
  return os;
}

istream &operator>>(istream &is, ggQuaternion &qin)
{
  is >> qin[X] >> qin[Y] >> qin[Z] >> qin[W];
  return is;
}

// Interpolate with constant angular velocity between the two quaternions
// Note that some of this should be precomputed for interaction 
// (but we won't be interacting in a ray-tracer!).
ggQuaternion ggSlerp(const ggQuaternion &q0, const ggQuaternion &q1, double t)
{
  double omega, cosom, sinom, sclp, sclq;

  cosom = ggDot(q0,q1);
  if ((1.0+cosom)>ggEpsilon){
    // usual case
    if ((1.0-cosom)>ggEpsilon){
      // usual case
      omega = acos(cosom);
      sinom = sin(omega);
      sclp = sin((1.0-t)*omega)/sinom;
      sclq = sin(t*omega)/sinom;
    }else{
      // ends very close
      sclp = 1.0-t; sclq = t;
    }
    return sclp*q0 + sclq*q1;
  }else{
    // ends nearly opposite
    sclp = sin((1.0-t)*ggHalfPi);
    sclq = sin(t*ggHalfPi);
    return ggQuaternion(sclp*q0[X]+sclq*-q0[Y],sclp*q0[Y]+sclq*-q0[X],
			sclp*q0[Z]+sclq*-q0[W],sclp*q0[W]+sclq*-q0[Z]);
  }
}

