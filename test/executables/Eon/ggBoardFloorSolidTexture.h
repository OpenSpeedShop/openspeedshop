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



#ifndef GGBOARDFLOORSOLIDTEXTURE_H
#define GGBOARDFLOORSOLIDTEXTURE_H


#include <ggSolidTexture.h>

class ggBoardFloorSolidTexture : public ggSolidTexture {
public:
    virtual ggBoolean getTexture(const ggPoint3&,
                              ggSpectrum&,
                              double&) const;

    ggBoardFloorSolidTexture(ggSolidTexture *tex,
                             double bl, 
                             double bw, 
                             double bd,
                             int a)  {

              tPtr = tex;
              boardLength0 = bl;
              boardLength1 = bl * 1.2923 ;
              boardLength2 = bl * 0.7253;
              boardWidth = bw;
              boardDepth = bd;
              axis = a;
    }

protected:
    ggBoardFloorSolidTexture() {};
    ggSolidNoise3 noise;
    ggSolidTexture *tPtr;
    double boardLength0;
    double boardLength1;
    double boardLength2;
    double boardWidth;
    double boardDepth;
    int axis;
};

#endif

