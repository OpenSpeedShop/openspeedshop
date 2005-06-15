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



#include <mrXZDisk.h>
#include <ggGeometry.h>
#include <ggMacros.h>
#include <math.h>
#include <kai.h>

ggBoolean mrXZDisk::print(ostream& s) const {
    s << "mrXZDisk" << PTR_TO_INTEGRAL(this) << " center = " << center << 
          ", rad = " << radius <<  "\n";
    return ggTrue;
}

ggBoolean mrXZDisk::shadowHit(        
             const ggRay3& ray,
             double ,
             double tmin,
             double tmax,
             double& t,
             ggVector3& N,
             ggBoolean& emits,
             ggSpectrum& ) const {

    double x, z;
// this is not an emitter, so just see if there is a hit or not
   if (ggRayXZDiskIntersect(ray, center, radius, tmin, tmax, x, z, t)) {
       emits = ggFalse;
       N = ggYAxis;
       return ggTrue;
   }
   else
      return ggFalse; 
}

ggBoolean mrXZDisk::viewingHit(        
             const ggRay3& ray,
             double,
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord&
             ) const
{
   double x, z;
   if (ggRayXZDiskIntersect(ray, center, radius, tmin, tmax, x, z, VHR.t))
   {
       double theta, r;

       VHR.hasUVW = ggTrue;
       VHR.hasUV = ggTrue;

       VHR.p.Set(x, center.y(), z);
//Compute U & V from the hit point on a disk.
       theta = atan( (VHR.p.z() - center.z()) / (VHR.p.x() - center.x()));
       if (ggAbs( cos( theta ) )  > ggEpsilon )
	   r = (VHR.p.x() - center.x()) / cos( theta );
       else
	   r = (VHR.p.z() - center.z()) / sin( theta );
       VHR.UV.Set( r * r / (radius * radius), theta / ggTwoPi );
          VHR.UVW = ggONB3( ggVector3 (1, 0, 0),
			    ggVector3 (0, 0, -1),
			    ggVector3 (0, 1, 0));
       return ggTrue;
   }
   else
      return ggFalse;
}


ggBoolean mrXZDisk::boundingBox(
             double, // low end of time range
             double, // low end of time range
             ggBox3& box) const
{ 
   box.min().Set(center.x() - radius, center.y(), center.z() - radius);
   box.max().Set(center.x() + radius, center.y(), center.z() + radius);

   return ggTrue;
}

