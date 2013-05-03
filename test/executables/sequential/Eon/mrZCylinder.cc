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



#include <mrZCylinder.h>
#include <ggGeometry.h>
#include <ggMacros.h>
#include <math.h>
#include <kai.h>

ggBoolean  mrZCylinder::print(ostream& s ) const {
    s << "mrZCylinder " << PTR_TO_INTEGRAL(this) << " zMin = " << zMin << 
          ", zMax = " << zMax << 
          ", radius = " << r <<
          ", xCenter = " << xCenter <<
          ", yCenter = " << yCenter << "\n";
    return ggTrue;
}

ggBoolean mrZCylinder::shadowHit(        
             const ggRay3& ray,
             double ,
             double tmin,
             double tmax,
             double& t,
             ggVector3&  N,
             ggBoolean& emits,
             ggSpectrum& ) const {

   ggPoint3 p;

// this is not an emitter, so just see if there is a hit or not
   if (ggRayZCylinderIntersect(ray, xCenter, yCenter, zMin, zMax, r,
                    tmin, tmax, p, t)) {
       emits = ggFalse;
       double nx = (p.x()-xCenter)/r;
       double ny = (p.y()-yCenter)/r;
       N = ggVector3 (nx, ny, 0);
       return ggTrue;
   }
   else
      return ggFalse; 
}

ggBoolean mrZCylinder::viewingHit(        
             const ggRay3& ray,
             double,
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord&
             ) const
{
   double nx, ny;
   if (ggRayZCylinderIntersect(ray, xCenter, yCenter, zMin, zMax,
                    r, tmin, tmax, VHR.p, VHR.t)) {
       VHR.hasUVW = ggTrue;
       VHR.hasUV = ggTrue;
       ny = (VHR.p.y()-yCenter)/r;
       nx = (VHR.p.x()-xCenter)/r;
       double cosfi = - ny;
       double fi = acos(cosfi);
       if(VHR.p.x() > xCenter) fi = 2 * ggPi - fi;

       VHR.UV.Set((0.5 / ggPi) * fi, (VHR.p.z()-zMin)/(zMax-zMin));

       VHR.UVW.InitFromW(ggVector3 (nx, ny, 0));
       return ggTrue;
   }
   else
       return ggFalse;
}


ggBoolean mrZCylinder::boundingBox(
             double, // low end of time range
             double, // high end of time range
             ggBox3& box) const
{ 
   box.min().Set(xCenter-r, yCenter-r, zMin);
   box.max().Set(xCenter+r, yCenter+r, zMax);

   return ggTrue;
}

