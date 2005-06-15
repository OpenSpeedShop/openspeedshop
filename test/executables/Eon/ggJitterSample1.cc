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
// the source file ggJitterSample1.h
//
// Author:     Peter Shirley, January, 1994
#include <assert.h>
#include <ggJitterSample1.h>
#include <ggRanNum.h>



//Constructor: 1
//Inputs: none
ggJitterSample1::ggJitterSample1() {
  n = 1;
  delta = 1.0;
  Generate();
}



// Constructor: 2

ggJitterSample1::ggJitterSample1(int num) {
#ifdef GGSAFE
  assert( (num > 0) );
#endif
  n = num;

  delta = 1.0/(double)n;		// SubCell divisions set
  Generate();
}


// Member Function: SetN
// Inputs: number of sample cells along each dimension
// Return: none
// Side Effects: npixel set, Nsamples set to 0, data Index initialized
//               Subcell dimensions set.
// Checks: Sanity of input, clearing of old data and indices, if any

void ggJitterSample1::SetNSamples(int num) {
#ifdef GGSAFE
  assert( (num > 0) );
#endif
  n = num;

  delta = 1.0/(double)n;		// SubCell divisions set
  
  Generate();
}


// Member Function: Generate
// Inputs: none
// Return: number of samples generated
// Side effects: data Index freshly permuted, Nsamples set
// Checks: pixel requirements must be set. Clears any existing data

ggBoolean ggJitterSample1::Generate() {

  data.Clear();

  for (int i = 0; i < n; i++)
	if (!data.Append( ((double)i + ggCanonicalRandom())*delta))
               return ggFalse;
   return ggTrue;
}
