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



#include <mrXYDisk.h>
#include <ggGeometry.h>
#include <ggMacros.h>
#include <math.h>
#include <kai.h>


ggBoolean mrXYDisk::print(ostream& s) const {
    s << "mrXYDisk" << PTR_TO_INTEGRAL(this) << " center = " << center << 
          ", rad = " << radius <<  "\n";
    return ggTrue;
}

ggBoolean mrXYDisk::shadowHit(        
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
   if (ggRayXYDiskIntersect(ray, center, radius, tmin, tmax, x, y, t)) {
       emits = ggFalse;
       N = ggZAxis ;
       return ggTrue;
   }
   else
      return ggFalse; 
}

ggBoolean mrXYDisk::viewingHit(        
             const ggRay3& ray,
             double,
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord& ) const
{
   double x, y;
   if (ggRayXYDiskIntersect(ray, center, radius, tmin, tmax, x, y, VHR.t))
   {
       double theta, r;
       VHR.p.Set(x, y, center.z());

       VHR.hasUVW = ggTrue;
       VHR.hasUV = ggTrue;
//Compute U & V from the hit point on a disk.
       theta = atan( (VHR.p.y() - center.y()) / (VHR.p.x() - center.x()));
       if (ggAbs( cos( theta ) )  > ggEpsilon )
	   r = (VHR.p.x() - center.x()) / cos( theta );
       else
	   r = (VHR.p.y() - center.y()) / sin( theta );
       VHR.UV.Set( r * r / (radius * radius), theta / ggTwoPi );
          VHR.UVW = ggONB3( ggVector3 (1, 0, 0),
			    ggVector3 (0, 1, 0),
			    ggVector3 (0, 0, 1));
       return ggTrue;
   }
   else
      return ggFalse;
}


ggBoolean mrXYDisk::boundingBox(
             double, // low end of time range
             double, // low end of time range
             ggBox3& box) const
{ 
   box.min().Set(center.x() - radius, center.y() - radius, center.z());
   box.max().Set(center.x() + radius, center.y() + radius, center.z());

   return ggTrue;
}

