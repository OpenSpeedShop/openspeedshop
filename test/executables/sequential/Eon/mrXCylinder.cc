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



#include <mrXCylinder.h>
#include <ggGeometry.h>
#include <ggMacros.h>
#include <math.h>
#include <kai.h>

ggBoolean  mrXCylinder::print(ostream& s ) const {
    s << "mrXCylinder " << PTR_TO_INTEGRAL(this) << " xMin = " << xMin << 
          ", xMax = " << xMax << 
          ", radius = " << r <<
          ", yCenter = " << yCenter <<
          ", zCenter = " << zCenter << "\n";
    return ggTrue;
}

ggBoolean mrXCylinder::shadowHit(        
             const ggRay3& ray,
             double ,
             double tmin,
             double tmax,
             double& t,
             ggVector3& N ,
             ggBoolean& emits,
             ggSpectrum& ) const {

   ggPoint3 p;

// this is not an emitter, so just see if there is a hit or not
   if (ggRayXCylinderIntersect(ray, xMin, xMax, yCenter,
                    zCenter, r, tmin, tmax, p, t)) {
       
       double rInverse = 1.0 / r;
       double ny = (p.y()-yCenter)*rInverse;
       double nz = (p.z()-zCenter)*rInverse;
       N = ggVector3(0, ny, nz);

       emits = ggFalse;
       return ggTrue;
   }
   else
      return ggFalse; 
}

ggBoolean mrXCylinder::viewingHit(        
             const ggRay3& ray,
             double,
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord& ) const
{
   double cosfi, fi,  ny, nz;

   if (ggRayXCylinderIntersect(ray, xMin, xMax, yCenter, 
                    zCenter, r, tmin, tmax, VHR.p, VHR.t))
   {
       VHR.hasUVW = ggTrue;
       VHR.hasUV = ggTrue;
// use default BRDF
       double rInverse = 1.0 / r;
       ny = (VHR.p.y()-yCenter)*rInverse;
       nz = (VHR.p.z()-zCenter)*rInverse;
       cosfi = - ny;
       fi = acos(cosfi);
       if(VHR.p.z() > zCenter) fi = ggTwoPi - fi;
       VHR.UV.Set((0.5/ ggPi) * fi, (VHR.p.x()-xMin)/(xMax-xMin));
// are these right-handed? I think,
         VHR.UVW = ggONB3( ggVector3 (-1, 0, 0),
                        ggVector3 (0, - nz, ny),
                        ggVector3 (0, ny, nz));
       return ggTrue;
   }
   else
      return ggFalse;
}


ggBoolean mrXCylinder::boundingBox(
             double, // low end of time range
             double, // high end of time range
             ggBox3& box) const
{ 
   box.min().Set(xMin, yCenter-r, zCenter-r);
   box.max().Set(xMax, yCenter+r, zCenter+r);

   return ggTrue;
}

