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



#include <mrPhongAreaTriangleLuminaire.h>
#include <math.h>



mrPhongAreaTriangleLuminaire::mrPhongAreaTriangleLuminaire(const ggPoint3 p0,
                               const ggPoint3 p1,
                               const ggPoint3 p2,
                        const ggSpectrum& emit,
                        double N) : mrTriangle(p0, p1, p2) {
              phongN = N;
              kEmit = emit;
              centroid.Set( 0.333*(p0.x() + p1.x() + p2.x()),
                            0.333*(p0.y() + p1.y() + p2.y()),
                            0.333*(p0.z() + p1.z() + p2.z()));
   ggVector3 v1 = p1 - p0;
   ggVector3 v2 = p2 - p0;

   ggVector3 v3 = ggCross(v1, v2);
   uvw.InitFromWV(v3,v2);
   area = (0.5 * sqrt(ggDot(v3,v3)));     
}


ggBoolean mrPhongAreaTriangleLuminaire::shadowHit(        
             const ggRay3& ray,
             double ,
             double tmin,
             double tmax,
             double& t,
             ggVector3& N,
             ggBoolean& emits,
             ggSpectrum& E
             ) const {

   double beta, gamma;
   if (ggRayTriangleIntersect(ray, triangle, tmin, tmax, beta, gamma, t))
   {
       ggVector3 nv = ggUnitVector(ray.direction());
       double cost = -ggDot(uvw.w(), nv);
       if (cost <= 0.0) 
           return ggFalse;
       emits = ggTrue;
       E = kEmit * 0.5 * (phongN +1) * pow(cost, phongN -1);
       N = uvw.w();

       return ggTrue;
   }
   else
      return ggFalse; 
}

ggBoolean mrPhongAreaTriangleLuminaire::viewingHit(        
             const ggRay3& ray,
             double,
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord& ) const

{

   double beta, gamma;

if (ggRayTriangleIntersect(ray, triangle, tmin, tmax, beta, gamma, VHR.t))
   {
       ggVector3 nv = ggUnitVector(ray.direction());
       double cost = -ggDot(VHR.UVW.w(), nv);
       if (cost <= 0.0) 
           return ggFalse;
       VHR.kEmit = kEmit * 0.5 * (phongN +1) * pow(cost, phongN -1);
       VHR.hasEmit = ggTrue;

       VHR.hasUVW = ggTrue;
       VHR.hasUV = ggTrue;
       VHR.UV = ggPoint2(beta, gamma);
       VHR.p = ray.pointAtParameter(VHR.t);
       VHR.UVW = uvw;

       return ggTrue;
   }
   else
      return ggFalse;
}



// Selects a point visible from x given a uv-pair.  Sometimes
// returning a non-visible point is allowed, but not desirable.
// Here, visible means not SELF-shadowed.
ggBoolean mrPhongAreaTriangleLuminaire::selectVisiblePoint(
             const ggPoint3&,  // viewpoint
             const ggVector3&,  // normal at x
             const ggPoint2& uv,  // input coordinate for warping
             const double, // time (ignored)
             ggPoint3& onLight,  // point corresponding to uv
             double& invProb ) const  // probability if uv is random in [0,1]^2
{
 
   onLight =  ggPointOnTriangle(triangle[0],triangle[1],triangle[2],uv);

   invProb =  area;
   return ggTrue;
}



ggBoolean mrPhongAreaTriangleLuminaire::approximateDirectRadiance(
             const ggPoint3& x,     // point on object being lighted
             const ggVector3& N,    // normal at x
             const double ,  // time of query
             ggSpectrum& L)        // estimate
             const
{
      double cosx, cosl;
      ggVector3 tox = ggUnitVector(x - centroid);
      ggPoint3 p0(triangle[0]);
      ggPoint3 p1(triangle[1]);
      ggPoint3 p2(triangle[2]);

      if ((cosl = ggDot(uvw.w(), tox)) < 0.00001 ||
          (cosx = -ggDot(N,tox)) < 0.0001)  {
               tox = ggUnitVector(x - p0);
               if  ((cosl = ggDot(uvw.w(), tox)) < 0.00001 ||
                    (cosx = -ggDot(N,tox)) < 0.0001)  {
                  tox = ggUnitVector(x - p1);
                  if ((cosl = ggDot(uvw.w(), tox)) < 0.00001 ||
                    (cosx = -ggDot(N,tox)) < 0.0001)  {
                       tox = ggUnitVector(x - p2);
                       if ((cosl = ggDot(uvw.w(), tox)) < 0.00001 ||
                           (cosx = -ggDot(N,tox)) < 0.0001)  {
                              L = ggSpectrum((float)0.0);
                              return ggTrue;
                       }
                  }

               }
         ggSpectrum E = kEmit * 0.5 * (phongN +1) * pow(cosl, phongN -1);
         L =  E *  (0.5*cosx*cosl*area) / ggSquaredDistance(x,centroid);
         return ggTrue;
      }
      else {
         ggSpectrum E = kEmit * 0.5 * (phongN +1) * pow(cosl, phongN -1);
         L =  E *  (cosx*cosl*area) / ggSquaredDistance(x,centroid);
         return ggTrue;
      }
}
