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



#ifndef GGDIFFUSEBRDF_H
#define GGDIFFUSEBRDF_H

#include <math.h>
#include <ggBRDF.h>


class ggDiffuseBRDF : public ggBRDF {
public:
  virtual ggSpectrum averageValue() const;
  virtual ggSpectrum value(const ggVector3& in,
                             const ggVector3& out,
                             const ggONB3&) const;
  ggDiffuseBRDF(const ggSpectrum& r) {R = divPi * r;}
protected:
  ggDiffuseBRDF() {}
  ggSpectrum R;
};

#endif

// R: absolute reflectance of the diffuse material

