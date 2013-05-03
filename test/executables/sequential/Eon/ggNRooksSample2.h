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



// This class definition defines a strategy for generating sample points
// that maintain the N-Rooks property, i.e., the sample points cannot
// capture each other as rooks on the n-row, n-column  chessboard where
// n = number of samples.
//
// Author:     Peter Shirley
// Modified:   Rajesh Kamath, 1993
// Modified:   Peter Shirley, Feb 1994
//
#ifndef GGNROOKSSAMPLE2_H
#define GGNROOKSSAMPLE2_H

#include <ggSample2.h>

class ggNRooksSample2 : public ggSample2 {
public:
				// Constructors
  ggNRooksSample2();
  ggNRooksSample2(int n);
    
  void SetNSamples(int n);
  virtual ggBoolean Generate();

protected:
  ggTrain<double> yValues;
  double dx;		        // row or column width
  int n; 
};

#endif  
