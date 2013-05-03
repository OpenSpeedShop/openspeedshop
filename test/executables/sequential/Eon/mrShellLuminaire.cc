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



#include <mrShellLuminaire.h>
#include <ggMacros.h>
#include <ggGeometry.h>
#include <math.h>

ggBoolean mrShellLuminaire::print(ostream& s) const {
    s << "mrShellLuminaire" << "\n";
    return ggTrue;
}


mrShellLuminaire::mrShellLuminaire(double r,
                        const ggSpectrum& emit) {

     kEmit = emit;
     radius = r;
}


ggBoolean mrShellLuminaire::shadowHit(        
             const ggRay3& ray,
             double ,
             double tmin,
             double tmax,
             double& t,
             ggVector3& N,
             ggBoolean& emits,
             ggSpectrum& E
             ) const {

  if (ggRaySphereIntersect(ray, ggOrigin, radius, tmin, tmax, t))
   {
       emits = ggTrue;
       E = kEmit;
       ggPoint3 p = ray.pointAtParameter(t);

       N = ggUnitVector(ggOrigin - p);

       return ggTrue;
   }
   else
      return ggFalse; 
}

ggBoolean mrShellLuminaire::viewingHit(        
             const ggRay3& ray,
             double,
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord& MR) const
{

  if (ggRaySphereIntersect(ray, ggOrigin, radius, tmin, tmax, VHR.t))
   {
       VHR.hasEmit = ggTrue;
       VHR.kEmit = kEmit;

       VHR.p = ray.pointAtParameter(VHR.t);

       VHR.hasUVW = ggTrue;
       VHR.UVW.InitFromW(ggOrigin - VHR.p);
       MR.BRDFPointer = 0;

       return ggTrue;
   }
   else
      return ggFalse;
}



// Selects a point visible from x given a uv-pair.  Sometimes
// returning a non-visible point is allowed, but not desirable.
// Here, visible means not SELF-shadowed.
ggBoolean mrShellLuminaire::selectVisiblePoint(
             const ggPoint3&,  // viewpoint
             const ggVector3&,  // normal at x
             const ggPoint2& uv,  // input coordinate for warping
             const double, // time (ignored)
             ggPoint3& onLight,  // point corresponding to uv
             double& invProb ) const  //  one over probability
                                      // if uv is random in [0,1]^2
{
   double theta = acos(1 - 1.999999*uv.u());
   double phi = 2*ggPi*uv.v();

   onLight = ggPoint3(radius*cos(phi)*sin(theta),
                      radius*sin(phi)*sin(theta),
                      radius*cos(theta));


   invProb = 4*ggPi *radius * radius;
   return ggTrue;
}



ggBoolean mrShellLuminaire::approximateDirectRadiance(
             const ggPoint3& ,     // point on object being lighted
             const ggVector3&,    // normal at x
             const double ,  // time of query
             ggSpectrum& L)        // estimate
             const
{

      L = kEmit;
      return ggTrue;
}


ggBoolean mrShellLuminaire::boundingBox(
             double, // low end of time range
             double, // low end of time range
             ggBox3& box) const
{

   box.SetMin(ggOrigin + ggVector3(-radius, -radius, -radius));
   box.SetMax(ggOrigin + ggVector3(radius, radius, radius));
   return ggTrue; 
}

