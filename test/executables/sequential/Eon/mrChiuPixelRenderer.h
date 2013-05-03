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



#ifndef MRCHIUPIXELRENDERER_H
#define MRCHIUPIXELRENDERER_H

#include <iostream.h>
#include <stdlib.h>
#include <mrSurface.h>
#include <mrPixelRenderer.h>
#include <mrCamera.h>
#include <mrScene.h>
#include <kcPriorityQueue.h>
#include <kcBox2.h>

class mrChiuPixelRenderer : public mrPixelRenderer {

   public:

		struct Node {

			unsigned char d;
			unsigned char leaf;
			float s[2];
			ggSpectrum v;
			float cost;
			kcBox2<float> cell;
			float a;
			float diff;

			Node *p, *lc, *rc;
			kcPriorityQueueNode<Node *> *pq_node;

			static float s_maxArea;
		};

		class NodeCmp {

			public:

				static int cmp(const Node *, const Node *);
		};

   public:
     
      virtual ggBoolean samplePixel(int i, int j, int nSamples,
										  ggTrain<ggSpectrum>& train);

      mrChiuPixelRenderer(mrScene *s, mrCamera *c,
			  int md, double t1, double t2, int nlr, float max_area, int ndr)
	  {
		   scenePtr = s;
		   cameraPtr = c;
		   maxDepth = md;
		   time1 = t1;
		   time2 = t2;
		   d_nLightRays = nlr;
		   Node::s_maxArea = max_area;
		   d_nDumpRays = ndr;
      }

   protected:
      ggSpectrum chiuRadiance(const ggRay3& r, double time,
	   const ggPoint2& uvReflect, int depth);
      ggSpectrum chiuApproximateRadiance(const ggRay3& r, double time, int depth);

       mrScene *scenePtr;
	   mrCamera *cameraPtr;
	   int maxDepth;
	   double time1, time2;

       ggJitterSample1 timeSamp;
       ggJitterSample2 shadowSamp, reflectSamp, lensSamp, pixelSamp;

	   int d_nLightRays, d_nDumpRays;
	   kcPriorityQueue<Node *, NodeCmp> d_queue;

	private:

		ggSpectrum adapt(const mrViewingHitRecord &VHR,
		 const ggMaterialRecord &MR, double time);
		ggSpectrum sample(const ggPoint2 &p,
		 const mrViewingHitRecord &VHR, const ggMaterialRecord &MR,
		 double time);

		void maxNeighborDiff(Node *n, Node *m);
		void UpdateNeighbors(Node *root, Node *n, const Node *m);
		void freeTree(Node *n);
};

#endif
