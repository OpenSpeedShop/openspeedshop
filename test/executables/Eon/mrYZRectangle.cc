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



#include <mrYZRectangle.h>
#include <ggGeometry.h>
#include <ggMacros.h>
#include <math.h>

ggBoolean mrYZRectangle::shadowHit(        
             const ggRay3& ray,
             double ,
             double tmin,
             double tmax,
             double& t,
             ggVector3& N,
             ggBoolean& emits,
             ggSpectrum& ) const {

   double y, z;

   if (ggRayYZRectangleIntersect(ray, x, yMin, yMax, zMin, zMax,
                                 tmin, tmax, y, z, t))
   {
       emits = ggFalse;
       N = ggXAxis;
       return ggTrue;
   }
   else
      return ggFalse;
}


ggBoolean mrYZRectangle::viewingHit(        
             const ggRay3& ray,
             double,
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord& ) const
{
   double y, z;

   if (ggRayYZRectangleIntersect(ray, x, yMin, yMax, zMin, zMax,
                                 tmin, tmax, y, z, VHR.t))
   {
       VHR.hasUVW = ggTrue;
       VHR.hasUV = ggTrue;
       VHR.p.Set(x, y, z);
       VHR.UV.Set((y-yMin)/(yMax-yMin), (z-zMin)/(zMax-zMin));

          VHR.UVW = ggONB3( ggVector3 (0, 0, -1),
                       ggVector3 (0, 1, 0),
                       ggVector3 (1, 0, 0));
       return ggTrue;
   }
   else
      return ggFalse;
}


ggBoolean mrYZRectangle::boundingBox(
             double, // low end of time range
             double, // low end of time range
             ggBox3& box) const
{ 
   box.min().Set(x, yMin, zMin);
   box.max().Set(x, yMax, zMax);

   return ggTrue;
}

