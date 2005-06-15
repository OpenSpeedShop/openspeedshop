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



// Function definition for Multijitter sampling over unit square. Refer to 
// the source file ggMultiJitterSample2.h
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
#include <ggMultiJitterSample2.h>


//Constructor 1
//Inputs: none
//Side effects: All index arrays initialized to (int *)0
//              All sample and pixel values = 0
ggMultiJitterSample2::ggMultiJitterSample2()
{
  npixel = Nsamples = 0;
  xindex = yindex = (int *)0;
  dataIndex = (int *)0;

}

//Constructor 2
//Calls SetN member function. 
ggMultiJitterSample2::ggMultiJitterSample2(const int& numX,
					       const int& numY)
{

  npixel = Nsamples = 0;
  xindex = yindex = (int *)0;
  SetN(numX, numY);
}  
  
//Member function:  SetN
//Inputs:  number of sample divisions along each dimension.
//Return: none
//Side effects: Initializes all the indices from 1->max
//              Initializes all permuting indices to subdivisions along each
//               dimension.
//Checks: input sanity, removal of old data if necessary
void ggMultiJitterSample2::SetN(const int& numX, const int& numY)
			
{
  register int count1, count2;
  register int val;
#ifdef GGSAFE
  assert( (numX > 0) && (numY > 0) );
  assert( (numX < ggMaxSamplesPerD) && (numY < ggMaxSamplesPerD) );  
#endif

  nx = numX;			// Initialize the two counts
  ny = numY;

  if(npixel)
    {				// If npixel has been intitialized
      if(Nsamples)		// If there is old data (not necessarily
	data.Clear();		// true if npixel is set
      delete [] xindex;
      delete [] yindex;
      delete [] dataIndex;
    }
  Nsamples = 0;

  npixel = numX*numY;
  dataIndex = InitSequence(npixel);

  xindex = new int[npixel];	// Arrange the canonical form
  val = 0;
  for(count1 = 0; count1 < ny; count1++)
    for(count2 = count1; count2 < npixel; count2 += ny)
	xindex[count2] = val++;

  yindex = InitSequence(npixel);
  delta = 1/(double)npixel;	// Sub cell edge
  
}


//Member Function: Generate
//Inputs: none
//Return Value: Number of Sample points generated
//Side Effects: data generated, Data index permuted freshly
//Checks:       The sample requirements must have been recorded earlier.
//              Old data removed, if necessary
int ggMultiJitterSample2::Generate()
{
#ifdef GGSAFE
  assert(npixel);
#endif
  int count;
  int *addr;
  ggRanReal<double> r01;


  ggPermute(dataIndex, npixel);
  
  for(addr=xindex,count=0; count<nx; count++,addr+= ny )
      ggPermute(addr,ny);	// Permute the columns in each subcell

  for(addr=yindex,count=0; count<ny; count++,addr+= nx )
      ggPermute(addr,nx);	// Permute the rows in each subcell

  if(Nsamples)			// If old data existed
    data.Clear();
  
  for(count=0; count < npixel; count++)
    data.Append( ggPoint2(
			  (count+r01())*delta,
			  (yindex[xindex[count]] + r01())*delta));
			  
  Nsamples = count;
  return(Nsamples);
}
