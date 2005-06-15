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



// Function definitions for MultiJitter sampling over unit cube
// Refer to file ggMultiJitterSample3.h 
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
#include <ggMultiJitterSample3.h>


//Constructor 1
//Inputs: none
//Side effects: All index arrays initialized to (int *)0
//              All sample and pixel values = 0
ggMultiJitterSample3::ggMultiJitterSample3()
{
  npixel = Nsamples = n = 0;
  xindex = yindex = zindex = xyindex = yzindex = (int *)0;
  dataIndex = (int *)0;

}

//Constructor 2
//Calls SetN member function. 
ggMultiJitterSample3::ggMultiJitterSample3(const int& numX)
{

  npixel = Nsamples = n = 0;
  xindex = yindex = zindex = xyindex = yzindex = (int *)0;
  SetN(numX);
}  

//Destructor
ggMultiJitterSample3::~ggMultiJitterSample3()
{
  if(xindex !=0 ) delete [] xindex;
  if(yindex !=0 ) delete [] yindex;
  if(zindex !=0 ) delete [] zindex;
  if(xyindex !=0 ) delete [] xyindex;
  if(yzindex !=0 ) delete [] yzindex;
  if(dataIndex !=0 ) delete [] dataIndex;
       

}
  
  
//Member function:  SetN
//Inputs:  number of sample divisions along each dimension.
//Return: none
//Side effects: Initializes all the indices from 1->max
//              Initializes all permuting indices to subdivisions along each
//               dimension.
//Checks: input sanity, removal of old data if necessary
void ggMultiJitterSample3::SetN(const int& numX)
{
  register int count1, count2;
  register int val;

#ifdef GGSAFE
  assert((numX>0));
  assert( (numX<ggMaxSamplesPerD) );
				// Max  
#endif    
 n = numX;

  if(npixel)
    {				// If npixel has been intitialized
      if(Nsamples)		// If there is old data (not necessarily
	data.Clear();		// true if npixel is set
      delete [] xyindex;
      delete [] yzindex;
      delete [] xindex;
      delete [] yindex;
      delete [] zindex;
      delete [] dataIndex;
    }

  npixel = n*n;
  dataIndex = InitSequence(npixel); // Initialize the random retrieval index

  xyindex = new int[npixel];	// Arrange the canonical form
  val = 0;

				// 11 21 31 41 51 61..... N1
				// 12 22 32 42 .......... N2
				// ....
				// N1 N2 N3 N4 .......... NN

  for(count1 = 0; count1 < n; count1++)
    for(count2 = count1; count2 < npixel; count2 += n)
      xyindex[count2] = val++;

  yzindex = new int[npixel];

  val=0;			// Arrange the other canonical form

				// 11 21 31 41 51 61..... N1
				// N2 12 22 32 .......... N-1,2
				// ....
				// 2N 3N 4N.............  1N

  for(count1 = 0; count1 < n; count1++)
   for(count2 = count1; count2 < npixel; count2 += ((count2%n)==(n-1)?1:(n+1)))
      yzindex[count2] = val++;

 
  xindex = InitSequence(npixel); // Initialize the permuting indices
  yindex = InitSequence(npixel);
  zindex = InitSequence(npixel);
  
  delta = 1/(double)npixel;	// Sub cell edge
  
}

 
//Member Function: Generate
//Inputs: none
//Return Value: Number of Sample points generated
//Side Effects: data generated, Data index permuted freshly
//Checks:       The sample requirements must have been recorded earlier.
//              Old data removed, if necessary
int ggMultiJitterSample3::Generate()
{
#ifdef GGSAFE
  assert(npixel);
#endif
  int count;
  int *addr;
  ggRanReal<double> r01;


  ggPermute(dataIndex, npixel);


  
  for(addr=xindex,count=0; count<n; count++,addr+= n)
      ggPermute(addr,n);	// Permute the yz planes in each subcell

  for(addr=yindex,count=0; count<n; count++,addr+= n)
      ggPermute(addr, n);	// Permute the xz planes in each subcell

  for(addr=zindex,count=0; count<n; count++,addr+= n)
      ggPermute(addr, n);	// Permute the xy planes in each subcell

  if(Nsamples)			// If old data existed
    data.Clear();
  
  for(count=0; count < npixel; count++)
    data.Append( ggPoint3(
			  (xindex[xyindex[yindex[count]]] + r01())*delta,
			  (count+r01())*delta,
			  (zindex[yzindex[yindex[count]]] + r01())*delta));
			  
  Nsamples = count;
  return(Nsamples);
}
