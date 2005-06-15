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



// The class definition for the Random Sample class over a unit cube.
// The sample points are uniformly disributed over the pixel but each is
// restricted to occur randomly in its own little cell.
//
// Author:     Peter Shirley
// Modified:   Rajesh Kamath, 1993
// Modified:   Shirley,Kamath, January 1994-- changes to abstract class.
//

#ifndef GGRANDOMSAMPLE2_H
#define GGRANDOMSAMPLE2_H

#include <ggSample2.h>

class ggRandomSample2 : public ggSample2 {

public:
				// Constructors
  ggRandomSample2();
  ggRandomSample2(int a);
  
  void SetNSamples(int num); 
  virtual ggBoolean Generate();

protected:			
  int   n;			// number of samples
};



#endif
