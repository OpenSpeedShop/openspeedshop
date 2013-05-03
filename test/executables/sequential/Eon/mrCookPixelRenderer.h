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



#ifndef MRCOOKPIXELRENDERER_H
#define MRCOOKPIXELRENDERER_H

#include <iostream.h>
#include <stdlib.h>
#include <mrSurface.h>
#include <mrPixelRenderer.h>
#include <mrCamera.h>
#include <mrScene.h>

class mrCookPixelRenderer : public mrPixelRenderer {
   public:
     
      virtual ggBoolean samplePixel(int i, int j, int nSamples,
										  ggTrain<ggSpectrum>& train);

      mrCookPixelRenderer(mrScene *s, mrCamera *c, int md, double t1, double t2) {
		   scenePtr = s;
		   cameraPtr = c;
		   maxDepth = md;
		   time1 = t1;
		   time2 = t2;
      }
   protected:
      ggSpectrum cookRadiance(const ggRay3& r,
                                double time,
                                const ggPoint2& uvShadow,
                                const ggPoint2& uvReflect,
                                ggBoolean corl,
                                int depth) const;
       virtual ggSpectrum directLight(const ggRay3& r, double time,
                           const ggPoint3& p,
                           const ggONB3& uvw,
                           const ggPoint2& uv,
                           ggBRDF *brdfPtr) const;
       mrScene *scenePtr;
	   mrCamera *cameraPtr;
	   int maxDepth;
	   double time1, time2;

       ggJitterSample1 timeSamp;
       ggJitterSample2 shadowSamp, reflectSamp, lensSamp, pixelSamp;

};

#endif
