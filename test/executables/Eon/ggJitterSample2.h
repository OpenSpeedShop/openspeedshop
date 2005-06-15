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



// The class definition for the Jittered Sample class over a unit square.
// The sample points are uniformly disributed over the pixel but each is
// restricted to occur randomly in its own little cell.
//
// Author:     Peter Shirley
// Modified:   Rajesh Kamath, 1993
// Modified:   Shirley,Kamath, January 1994-- changes to abstract class.
//
// Copyright 1993 by Peter Shirley, Rajesh Kamath
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
//

#ifndef GGJITTERSAMPLE2_H
#define GGJITTERSAMPLE2_H

#include <ggSample2.h>

class ggJitterSample2 : public ggSample2 {

public:
				// Constructors
  ggJitterSample2();
  ggJitterSample2(int a, int b);
  
  void SetNSamples(int numX, int numY); 
  virtual ggBoolean Generate();

protected:			
  int   nx, ny;			// Subpixels along each dimension
  double dx, dy;		// Subpixel dimensions
				// Nsamples and data inherited from base class
};



#endif
