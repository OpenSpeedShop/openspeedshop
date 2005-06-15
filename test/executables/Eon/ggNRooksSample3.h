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
// capture each other as rooks on the n-row, n-column, n-deep "chessboard"
// where  n = number of samples.
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
#ifndef GGNROOKSSAMPLE3_H
#define GGNROOKSSAMPLE3_H

#include <ggSample3.h>

class ggNRooksSample3 : public ggSample3 {
public:
				// Constructors
  ggNRooksSample3();
  ggNRooksSample3(const int&);
  ~ggNRooksSample3()		// Destructor
  {
    if(dataIndex !=0) delete [] dataIndex;
    if(yData !=0) delete [] yData;
    if(zData !=0) delete [] zData;
  } 

  void SetN(const int&);
  virtual int  Generate();

protected:
  int *yData, *zData;   	// Used to permute y and z indices
  double subWidth;		// Edge of a subcube

};

#endif  
