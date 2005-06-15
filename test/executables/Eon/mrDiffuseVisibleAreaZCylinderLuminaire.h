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



#ifndef MRDIFFUSEVISIBLEAREAZCYLINDERLUMINAIRE_H
#define MRDIFFUSEVISIBLEAREAZCYLINDERLUMINAIRE_H


// an axis-aligned cylinder parallel to the Z axis
#include <mrDiffuseAreaZCylinderLuminaire.h>

class mrDiffuseVisibleAreaZCylinderLuminaire :
                  public mrDiffuseAreaZCylinderLuminaire {
public:

  virtual ggBoolean selectVisiblePoint(
             const ggPoint3& x,   // viewpoint
             const ggVector3& N, // normal at x
             const ggPoint2& uv, // input coordinate
             const double time,          // time of query
             ggPoint3& on_light, // point corresponding to uv
             double& prob)       // probability of selecting on_light
             const;

// cylinder of radius r on z axis in z range [0,h]
 mrDiffuseVisibleAreaZCylinderLuminaire(double radius, double height,
                                  const ggSpectrum& s) :
             mrDiffuseAreaZCylinderLuminaire(radius, height, s) {}


  protected:
     mrDiffuseVisibleAreaZCylinderLuminaire() { }

};


#endif
