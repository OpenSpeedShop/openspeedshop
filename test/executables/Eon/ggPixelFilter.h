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



#ifndef GGPIXELFILTER_H
#define GGPIXELFILTER_H

#include <ggPoint2.h>

// warp should take numbers in the range [0,1]^2 and return
// numbers still centered at (0.5, 0.5)

class ggPixelFilter {
public:
    ggPixelFilter() {}
    virtual ggPoint2 warp(const ggPoint2& p) const;
    virtual ggPoint2 warpScreen(const int& i, const int& j,
                                const ggPoint2& p) const;

};


#endif
