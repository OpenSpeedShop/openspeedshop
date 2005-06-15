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
// ggSphere.h-- declarations for class ggSphere
//
// Author:    Jai-hoon Kim
//            Nov. 1, 1993
//		
// Modified:  Peter Shirley
//            Feb, 1994-- minor changes in arg passing, added helper functions
//

#ifndef GGSPHERE_H
#define GGSPHERE_H

#include <ggMacros.h>
#include <ggGeometry.h>
#include <math.h>

class ggSphere {
  public:
    ggSphere() { r = 1.0;}
    ggPoint3 center() const { return c;}
    double radius() const { return r;}
    ggSphere(const ggPoint3& center, double radius)
	{c = center; r = radius;}
    ggSphere(const ggSphere& s)
	{ c = s.center(); r = s.radius(); }
    void Set(const ggPoint3& center, double radius)
	{c = center; r = radius;}

    ggPoint3& center() { return c;}
    double& radius() { return r;}
    double area() const { return 4.0*ggPi*r*r;}

    ggPoint2 getUV(const ggPoint3& p) const;
    ggPoint3 getPoint(const ggPoint2&) const;
    ggPoint3 getUniformRandomPoint(const ggPoint2&) const;
    ggBox3 boundingBox() const {
        ggVector3 v(r, r, r);
        return ggBox3(c - v, c + v); 
    }

    ggVector3 normal(const ggPoint3& p) const {
        return p - c;
    }
    ggVector3 unitNormal(const ggPoint3& p) const {
        return ggUnitVector(p - c);
    }

    ggSphere &operator=(const ggSphere& s);
    friend ggBoolean operator==(const ggSphere& s1,
				const ggSphere& s2);
    friend ostream &operator<<(ostream &os, const ggSphere& t);
    friend istream &operator>>(istream &is, ggSphere& t);

  protected:
    ggPoint3 c;
    double r;
};


inline ggPoint3 ggSphere:: getUniformRandomPoint(const ggPoint2& uv) const {
    return ggPointOnSphere(center(), radius(), uv);
}

inline ggPoint3 ggSphere:: getPoint(const ggPoint2& uv) const {
    return ggPointOnSphere(center(), radius(), uv);
}

inline ggSphere& ggSphere::operator=(const ggSphere& s) {
    c = s.center();
    r = s.radius();
    return *this;
}

inline ggBoolean operator==(const ggSphere& s1, const ggSphere& s2) {
    return (s1.center() == s2.center() && ggEqual(s1.radius(), s2.radius()));
}

inline ostream &operator<<(ostream &os, const ggSphere& t) {
    os <<  t.center() << " " << t.radius() ;
    return os;
}

inline istream &operator>>(istream &is, ggSphere& t) {
    ggPoint3 center;
    double radius;
    is >> center >> radius;
    t.Set(center, radius);
    return is;
}

inline ggBoolean ggRaySphereIntersect(const ggRay3& ray,
    const ggSphere& sphere, double tmin, double tmax, double& t) {
    return ggRaySphereIntersect(ray, sphere.center(), sphere.radius(),
                                tmin, tmax, t);
}

ggBoolean ggSphereBoxOverlap
    (const ggSphere& s, const ggBox3& box);

#endif
