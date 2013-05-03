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




//
// ggRasterSurfaceTexture.h-- declarations for class ggRasterSurfaceTexture
//
// Author: Peter Shirley
//         Aug. 30, 1993
//
// Modified: Cho and Shirley
//

#ifndef GGRASTERSURFACETEXTURE_H
#define GGRASTERSURFACETEXTURE_H


#include <ggRaster.h>
#include <ggRGBPixel.h>
#include <ggRGBFPixel.h>

class ggRasterSurfaceTexture {
public:
    ggRGBFPixel pointColor(const ggPoint2&) const;
    ggRGBFPixel bilinearColor(const ggPoint2&) const;
    ggRGBFPixel bicubicColor(const ggPoint2&) const;
    ggRasterSurfaceTexture( istream& s);
    ggRasterSurfaceTexture( ggRaster< ggRGBPixel<unsigned char> > *ptr)
                         { imagePtr = ptr; }
protected:
    ggRasterSurfaceTexture() {}
    ggRaster< ggRGBPixel<unsigned char> > *imagePtr;
};

#endif
