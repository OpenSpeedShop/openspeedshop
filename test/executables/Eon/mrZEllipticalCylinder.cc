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



#include <mrZEllipticalCylinder.h>
#include <ggGeometry.h>
#include <ggMacros.h>
#include <math.h>
#include <kai.h>

ggBoolean  mrZEllipticalCylinder::print(ostream& s ) const {
    s << "mrZEllipticalCylinder " << PTR_TO_INTEGRAL(this) << " zMin = " << zMin << 
          ", zMax = " << zMax << 
          ", radiusX = " << rX <<
          ", radiusY = " << rY <<
          ", xCenter = " << xCenter <<
          ", yCenter = " << yCenter << "\n";
    return ggTrue;
}

ggBoolean mrZEllipticalCylinder::shadowHit(        
             const ggRay3& ray,
             double ,
             double tmin,
             double tmax,
             double& t,
             ggVector3& N,
             ggBoolean& emits,
             ggSpectrum& ) const {

   ggPoint3 p;

// this is not an emitter, so just see if there is a hit or not
   if (ggRayZEllipticalCylinderIntersect(ray, xCenter, yCenter,
               zMin, zMax, rX, rY, 
               tmin, tmax, p, t)) {
       emits = ggFalse;
       double n1 = (p.x()-xCenter)/rX;
       double n2 = (p.y()-yCenter)/rY;
       N = ggVector3 (n1, n2, 0);
       return ggTrue;
   }
   else
      return ggFalse; 
}

ggBoolean mrZEllipticalCylinder::viewingHit(        
             const ggRay3& ray,
             double,
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord&
             ) const
{
   double cosfi, fi,  d1, d2, n1, n2;
   ggONB3 uvw;
   if (ggRayZEllipticalCylinderIntersect(ray, xCenter, yCenter, zMin, zMax,
                    rX, rY, tmin, tmax, VHR.p, VHR.t)) {
       VHR.hasUVW = ggTrue;
       VHR.hasUV = ggTrue;
       d1 = VHR.p.x()-xCenter;
       d2 = VHR.p.y()-yCenter;
       cosfi = - (d1/sqrt(d1*d1+d2*d2));
       fi = acos(cosfi);
       if(VHR.p.y() > yCenter) fi = 2 * ggPi - fi;
       VHR.UV.Set((0.5/ggPi) * fi, (VHR.p.z()-zMin)/(zMax-zMin));
       n1 = (VHR.p.x()-xCenter);
       n2 = (VHR.p.y()-yCenter);
         uvw.InitFromW(ggVector3 (n1, n2, 0));
         VHR.UVW = uvw;
       return ggTrue;
   }
   else
      return ggFalse;
}


ggBoolean mrZEllipticalCylinder::boundingBox(
             double, // low end of time range
             double, // high end of time range
             ggBox3& box) const
{ 
   box.min().Set(xCenter-rX, yCenter-rY, zMin);
   box.max().Set(xCenter+rX, yCenter+rY, zMax);

   return ggTrue;
}

