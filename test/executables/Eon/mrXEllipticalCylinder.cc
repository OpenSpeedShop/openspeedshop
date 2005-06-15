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



#include <mrXEllipticalCylinder.h>
#include <ggGeometry.h>
#include <ggMacros.h>
#include <math.h>
#include <kai.h>

ggBoolean  mrXEllipticalCylinder::print(ostream& s ) const {
    s << "mrXEllipticalCylinder " << PTR_TO_INTEGRAL(this) << " xMin = " << xMin << 
          ", xMax = " << xMax << 
          ", radiusY = " << rY <<
          ", radiusZ = " << rZ <<
          ", yCenter = " << yCenter <<
          ", zCenter = " << zCenter << "\n";
    return ggTrue;
}

ggBoolean mrXEllipticalCylinder::shadowHit(        
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
   if (ggRayXEllipticalCylinderIntersect(ray, xMin, xMax, 
               yCenter, zCenter, rY, rZ, tmin, tmax, p, t)) {
       emits = ggFalse;
       double invasq = 1/(rY*rY);
       double invbsq = 1/(rZ*rZ);
       double n1 = (p.y()-yCenter)*invasq;
       double n2 = (p.z()-zCenter)*invbsq;
       N = ggVector3 (0, n1, n2);
       return ggTrue;
   }
   else
      return ggFalse; 
}

ggBoolean mrXEllipticalCylinder::viewingHit(        
             const ggRay3& ray,
             double,
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord&
             ) const
{
   double cosfi, fi,  d1, d2, invasq, invbsq, n1, n2;
   ggONB3 uvw;
   if (ggRayXEllipticalCylinderIntersect(ray, xMin, xMax, yCenter, 
                    zCenter, rY, rZ, tmin, tmax, VHR.p, VHR.t))
   {
       VHR.hasUVW = ggTrue;
       VHR.hasUV = ggTrue;
// use default BRDF
       d1 = VHR.p.y()-yCenter;
       d2 = VHR.p.z()-zCenter;
       cosfi = - (d1/sqrt(d1*d1+d2*d2));
       fi = acos(cosfi);
       if(VHR.p.z() > zCenter) fi = 2 * ggPi - fi;
       VHR.UV.Set((0.5/ggPi) * fi, (VHR.p.x()-xMin)/(xMax-xMin));
       invasq = 1/(rY*rY);
       invbsq = 1/(rZ*rZ);
       n1 = (VHR.p.y()-yCenter)*invasq;
       n2 = (VHR.p.z()-zCenter)*invbsq;
       uvw.InitFromW(ggVector3 (0, n1, n2));
       VHR.UVW = uvw;
       return ggTrue;
   }
   else
      return ggFalse;
}


ggBoolean mrXEllipticalCylinder::boundingBox(
             double, // low end of time range
             double, // high end of time range
             ggBox3& box) const
{ 
   box.min().Set(xMin, yCenter-rY, zCenter-rZ);
   box.max().Set(xMax, yCenter+rY, zCenter+rZ);

   return ggTrue;
}

