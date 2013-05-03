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



// Function definitions for NRooks sampling over unit cube
// Refer to file ggNRooksSample3.h 
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
#include <ggRanNum.h>
#include <ggNRooksSample3.h>

//Constructor: 1
//Inputs: none
//Side effects: All index arrays initialized to (int *)0
//              All other variables intialized to 0
ggNRooksSample3 :: ggNRooksSample3(){
  npixel = Nsamples =0;
  dataIndex = (int *)0;
  yData = zData  = (int *)0;	// need to do this to initialize the 
				// permute pointers
}


//Constructor: 2
//Inputs:   number of samples required
//Side effects: sets npixel, initializes the data Index
//              sets Nsamples to 0
//Checks:   If requirements are sane

ggNRooksSample3 :: ggNRooksSample3(const int& numSamples ){

#ifdef GGSAFE
  assert( numSamples > 0);
  assert( numSamples < ggMaxSamples);
#endif

  npixel = numSamples;
  subWidth = 1.0/(double)npixel;
  Nsamples = 0;
                                   // For data fetching later 
  dataIndex = InitSequence(npixel);
  yData     = InitSequence(npixel);
  zData     = InitSequence(npixel);
}


//Member Function : SetN
//Inputs:  number of samples required
//Return:  none
//Side effects: sets Nsamples = 0, npixel, initilializes index arrays
//Checks:  If input is sane, whether old data exists

void ggNRooksSample3 :: SetN(const int& numSamples)
{
#ifdef GGSAFE
  assert( numSamples > 0);
  assert( numSamples < ggMaxSamples);
#endif


  if(npixel)
    {
      delete [] yData;
      delete [] zData;
      delete [] dataIndex;
    }
  
  npixel = numSamples;
  subWidth = 1.0/(double)npixel;

  if(Nsamples)
    data.Clear();		// Delete the old train, if any
  Nsamples=0;

  dataIndex = InitSequence(npixel);
  yData     = InitSequence(npixel);
  zData     = InitSequence(npixel);
}



//Member Function: Generate
//Inputs: none
//Return: Number of samples actually generated
//Side Effects: Permutes all indices
//Checks: If pixel requirements have already been set

int ggNRooksSample3 :: Generate()
{
  int i = 0;
  ggRanReal<double> r01;

#ifdef GGSAFE
  assert(npixel);
#endif

  if(Nsamples)			// If old data exists, start afresh
    data.Clear();
  
  ggPermute(yData, npixel);	// Permute indices to maintain N-Rooks property
  ggPermute(zData, npixel);

  ggPermute(dataIndex, npixel);	// For data fetching

  while(i< npixel)
    {		
      data.Append( ggPoint3( ((double)i + r01())*subWidth,
			     ((double)yData[i] + r01())*subWidth,
			     ((double)zData[i] + r01())*subWidth));
      i++;
    }
  Nsamples = i;
  return(Nsamples);
}

