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
// the source file ggRandomSample3.h
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
#include <ggRandomSample3.h>
#include <ggRanNum.h>



//Constructor: 1
ggRandomSample3::ggRandomSample3() {
  n = 1;
  Generate();
}



// Constructor: 2
// Checks: Sanity of input

ggRandomSample3::ggRandomSample3(int num) {
#ifdef GGSAFE
  assert( (num > 0) );
#endif
  n = num;
  Generate();
}


// Member Function: SetNSamples

void ggRandomSample3::SetNSamples(int num) {
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

ggBoolean ggRandomSample3::Generate() {

  data.Clear();

#if defined ORIGINAL
  for (int i = 0; i < n; i++)
	if (!data.Append( ggPoint3( ggCanonicalRandom(),
	                            ggCanonicalRandom(),
	                            ggCanonicalRandom()) ))
               return ggFalse;
#else
	double t0=ggCanonicalRandom();
	double t1=ggCanonicalRandom();
	if (!data.Append( ggPoint3( t0, t1, ggCanonicalRandom()) ))
               return ggFalse;
#endif
   return ggTrue;
}
