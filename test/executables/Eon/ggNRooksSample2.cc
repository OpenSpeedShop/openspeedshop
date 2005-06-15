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



// Function definition for NRooks sampling over unit square. Refer to the
// source file ggNRooksSample2.h
//
// Author:     Peter Shirley
// Modified:   Rajesh Kamath, 1993
// Modified:   Peter Shirley, Feb 1994
//
//
#include <assert.h>
#include <ggRanNum.h>
#include <ggNRooksSample2.h>


ggNRooksSample2 :: ggNRooksSample2(){
   SetNSamples(1);
}

ggNRooksSample2 :: ggNRooksSample2(int num) {
   SetNSamples(num);
}


void ggNRooksSample2 :: SetNSamples(int num)
{
#ifdef GGSAFE
  assert(num>0);
#endif
   n = num;
   dx = 1.0 / num;
   Generate();
}


ggBoolean ggNRooksSample2::Generate() {

  int i;
  data.Clear();
  yValues.Clear();

  for (i = 0; i < n; i++)
	if (!yValues.Append((i + ggCanonicalRandom())*dx))
              return ggFalse;

  yValues.Permute();

  for (i = 0; i < n; i++)
	if (!data.Append(ggPoint2((i + ggCanonicalRandom())*dx,
                                  yValues[i])
                        ))
              return ggFalse;
  return ggTrue;
}
