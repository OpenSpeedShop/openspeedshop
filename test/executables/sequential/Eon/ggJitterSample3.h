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



// The class definition for the Jittered Sample class over three dimensions.
// The sample points are uniformly disributed over the unit cube but each is
// restricted to occur randomly in its own little cell.
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
#ifndef GGJITTERSAMPLE3_H
#define GGJITTERSAMPLE3_H

#include <ggSample3.h>


class ggJitterSample3: public ggSample3 {

public:
				// Constructors
  ggJitterSample3();
  ggJitterSample3(const int&, const int&, const int&);
  ~ggJitterSample3(){
    if (dataIndex != 0) delete [] dataIndex;
  }
  
  void SetN(const int&, const int& , const int&);
  virtual int Generate();

protected:			
  int   nx, ny, nz;		// The number of cells along each dimension.
				// Nsamples and data inherited from base class
  double dx, dy, dz;		// The subpixel widths along each dimension.

};

#endif
