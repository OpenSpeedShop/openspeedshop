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
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <math.h>
#include <mrCoarsePixelRenderer.h>

ggBoolean mrCoarsePixelRenderer::samplePixel(int i, int j, int n,
                                         ggTrain<ggSpectrum>& train) {
     ggRay3 r;
     double time;
	 int samples = n*n;

     shadowSamp.SetNSamples(n,n);
     reflectSamp.SetNSamples(n,n);
     lensSamp.SetNSamples(n,n);
     pixelSamp.SetNSamples(n,n);
     timeSamp.SetNSamples(samples);


     ggVector2 offset((double)i, (double)j);

     for (int s = 0; s < samples; s++) {
          time = time1 + timeSamp[s] * (time2 - time1);
          r = cameraPtr->getRay(pixelSamp[s] + offset, time, lensSamp[s]);
          r.direction() = ggUnitVector(r.direction());
          train.Append(radiance(r, time, shadowSamp[s],
			   reflectSamp[s])); 
   }
   return ggTrue;

}


// send ray with cont-only-reflected-light false (so we can see
// luminaires as bright) and ray depth 1
ggSpectrum mrCoarsePixelRenderer::radiance(const ggRay3& r,
                                        double time,
                                        const ggPoint2& ,
                                        const ggPoint2& ) const {

     return  coarseRadiance(r, time); 
}

ggSpectrum mrCoarsePixelRenderer::coarseRadiance(const ggRay3& r,
                                double time) const {
      mrViewingHitRecord VHR;
      ggMaterialRecord MR;

 if (indirectSurfacesPtr->viewingHit(r, time, ggEpsilon, ggInfinity, VHR, MR))
   {
     if(ggDot(r.direction(),VHR.UVW.w()) > 0.0)
       return VHR.kEmit;
     else   
       return VHR.kEmit + *(scenePtr->spectra.lookUp(ggString("red")));


   }
 else
   return  background;
}


