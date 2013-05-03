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



#include <ggFrameInterpolation.h>

// Interpolation of frames

// Given an array of frames, # in the array and a parameter ranging from
// 0 to 1, interpolate between the specified frames.

// Remember that CR splines never hit the first or last control point
// (you can fake this behavior by duplicating the first and last).
ggFrame3 ggFrameInterpolation(const ggFrame3 keyframe[], int n, double t)
{
  int nstart;
  double subt;

  nstart = (int)(t*(n-3));
  subt = t*((double)n-3.0) - (double)((int)(t*(n-3)));

  // Check for obvious start/finish
  if (t<ggEpsilon) // Zero case
    return keyframe[1];
  else if (t>(1.0-ggEpsilon)) // 1.0 case
    return keyframe[n-2];
  else return ggCRSpline(keyframe[nstart],   keyframe[nstart+1],
			 keyframe[nstart+2], keyframe[nstart+3], subt);
}
