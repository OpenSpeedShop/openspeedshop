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



#include <mrXYRectangle.h>
#include <ggGeometry.h>
#include <ggMacros.h>
#include <math.h>

ggBoolean mrXYRectangle::shadowHit(        
             const ggRay3& ray,
             double ,
             double tmin,
             double tmax,
             double& t,
             ggVector3& N,
             ggBoolean& emits,
             ggSpectrum& ) const {

   double x, y;

// this is not an emitter, so just see if there is a hit or not
   if (ggRayXYRectangleIntersect(ray, xMin, xMax, yMin, yMax, z,
                                 tmin, tmax, x, y, t)) {
       N = ggZAxis;
       emits = ggFalse;
       return ggTrue;
   }
   else
      return ggFalse; 
}

ggBoolean mrXYRectangle::viewingHit(        
             const ggRay3& ray,
             double,
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord& 
             ) const
{
   double x, y;

   if (ggRayXYRectangleIntersect(ray, xMin, xMax, yMin, yMax, z,  
                                 tmin, tmax,  x, y, VHR.t))
   {
       VHR.hasUVW = ggTrue;
       VHR.hasUV = ggTrue;
       VHR.p.Set(x, y, z);
       VHR.UV.Set((x-xMin)/(xMax-xMin), (y-yMin)/(yMax-yMin));
       VHR.UVW = ggONB3( ggVector3 (1, 0, 0),
                        ggVector3 (0, 1, 0),
                        ggVector3 (0, 0, 1));
       return ggTrue;
   }
   else
      return ggFalse;
}


ggBoolean mrXYRectangle::boundingBox(
             double, // low end of time range
             double, // low end of time range
             ggBox3& box) const
{ 
   box.min().Set(xMin, yMin, z);
   box.max().Set(xMax, yMax, z);

   return ggTrue;
}

