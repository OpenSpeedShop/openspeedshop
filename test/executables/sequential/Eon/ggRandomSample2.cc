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



// Function definition for Random sampling over unit square. Refer to 
// the source file ggRandomSample2.h
//
// Author:     Peter Shirley
// Modified:   Rajesh Kamath, 1993
// Modified:   Shirley,Kamath, January 1994-- changes to abstract class.
//
#include <assert.h>
#include <ggRandomSample2.h>
#include <ggRanNum.h>



//Constructor: 1
ggRandomSample2::ggRandomSample2() {
  n = 1;
  Generate();
}



// Constructor: 2
// Checks: Sanity of input

ggRandomSample2::ggRandomSample2(int num) {
#ifdef GGSAFE
  assert( (num > 0) );
#endif
  n = num;
  Generate();
}


// Member Function: SetNSamples

void ggRandomSample2::SetNSamples(int num) {
#ifdef GGSAFE
  assert( (num > 0) );
#endif
  n = num;
  Generate();
}


// Member Function: Generate
// Inputs: none
// Return: number of samples generated
// Side effects: data Index freshly permuted, Nsamples set
// Checks: pixel requirements must be set. Clears any existing data

ggBoolean ggRandomSample2::Generate() {

  data.Clear();

  for (int i = 0; i < n; i++) {
#if defined ORIGINAL
	if (!data.Append( ggPoint2( ggCanonicalRandom(),
	                            ggCanonicalRandom()) ))
               return ggFalse;
#else
	double t=ggCanonicalRandom();
	if (!data.Append( ggPoint2( t, ggCanonicalRandom()) ))
               return ggFalse;
#endif
  }
   return ggTrue;
}
