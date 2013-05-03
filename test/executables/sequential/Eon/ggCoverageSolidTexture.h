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



#ifndef GGCOVERAGESOLIDTEXTURE_H
#define GGCOVERAGESOLIDTEXTURE_H


#include <ggSolidTexture.h>

class ggCoverageSolidTexture : public ggSolidTexture {
public:
    virtual ggBoolean getTexture(const ggPoint3&,
                              ggSpectrum&,
                              double&) const;

    ggCoverageSolidTexture(
                             double bl, 
                             double a, int t)  {

              baseline = bl;
              scale = 1/a;
              terms = t;
    }

protected:
    ggCoverageSolidTexture() {};
    ggSolidNoise3 noise;
    double baseline;
    double scale;
    int terms;
};

#endif

