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



// Class definition for Multijittered samples over a unit cube - these preserve
// both N-Rooks features (having uniform random distributions along each
// dimension as well as provide jittering over subdomains.
//
// Author:     Peter Shirley
// Modified:   Rajesh Kamath, 1993
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
#ifndef GGMULTIJITTERSAMPLE3_H
#define GGMULTIJITTERSAMPLE3_H

#include <ggSample3.h>

class ggMultiJitterSample3 : public ggSample3 {
public:
				// Constructors
  ggMultiJitterSample3();
  ggMultiJitterSample3(const int &);
  ~ggMultiJitterSample3();
      
  void SetN(const int &);
  virtual int  Generate();

protected:
  int n;
  int *xindex, *yindex, *zindex;
  int *xyindex, *yzindex;       // Indices used in permuting row and column
				// numbers
  double delta;			// The subpixel width

				// Nsamples, dataIndex, npixel inherited
};



#endif
  
