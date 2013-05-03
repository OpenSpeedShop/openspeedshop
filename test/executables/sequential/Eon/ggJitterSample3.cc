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



// Function definitions for Jitter sampling over unit cube
// Refer to file ggJitterSample3.h 
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
#include <assert.h>
#include <ggJitterSample3.h>
#include <ggRanNum.h>


// Constructor: 1
// Inputs: none
// Side Effects: data index initialized to (int *)0
//               all integer variables set to 0

ggJitterSample3::ggJitterSample3() {
  Nsamples = 0;
  nx = ny = nz = npixel =0;
  dataIndex = (int *)0;
 }


// Constructor: 2
// Inputs: number of sample cells along each dimension
// Side Effects: npixel set, Nsamples set to 0, data Index initialized
//               Subcell dimensions set.
// Checks: Sanity of input

ggJitterSample3::ggJitterSample3(const int& numX,
				     const int& numY, const int& numZ) {
#ifdef GGSAFE
  assert((numX>0) && (numY>0) && (numZ>0) ); // Min
  assert( (numX<ggMaxSamplesPerD) && (numY<ggMaxSamplesPerD)
	 && (numZ < ggMaxSamplesPerD));
				// Max  
#endif

  npixel = (nx = numX)*(ny = numY)*(nz = numZ);

  dataIndex = InitSequence(npixel);
    
  dx = 1/(double)nx;		// Sub cell dimensions set
  dy = 1/(double)ny;
  dz = 1/(double)nz;
  Nsamples = 0;
}



// Member Function: SetN
// Inputs: number of sample cells along each dimension
// Return: none
// Side Effects: npixel set, Nsamples set to 0, data Index initialized
//               Subcell dimensions set.
// Checks: Sanity of input, clearing of old data and indices, if any

void ggJitterSample3::SetN(const int& numX,
			     const int& numY, const int& numZ)
{

#ifdef GGSAFE
  assert((numX>0) && (numY>0) && (numZ>0) ); // Min
  assert( (numX<ggMaxSamplesPerD) && (numY<ggMaxSamplesPerD)
	 && (numZ < ggMaxSamplesPerD));
				// Max  
#endif  

  if(npixel)			// To initialize dataIndex (random access)
    delete [] dataIndex;	// array. It is permuted in Generate below.
  npixel = (nx = numX)*(ny = numY)*(nz = numZ);
  dataIndex = InitSequence(npixel);
  
  dx = 1/(double)nx;
  dy = 1/(double)ny;
  dz = 1/(double)nz;

  if(Nsamples)
      data.Clear();		// Old data must be cleared
  Nsamples = 0;
				// Note that Nsamples is still 0
				// until Generate is called!
}



// Member Function: Generate
// Inputs: none
// Return: number of samples generated
// Side effects: data Index freshly permuted, Nsamples set
// Checks: pixel requirements must be set. If old data exists, clear it..

int ggJitterSample3::Generate()
{
#ifdef GGSAFE
  assert(npixel);
#endif
  ggRanReal<double> r01;
  register int i,j,k;

  if(Nsamples)
    data.Clear();		// If old data existed, start afresh
  Nsamples = 0;
				// To have random access to data
  ggPermute(dataIndex, npixel);	// from base class [] operator.

  for (i = 0; i < nx; i++)
    for (j = 0; j < ny; j++)
      for(k =0; k < nz; k++)
	{			// Append each data element to Train
	  data.Append( ggPoint3( ((double)i + r01())*dx,
			         ((double)j + r01())*dy,
				 ((double)k + r01())*dz));
	  Nsamples ++;
	}

   return(Nsamples);
}



