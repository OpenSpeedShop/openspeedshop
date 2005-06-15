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



#include <ggTrianglePixelFilter.h>


ggPoint2 ggTrianglePixelFilter::warp(const ggPoint2& pin) const
{
    ggPoint2 pout;

   if (pin.x() >= 0.5)
      pout.x() = 1.5 - sqrt(2.0 - 2.0*pin.x());
   else
      pout.x() = -0.5 + sqrt(2.0*pin.x());

   if (pin.y() >= 0.5)
      pout.y() = 1.5 - sqrt(2.0 - 2.0*pin.y());
   else
      pout.y() = -0.5 + sqrt(2.0*pin.y());

   return pout;

}

ggPoint2 ggTrianglePixelFilter::warpScreen(const int& a, const int& b, const ggPoint2& pin) const 
{
  ggPoint2 pout;

  if (pin.x() >= 0.5)
     pout.x() = 1.5 - sqrt(2.0 - 2.0*pin.x());
   else
     pout.x() = -0.5 + sqrt(2.0*pin.x());

   if (pin.y() >= 0.5)
      pout.y() = 1.5 - sqrt(2.0 - 2.0*pin.y());
   else
      pout.y() = -0.5 + sqrt(2.0*pin.y());

  ggVector2 v(a,b);
  return pout + v;
}
