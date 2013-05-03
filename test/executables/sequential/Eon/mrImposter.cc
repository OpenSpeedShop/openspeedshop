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
#include <mrImposter.h>


ggBoolean mrImposter::print(ostream& s) const {
    s << "mrImposter\n ";
    return ggTrue;
}

mrImposter::mrImposter(mrSurface *g, mrSurface *i) {
    geometryPtr = g;
    imposterPtr = i;
    assert ( imposterPtr != 0) ;
    assert ( geometryPtr != 0) ;
    ggBox3 b1, b2;
    assert(imposterPtr->boundingBox(0, 0, b1));
    assert(geometryPtr->boundingBox(0, 0, b2));
    ggPoint3 min, max;
    min.x() = (b1.min().x() < b2.min().x())? b1.min().x() : b2.min().x();
    min.y() = (b1.min().y() < b2.min().y())? b1.min().y() : b2.min().y();
    min.z() = (b1.min().z() < b2.min().z())? b1.min().z() : b2.min().z();
    max.x() = (b1.max().x() > b2.max().x())? b1.max().x() : b2.max().x();
    max.y() = (b1.max().y() > b2.max().y())? b1.max().y() : b2.max().y();
    max.z() = (b1.max().z() > b2.max().z())? b1.max().z() : b2.max().z();
    box.Set(min, max);
}


ggBoolean mrImposter::shadowHit(
             const ggRay3& r,    // ray being sent
             double time,     // time ray is sent
             double tmin,
             double tmax,
             double& t,
             ggVector3& N,
             ggBoolean& emits,
             ggSpectrum& E
             ) const {

     return imposterPtr->shadowHit(r, time, tmin, tmax, t, N, emits, E);
}

ggBoolean mrImposter::viewingHit(        
             const ggRay3& r,    // ray being sent
             double time,     // time ray is sent
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord& MR
             ) const {

     return geometryPtr->viewingHit(r, time, tmin, tmax, VHR, MR);
}


//  returns true if bounding box is found
ggBoolean mrImposter::boundingBox(
             double , // low end of time range
             double , // low end of time range
             ggBox3& bbox)   // highest xyz values
             const
{
    bbox = box;
    return ggTrue;

}

ggBoolean mrImposter::selectVisiblePoint(
             const ggPoint3& x,  // viewpoint
             const ggVector3& Nx,  // normal at x
             const ggPoint2& uv,  // input coordinate for warping
             const double time,
             ggPoint3& onLight,  // point corresponding to uv
             double& prob ) const  // probability if uv is random in [0,1]^2
{
     return imposterPtr->selectVisiblePoint(x, Nx, uv, time, onLight, prob);
}

