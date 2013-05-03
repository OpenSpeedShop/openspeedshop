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



#ifndef MRCOARSEPIXELRENDERER_H
#define MRCOARSEPIXELRENDERER_H

#include <iostream.h>
#include <stdlib.h>
#include <mrScene.h>
#include <mrCamera.h>
#include <mrPixelRenderer.h>
#include <assert.h>

class mrCoarsePixelRenderer : public  mrPixelRenderer {
   public:
       virtual ggBoolean samplePixel(int i, int j, int nSamples,
				     ggTrain<ggSpectrum>& train);

       virtual ggSpectrum radiance(const ggRay3& r,
                                    double time,
                                    const ggPoint2& uvShadow,
                                    const ggPoint2& uvReflect) const;

      mrCoarsePixelRenderer(mrScene *s, mrCamera *c,
			      double t1, double t2)
	{
	  background = s->background();
	  indirectSurfacesPtr = s->lowResolutionObjects();
	  scenePtr = s;
	  cameraPtr = c;
	  time1 = t1;
	  time2 = t2;
	  
	  assert(indirectSurfacesPtr);
	}
             
   protected:
      ggSpectrum coarseRadiance(const ggRay3& r,
                                double time) const;

      mrSurface *indirectSurfacesPtr;
       ggSpectrum background;
       mrScene *scenePtr;
       mrCamera *cameraPtr;
       double time1, time2;
       
       ggJitterSample1 timeSamp;
       ggJitterSample2 shadowSamp, reflectSamp, lensSamp, pixelSamp;

};

#endif
