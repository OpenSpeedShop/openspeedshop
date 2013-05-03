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



#include <mrDiffuseAreaSphereLuminaire.h>
#include <math.h>



// Selects a point visible from x given a uv-pair.  Sometimes
// returning a non-visible point is allowed, but not desirable.
// Here, visible means not SELF-shadowed.
ggBoolean mrDiffuseAreaSphereLuminaire::selectVisiblePoint(
             const ggPoint3& ,  // viewpoint
             const ggVector3&,  // normal at x
             const ggPoint2& uv,  // input coordinate for warping
             const double, // time (ignored)
             ggPoint3& onLight,  // point corresponding to uv
             double& invProb ) const  // probability if uv is random in [0,1]^2
{
     onLight = sphere.getUniformRandomPoint(uv);
     invProb = sphere.area();
     
     return ggTrue;
}



ggBoolean mrDiffuseAreaSphereLuminaire::shadowHit(        
             const ggRay3& ray,
             double time,
             double tmin,
             double tmax,
             double& t,
              ggVector3& N,
             ggBoolean& emits,
             ggSpectrum& E
             ) const {

   if (mrSphere::shadowHit(ray, time, tmin, tmax, t, N, emits, E)) {
       emits = ggTrue;
       E = kEmit;
       return ggTrue;
   }
   else
      return ggFalse; 
}

ggBoolean mrDiffuseAreaSphereLuminaire::viewingHit(        
             const ggRay3& ray,
             double time,
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord& MR) const
{
    if (mrSphere::viewingHit(ray, time, tmin, tmax, VHR, MR)) {
       VHR.hasEmit = ggTrue;
       VHR.kEmit = kEmit;
       return ggTrue;
    }
    else
       return ggFalse;
}



ggBoolean mrDiffuseAreaSphereLuminaire::approximateDirectRadiance(
             const ggPoint3& x,     // point on object being lighted
             const ggVector3& N,    // normal at x
             const double,             // time of query
             ggSpectrum& L)               // estimate
             const
{
     ggPoint3 c = center();
     double r = radius();
     double t, omega, Ptotal, cosT, Nw;
     ggVector3 psi = c - x;

     // if the whole luminaire is below the horizon.
     if(ggDot(N, psi + r * N) < 0)
        L = ggSpectrum((float)0.0);
     // if the whole luminaire is above the horizon.
     if(ggDot(N, psi - r * N) > 0)
     {
        Nw = ggDot(N,c -x) / ggDistance(x,c);
        Ptotal = ggPi * Nw * r * r / ggSquaredDistance(x,c);  // This is analytical solution
        L = kEmit * Ptotal;
     }
     else // if part of the luminaire is above and part of it is below the horizon.
     {
        t = - ggDot(N,ggUnitVector(psi)) * ggDistance(x,c);  // x + psi + t N sits on the horizon
        psi += 0.5 * (t + r) * N;
        cosT = ggDot(N,ggUnitVector(psi));
        omega = (0.5 * ggPi * r*r - t * sqrt(r*r - t*t) - r*r*asin(t/r)) / ggSquaredDistance(x,c);
        Ptotal = cosT * omega;  // This is a guess of real Ptotal
        L = kEmit * Ptotal;
     }

     return ggTrue;
}
