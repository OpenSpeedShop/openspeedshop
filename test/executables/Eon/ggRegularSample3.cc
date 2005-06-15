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



// Function definitions for Regular sampling over unit cube
// Refer to file ggRegularSample3.h 
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
#include <ggRegularSample3.h>

//Constructor: 1
//Inputs: none
//Side Effects: All integer data members set to 1
//              Data Index initialized to (int *)0
ggRegularSample3::ggRegularSample3(){
  Nsamples = npixel= 0;
  nx = ny = nz = 0;
  dataIndex = (int *)0;
}


//Constructor: 2
//Inputs: number of required samples along each dimension
//Side Effects: npixel set, data Index intialized
//Checks: sanity of input

ggRegularSample3::ggRegularSample3( const int& numX,
					const int& numY,const int& numZ){
#ifdef GGSAFE
  assert((numX>0) && (numY>0) && (numZ>0) ); // Min
  assert( (numX<ggMaxSamplesPerD) && (numY<ggMaxSamplesPerD)
	 && (numZ < ggMaxSamplesPerD));
				// Max  
#endif  


  Nsamples = 0;
  npixel = (nx = numX)*(ny = numY)*( nz = numZ);
  dataIndex = InitSequence(npixel);

  dx = 1.0/(double)nx;
  dy = 1.0/(double)ny;
  dz = 1.0/(double)nz;
  
}

//Member Function: SetN
//Inputs: number of required samples along each dimension
//Return: none
//Side Effects: npixel set, data Index intialized
//Checks: sanity of input, deletes old data and index if necessary

void ggRegularSample3::SetN(const int& numX,
			      const int& numY, const int& numZ){
#ifdef GGSAFE
  assert((numX>0) && (numY>0) && (numZ>0) ); // Min
  assert( (numX<ggMaxSamplesPerD) && (numY<ggMaxSamplesPerD)
	 && (numZ < ggMaxSamplesPerD));
				// Max  
#endif  

  if(npixel)
    delete [] dataIndex;
      
  npixel = (nx = numX)*(ny = numY)*( nz = numZ);
  dataIndex = InitSequence(npixel);

  dx = 1.0/(double)nx;
  dy = 1.0/(double)ny;
  dz = 1.0/(double)nz;

  if(Nsamples)
    data.Clear();			// The old data train must deleted
  Nsamples = 0;
}


//Member Function: Generate
//Inputs: none
//Return: number of samples actually generated
//Side Effects: fresh data available, data Index freshly permuted
//Checks: removes old data and index

int ggRegularSample3::Generate(){

#ifdef GGSAFE
  assert(npixel);
#endif

  ggPermute(dataIndex, npixel);	// Permute the dataIndex Values for 
				// every call to this function
  if(Nsamples)
    data.Clear();		// Old Data must be cleared
  Nsamples = 0;
  
  for(int i=0; i<nx; i++)
    for( int j=0; j < ny; j++)
      for(int k=0; k < nz; k++){
	data.Append(ggPoint3( ((double)i + 0.5)*dx,
			      ((double)j + 0.5)*dy,
			      ((double)k + 0.5)*dz));
      Nsamples++;
      }

  return(Nsamples);
}
		 
