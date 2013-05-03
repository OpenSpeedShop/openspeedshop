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



#include <assert.h>
#include <ggMacros.h>
#include <mrEmitter.h>


ggBoolean mrEmitter::print(ostream& s) const {
    s << "mrEmitter\n ";
    return ggTrue;
}


ggBoolean mrEmitter::shadowHit(
             const ggRay3& r,    // ray being sent
             double time,     // time ray is sent
             double tmin,
             double tmax,
             double& t,
             ggVector3& N,
             ggBoolean& emits,
             ggSpectrum& e
             ) const {

     if (geometryPtr->shadowHit(r, time, tmin, tmax, t, N, emits, e)) {
             emits = ggTrue;
             e = E;
             return ggTrue;
     }
         else return ggFalse;
}

ggBoolean mrEmitter::viewingHit(        
             const ggRay3& r,    // ray being sent
             double time,     // time ray is sent
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord& MR
             ) const {

     if(geometryPtr->viewingHit(r, time, tmin, tmax, VHR, MR)) {
             VHR.hasEmit = ggTrue;
             VHR.kEmit = E;
             return ggTrue;
     }
             return ggFalse;
}


//  returns true if bounding box is found
ggBoolean mrEmitter::boundingBox(
             double t1 , // low end of time range
             double t2 , // low end of time range
             ggBox3& bbox)   // highest xyz values
             const
{
    return geometryPtr->boundingBox(t1, t2, bbox);

}

ggBoolean mrEmitter::selectVisiblePoint(
             const ggPoint3& x,  // viewpoint
             const ggVector3& Nx,  // normal at x
             const ggPoint2& uv,  // input coordinate for warping
             const double time,
             ggPoint3& onLight,  // point corresponding to uv
             double& prob ) const  // probability if uv is random in [0,1]^2
{
     return geometryPtr->selectVisiblePoint(x, Nx, uv, time, onLight, prob);
}

