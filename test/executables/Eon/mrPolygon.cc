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



#include <mrPolygon.h>
#include <ggMacros.h>
#include <ggGeometry.h>
#include <math.h>

mrPolygon::mrPolygon(const ggPolygon& poly) {
   polygon = poly;
   box.min() = polygon.vertex(0);
   box.max() = polygon.vertex(0);
   int n = polygon.nVertices();
   for (int i = 1; i < n; i++) {
       ggPoint3 p = polygon.vertex(i);
       if (p.x() < box.min().x()) box.min().x() = p.x(); 
       if (p.y() < box.min().y()) box.min().y() = p.y(); 
       if (p.z() < box.min().z()) box.min().z() = p.z(); 
       if (p.x() > box.max().x()) box.max().x() = p.x(); 
       if (p.y() > box.max().y()) box.max().y() = p.y(); 
       if (p.z() > box.max().z()) box.max().z() = p.z(); 
   }
}

ggBoolean mrPolygon::print(ostream& s) const {
    s << "mrPolygon" << "\n";
    return ggTrue;
}


ggBoolean mrPolygon::shadowHit(        
             const ggRay3& ray,
             double ,
             double tmin,
             double tmax,
             double& t,
             ggVector3& N,
             ggBoolean& emits,
             ggSpectrum& 
             ) const {

   if (ggRayPolygonIntersect(ray, polygon,t, tmin, tmax)) {
       N = polygon.normal();
       emits = ggFalse;
       return ggTrue;
   }
   else
      return ggFalse; 
}

ggBoolean mrPolygon::viewingHit(        
             const ggRay3& ray,
             double,
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord& ) const
{


   if (ggRayPolygonIntersect(ray, polygon, VHR.t, tmin, tmax)) {
       VHR.hasUVW = ggTrue;
       VHR.hasUV = ggTrue;
       VHR.p = ray.pointAtParameter(VHR.t);
       VHR.UV = polygon.getUV(VHR.p);
       VHR.UVW.InitFromW(polygon.normal());

       return ggTrue;
   }
   else
      return ggFalse;
}


ggBoolean mrPolygon::boundingBox(
             double, // low end of time range
             double, // low end of time range
             ggBox3& bbox) const
{ 
   bbox = box; 
   return ggTrue;
}

