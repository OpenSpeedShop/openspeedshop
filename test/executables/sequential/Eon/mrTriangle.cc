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



#include <mrTriangle.h>
#include <ggMacros.h>
#include <ggGeometry.h>
#include <math.h>


ggBoolean mrTriangle::print(ostream& s) const {
    s << "mrTriangle" << "\n";
    s << triangle << "\n";
    return ggTrue;
}



ggBoolean mrTriangle::shadowHit(        
             const ggRay3& ray,
             double ,
             double tmin,
             double tmax,
             double& t,
             ggVector3& N,
             ggBoolean& emits,
             ggSpectrum& 
             ) const {

   double beta, gamma;

   if (ggRayTriangleIntersect(ray, triangle, tmin, tmax, beta, gamma, t))
   {
       N = triangle.normal();
       emits = ggFalse;
       return ggTrue;
   }
   else
      return ggFalse; 
}

ggBoolean mrTriangle::viewingHit(        
             const ggRay3& ray,
             double,
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord& ) const
{

   double beta, gamma;

   if (ggRayTriangleIntersect(ray, triangle, tmin, tmax, beta, gamma, VHR.t))
   {
       VHR.hasUVW = ggTrue;
       VHR.hasUV = ggTrue;
       VHR.UV = ggPoint2(beta, gamma);
       VHR.p = ray.pointAtParameter(VHR.t);
       VHR.UVW.InitFromW(triangle.normal());

       return ggTrue;
   }
   else
      return ggFalse;
}


ggBoolean mrTriangle::boundingBox(
             double, // low end of time range
             double, // low end of time range
             ggBox3& box) const
{ 
   box = triangle.boundingBox();
   return ggTrue;
}


ggBoolean mrTriangle::overlapsBox(
             double , // low end of time range
             double , // low end of time range
             const ggBox3& box)
             const {
    return ggTriangleBoxOverlap(triangle, box);
}

