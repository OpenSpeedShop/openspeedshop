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



#include <mrDiffuseAreaZCylinderLuminaire.h>
#include <ggGeometry.h>
#include <ggMacros.h>
#include <math.h>


ggBoolean mrDiffuseAreaZCylinderLuminaire::shadowHit(        
             const ggRay3& ray,
             double time,
             double tmin,
             double tmax,
             double& t,
             ggVector3&  N,
             ggBoolean& emits,
             ggSpectrum& E ) const {


// this is not an emitter, so just see if there is a hit or not
   if (mrZCylinder::shadowHit(ray, time, tmin, tmax, t, N, emits, E)) {
       emits = ggTrue;
       E = kEmit;
       return ggTrue;
   }
   else
      return ggFalse; 
}

ggBoolean mrDiffuseAreaZCylinderLuminaire::viewingHit(        
             const ggRay3& ray,
             double time,
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord& MR
             ) const
{

   if (mrZCylinder::viewingHit(ray, time, tmin, tmax, VHR, MR)) {
       VHR.hasEmit = ggTrue;
       VHR.kEmit = kEmit;
       return ggTrue;
   }
   else
      return ggFalse;
}

ggBoolean mrDiffuseAreaZCylinderLuminaire::selectVisiblePoint(
             const ggPoint3&,  // viewpoint
             const ggVector3&,  // normal at x
             const ggPoint2& uv,  // input coordinate for warping
             const double, // time (ignored)
             ggPoint3& onLight,  // point corresponding to uv
             double& invProb ) const  //  one over probability
                                      // if uv is random in [0,1]^2
{

   double z = uv.u() * h;
   double theta = ggTwoPi * uv.v();
   double x = r*cos(theta);
   double y = r*sin(theta);
   onLight.Set(x, y, z);

   invProb = area;
   return ggTrue;
}


ggBoolean mrDiffuseAreaZCylinderLuminaire::approximateDirectRadiance(
             const ggPoint3& ,     // point on object being lighted
             const ggVector3& ,    // normal at x
             const double ,  // time of query
             ggSpectrum& L)        // estimate
             const
{

// needs to be changed!

   L = ggSpectrum(1.0);
   return ggTrue;
}



