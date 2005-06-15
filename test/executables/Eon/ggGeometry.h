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



#ifndef GGGEOMETRY_H
#define GGGEOMETRY_H


#include <ggRay3.h>
#include <ggONB3.h>
#include <ggBox2.h>
#include <ggBox3.h>
#include <math.h>
#include <ggHRotationMatrix3.h>

ggVector3 ggUniformVector(const ggONB3& uvw, const ggPoint2& p);

ggVector3 ggRotateVectorAboutAxis(const ggVector3& vin,
                                         const ggVector3& axis,
                                         double angle);

ggRay2 randomRayInUnitSquare(const ggPoint2& uv);

ggRay3 randomRayInUnitCube(const ggPoint2& st, const ggPoint2& uv);


inline double ggCosBetweenVectors(const ggVector3& a, const ggVector3& b) {
        return ggDot(a,b) / (a.length() * b.length());
}


// return a point on a sphere of radius r and center c

ggPoint3 ggPointOnSphere(const ggPoint3& c,
                                double r,
                                const ggPoint2& uv);


// return a vector perpendicular to input.
inline ggVector2 ggPerpendicularVector(const ggVector2& v) {
    return ggVector2(v.y(), -v.x());
}

// return a vector perpendicular to input.
inline ggVector3 ggPerpendicularVector(const ggVector3& v) {
   int axis = v.indexOfMinAbsComponent();
   if (axis == 0)
       return ggVector3(0.0, v.e[2], -v.e[1]);
   else if (axis == 1)
       return ggVector3(v.e[2], 0.0, -v.e[0]);
   else
       return ggVector3(v.e[1], -v.e[0], 0.0);
}


ggBoolean ggTriangleBoxOverlap(const ggPoint3& p0, const ggPoint3& p1,
                               const ggPoint3& p2, const ggBox3& box);

// returns a vector with density proportional to cos^n (theta)
ggVector3 ggPhongVector(const ggONB3& uvw, const ggPoint2& p, double n);

inline ggVector3 ggPhongVector(const ggVector3& v, const ggPoint2& p, double n){
   ggONB3 uvw;
   uvw.InitFromW(v);
   return ggPhongVector( uvw, p, n);
}

// returns a vector with density proportional to cos^n (theta/2)
ggVector3 ggHalfPhongVector(const ggONB3& uvw, const ggPoint2& p, double n);

// returns a vector with density proportional to cos (theta)
ggVector3 ggDiffuseVector(const ggONB3& uvw, const ggPoint2& p);


// perturb by angles down and around (in radians).
ggVector3 ggPerturb(const ggVector3& v, double down, double around); 


ggBoolean ggRayBoxIntersect(const ggRay3& r,
			    const ggBox3& box,
                            double tmin,
                            double tmax,
                            ggONB3& uvw,
                            ggPoint3& p,
			    double& t);

ggBoolean ggRayXYDiskIntersect(const ggRay3& r,
			       const ggPoint3& center,
			       double radius,
                               double tmin,
                               double tmax,
                               double& x,
                               double& y,
			       double& t);

ggBoolean ggRayYZDiskIntersect(const ggRay3& r,
			       const ggPoint3& center,
			       double radius,
                               double tmin,
                               double tmax,
                               double& y,
                               double& z,
			       double& t);

ggBoolean ggRayXZDiskIntersect(const ggRay3& r,
			       const ggPoint3& center,
			       double radius,
                               double tmin,
                               double tmax,
                               double& x,
                               double& z,
			       double& t);

ggBoolean ggRayXYEllipseIntersect(const ggRay3& r,
				  const ggPoint3& center,
				  double a,
				  double b,
                               double tmin,
                               double tmax,
				  double& t);

ggBoolean ggRayYZEllipseIntersect(const ggRay3& r,
				  const ggPoint3& center,
				  double a,
				  double b,
                               double tmin,
                               double tmax,
				  double& t);

ggBoolean ggRayXZEllipseIntersect(const ggRay3& r,
				  const ggPoint3& center,
				  double a,
				  double b,
                               double tmin,
                               double tmax,
				  double& t);

ggBoolean ggRaySphereIntersect(const ggRay3& r, 
			       const ggPoint3& c,
                               double radius, 
                               double tmin,
                               double tmax,
			       double& t);

ggBoolean ggRayOriginSphereIntersect(const ggRay3& r, 
                                  double radius, 
                               double tmin,
                               double tmax,
				  double& t);



ggBoolean ggRayZCylinderIntersect(const ggRay3& r,
			    double wmax, // height
			    double radius,
                               double tmin,
                               double tmax,
			    double& t);

ggBoolean ggRayTriangleIntersect(const ggRay3& r,
                              const ggPoint3& p0,
                              const ggPoint3& p1,
                              const ggPoint3& p2,
                              double tmin,
                              double tmax,
                              double& beta,
                              double& gamma,
                              double& t);

ggBoolean ggRayXYRectangleIntersect(const ggRay3& r,
                               float xmin,
                               float xmax,
                               float ymin,
                               float ymax,
                               float z,
                               double tmin,
                               double tmax,
			       double& x,
			       double& y,
                               double& t);

ggBoolean ggRayXZRectangleIntersect(const ggRay3& r,
                               float xmin,
                               float xmax,
                               float y,
                               float zmin,
                               float zmax,
                               double tmin,
                               double tmax,
			       double& x,
			       double& z,
                               double& t);

ggBoolean ggRayYZRectangleIntersect(const ggRay3& r,
                               float x,
                               float ymin,
                               float ymax,
                               float zmin,
                               float zmax,
                               double tmin,
                               double tmax,
			       double& y,
			       double& z,
                               double& t);



inline double ggSquaredDistance(const ggPoint3& p1, 
                                const ggPoint3& p2) {
     // ggVector3 v(p1-p2);
     // return ggDot(v, v);
     double x = p1.e[0] - p2.e[0];
     double y = p1.e[1] - p2.e[1];
     double z = p1.e[2] - p2.e[2];
     return x*x + y*y + z*z;
}

inline double ggDistance(const ggPoint3& p1, 
                         const ggPoint3& p2) {
     double x = p1.e[0] - p2.e[0];
     double y = p1.e[1] - p2.e[1];
     double z = p1.e[2] - p2.e[2];
     return sqrt(x*x + y*y + z*z);
}


inline ggVector3 ggSphericalToVector(double theta, double phi) {
     double sint = sin(theta);
     return ggVector3(sint*cos(phi),
                      sint*sin(phi),
                      cos(theta));
}


inline ggPoint2 ggPointOnDisk(double R, const ggPoint2& uv) {
    double r = R * sqrt(uv.u());
    double t = ggTwoPi * uv.v();
    return ggPoint2(r*cos(t), r*sin(t));
}

inline ggPoint3 ggPointOnTriangle( const ggPoint3& p0,
                                  const ggPoint3& p1,
                                  const ggPoint3& p2,
                                  const ggPoint2& uv) {
    double a =  sqrt(1-uv.u());
    double beta = (1 - a);
    double gamma = a*uv.v();
    return ggWeightedAverage(p0, p1, p2, 1-beta-gamma, beta, gamma);
}


inline ggBox3 ggBoundingBox(const ggPoint3& p0,
                            const ggPoint3& p1,
                            const ggPoint3& p2) {
    ggBox3 bbox;
    ggMinMax((double) p0.x(), p1.x(), p2.x(), bbox.min().x(), bbox.max().x());
    ggMinMax((double) p0.y(), p1.y(), p2.y(), bbox.min().y(), bbox.max().y());
    ggMinMax((double) p0.z(), p1.z(), p2.z(), bbox.min().z(), bbox.max().z());
    return bbox;
}


// XCylinder
// (r.y-yCenter)^2 + (r.z-zCenter)^2 - radius^2 = 0.
// (v.y*v.y + v.z*v.z)t^2 + 2(v.y*(o.y-yCenter) + v.z*(o.z-zCenter))t + 
// ((o.y-yCenter)*(o.y-yCenter) + (o.z-zCenter)*(o.z-zCenter) - radius^2 = 0
ggBoolean ggRayXCylinderIntersect(const ggRay3& r,
                            double xMin,
                            double xMax,
                            double yCenter,
                            double zCenter,
                            double radius,
                            double tmin,
                            double tmax,
                            ggPoint3& p,
                            double& t);

// YCylinder
// (r.z-zCenter)^2 + (r.x-xCenter)^2 - radius^2 = 0.
// (v.z*v.z + v.x*v.x)t^2 + 2(v.z*(o.z-zCenter) + v.x*(o.x-xCenter))t + 
// ((o.z-zCenter)*(o.z-zCenter) + (o.x-xCenter)*(o.x-xCenter) - radius^2 = 0
ggBoolean ggRayYCylinderIntersect(const ggRay3& r,
                            double xCenter,
                            double yMin,
                            double yMax,
                            double zCenter,
                            double radius,
                            double tmin,
                            double tmax,
                            ggPoint3& p,
                            double& t);
// ZCylinder
// (r.x-xCenter)^2 + (r.y-yCenter)^2 - radius^2 = 0.
// (v.x*v.x + v.y*v.y)t^2 + 2(v.x*(o.x-xCenter) + v.y*(o.y-yCenter))t + 
// ((o.x-xCenter)*(o.x-xCenter) + (o.y-yCenter)*(o.y-yCenter) - radius^2 = 0
ggBoolean ggRayZCylinderIntersect(const ggRay3& r,
                            double xCenter,
                            double yCenter,
                            double zMin,
                            double zMax,
                            double radius,
                            double tmin,
                             double tmax,
                            ggPoint3& p,
                            double& t);

// XEllipticalCylinder
// b^2*(r.y-yCenter)^2 + a^2*(r.z-zCenter)^2 - a^2 * b^2 = 0.
// (v.y*v.y*b^2 + v.z*v.z*a^2)t^2 + 2(v.y*(o.y-yCenter)*b^2 +
// v.z*(o.z-zCenter)*a^2)t + 
// ((o.y-yCenter)*(o.y-yCenter)*b^2 + (o.z-zCenter)*(o.z-zCenter)*a^2 - a^2 * b^2= 0
ggBoolean ggRayXEllipticalCylinderIntersect(const ggRay3& r,
                            double xMin,
                            double xMax,
                            double yCenter,
                            double zCenter,
                            double r1,
                            double r2,
                            double tmin,
                            double tmax,
                            ggPoint3& p,
                            double& t);

// YEllipticalCylinder
// b^2*(r.z-zCenter)^2 + a^2*(r.x-xCenter)^2 - a^2 * b^2 = 0.
// (v.z*v.z*b^2 + v.x*v.x*a^2)t^2 + 2(v.z*(o.z-zCenter)*b^2 + v.x*(o.x-xCenter)*a^2)t + 
// ((o.z-zCenter)*(o.z-zCenter)*b^2 + (o.x-xCenter)*(o.x-xCenter)*a^2 - a^2 * b^2= 0
ggBoolean ggRayYEllipticalCylinderIntersect(const ggRay3& r,
                            double xCenter,
                            double yMin,
                            double yMax,
                            double zCenter,
                            double rx,
                            double rz,
                            double tmin,
                            double tmax,
                            ggPoint3& p,
                            double& t);

// ZEllipticalCylinder
// b^2*(r.x-xCenter)^2 + a^2*(r.y-yCenter)^2 - a^2 * b^2 = 0.
// (v.x*v.x*b^2 + v.y*v.y*a^2)t^2 + 2(v.x*(o.x-xCenter)*b^2 + v.y*(o.y-yCenter)*a^2)t + 
// ((o.x-xCenter)*(o.x-xCenter)*b^2 + (o.y-yCenter)*(o.y-yCenter)*a^2 - a^2 * b^2= 0
ggBoolean ggRayZEllipticalCylinderIntersect(const ggRay3& r,
                            double xCenter,
                            double yCenter,
                            double zMin,
                            double zMax,
                            double r1,
                            double r2,
                            double tmin,
                            double tmax,
                            ggPoint3& p,
                            double& t);
#endif
