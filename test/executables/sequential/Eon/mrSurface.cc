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



#include <mrSurface.h>
#include <ggDiffuseBRDF.h>


ggBoolean mrSurface::print(ostream& ) const
{ return ggFalse; }


ggBoolean mrSurface::shadowHit(        
             const ggRay3&,
             double,
             double,
             double,
             double&,
             ggVector3&,
             ggBoolean&,
             ggSpectrum&) const
{ return ggFalse; }


ggBoolean mrSurface::viewingHit(        
             const ggRay3&,
             double,
             double,
             double,
             mrViewingHitRecord&,
             ggMaterialRecord&) const
{ return ggFalse; }

ggBoolean mrSurface::overlapsBox(
             double time1, // low end of time range
             double time2, // low end of time range
             const ggBox3& box)
             const {
    ggBox3 myBox;
    return (this->boundingBox(time1, time2, myBox) &&
            ggOverlapBox3(myBox, box));
}

ggBoolean mrSurface::boundingBox(
             double, // low end of time range
             double, // low end of time range
             ggBox3&) const
{ return ggFalse; }


ggBoolean mrSurface::selectVisiblePoint(
             const ggPoint3&,
             const ggVector3&,
             const ggPoint2&,
             const double,
             ggPoint3&,
             double& ) const
{ return ggFalse; }


ggBoolean mrSurface::approximateDirectRadiance(
             const ggPoint3&,     // point on object being lighted
             const ggVector3&,    // normal at x
             const double,  // time of query
             ggSpectrum& L)        // estimate
             const
{ L.Set(1.0); return ggTrue; }


