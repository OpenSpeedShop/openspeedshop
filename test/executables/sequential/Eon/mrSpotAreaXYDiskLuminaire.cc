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



#include <mrSpotAreaXYDiskLuminaire.h>
#include <ggMacros.h>
#include <ggGeometry.h>
#include <math.h>

ggBoolean mrSpotAreaXYDiskLuminaire::print(ostream& s) const {
    s << "mrSpotAreaXYDiskLuminaire" << "\n";
    s << center << " " << radius << "\n";
    s << kEmit << "\n";
    return ggTrue;
}



ggBoolean mrSpotAreaXYDiskLuminaire::shadowHit(        
             const ggRay3& ray,
             double time,
             double tmin,
             double tmax,
             double& t,
             ggVector3& N,
             ggBoolean& emits,
             ggSpectrum& E
             ) const {

   if (mrXYDisk::shadowHit(ray, time, tmin, tmax, t, N, emits, E)) {
       double cosine = -ggDot(ggUnitVector(ray.direction()), N);
       if ( cosine > maxCosine) {
          emits = ggTrue;
          E = kEmit;
       }
       return ggTrue;
   }
   else
      return ggFalse; 
}

ggBoolean mrSpotAreaXYDiskLuminaire::viewingHit(        
             const ggRay3& ray,
             double time,
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord& MR) const
{


  if (mrXYDisk::viewingHit(ray, time, tmin, tmax, VHR, MR)) {
       double cosine = -ggDot(ray.direction(), VHR.UVW.w());
       if (cosine > maxCosine) {
          VHR.hasEmit = ggTrue;
          VHR.kEmit = kEmit;
       }
       return ggTrue;
   }
   else
      return ggFalse;
}



// Selects a point visible from x given a uv-pair.  Sometimes
// returning a non-visible point is allowed, but not desirable.
// Here, visible means not SELF-shadowed.
ggBoolean mrSpotAreaXYDiskLuminaire::selectVisiblePoint(
             const ggPoint3&,  // viewpoint
             const ggVector3&,  // normal at x
             const ggPoint2& uv,  // input coordinate for warping
             const double, // time (ignored)
             ggPoint3& onLight,  // point corresponding to uv
             double& invProb ) const  //  one over probability
                                      // if uv is random in [0,1]^2
{

   ggPoint2 p2 = ggPointOnDisk(radius, uv);
   onLight.Set(p2.u(), p2.v(), 0.0);

   invProb = area;
   return ggTrue;
}



ggBoolean mrSpotAreaXYDiskLuminaire::approximateDirectRadiance(
             const ggPoint3& x,     // point on object being lighted
             const ggVector3& N,    // normal at x
             const double ,  // time of query
             ggSpectrum& L)        // estimate
             const
{
      double cosx, cosl;
      ggVector3 tox;
      double Lc;

      double d2 = ggSquaredDistance(x,ggOrigin3);
      tox = ggUnitVector(x - ggOrigin3);
      if ((cosl = ggDot(ggZAxis3, tox)) > 0.00001 &&
          (cosx = -ggDot(N,tox)) > 0.00001)
           Lc =  (cosx*cosl*area) / d2;
      L = kEmit * Lc;
      return ggTrue;

}
