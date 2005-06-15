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



//Class definition for Multijittered samples over a pixel - these preserve
//both N-Rooks features (having uniform random distributions along each
//dimension as well as provide jittering over subpixel areas.
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
#ifndef GGMULTIJITTERSAMPLE2_H
#define GGMULTIJITTERSAMPLE2_H

#include <ggSample2.h>


class ggMultiJitterSample2 : public ggSample2 {
public:
				// Constructors
  ggMultiJitterSample2();
  ggMultiJitterSample2(const int &, const int &);

  ~ggMultiJitterSample2()
    {
      if (dataIndex != 0) delete [] dataIndex;
      if (xindex != 0) delete [] xindex;
      if (yindex != 0) delete [] yindex;
    }
  void SetN(const int &, const int &);
  virtual int  Generate();

protected:
  int nx, ny;			// number of samples along each dimension
  int *xindex, *yindex;		// Indices used in permuting row and column
				// numbers
  double delta;			// The subpixel width

				// Nsamples, dataIndex, npixel inherited
};



#endif
  
