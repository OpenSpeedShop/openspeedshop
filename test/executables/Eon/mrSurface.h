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



#ifndef MRSURFACE_H
#define MRSURFACE_H


//  Abstract class mrSurface may be subclassed to a particular type of 
//  surface such as a sphere, or may be subclassed to an object set such
//  as a space subdivision grid.
//
//  Most member functions return ggFalse if the arguments are not filled
//

#include <ggMaterial.h>
#include <ggDiffuseBRDF.h>
#include <ggBox3.h>
#include <ggGeometry.h>

class mrViewingHitRecord {
public:
    double t;            // ray hits at p = ray.origin() + r*ray.direction()
    ggPoint3 p;          // point of intersection
    ggONB3 UVW;          // basis at p.  basis.w() is the surface normal
    ggPoint2 UV;         // 2d (texture) coords at p.
    double coverage;    // coverage parameter-- scalar texture param
    ggSpectrum kEmit; // emitted radiance along r
    ggSpectrum kAdd;  // added radiance
    ggBoolean hasUV;     // has 2d (texture) coords at p?
    ggBoolean hasEmit;   // surface emits light?
    ggBoolean hasAdd;   // light added?
    ggBoolean hasUVW;    // is basis defined at p?

// default constructir sets flags to be false;
    mrViewingHitRecord() : p(ggFalse), UVW(ggFalse), UV(ggFalse)
             { hasUVW = hasUV = hasEmit = hasAdd = ggFalse; coverage = 1.0; }
    mrViewingHitRecord(ggBoolean) {};
};

class mrSurface {
public:
  // returns true if ray hits something
  virtual ggBoolean print(ostream&) const;

  // does a Shadow ray hit the surface?  N, E, do not need to be set
  // if emits is false.
  virtual ggBoolean shadowHit(        
             const ggRay3& r,   // ray being sent
             double time,       // time ray is sent
             double tmin,       // minimum hit parameter to be searched for
             double tmax,       // maximum hit parameter to be searched for
             double& t,         // ray parameter at intersection
             ggVector3& N,      // surface normal on luminaire at hit point
             ggBoolean& emits,  // surface emits light?
             ggSpectrum& E    // emitted radiance along r
             ) const;

  // does a viewing ray hit the surface?
  virtual ggBoolean viewingHit(        
             const ggRay3& r,   // ray being sent
             double time,       // time ray is sent
             double tmin,       // minimum hit parameter to be searched for
             double tmax,       // maximum hit parameter to be searched for
             mrViewingHitRecord& VHR, 
             ggMaterialRecord& MR  // material properties-- see ggMaterial.h
             ) const;

     //  returns true if bounding box is found
     virtual ggBoolean boundingBox(
             double time1, // low end of time range
             double time2, // low end of time range
             ggBox3& box)
             const;

     // returns true if primitive is at least partially inside box
     virtual ggBoolean overlapsBox(
             double time1, // low end of time range
             double time2, // low end of time range
             const ggBox3& box)
             const;



     // Selects a point visible from x given a uv-pair.  Sometimes
     // returning a non-visible point is allowed, but not desirable.
     // Here, visible means not SELF-sahdowed.  This routine only needs
     // to be defined for light emitting surfaces.
     virtual ggBoolean selectVisiblePoint(
             const ggPoint3& x,   // viewpoint
             const ggVector3& Nx, // unit vector at x
             const ggPoint2& uv,  // input coordinate
             const double time,   // time of query
             ggPoint3& onLight,  // point corresponding to uv
             double& invProb)     // one over probability of selecting onLight
             const;
                               // assuming random uv

     // Estimate the approximate value of E times Omega time cosine theta,
     // where Omega is the solid angle of the surface as seen from x,
     // and E is the radiance of a point on the surface.
     // This routine only needs  to be defined for light emitting surfaces.
      virtual ggBoolean approximateDirectRadiance(
             const ggPoint3& x,     // point on object being lighted
             const ggVector3& N,    // normal at x
             const double time,     // time of query
             ggSpectrum& L)       // estimate
             const;

};



#endif
