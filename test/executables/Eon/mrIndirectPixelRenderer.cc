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
#include <mrIndirectPixelRenderer.h>



ggBoolean mrIndirectPixelRenderer::samplePixel(int i, int j, int n,
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
          train.Append(indirectRadiance(r, time, shadowSamp[s],
			   reflectSamp[s], ggFalse, 0, 1.0)); 
   }
   return ggTrue;

}


// what is the radiance seen at point r.origin() looking in
// direction r.direction().  This implements the 1986 path tracing
// article by Indirect in Siggraph 86
ggSpectrum mrIndirectPixelRenderer::indirectRadiance(const ggRay3& r,
                                double time,
                                const ggPoint2& uvShadow,
                                const ggPoint2& uvReflect,
                                ggBoolean CORL,
                                int depth,
                                double attenuation) const {
      mrViewingHitRecord VHR;
      ggMaterialRecord MR;
      MR.UV = uvReflect;

      if (scenePtr->objects()->viewingHit(r, time, ggEpsilon, ggInfinity,
										  VHR, MR)) {
         ggSpectrum s((float)0.0);
         if (MR.BRDFPointer && VHR.hasUVW) {
             if (depth >= maxDepth)
             s += ggPi *scenePtr->ambient(VHR.p, VHR.UVW.w(), time) * MR.kBRDF *
                             MR.BRDFPointer->averageValue();
             if (depth > 0)
             s += MR.kBRDF * directLight(r, time, VHR.p, VHR.UVW, uvShadow,
                                          MR.BRDFPointer);
         }
         if (MR.hasRay1 && (depth < maxDepth || !MR.CORLRay1)
              && attenuation > maxAttenuation)
             s += MR.kRay1 * indirectRadiance(MR.ray1, time, uvShadow,
                                           MR.UV, MR.CORLRay1, depth+1,
                                           attenuation*MR.kRay1[3]);
         if (MR.hasRay2 && (depth  < maxDepth || !MR.CORLRay1) &&
             attenuation > maxAttenuation)
             s += MR.kRay2 * indirectRadiance(MR.ray2, time, uvShadow,
                                           MR.UV, MR.CORLRay2, depth+1,
                                           attenuation*MR.kRay2[3]);
        if (VHR.hasEmit && !CORL)
             s += VHR.kEmit;

        return s;
      }
      else
         return  scenePtr->background(r, time);

}


