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
// ggSample2.C: The base class definition for sampling over two dimensions.
//
// Author:     Peter Shirley
// Modified:   Rajesh Kamath, 1993
// Modified:   Kamath, Shirley  January 94:  removed indexing, discrepancy code
//
//


#ifndef GGSAMPLE3_H
#define GGSAMPLE3_H

#include <ggMacros.h>
#include <ggRanNum.h>
#include <ggPoint3.h>
#include <ggBoolean.h>
#include <ggTrain.h>


// This is the base class for all sampling strategies over two dimensions.
// The following are the derived classes:
//                 ggRandomSample3


class ggSample3 {
public:

  int nSamples() const { return data.length(); }
  virtual ggBoolean Generate() { return ggFalse; }
				
  void Permute() { data.Permute(); }
  ggPoint3 operator[] (int i) const { // Used for retrieving data from the train
#ifdef GGSAFE
    assert( (i<nSamples()) && (i>=0));
#endif
    return data[i];
  }

protected:
  ggTrain<ggPoint3> data;
};

#endif
