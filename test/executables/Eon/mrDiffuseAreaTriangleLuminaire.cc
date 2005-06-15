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



#include <mrDiffuseAreaTriangleLuminaire.h>
#include <ggMacros.h>
#include <ggGeometry.h>
#include <math.h>

ggBoolean mrDiffuseAreaTriangleLuminaire::print(ostream& s) const {
    s << "mrDiffuseAreaTriangleLuminaire" << "\n";
    s << triangle << "\n";
    s << kEmit << "\n";
    return ggTrue;
}


mrDiffuseAreaTriangleLuminaire::mrDiffuseAreaTriangleLuminaire(
                              const ggPoint3 pt0,
                              const ggPoint3 pt1,
                              const ggPoint3 pt2,
                              const ggSpectrum& emit) 
                         : mrFastTriangle(pt0, pt1, pt2) {
              kEmit = emit;
              centroid.Set( 0.333*(pt0.x() + pt1.x() + pt2.x()),
                            0.333*(pt0.y() + pt1.y() + pt2.y()),
                            0.333*(pt0.z() + pt1.z() + pt2.z()));
   ggVector3 v = pt1 - pt0;
   uvw.InitFromWU( triangle.normal(), v );
   area = triangle.area();
}


ggBoolean mrDiffuseAreaTriangleLuminaire::shadowHit(        
             const ggRay3& ray,
             double time,
             double tmin,
             double tmax,
             double& t,
             ggVector3& N,
             ggBoolean& emits,
             ggSpectrum& E
             ) const {

   if (mrFastTriangle::shadowHit(ray, time, tmin, tmax, t, N, emits, E)) {
       emits = ggTrue;
       E = kEmit;
       return ggTrue;
   }
   else
      return ggFalse; 
}

ggBoolean mrDiffuseAreaTriangleLuminaire::viewingHit(        
             const ggRay3& ray,
             double time,
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord& MR) const
{


  if (mrFastTriangle::viewingHit(ray, time, tmin, tmax, VHR, MR)) {
       VHR.hasEmit = ggTrue;
       VHR.kEmit = kEmit;
       return ggTrue;
   }
   else
      return ggFalse;
}



// Selects a point visible from x given a uv-pair.  Sometimes
// returning a non-visible point is allowed, but not desirable.
// Here, visible means not SELF-shadowed.
ggBoolean mrDiffuseAreaTriangleLuminaire::selectVisiblePoint(
             const ggPoint3&,  // viewpoint
             const ggVector3&,  // normal at x
             const ggPoint2& uv,  // input coordinate for warping
             const double, // time (ignored)
             ggPoint3& onLight,  // point corresponding to uv
             double& invProb ) const  //  one over probability
                                      // if uv is random in [0,1]^2
{

   onLight =  ggPointOnTriangle( triangle[0], triangle[1] , triangle[2], uv);

   invProb = area;
   return ggTrue;
}



ggBoolean mrDiffuseAreaTriangleLuminaire::approximateDirectRadiance(
             const ggPoint3& x,     // point on object being lighted
             const ggVector3& N,    // normal at x
             const double ,  // time of query
             ggSpectrum& L)        // estimate
             const
{
      double cosx, cosl;
      ggVector3 tox;
      ggSpectrum L0((float)0.0), L1((float)0.0), L2((float)0.0), Lc((float)0.0);
      ggPoint3 p0 = triangle[0];
      ggPoint3 p1 = triangle[1];
      ggPoint3 p2 = triangle[2];

      double d2 = ggSquaredDistance(x,centroid);
      tox = ggUnitVector(x - centroid);
      if ((cosl = ggDot(uvw.w(), tox)) > 0.00001 &&
          (cosx = -ggDot(N,tox)) > 0.00001)
           Lc =  (cosx*cosl*area) / d2;
      if (d2 > 10) {
          L = kEmit * Lc;
          return ggTrue;
      }

      tox = ggUnitVector(x - p0);
      if ((cosl = ggDot(uvw.w(), tox)) > 0.00001 &&
          (cosx = -ggDot(N,tox)) > 0.00001)
           L0 =  (cosx*cosl*area) / ggSquaredDistance(x,p0);

      tox = ggUnitVector(x - p1);
      if ((cosl = ggDot(uvw.w(), tox)) > 0.00001 &&
          (cosx = -ggDot(N,tox)) > 0.00001)
           L1 =  (cosx*cosl*area) / ggSquaredDistance(x,p1);

      tox = ggUnitVector(x - p2);
      if ((cosl = ggDot(uvw.w(), tox)) > 0.00001 &&
          (cosx = -ggDot(N,tox)) > 0.00001)
           L2 =  (cosx*cosl*area) / ggSquaredDistance(x,p2);

      L = 0.25*kEmit * (Lc + L0 + L1 + L2);

      return ggTrue;
}
