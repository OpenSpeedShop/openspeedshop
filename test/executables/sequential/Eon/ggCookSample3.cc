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



#include <assert.h>
#include <ggCookSample3.h>
#include <ggRanNum.h>

ggCookSample3::ggCookSample3(int nx, int ny) {
   iData = 0;
   zData = 0;
   data = 0;
   N = 0;
   SetN(num);
}

void ggCookSample3::SetN(int num) {

   nx = ny = int(sqrt(num));
   if (nx * ny != num) {
      nx++; ny++;
   }
   nxTimesNy = nx*ny;
   dx = dy  = 1.0/double(nx);
   dz = 1.0/nxTimesNy;

   if (num > N) {
       if (iData != 0)
           delete [] iData;
       if (data != 0)
           delete [] data;
 
       iData = new int[nxTimesNy];
       zData = new int[nxTimesNy];
       data = new ggPoint3[nxTimesNy];
   }
   N = num;
   for (int i = 0; i < nxTimesNy; i++) {
       iData[i] = zData[i] = i;
    }
  
   Generate();
}

void ggCookSample3::Generate() {
   ggRanReal<double> r01;
   ggPermute(zData, nxTimesNy);
   for (int i = 0; i < nx; i++)
      for (int j = 0; j < ny; j++)
        data[i + nx*j].Set((double(i) + r01())* dx,
                       (double(j) + r01())* dy,(double(zData[i+nx*j]) + r01())*dz);
   ggPermute(iData, nxTimesNy);
}


ggPoint3 ggCookSample3::getPoint(int num) const {
    assert (num >= 0 && num < N);
    return data[ iData[num] ];
}
