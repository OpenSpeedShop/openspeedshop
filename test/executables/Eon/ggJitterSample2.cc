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



// Function definition for Jitter sampling over unit square. Refer to 
// the source file ggJitterSample2.h
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
#include <assert.h>
#include <ggJitterSample2.h>
#include <ggRanNum.h>

//Constructor: 1
//Inputs: none
//Side Effects: All data members initialized to 0
//              Data Index initialized to be (int *)0
ggJitterSample2::ggJitterSample2() {
  nx = ny = 1;
  dx = dy = 1.0;
  Generate();
}



// Constructor: 2
// Inputs: number of sample cells along each dimension
// Side Effects: npixel set, Nsamples set to 0, data Index initialized
//               Subcell dimensions set.
// Checks: Sanity of input

ggJitterSample2::ggJitterSample2(int numX, int numY) {
#ifdef GGSAFE
  assert( (numX > 0) && (numY > 0) );
#endif
  nx = numX;
  ny = numY;

  dx = 1.0/(double)nx;		// SubCell divisions set
  dy = 1.0/(double)ny;
  Generate();
}


// Member Function: SetN
// Inputs: number of sample cells along each dimension
// Return: none
// Side Effects: npixel set, Nsamples set to 0, data Index initialized
//               Subcell dimensions set.
// Checks: Sanity of input, clearing of old data and indices, if any

void ggJitterSample2::SetNSamples(int numX, int numY) {
#ifdef GGSAFE
  assert( (numX > 0) && (numY > 0) );
#endif
  nx = numX;
  ny = numY;

  dx = 1.0/(double)nx;		// SubCell divisions set
  dy = 1.0/(double)ny;
  
  Generate();
}


// Member Function: Generate
// Inputs: none
// Return: number of samples generated
// Side effects: data Index freshly permuted, Nsamples set
// Checks: pixel requirements must be set. Clears any existing data

ggBoolean ggJitterSample2::Generate() {

  data.Clear();

  for (int i = 0; i < nx; i++)
    for (int j = 0; j < ny; j++){
      //explicitly order the calls to ggCanonicalRandom
#if defined ORIGINAL
	if (!data.Append( ggPoint2( ((double)i + ggCanonicalRandom())*dx,
		                    ((double)j + ggCanonicalRandom())*dy)))
	return ggFalse;
#else
      double t1 = ((double)i + ggCanonicalRandom())*dx;
      double t2 = ((double)j + ggCanonicalRandom())*dy;
      if (!data.Append( ggPoint2( t1,t2 ) ) )
	return ggFalse;
#endif      
    }
   return ggTrue;
}
