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



#include <mrYEllipticalCylinder.h>
#include <ggGeometry.h>
#include <ggMacros.h>
#include <math.h>
#include <kai.h>

ggBoolean  mrYEllipticalCylinder::print(ostream& s ) const {
    s << "mrYEllipticalCylinder " << PTR_TO_INTEGRAL(this) << " yMin = " << yMin << 
          ", yMax = " << yMax << 
          ", radiusZ = " << rZ <<
          ", radiusX = " << rX <<
          ", zCenter = " <<zCenter <<
          ", xCenter = " << xCenter << "\n";
    return ggTrue;
}

ggBoolean mrYEllipticalCylinder::shadowHit(        
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
   if (ggRayYEllipticalCylinderIntersect(ray,  xCenter,  yMin, yMax,
               zCenter, rX, rZ, tmin, tmax, p, t)) {
       emits = ggFalse;
       double invasq = 1/(rZ*rZ);
       double invbsq = 1/(rX*rX);
       double n1 = (p.z()-zCenter)*invasq;
       double n2 = (p.x()-xCenter)*invbsq;
       N = ggVector3 (n2, 0, n1);
       return ggTrue;
   }
   else
      return ggFalse; 
}

ggBoolean mrYEllipticalCylinder::viewingHit(        
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

   if (ggRayYEllipticalCylinderIntersect(ray, xCenter, yMin, yMax,
                    zCenter, rX, rZ, tmin, tmax, VHR.p, VHR.t)) {
       VHR.hasUVW = ggTrue;
       VHR.hasUV = ggTrue;
       d1 = VHR.p.z()-zCenter;
       d2 = VHR.p.x()-xCenter;
       cosfi = - (d1/sqrt(d1*d1+d2*d2));
       fi = acos(cosfi);
       if(VHR.p.x() > xCenter) fi = 2 * ggPi - fi;
       VHR.UV.Set((0.5 / ggPi) * fi, (VHR.p.y()-yMin)/(yMax-yMin));
       invasq = 1/(rZ*rZ);
       invbsq = 1/(rX*rX);
       n1 = (VHR.p.z()-zCenter)*invasq;
       n2 = (VHR.p.x()-xCenter)*invbsq;
         uvw.InitFromW(ggVector3 (n2, 0, n1));
         VHR.UVW = uvw;
       return ggTrue;
   }
   else
      return ggFalse;
}


ggBoolean mrYEllipticalCylinder::boundingBox(
             double, // low end of time range
             double, // high end of time range
             ggBox3& box) const
{ 
   box.min().Set(xCenter-rX, yMin, zCenter-rZ);
   box.max().Set(xCenter+rX, yMax, zCenter+rZ);

   return ggTrue;
}

