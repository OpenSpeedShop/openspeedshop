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



#ifndef GGPOLISHEDBRDF_H
#define GGPOLISHEDBRDF_H

#include <math.h>
#include <ggBRDF.h>
#include <assert.h>


class ggPolishedBRDF : public ggBRDF {
public:
  virtual ggSpectrum averageValue() const;
  virtual ggSpectrum value(const ggVector3& in,
                             const ggVector3& out,
                             const ggONB3&) const;
  ggPolishedBRDF(const ggSpectrum& r, const ggSpectrum& index)
  {
    #ifdef GGSAFE
        assert(index[0] >= 1.0);
    #endif

    R = divPi * r;
    Nt = index;

  } 

protected:
  ggPolishedBRDF() {}
  ggSpectrum R;
  ggSpectrum Nt;
};

#endif

// Nt: refractive index of polish
// R: absolute reflectance of polish


