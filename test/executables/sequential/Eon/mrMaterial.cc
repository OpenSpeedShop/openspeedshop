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



#include <mrMaterial.h>
#include <kai.h>



ggBoolean mrMaterial::print(ostream& s ) const {
    s << "mrMaterial " << PTR_TO_INTEGRAL(this) << " obj = " << "\n";
    objPtr->print(s);
    return ggTrue;
}


ggBoolean mrMaterial::shadowHit(        
             const ggRay3& ray,
             double time,
             double tmin,
             double tmax,
             double& t,
             ggVector3& N,
             ggBoolean& emits,
             ggSpectrum& E ) const {

   return objPtr->shadowHit(ray, time, tmin, tmax, t, N, emits, E);
}

ggBoolean mrMaterial::viewingHit(        
             const ggRay3& ray,
             double time,
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord& MR
             ) const

{
   if (objPtr->viewingHit(ray, time, tmin, tmax, VHR, MR)) {
             materialPtr->getInfo(ray, VHR.p, VHR.UVW, VHR.coverage, MR); 
            if (MR.hasRay1 && MR.CORLRay1)
                MR.kRay1 *= MR.kBRDF;
            if (MR.hasRay2 && MR.CORLRay2)
               MR.kRay2 *= MR.kBRDF;

             return ggTrue;
   }
   else
      return ggFalse;
}

ggBoolean mrMaterial::boundingBox(
             double t1, // low end of time range
             double t2, // low end of time range
             ggBox3& box) const {
   return (objPtr->boundingBox(t1, t2, box));
}


ggBoolean mrMaterial::selectVisiblePoint(
             const ggPoint3& x,   // viewpoint
             const ggVector3& Nx, // unit vector at x
             const ggPoint2& uv,  // input coordinate
             const double time,   // time of query
             ggPoint3& onLight,  // point corresponding to uv
             double& invProb)     // one over probability of selecting
             const {
      return objPtr->selectVisiblePoint(x, Nx, uv, time, onLight, invProb);
}

ggBoolean mrMaterial::approximateDirectRadiance(
             const ggPoint3& x,     // point on object being lighted
             const ggVector3& N,    // normal at x
             const double time,     // time of query
             ggSpectrum& L)       // estimate
             const {
      return objPtr->approximateDirectRadiance(x, N, time, L);
}
