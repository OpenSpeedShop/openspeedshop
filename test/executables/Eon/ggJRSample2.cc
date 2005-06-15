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
#include <ggJRSample2.h>
#include <ggRanNum.h>


ggJRSample2::ggJRSample2(int num) {
   iData = 0;
   xData = 0;
   yData = 0;
   N = 0;
   SetN(num);
}

void ggJRSample2::SetN(int num) {

   n = int(sqrt(num));
   if (n * n != num) {
      n++;
   }
   nSquared = n*n;

   if (num > N) {
       if (xData != 0)
           delete [] xData;
       if (yData != 0)
           delete [] yData;
       if (iData != 0)
           delete [] iData;
       if (jData != 0)
           delete [] jData;
 
       iData = new int[nSquared];
       jData = new int[nSquared];
       xData = new double[nSquared];
       yData = new double[nSquared];
   }
   N = num;
  
   Generate();
}

void ggJRSample2::Generate() {
   ggRanReal<double> r01;
   int i;
   for (i = 0; i < nSquared; i++) {
       xData[i] = (i + r01()) / double(nSquared);
       yData[i] = (i + r01()) / double(nSquared);
       iData[i] = i;
       jData[i] = i;
   }
   for (i = 0; i < n; i++) {
      ggPermute(&(jData[i*n]), n);
      ggPermute(&(iData[i*n]), n);
   }
}


ggPoint2 ggJRSample2::getPoint(int num) const {
    assert (num >= 0 && num < N);
    int i = num;
    int iCell = i/n;
    int iOffset = i % n;
    int jCell = iOffset;
    int jOffset = iCell;
    return ggPoint2( xData[ iData[ i ] ], 
                     yData[ jData[ jCell*n + jOffset] ] );
}

