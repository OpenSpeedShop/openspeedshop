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



#ifndef GGBRDF_H
#define GGBRDF_H

#include <ggSpectrum.h>
#include <ggONB3.h>


double const divPi = 1 / ggPi;

// abstract class ggBRDF (BiDirectional Reflectance Function)


class ggBRDF {
public:
  ggBRDF() {}
  virtual ggSpectrum averageValue() const;
  // returns the average value of BRDF to compute the ambient ligth

  virtual ggSpectrum value(const ggVector3& in,
                             const ggVector3& out,
                             const ggONB3&) const;
  // returns the BRDF value

};

#endif
