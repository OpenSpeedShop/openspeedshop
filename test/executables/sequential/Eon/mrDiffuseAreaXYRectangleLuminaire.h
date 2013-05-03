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



#ifndef MRDIFFUSEAREAXYRECTANGLELUMINAIRE_H
#define MRDIFFUSEAREAXYRECTANGLELUMINAIRE_H


#include <mrXYRectangle.h>

// an axis-aligned rectangle parallel to the XY plane

class mrDiffuseAreaXYRectangleLuminaire : public mrXYRectangle {
public:

  virtual ggBoolean shadowHit(        
             const ggRay3& r,    // ray being sent
             double time,     // time ray is sent
             double tmin, // minimum hit parameter to be searched for
             double tmax, // maximum hit parameter to be searched for
             double& t,
             ggVector3& N,  
             ggBoolean& emits,
             ggSpectrum& kEmit
             ) const;

  virtual ggBoolean viewingHit(        
             const ggRay3& r,    // ray being sent
             double time,     // time ray is sent
             double tmin, // minimum hit parameter to be searched for
             double tmax, // maximum hit parameter to be searched for
             mrViewingHitRecord&,
             ggMaterialRecord& rec
             ) const;

  virtual ggBoolean selectVisiblePoint(
             const ggPoint3& x,   // viewpoint
             const ggVector3& N, // normal at x
             const ggPoint2& uv, // input coordinate
             const double time,          // time of query
             ggPoint3& on_light, // point corresponding to uv
             double& prob)               // probability of selecting on_light
             const;
                                         // assuming random uv

 virtual ggBoolean approximateDirectRadiance(
             const ggPoint3& x,     // point on object being lighted
             const ggVector3& N,    // normal at x
             const double time,  // time of query
             ggSpectrum& L)        // estimate
             const;
  


mrDiffuseAreaXYRectangleLuminaire(double xmin, 
               double xmax,
               double ymin,
               double ymax,
               double zval,
               const ggSpectrum& emit) : mrXYRectangle(xmin, xmax,
                         ymin, ymax, zval) {
                                              kEmit = emit; }

  protected:
      mrDiffuseAreaXYRectangleLuminaire() {}
      ggSpectrum kEmit;
};


#endif


