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



#include <mrSphere.h>
#include <ggGeometry.h>
#include <math.h>

ggBoolean  mrSphere::print(ostream& s ) const {
    s << "mrSphere " << sphere << "\n";
    return ggTrue;
}

ggBoolean mrSphere::shadowHit(        
             const ggRay3& ray,
             double , // time
             double tmin,
             double tmax,
             double& t,
             ggVector3& N,
             ggBoolean& emits,
             ggSpectrum&
             ) const {


   if (ggRaySphereIntersect(ray, sphere, tmin, tmax, t))
   {
       ggPoint3 p = ray.pointAtParameter(t);
       N = p - center();
       emits = ggFalse;
       return ggTrue;
   }
   else
      return ggFalse; 
}


ggBoolean mrSphere::viewingHit(        
             const ggRay3& ray,
             double ,  // time
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord& ) const

{
if (ggRaySphereIntersect(ray, sphere,tmin, tmax, VHR.t))
   {
       VHR.hasUVW = ggTrue;
       VHR.hasUV = ggTrue;
       VHR.p = ray.pointAtParameter(VHR.t);
       VHR.UV = sphere.getUV(VHR.p);
       VHR.UVW.InitFromW(sphere.normal(VHR.p));
       return ggTrue;
   }
   else
      return ggFalse;
}


ggBoolean mrSphere::boundingBox(
             double, // low end of time range
             double, // low end of time range
             ggBox3& box) const
{
  box = sphere.boundingBox();
  return ggTrue; }
