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



#include <math.h>
#include <ggNA.h>
#include <ggGeometry.h>
#include <ggHRotationMatrix3.h>


ggBoolean ggRayBoxIntersect(const ggRay3& r,
                            const ggBox3& box,
                            double tmin,
                            double tmax,
                            ggONB3& uvw,
                            ggPoint3& p,
                            double& t) {


    double t1, t2, tx1, tx2, ty1, ty2, tz1, tz2;
    int x, y, z, xyz1, xyz2;

    double xinv =  r.direction().x();
    double yinv =  r.direction().y();
    double zinv =  r.direction().z();

    ggPoint3 min = box.min();
    ggPoint3 max = box.max();

    if (xinv > ggEpsilon) {
        xinv = 1.0 / xinv;
        x = 1;
        tx1 = (min.x() - r.origin().x()) * xinv;
        tx2 = (max.x() - r.origin().x()) * xinv;
    }
    else if (xinv < -ggEpsilon) {
        xinv = 1.0 / xinv;
        x = 0;
        tx1 = (max.x() - r.origin().x()) * xinv;
        tx2 = (min.x() - r.origin().x()) * xinv;
    }
    else {
         x = 0;
         tx1 = -ggInfinity;
         tx2 = ggInfinity;
    }

    if (yinv > ggEpsilon) {
        yinv = 1.0 / yinv;
        y = 1;
        ty1 = (min.y() - r.origin().y()) * yinv;
        ty2 = (max.y() - r.origin().y()) * yinv;
    }
    else if (yinv < -ggEpsilon) {
        yinv = 1.0 / yinv;
        y = 0;
        ty1 = (max.y() - r.origin().y()) * yinv;
        ty2 = (min.y() - r.origin().y()) * yinv;
    }
    else {
         y = 0;
         ty1 = -ggInfinity;
         ty2 = ggInfinity;
    }

    if (zinv > ggEpsilon) {
        zinv = 1.0 / zinv;
        z = 1;
        tz1 = (min.z() - r.origin().z()) / r.direction().z();
        tz2 = (max.z() - r.origin().z()) / r.direction().z();
    }
    else if (zinv < -ggEpsilon) {
        zinv = 1.0 / zinv;
        z = 0;
        tz1 = (max.z() - r.origin().z()) / r.direction().z();
        tz2 = (min.z() - r.origin().z()) / r.direction().z();
    }
    else {
         z = 0;
         tz1 = -ggInfinity;
         tz2 = ggInfinity;
    }
   
    if (tx1 > ty1) {
        t1 = tx1;
        xyz1 = 0;
    }
    else {
        t1 = ty1;
        xyz1 = 1;
    }
    if (tz1 > t1) {
        t1 = tz1;
        xyz1 = 2;
    }
   
    if (tx2 < ty2) {
        t2 = tx2;
        xyz2 = 0;
    }
    else {
        t2 = ty2;
        xyz2 = 1;
    }
    if (tz2 < t2) {
        t2 = tz2;
        xyz2 = 2;
    }
    if (t2 > t1) { // there might be a hit
        if (t1 > tmax) return ggFalse; 

        if (t1 > tmin) {
            t = t1;
            if (xyz1 == 0) { // hit x plane
               if (x)  {// ray has +x component
                   uvw.Set(ggYAxis3,-ggZAxis3,-ggXAxis3);
                   p.x() = min.x();
               }
               else {
                   uvw.Set(ggYAxis3,ggZAxis3,ggXAxis3);
                   p.x() = max.x();
               }
               p.y() = r.origin().y() + t1*r.direction().y();
               p.z() = r.origin().z() + t1*r.direction().z();
            }
            else if (xyz1 == 1) {  // yplane
               if (y)  {// ray has +y component
                   uvw.Set(ggZAxis3,-ggXAxis3,-ggYAxis3);
                   p.y() = min.y();
               }
               else {
                   uvw.Set(ggZAxis3,ggXAxis3,ggYAxis3);
                   p.y() = max.y();
               }
               p.x() = r.origin().x() + t1*r.direction().x();
               p.z() = r.origin().z() + t1*r.direction().z();
            }
            else { // z
               if (z)  {// ray has +z component
                   uvw.Set(ggXAxis3,-ggYAxis3,-ggZAxis3);
                   p.z() = min.z();
               }
               else {
                   uvw.Set(ggXAxis3,ggYAxis3,ggZAxis3);
                   p.z() = max.z();
               }
               p.x() = r.origin().x() + t1*r.direction().x();
               p.y() = r.origin().y() + t1*r.direction().y();
            }
            return ggTrue;
        }
        else if (t2 > tmin && t2 < tmax) {
            t = t2;
            if (xyz1 == 0) { // hit x plane
               if (x)  {// ray has +x component
                   uvw.Set(ggYAxis3,ggZAxis3,ggXAxis3);
                   p.x() = max.x();
               }
               else {
                   uvw.Set(ggYAxis3,-ggZAxis3,-ggXAxis3);
                   p.x() = min.x();
               }
               p.y() = r.origin().y() + t2*r.direction().y();
               p.z() = r.origin().z() + t2*r.direction().z();
            }
            else if (xyz1 == 1) {  // yplane
               if (y)  {// ray has +y component
                   uvw.Set(ggZAxis3,ggXAxis3,ggYAxis3);
                   p.y() = max.y();
               }
               else {
                   uvw.Set(ggZAxis3,-ggXAxis3,-ggYAxis3);
                   p.y() = min.y();
               }
               p.x() = r.origin().x() + t2*r.direction().x();
               p.z() = r.origin().z() + t2*r.direction().z();
            }
            else { // z
               if (z)  {// ray has +z component
                   uvw.Set(ggXAxis3,ggYAxis3,ggZAxis3);
                   p.z() = max.z();
               }
               else {
                   uvw.Set(ggXAxis3,-ggYAxis3,-ggZAxis3);
                   p.z() = min.z();
               }
               p.x() = r.origin().x() + t2*r.direction().x();
               p.y() = r.origin().y() + t2*r.direction().y();
            }
            return ggTrue;
            
        }
        else
           return ggFalse;
    }
    else
       return ggFalse;


}


// gives vector with respect to uvw that is uniform on hemisphere
ggVector3 ggUniformVector(const ggONB3& uvw, const ggPoint2& p) {

    double x = p.u();
	double y = p.v();

    float xx, yy, offset, phi;

    x = 2*x - 1;
    y = 2*y - 1;

    if (y > -x) {               // Above y = -x
        if (y < x) {                // Below y = x
            xx = x;
            if (y > 0) {                // Above x-axis
                /*
                 * Octant 1
                 */
                offset = 0;
                yy = y;
            } else {                    // Below and including x-axis
                /*
                 * Octant 8
                 */
                offset = (7*ggPi)/4;
                yy = x + y;
            }
        } else {                    // Above and including y = x
            xx = y;
            if (x > 0) {                // Right of y-axis
                /*
                 * Octant 2
                 */
                offset = ggPi/4;
                yy = (y - x);
            } else {                    // Left of and including y-axis
                /*
                 * Octant 3
                 */
                offset = (2*ggPi)/4;
                yy = -x;
            }
        }
    } else {                    // Below and including y = -x
        if (y > x) {                // Above y = x
            xx = -x;
            if (y > 0) {                // Above x-axis
                /*
                 * Octant 4
                 */
                offset = (3*ggPi)/4;
                yy = -x - y;
            } else {                    // Below and including x-axis
                /*
                 * Octant 5
                 */
                offset = (4*ggPi)/4;
                yy = -y;
            }
        } else {                    // Below and including y = x
            xx = -y;
            if (x > 0) {                // Right of y-axis
                /*
                 * Octant 7
                 */
                 offset = (6*ggPi)/4;
                 yy = x;
            } else {                    // Left of and including y-axis
                if (y != 0) {
                    /*
                     * Octant 6
                     */
                    offset = (5*ggPi)/4;
                    yy = x - y;
                } else {
                    /*
                     * Origin
                     */
                    return uvw.w();
                }
            }
        }
    }

	double cost = 1 - xx*xx;
	double sint = sqrt(1 - cost*cost);

    phi = offset + (ggPi/4)*(yy/xx);

    ggVector3 a(cos(phi)*sint, sin(phi)*sint, cost);
    
    return ggVector3 (
                  uvw.u().x()*a.x() + uvw.v().x()*a.y() + uvw.w().x()*a.z(), 
                  uvw.u().y()*a.x() + uvw.v().y()*a.y() + uvw.w().y()*a.z(), 
                  uvw.u().z()*a.x() + uvw.v().z()*a.y() + uvw.w().z()*a.z());
}



ggVector3 ggRotateVectorAboutAxis(const ggVector3& vin,
                                         const ggVector3& axis,
                                         double angle) {
    ggONB3 uvw;
    uvw.InitFromW(axis);
    ggHCanonicalToBasisRotationMatrix3 Mto(uvw);
    ggHBasisToCanonicalRotationMatrix3 Mfrom(uvw);
    ggVector3 vout = Mto * vin;
    ggHZRotationMatrix3 Mr(angle);
    vout = Mr * vout;
    vout = Mfrom * vout;
    return vout;
}


ggRay2 randomRayInUnitSquare(const ggPoint2& uv) {
    int side = int(uv.u() * 3.999999999);
    double u = uv.u() * 3.999999999 - side;
    double vn = 2*uv.v() - 1;
    double vt = sqrt(1 - vn*vn);
    if (side == 0)  // y = 0
       return ggRay2 ( ggPoint2(u, 0.0), ggVector2(vt, vn) );
    else if (side == 1) // x = 0
       return ggRay2 ( ggPoint2(0.0, u), ggVector2(vn, vt) );
    if (side == 2)  // y = 1
       return ggRay2 ( ggPoint2(u, 1.0), ggVector2(vt, -vn) );
    else // x = 1
       return ggRay2 ( ggPoint2(1.0, u), ggVector2(-vn, vt) );

}

ggRay3 randomRayInUnitCube(const ggPoint2& st, const ggPoint2& uv) {
    int side = int(st.u() * 5.999999999);
    double s = st.u() * 5.999999999 - side;
    
    double phi = ggTwoPi * uv.u();
    double sint = sqrt(uv.v());
    double cost = sqrt(1 - uv.v());

    if (side == 0) { // z = 0
       ggVector3 a(cos(phi)*sint, sin(phi)*sint, cost);
       return ggRay3 (ggPoint3(s, st.v(), 0.0), a);
    }
    else if (side == 1) { // z = 1
       ggVector3 a(cos(phi)*sint, sin(phi)*sint, -cost);
       return ggRay3 (ggPoint3(s, st.v(), 1.0), a);
    }
    else if (side == 2) { // y = 0
       ggVector3 a(cos(phi)*sint, cost, sin(phi)*sint);
       return ggRay3 (ggPoint3(s, 0.0, st.v()), a);
    }
    else if (side == 3) { // y = 1
       ggVector3 a(cos(phi)*sint, -cost, sin(phi)*sint);
       return ggRay3 (ggPoint3(s, 1.0, st.v()), a);
    }
    else if (side == 4) { // x = 0
       ggVector3 a(cost, cos(phi)*sint, sin(phi)*sint);
       return ggRay3 (ggPoint3(0.0, s, st.v()), a);
    }
    else  { // x = 1
       ggVector3 a(-cost, cos(phi)*sint, sin(phi)*sint);
       return ggRay3 (ggPoint3(1.0,s, st.v()), a);
    }
}


// return a point on a sphere of radius r and center c

ggPoint3 ggPointOnSphere(const ggPoint3& c,
                                double r,
                                const ggPoint2& uv) {

  double costheta = 1.0 - 2.0 * uv.u();
  double sintheta = sqrt(1 - costheta * costheta);
  double phi = 2 * ggPi * uv.v();

  ggVector3 v(r * cos(phi) * sintheta,
              r * sin(phi) * sintheta,
              r * costheta);

  return c + v;
}



ggBoolean ggTriangleBoxOverlap(const ggPoint3& p0, const ggPoint3& p1,
                               const ggPoint3& p2, const ggBox3& box) {

   ggBox3 tbox;  // bounding box of p0,p1,p2
   ggMinMax((double) p0.x(), p1.x(), p2.x(), tbox.min().x(), tbox.max().x());
   ggMinMax((double) p0.y(), p1.y(), p2.y(), tbox.min().y(), tbox.max().y());
   ggMinMax((double) p0.z(), p1.z(), p2.z(), tbox.min().z(), tbox.max().z());

   if (!ggOverlapBox3(box, tbox)) return ggFalse;

   if (box.surroundsPoint(p0) ||
       box.surroundsPoint(p1) ||
       box.surroundsPoint(p2)) return ggTrue; 

   ggRay3 r01(p0, p1-p0);  // ray for side 01 of triangle
   double t1, t2;

   if (box.hitByRay(r01, t1, t2) &&
       ((0 <= t1 && t1 <= 1) || (0 <= t2 && t2 <= 1))) return ggTrue;

   ggRay3 r02(p0, p2-p0);  //  ray for side 02 of triangle

   if (box.hitByRay(r02, t1, t2) &&
       ((0 <= t1 && t1 <= 1) || (0 <= t2 && t2 <= 1))) return ggTrue;

   ggRay3 r12(p1, p2-p1);  //  ray for side 12 of triangle

   if (box.hitByRay(r12, t1, t2) &&
       ((0 <= t1 && t1 <= 1) || (0 <= t2 && t2 <= 1))) return ggTrue;

   double t, beta, gamma;
   ggPoint3 p000 = box.corner(0,0,0);
   ggPoint3 p001 = box.corner(0,0,1);
   ggPoint3 p010 = box.corner(0,1,0);
   ggPoint3 p011 = box.corner(0,1,1);
   ggPoint3 p100 = box.corner(1,0,0);
   ggPoint3 p101 = box.corner(1,0,1);
   ggPoint3 p110 = box.corner(1,1,0);
   ggPoint3 p111 = box.corner(1,1,1);

// bottom xy edges
   {
   ggRay3 r000_100(p000, p100 - p000); 
   if (ggRayTriangleIntersect(r000_100, p0, p1, p2, 0.0, 1.0, beta, gamma, t))
       return ggTrue;
   }
         
   {
   ggRay3 r000_010(p000, p010 - p000); 
   if (ggRayTriangleIntersect(r000_010, p0, p1, p2, 0.0, 1.0, beta, gamma, t))
       return ggTrue;
   }
         
   {
   ggRay3 r100_110(p100, p110 - p100); 
   if (ggRayTriangleIntersect(r100_110, p0, p1, p2, 0.0, 1.0, beta, gamma, t))
       return ggTrue;
   }
         
   {
   ggRay3 r010_110(p010, p110 - p010); 
   if (ggRayTriangleIntersect(r010_110, p0, p1, p2, 0.0, 1.0, beta, gamma, t))
       return ggTrue;
   }

         
// bottom xy edges
   {
   ggRay3 r001_101(p001, p101 - p001); 
   if (ggRayTriangleIntersect(r001_101, p0, p1, p2, 0.0, 1.0, beta, gamma, t))
       return ggTrue;
   }
         
   {
   ggRay3 r001_011(p001, p011 - p001); 
   if (ggRayTriangleIntersect(r001_011, p0, p1, p2, 0.0, 1.0, beta, gamma, t))
       return ggTrue;
   }
         
   {
   ggRay3 r101_111(p101, p111 - p101); 
   if (ggRayTriangleIntersect(r101_111, p0, p1, p2, 0.0, 1.0, beta, gamma, t))
       return ggTrue;
   }
         
   {
   ggRay3 r011_111(p011, p111 - p011); 
   if (ggRayTriangleIntersect(r011_111, p0, p1, p2, 0.0, 1.0, beta, gamma, t))
       return ggTrue;
   }

// z edges
   {
   ggRay3 r000_001(p000, p001 - p000); 
   if (ggRayTriangleIntersect(r000_001, p0, p1, p2, 0.0, 1.0, beta, gamma, t))
       return ggTrue;
   }

   {
   ggRay3 r010_011(p010, p011 - p010); 
   if (ggRayTriangleIntersect(r010_011, p0, p1, p2, 0.0, 1.0, beta, gamma, t))
       return ggTrue;
   }

   {
   ggRay3 r100_101(p100, p101 - p100); 
   if (ggRayTriangleIntersect(r100_101, p0, p1, p2, 0.0, 1.0, beta, gamma, t))
       return ggTrue;
   }
         
   {
   ggRay3 r110_111(p110, p111 - p110); 
   if (ggRayTriangleIntersect(r110_111, p0, p1, p2, 0.0, 1.0, beta, gamma, t))
       return ggTrue;
   }
         
   return ggFalse;

}


ggVector3 ggPhongVector(const ggONB3& uvw, const ggPoint2& p, double n)
{
    double phi = ggTwoPi * p.u();
    double cost = pow(1-p.v(), 1.0/(n + 1.0));
    double sint = sqrt(1.0 -cost*cost);
    ggVector3 a(cos(phi)*sint, sin(phi)*sint, cost);
    
    return ggVector3 (
                  uvw.u().x()*a.x() + uvw.v().x()*a.y() + uvw.w().x()*a.z(), 
                  uvw.u().y()*a.x() + uvw.v().y()*a.y() + uvw.w().y()*a.z(), 
                  uvw.u().z()*a.x() + uvw.v().z()*a.y() + uvw.w().z()*a.z());
}

ggVector3 ggHalfPhongVector(const ggONB3& uvw, const ggPoint2& p, double n)
{
    double phi = ggTwoPi * p.u();
    double cost = 2.0 * pow(1- p.v(), 2.0/(n+2.0)) - 1.0;
    double sint = sqrt(1.0 -cost*cost);
    ggVector3 a(cos(phi)*sint, sin(phi)*sint, cost);
    
    return ggVector3 (
                  uvw.u().x()*a.x() + uvw.v().x()*a.y() + uvw.w().x()*a.z(), 
                  uvw.u().y()*a.x() + uvw.v().y()*a.y() + uvw.w().y()*a.z(), 
                  uvw.u().z()*a.x() + uvw.v().z()*a.y() + uvw.w().z()*a.z());
}

ggVector3 ggDiffuseVector(const ggONB3& uvw, const ggPoint2& p)
{
    double phi = ggTwoPi * p.u();
    double sint = sqrt(p.v());
    double cost = sqrt(1 - p.v());
    ggVector3 a(cos(phi)*sint, sin(phi)*sint, cost);
    
    return ggVector3(
                  uvw.u().x()*a.x() + uvw.v().x()*a.y() + uvw.w().x()*a.z(), 
                  uvw.u().y()*a.x() + uvw.v().y()*a.y() + uvw.w().y()*a.z(), 
                  uvw.u().z()*a.x() + uvw.v().z()*a.y() + uvw.w().z()*a.z());
}


ggVector3 ggPerturb(const ggVector3& vec, double down, double around) {
    ggONB3 uvw;
    ggVector3 a;

    ggVector3 u = vec;
    ggVector3 v(u.y(), -u.x(), 0.0);
    if (fabs(ggDot(v, v)) < 0.01)
        v.Set(0.0, u.z(), -u.y());
    uvw.InitFromUV(u, v);
    ggHCanonicalToBasisRotationMatrix3 rthere(uvw);
    ggHBasisToCanonicalRotationMatrix3 rback(uvw);
    a = rthere * vec;
    ggHXRotationMatrix3 rx(around);
    ggHYRotationMatrix3 ry(down);
    a = ry * a;
    a = rx * a;
    a = rback * a;
    return a;
}


// ((o + tv) - c)^2 - r^2 = 0
// dot(v,v)t^2 + 2dot(o-c, v)t + dot(o-c,o-c) - r^2 = 0
// a = dot(v,v)
// b = 2dot(o-c, v)
// c = dot(o-c,o-c) - r^2
// first check for first solution t1 = (-b - sqrt(b^2 - 4ac)) / (2a)
// if that is less than tmin then t2 = t1 + sqrt(b^2 - 4ac) / a;
ggBoolean ggRaySphereIntersect(const ggRay3& r,
                               const ggPoint3& center,
                               double radius,
                               double tmin,
                               double tmax,
                               double& t) {

    ggVector3 oc = r.origin() - center;
    double a = ggDot(r.direction(), r.direction());
    double b = 2.0 * ggDot(oc, r.direction());
    double c = ggDot(oc, oc) - radius*radius;

    return ggSmallestQuadraticRoot(a, b, c, tmin, tmax, t);
}


ggBoolean ggRayTriangleIntersect(const ggRay3& r,
                              const ggPoint3& p0,
                              const ggPoint3& p1,
                              const ggPoint3& p2,
                              double tmin,
                              double tmax,
			      double& beta,
			      double& gamma,
                              double& t) {
/*

     the intersection of ray "o + tv" with triangle p0p1p2:

         o + tv = p0 + beta (p1 - p0) + gamma (p2 - p0)

     can be written as a 3 by 3 linear system:

       | (p0.x - p1.x)  (p0.x - p2.x)  v.x | |beta |   | (p0.x - o.x ) |
       | (p0.y - p1.y)  (p0.y - p2.y)  v.y | |gamma| = | (p0.y - o.y ) |
       | (p0.z - p1.z)  (p0.z - p2.z)  v.z | |  t  |   | (p0.z - o.z ) |

     we solve using Cramer's rule with the following:

       | A D G | |beta |   | J |
       | B E H | |gamma|   | K |
       | C F I | |  t  |   | L |

     we reuse some terms noting that:

              J(EI-HF) + K(GF-DI) + L(DH-EG)
       beta = ------------------------------
              A(EI-HF) + B(GF-DI) + C(DH-EG)

      if beta is not in [0,1] we return false, otherwise we look at gamma
      and t.

                 I(AK-JB) + H(JC-AL) + G(BL-KC)
       gamma =   ------------------------------
                            |M|

       
       return false if gamma is not in [0,1]

               
                  F(AK-JB) + E(JC-AL) + D(BL-KC)
       t     = -  ------------------------------
                            |M|

        return false if t < epsilon


      we also note that we can do some precomputing because we
      are only interested in solutions where:

      beta > 0, gamma > 0, beta+gamma < 1, t > tmin, t < tmax.

      Note that for valid solutions (arbitrary k in each expression):

                      k*beta > min(k, 0) 
                     k*gamma > min(k, 0) 
          k1*beta + k2*gamma < max(0, k1, k2)
                         k*t > min(k*tmin, k*tmax)
                         k*t < max(k*tmin, k*tmax)
               
      so, a*beta + b*gamma + c*t = d will not have a valid
      solution if:

          d < min(a,0) + min(b,0) + min(c*tmin, c*tmax)
      or
          d > max(0, a, b) + max(c*tmin, c*tmax)


*/
     double tolerance=0.000001;

     double A = p0.x() - p1.x();
     double B = p0.y() - p1.y();
     double C = p0.z() - p1.z();

     double D = p0.x() - p2.x();
     double E = p0.y() - p2.y();
     double F = p0.z() - p2.z();

     double G = r.direction().x();
     double H = r.direction().y();
     double I = r.direction().z();

     double J = p0.x() - r.origin().x();
     double K = p0.y() - r.origin().y();
     double L = p0.z() - r.origin().z();

     double EIHF = E*I-H*F;
     double GFDI = G*F-D*I;
     double DHEG = D*H-E*G;

/*
     // A beta + D gamma + G t = J
     double ctmin = G*tmin;
     double ctmax = G*tmax;
     if (J < ggMin(0, A) + ggMin(0, D) + ggMin(ctmin, ctmax) ||
         J > ggMax(0, A, D) + ggMax(ctmax, ctmin) ) return ggFalse;
*/

     double inv_denom = (A*EIHF + B*GFDI + C*DHEG);
     if (fabs(inv_denom) < tolerance) return ggFalse;

     inv_denom = 1.0 / inv_denom;

     beta = inv_denom * (J*EIHF + K*GFDI + L*DHEG);

     if (beta < 0.0 || beta > 1) return ggFalse;

     double AKJB = A*K - J*B;
     double JCAL = J*C - A*L;
     double BLKC = B*L - K*C;

     gamma = inv_denom * (I*AKJB + H*JCAL + G*BLKC);
     if (gamma < 0.0 || beta + gamma > 1.0) return ggFalse;

     t = -inv_denom * (F*AKJB + E*JCAL + D*BLKC);

     return (t >= tmin && t <= tmax);


}


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
                               double& t) {


       if (ggAbs(r.direction().z()) < 0.000001) return ggFalse;

       t = (z - r.origin().z()) / r.direction().z();
       if (t < tmin || t > tmax) return ggFalse;

       x = r.origin().x() + t * r.direction().x();
       if (x < xmin || x > xmax) return ggFalse;

       y = r.origin().y() + t * r.direction().y();
       return (y > ymin && y < ymax);
     
}


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
                               double& t) {


       if (ggAbs(r.direction().y()) < 0.000001) return ggFalse;

       t = (y - r.origin().y()) / r.direction().y();
       if (t < tmin || t > tmax) return ggFalse;

       x = r.origin().x() + t * r.direction().x();
       if (x < xmin || x > xmax) return ggFalse;

       z = r.origin().z() + t * r.direction().z();
       return (z > zmin && z < zmax);
     
}


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
                               double& t) {


       if (ggAbs(r.direction().x()) < 0.000001) return ggFalse;

       t = (x - r.origin().x()) / r.direction().x();
       if (t < tmin || t > tmax) return ggFalse;

       y = r.origin().y() + t * r.direction().y();
       if (y < ymin || y > ymax) return ggFalse;

       z = r.origin().z() + t * r.direction().z();
       return (z > zmin && z < zmax);

}


ggBoolean ggRayOriginSphereIntersect(const ggRay3& r,
                                  double radius, 
                                  double tmin,
                                  double tmax,
				  double& t) {
    return ggRaySphereIntersect(r,ggOrigin3,radius, tmin, tmax, t);
}



// r = o + tv.
// r.x^2 + r.y^2 - radius^2 = 0.
// (v.x*v.x + v.y*v.y)t^2 + 2(v.x*o.x + v.y*o.y)t + o.x*o.x + o.y*o.y - radius^2 = 0
ggBoolean ggRayZCylinderIntersect(const ggRay3& r,
                            double wmax,
                            double radius,
                            double tmin,
                            double tmax,
                            ggPoint3& p,
                            double& t)
{
    ggPoint3  o = r.origin();
    ggVector3 v = r.direction();
    double a = v.x() * v.x() + v.y() * v.y();
    double b = 2*(v.x() * o.x() + v.y() * o.y());
    double c = o.x() * o.x() + o.y() * o.y() - radius*radius;

    if (ggSmallestQuadraticRoot(a, b, c, tmin, tmax, t)) {
         p = r.pointAtParameter(t);
         return (p.z() > 0.0 && p.z() < wmax);
    }
    else
        return ggFalse;
}




ggBoolean ggRayXYDiskIntersect(const ggRay3& r,
			       const ggPoint3& center,
			       double radius,
                               double tmin,
                               double tmax,
                               double& x,
                               double& y,
			       double& t)
{

    if (ggAbs(r.direction().z()) < ggEpsilon) return ggFalse;

    t = (center.z() - r.origin().z()) / r.direction().z();
    if (t < tmin || t > tmax) return ggFalse;

    // Check if the ray hits the disk.
    x = r.origin().x() + t * r.direction().x() - center.x();
    y = r.origin().y() + t * r.direction().y() - center.y();
    
    if (x*x + y*y > radius*radius) return ggFalse;
    return ggTrue;
}


ggBoolean ggRayYZDiskIntersect(const ggRay3& r,
			       const ggPoint3& center,
			       double radius,
                               double tmin,
                               double tmax,
                               double& y,
                               double& z,
			       double& t)
{

    if (ggAbs(r.direction().x()) < ggEpsilon) return ggFalse;

    t = (center.x() - r.origin().x()) / r.direction().x();
    if (t < tmin || t > tmax) return ggFalse;

    // Check if the ray hits the disk.
    y = r.origin().y() + t * r.direction().y() - center.y();
    z = r.origin().z() + t * r.direction().z() - center.z();
    
    if (y*y + z*z > radius*radius) return ggFalse;
    return ggTrue;
}


ggBoolean ggRayXZDiskIntersect(const ggRay3& r,
			       const ggPoint3& center,
			       double radius,
                               double tmin,
                               double tmax,
                               double& x,
                               double& z,
			       double& t)
{

    if (ggAbs(r.direction().y()) < ggEpsilon) return ggFalse;

    t = (center.y() - r.origin().y()) / r.direction().y();
    if (t < tmin || t > tmax) return ggFalse;

    // Check if the ray hits the disk.
    x = r.origin().x() + t * r.direction().x() - center.x();
    z = r.origin().z() + t * r.direction().z() - center.z();
    
    if (x*x + z*z > radius*radius) return ggFalse;
    return ggTrue;
}


ggBoolean ggRayXYEllipseIntersect(const ggRay3& r,
				  const ggPoint3& center,
				  double a,
				  double b,
                               double tmin,
                               double tmax,
                               double& x,
                               double& y,
				  double& t)
{

    if (ggAbs(r.direction().z()) < ggEpsilon) return ggFalse;

    t = (center.z() - r.origin().z()) / r.direction().z();
    if (t < tmin || t > tmax) return ggFalse;

    // Check if the ray hits the ellipsoid.
    x = r.origin().x() + t * r.direction().x() - center.x();
    y = r.origin().y() + t * r.direction().y() - center.y();
    
    double b2 = b*b;
    double a2 = a*a;
    if (x * x * b2 + y * y * a2 > a2*b2)
      return ggFalse;
    return ggTrue;
}

ggBoolean ggRayYZEllipseIntersect(const ggRay3& r,
				  const ggPoint3& center,
				  double a,
				  double b,
                               double tmin,
                               double tmax,
                               double& y,
                               double& z,
				  double& t)
{

    if (ggAbs(r.direction().x()) < ggEpsilon) return ggFalse;

    t = (center.x() - r.origin().x()) / r.direction().x();
    if (t < tmin || t > tmax) return ggFalse;

    // Check if the ray hits the ellipsoid.
    y = r.origin().y() + t * r.direction().y() - center.y();
    z = r.origin().z() + t * r.direction().z() - center.z();
    
    double b2 = b*b;
    double a2 = a*a;
    if (y * y * b2 + z * z *a2 > a2*b2)
      return ggFalse;
    return ggTrue;
}

ggBoolean ggRayXZEllipseIntersect(const ggRay3& r,
				  const ggPoint3& center,
				  double a,
				  double b,
                               double tmin,
                               double tmax,
                               double& x,
                               double& z,
				  double& t)
{

    if (ggAbs(r.direction().y()) < ggEpsilon) return ggFalse;

    t = (center.y() - r.origin().y()) / r.direction().y();
    if (t < tmin || t > tmax) return ggFalse;

    // Check if the ray hits the ellipsoid.
    x = r.origin().x() + t * r.direction().x() - center.x();
    z = r.origin().z() + t * r.direction().z() - center.z();
    
    double b2 = b*b;
    double a2 = a*a;
    if (x * x * b2 + z * z * a2 > 1.0) 
      return ggFalse;
    return ggTrue;
}


// ray is r = o + tv.

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
                            double& t)

{
    ggVector3 v = r.direction();
    double a = v.y() * v.y() + v.z() * v.z();

    ggPoint3  o = r.origin();
    double oc1 = o.y() - yCenter;
    double oc2 = o.z() - zCenter;
    double b = 2*(v.y() * oc1 + v.z() * oc2);
    double c = oc1 * oc1 + oc2 * oc2 - radius*radius;

    if (ggSmallestQuadraticRoot(a, b, c, tmin, tmax, t)) {
         p = r.pointAtParameter(t);
         return (p.x() > xMin && p.x() < xMax);
    }
    else
        return ggFalse;
  }  

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
                            double& t)

{
    ggVector3 v = r.direction();
    double a = v.z() * v.z() + v.x() * v.x();

    ggPoint3  o = r.origin();
    double oc1 = o.z() - zCenter;
    double oc2 = o.x() - xCenter;
    double b = 2*(v.z() * oc1 + v.x() * oc2);
    double c = oc1 * oc1 + oc2 * oc2 - radius*radius;

    if (ggSmallestQuadraticRoot(a, b, c, tmin, tmax, t)) {
         p = r.pointAtParameter(t);
         return (p.y() > yMin && p.y() < yMax);
    }
    else
        return ggFalse;
  }

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
                            double& t)

{
    ggVector3 v = r.direction();
    double a = v.x() * v.x() + v.y() * v.y();

    ggPoint3  o = r.origin();
    double oc1 = o.x() - xCenter;
    double oc2 = o.y() - yCenter;
    double b = 2*(v.x() * oc1 + v.y() * oc2);
    double c = oc1 * oc1 + oc2 * oc2 - radius*radius;

    if (ggSmallestQuadraticRoot(a, b, c, tmin, tmax, t)) {
         p = r.pointAtParameter(t);
         return (p.z() > zMin && p.z() < zMax);
    }
    else
        return ggFalse;
}  

// XEllipticalCylinder
// b^2*(r.y-yCenter)^2 + a^2*(r.z-zCenter)^2 - a^2 * b^2 = 0.
// (v.y*v.y*b^2 + v.z*v.z*a^2)t^2 + 2(v.y*(o.y-yCenter)*b^2 + v.z*(o.z-zCenter)*a^2)t + 
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
                            double& t)

{
    ggVector3 v = r.direction();
    double asq = r1*r1;
    double bsq = r2*r2;

    double a = v.y() * v.y() * bsq + v.z() * v.z() * asq;

    ggPoint3  o = r.origin();
    double oc1 = o.y() - yCenter;
    double oc2 = o.z() - zCenter;
    double b = 2*(v.y() * oc1 * bsq + v.z() * oc2 * asq);
    double c = oc1 * oc1 * bsq + oc2 * oc2 * asq - asq*bsq;

    if (ggSmallestQuadraticRoot(a, b, c, tmin, tmax, t)) {
         p = r.pointAtParameter(t);
         return (p.x() > xMin && p.x() < xMax);
    }
    else
        return ggFalse;
    
  }  

// YEllipticalCylinder
// b^2*(r.z-zCenter)^2 + a^2*(r.x-xCenter)^2 - a^2 * b^2 = 0.
// (v.z*v.z*b^2 + v.x*v.x*a^2)t^2 + 2(v.z*(o.z-zCenter)*b^2 + v.x*(o.x-xCenter)*a^2)t + 
// ((o.z-zCenter)*(o.z-zCenter)*b^2 + (o.x-xCenter)*(o.x-xCenter)*a^2 - a^2 * b^2= 0
ggBoolean ggRayYEllipticalCylinderIntersect(const ggRay3& r,
                            double xCenter,
                            double yMin,
                            double yMax,
                            double zCenter,
                            double r1,
                            double r2,
                            double tmin,
                            double tmax,
                            ggPoint3& p,
                            double& t)

{
    ggVector3 v = r.direction();
    double bsq = r1*r1;
    double asq = r2*r2;

    double a = v.z() * v.z() * bsq + v.x() * v.x() * asq;

    ggPoint3  o = r.origin();
    double oc1 = o.z() - zCenter;
    double oc2 = o.x() - xCenter;
    double b = 2*(v.z() * oc1 * bsq + v.x() * oc2 * asq);
    double c = oc1 * oc1 * bsq + oc2 * oc2 * asq - asq*bsq;

    if (ggSmallestQuadraticRoot(a, b, c, tmin, tmax, t)) {
         p = r.pointAtParameter(t);
         return (p.y() > yMin && p.y() < yMax);
    }
    else
        return ggFalse;
}  

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
                            double& t)

{
    ggVector3 v = r.direction();
    double asq = r1*r1;
    double bsq = r2*r2;

    double a = v.x() * v.x() * bsq + v.y() * v.y() * asq;

    ggPoint3  o = r.origin();
    double oc1 = o.x() - xCenter;
    double oc2 = o.y() - yCenter;
    double b = 2*(v.x() * oc1 * bsq + v.y() * oc2 * asq);
    double c = oc1 * oc1 * bsq + oc2 * oc2 * asq - asq*bsq;

    if (ggSmallestQuadraticRoot(a, b, c, tmin, tmax, t)) {
         p = r.pointAtParameter(t);
         return (p.z() > zMin && p.z() < zMax);
    }
    else
        return ggFalse;
}
