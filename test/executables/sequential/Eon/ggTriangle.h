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
// ggTriangle.h-- declarations for class ggTriangle
//
// Author:    Hui Ma
//            Nov. 1, 1993
// Modified:  Hui Ma 
//            Nov. 21, 1993
// Modified:  Peter Shirley
//            Feb. 1994 to change point storage.
//

#ifndef GGTRIANGLE_H
#define GGTRIANGLE_H

#include <ggPoint3.h>
#include <ggFPoint3.h>
#include <ggVector3.h>
#include <ggGeometry.h>
#include <ggRay3.h>
#include <ggBox3.h>
#include <ggConstants.h>
#include <ggMacros.h>
#include <iostream.h>
#include <math.h>

class ggTriangle { 
 public: 
  ggTriangle() { }
  ggTriangle(const ggPoint3& x, const ggPoint3& y, const ggPoint3& z) 
    { p[0] = x; p[1] = y; p[2] = z; }

  void Set(const ggPoint3& x, const ggPoint3& y, const ggPoint3& z) 
    { p[0] = x; p[1] = y; p[2] = z; }

  ggFPoint3 operator[](int i) const {
#ifdef GGSAFE
   assert (i >= 0 && i < 3);
#endif
    return p[i];
  }

  ggFPoint3&  operator[](int i) {
#ifdef GGSAFE
   assert (i >= 0 && i < 3);
#endif
    return p[i];
  }

 ggTriangle(const ggTriangle& t) {
     p[0] = t[0];
     p[1] = t[1];
     p[2] = t[2];
  }

  double area() const {
    return ggCross(p[1] - p[0], p[2] - p[0]).length() * 0.5; }

  ggPoint3 getUniformRandomPoint(const ggPoint2& uv) const {
    return  ggPointOnTriangle(p[0], p[1], p[2], uv); }

  ggPoint3 getPoint(const ggPoint2& uv) const {
        return p[0] + uv.u() * (p[1] - p[0]) + uv.v() * (p[2] - p[0]);
  }

  ggPoint2 getUV(const ggPoint3&) const;

  ggVector3 unitNormal() const {
    ggVector3 w = ggCross(p[1] - p[0], p[2] - p[0]);
    w.MakeUnitVector();
    return w;}

  ggTriangle& operator=(const ggTriangle& t) {
     p[0] = t[0];
     p[1] = t[1];
     p[2] = t[2];
     return *this;
  }

  ggBox3 boundingBox() const {
    return ggBoundingBox(p[0], p[1], p[2]); }


  ggVector3 normal() const 
    { return ggCross(p[1] - p[0], p[2] - p[0]); }

  ggPoint3 vertex(int i) const { return p[i]; }

 protected:
  ggFPoint3 p[3];
};

ggBoolean operator==(const ggTriangle& t1, const ggTriangle& t2);
istream &operator>>(istream &is, ggTriangle &t) ;
ostream &operator<<(ostream &os, const ggTriangle &t); 
ggBoolean ggTriangleBoxOverlap(const ggTriangle& t, const ggBox3& box) ;
ggBoolean ggRayTriangleIntersect(const ggRay3& ray, 
                                        const ggTriangle& tri, 
				        double tmin, 
				        double tmax, 
                                        double& beta, 
				        double& gamma, 
				        double& t) ;



inline ggBoolean operator==(const ggTriangle& t1, const ggTriangle& t2)
    {  return ( t1[0] == t2[0] && t1[1] == t2[1] && t1[2] == t2[2]);
}
  

inline istream &operator>>(istream &is, ggTriangle &t) 
    { 
      ggPoint3 pt0, pt1, pt2;
     is >> pt0 >> pt1 >> pt2;
     t.Set(pt0, pt1, pt2);
      return is; }
  
inline ostream &operator<<(ostream &os, const ggTriangle &t) 
    { os << t[0] << " " << t[1] << " " << t[2] << "\n";
      return os; }
  
inline ggPoint2 ggTriangle::getUV(const ggPoint3& q) const {
// using Cramer's rule
  ggVector3 v0 = q - p[0];
  ggVector3 v1 = p[1] - p[0];
  ggVector3 v2 = p[2] - p[0];
  double a11 = ggDot(v1, v1);
  double a12 = ggDot(v1, v2);
  double a22 = ggDot(v2, v2);
  double b1 = ggDot(v0, v1);
  double b2 = ggDot(v0, v2);
  double d = a11 * a22 - a12 * a12;
  
  if(fabs(d) < ggSmallEpsilon) {
    cerr << "Error in ggTriangle::getUV. Degenerated triangle.\n";
    exit(1);
  }

  d = 1.0 / d;
  double u = (b1 * a22 - b2 * a12) * d;
  double v = (b2 * a11 - b1 * a12) * d;
  return ggPoint2(u, v);

}

inline ggBoolean ggTriangleBoxOverlap(const ggTriangle& t, 
			       const ggBox3& box) {
     return ggTriangleBoxOverlap(t[0], t[1], t[2], box);
}

inline ggBoolean ggRayTriangleIntersect(const ggRay3& ray, 
                                        const ggTriangle& tri, 
				        double tmin, 
				        double tmax, 
                                        double& beta, 
				        double& gamma, 
				        double& t) {
      return ggRayTriangleIntersect(ray, tri[0], tri[1], tri[2],
                                    tmin, tmax, beta, gamma, t);
}

#endif

