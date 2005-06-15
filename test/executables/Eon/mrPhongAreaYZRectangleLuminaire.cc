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



#include <mrPhongAreaYZRectangleLuminaire.h>
#include <ggGeometry.h>
#include <ggMacros.h>
#include <math.h>

ggBoolean mrPhongAreaYZRectangleLuminaire::shadowHit(        
             const ggRay3& ray,
             double time ,
             double tmin,
             double tmax,
             double& t,
             ggVector3& N,
             ggBoolean& emits,
             ggSpectrum& E) const {

   if (mrYZRectangle::shadowHit(ray,time,tmin,tmax,t,N,emits,E)) {
       emits = ggTrue;
       double cost = -ray.direction().x() * N.x() / ray.direction().length();
       E = kEmit * 0.5 * (phongN +1) * pow(cost, phongN -1);

       return ggTrue;
   }
   else
      return ggFalse; 
}

ggBoolean mrPhongAreaYZRectangleLuminaire::viewingHit(        
             const ggRay3& ray,
             double time,
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord& MR
             ) const
{

   if (mrYZRectangle::viewingHit(ray,time,tmin,tmax,VHR,MR)) {
       VHR.hasEmit = ggTrue;
       double cost = -ray.direction().x() * VHR.UVW.w().x() /
                                         ray.direction().length();
       VHR.kEmit = kEmit * 0.5 * (phongN +1) * pow(cost, phongN -1);


       return ggTrue;
   }
   else
      return ggFalse;
}

// Selects a point visible from x given a uv-pair.  Sometimes
// returning a non-visible point is allowed, but not desirable.
// Here, visible means not SELF-shadowed.
ggBoolean mrPhongAreaYZRectangleLuminaire::selectVisiblePoint(
             const ggPoint3&,  // viewpoint
             const ggVector3&,  // normal at x
             const ggPoint2& uv,  // input coordinate for warping
             const double, // time (ignored)
             ggPoint3& onLight,  // point corresponding to uv
             double& invProb ) const  //  one over probability
                                      // if uv is random in [0,1]^2
{

   onLight.Set(x, yMin + uv.u() * (yMax - yMin), 
               zMin + uv.v() * (zMax - zMin));

   invProb = (yMax - yMin) * (zMax - zMin);
   return ggTrue;
}



ggBoolean mrPhongAreaYZRectangleLuminaire::approximateDirectRadiance(
             const ggPoint3& p,     // point on object being lighted
             const ggVector3& N,    // normal at x
             const double ,  // time of query
             ggSpectrum& L)        // estimate
             const
{
      double cosx, cosl;
      ggVector3 tox;
      ggVector3 Nprime;
      ggSpectrum L0((float)0.0), L1((float)0.0), L2((float)0.0), L3((float)0.0), Lc((float)0.0);
      ggPoint3 centroid(x, 0.5*(yMax + yMin), 0.5*(zMax + zMin));
      ggPoint3 p0(x, yMin, zMin);
      ggPoint3 p1(x, yMax, zMin);
      ggPoint3 p2(x, yMin, zMax);
      ggPoint3 p3(x, yMax, zMax);

      double area = (yMax - yMin) * (zMax - zMin);

          Nprime = ggXAxis;

      tox = ggUnitVector(p - centroid);
      if ((cosl = ggDot(Nprime , tox)) > 0.00001 &&
          (cosx = -ggDot(N,tox)) > 0.00001)
           Lc =  0.5*(phongN +1)*pow(cosl, phongN)*(cosx*area) /
                             ggSquaredDistance(p,centroid);


      tox = ggUnitVector(p - p0);
      if ((cosl = ggDot(Nprime , tox)) > 0.00001 &&
          (cosx = -ggDot(N,tox)) > 0.00001)
           L0 =  0.5*(phongN +1)*pow(cosl, phongN)*(cosx*area) /
                             ggSquaredDistance(p,p0);

      tox = ggUnitVector(p - p1);
      if ((cosl = ggDot(Nprime , tox)) > 0.00001 &&
          (cosx = -ggDot(N,tox)) > 0.00001)
           L1 =  0.5*(phongN +1)*pow(cosl, phongN)*(cosx*area) /
                             ggSquaredDistance(p,p1);

      tox = ggUnitVector(p - p2);
      if ((cosl = ggDot(Nprime , tox)) > 0.00001 &&
          (cosx = -ggDot(N,tox)) > 0.00001)
           L2 =  0.5*(phongN +1)*pow(cosl, phongN)*(cosx*area) /
                             ggSquaredDistance(p,p2);

      tox = ggUnitVector(p - p3);
      if ((cosl = ggDot(Nprime , tox)) > 0.00001 &&
          (cosx = -ggDot(N,tox)) > 0.00001)
           L3 =  0.5*(phongN +1)*pow(cosl, phongN)*(cosx*area) /
                             ggSquaredDistance(p,p3);

      L = 0.2 * kEmit * (Lc + L0 + L1 + L2 + L3);

      return ggTrue;
}
