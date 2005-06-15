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



// The class definition for the Jittered Sample class over [0,1]
//
// Author:     Peter Shirley, January, 1994
//

#ifndef GGJITTERSAMPLE1_H
#define GGJITTERSAMPLE1_H

#include <ggSample1.h>

class ggJitterSample1 : public ggSample1 {

public:
				// Constructors
  ggJitterSample1();
  ggJitterSample1(int b);
  
  void SetNSamples(int num); 
  virtual ggBoolean Generate();

protected:			
  int   n;			// Subpixels along each dimension
  double delta;                 // Subpixel dimensions
				// Nsamples and data inherited from base class
};



#endif
