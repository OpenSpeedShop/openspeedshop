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
#include <ggGeometry.h>
#include <kc/kc.h>
#include <kc/kcPriorityQueue.h>
#include <kc/kcMemRaster.h>
#include <kc/kcRGBPixel.h>
#include <kc/kcRGBColor.h>
#include <kc/kcPicImgFile.h>
#include <kc/kcPoint2.h>
#include <kc/kcFormat.h>
#include <kc/kcBox2.h>
#include <mrChiuPixelRenderer.h>



extern int Flag;
kcMemRaster<kcRGBPixel<float> > raster1(500, 500);
kcMemRaster<kcRGBPixel<float> > raster2(500, 500);


float mrChiuPixelRenderer::Node::s_maxArea;



ggBoolean mrChiuPixelRenderer::samplePixel(int i, int j, int n,
                                         ggTrain<ggSpectrum>& train) {
     ggRay3 r;
     double time;
	 int samples = n*n;

     reflectSamp.SetNSamples(n,n);
     lensSamp.SetNSamples(n,n);
     pixelSamp.SetNSamples(n,n);
     timeSamp.SetNSamples(samples);


     ggVector2 offset((double)i, (double)j);

     for (int s = 0; s < samples; s++) {
          shadowSamp.SetNSamples(10,10);
          time = time1 + timeSamp[s] * (time2 - time1);
          r = cameraPtr->getRay(pixelSamp[s] + offset, time, lensSamp[s]);
          r.direction() = ggUnitVector(r.direction());
          train.Append(chiuRadiance(r, time, reflectSamp[s], 0)); 
   }
   return ggTrue;

}


ggSpectrum mrChiuPixelRenderer::chiuRadiance(const ggRay3& r, double time,
 const ggPoint2& uvReflect, int depth) {

	mrViewingHitRecord VHR;
	ggMaterialRecord MR;
	MR.UV = uvReflect;

	if (scenePtr->objects()->viewingHit(r, time, ggEpsilon, ggInfinity, VHR, MR)) {

		ggSpectrum s((float)0.0);

		if (MR.BRDFPointer && VHR.hasUVW) {
                        if (ggDot(VHR.UVW.w(), r.direction()) > 0.0)
                             VHR.UVW.FlipW();
			s = adapt(VHR, MR, time);
		}

		if (MR.hasRay1 && depth < maxDepth && !MR.CORLRay1)
			s += MR.kRay1 * chiuRadiance(MR.ray1, time, MR.UV, depth+1);
		if (MR.hasRay2 && depth < maxDepth && !MR.CORLRay2)
			s += MR.kRay2 * chiuRadiance(MR.ray2, time, MR.UV, depth+1);
		if (VHR.hasEmit)
			s += VHR.kEmit;

		return s;
	} else

		return  scenePtr->background(r, time);
}

ggSpectrum
mrChiuPixelRenderer::chiuApproximateRadiance(const ggRay3& r, double time, int depth) {

	mrViewingHitRecord VHR;
	ggMaterialRecord MR;

	if (scenePtr->objects()->viewingHit(r, time, ggEpsilon, ggInfinity, VHR,
	 MR)) {

		ggSpectrum s((float)0.0);

		if (MR.BRDFPointer && VHR.hasUVW) {
			s += ggPi * scenePtr->ambient(VHR.p, VHR.UVW.w(), time) * MR.kBRDF*
			MR.BRDFPointer->averageValue();
		}

		if (MR.hasRay1 && depth < maxDepth && !MR.CORLRay1)
			s += MR.kRay1 * chiuApproximateRadiance(MR.ray1, time, depth+1);
		if (MR.hasRay2 && depth < maxDepth && !MR.CORLRay2)
			s += MR.kRay2 * chiuApproximateRadiance(MR.ray2, time, depth+1);

		if (VHR.hasEmit)
			s += VHR.kEmit;

		return s;

	} else {

		return  scenePtr->background(r, time);
	}
}

ggSpectrum
mrChiuPixelRenderer::adapt(const mrViewingHitRecord &VHR,
 const ggMaterialRecord &MR, double time) {

	int i;
	Node *root = new Node;

	root->d = 0;
	root->cell.min() = kcPoint2<float>(0, 0);
	root->cell.max() = kcPoint2<float>(1, 1);
	root->s[0] = root->cell.width()*ggCanonicalRandom() + root->cell.min().x();
	root->s[1] = root->cell.height()*ggCanonicalRandom() + root->cell.min().y();

	root->a = 1;
	root->leaf = kcTrue;
	root->p = kcNull;
	root->lc = kcNull;
	root->rc = kcNull;
	root->pq_node = kcNull;
	root->v = sample(ggPoint2(root->s[0], root->s[1]), VHR, MR, time);

	d_queue.Enqueue(root, &root->pq_node);

	int limit = d_nLightRays;
	if (Flag) {
		limit = d_nDumpRays;
	} else {
		limit = d_nLightRays;
	}

	for (i = 0; i < limit; i++) {

		Node *n, *lc, *rc, *oc, *nc;
		int d;
		float c;

		n = d_queue.Dequeue();
		n->pq_node = kcNull;
		d = n->d;
		c = n->cell[d]/2 + n->cell.min()[d];
		n->leaf = kcFalse;

		/*
		cerr << kcFormat("%d: ", i)
			 << kcFormat("(%g, %g)-->(%g, %g), area: %g, diff: %g",
						 n->ll[0], n->ll[1], n->ur[0], n->ur[1],
						 n->a, n->diff)
			 << endl;
		cerr << "      " << kcFormat("sampled: (%g, %g)", n->s[0], n->s[1])
			 << endl;
		*/

		lc = n->lc = new Node;
		rc = n->rc = new Node;
		*rc = *lc = *n;

		rc->cell.min()[d] = lc->cell.max()[d] = c;

		rc->p = lc->p = n;
		rc->d = lc->d = (d + 1)%2;
		rc->a = lc->a /= 2;
		rc->leaf = lc->leaf = kcTrue;
		rc->lc = rc->rc = lc->lc = lc->rc = kcNull;
		rc->pq_node = lc->pq_node = kcNull;

		if (n->s[d] < c) {

			oc = lc;
			nc = rc;

		} else {

			oc = rc;
			nc = lc;
		}

		oc->s[0] = n->s[0];
		oc->s[1] = n->s[1];
		oc->v = n->v;

		nc->s[0] = nc->cell.width()*ggCanonicalRandom() + nc->cell.min().x();
		nc->s[1] = nc->cell.height()*ggCanonicalRandom() + nc->cell.min().y();
		nc->v = sample(ggPoint2(nc->s[0], nc->s[1]), VHR, MR, time);

		//UpdateNeighbors(root, root, n);

		lc->diff = 0;
		maxNeighborDiff(root, lc);

		rc->diff = 0;
		maxNeighborDiff(root, rc);

		d_queue.Enqueue(lc, &lc->pq_node);
		d_queue.Enqueue(rc, &rc->pq_node);

		/*
		if ((i + 1) % 1000 == 0) {
			cerr << (i + 1) << " samples. . ." << endl;
		}
		*/
	}

//	cerr << "done sampling" << endl;

	ggSpectrum s((float)0.0);
	while (!d_queue.empty_q()) {

		Node *n = d_queue.Dequeue();
		double dw;

		/*
		cerr << kcFormat("%d: ", ++i)
			 << kcFormat("(%g, %g)-->(%g, %g), area: %g, diff: %g",
						 n->ll[0], n->ll[1], n->ur[0], n->ur[1],
						 n->a, maxNeighborDiff(root, n))
			 << endl;
		cerr << "      " << kcFormat("sampled: (%g, %g)", n->s[0], n->s[1])
			 << endl;
		*/

		/*
		cerr << "maxNeighborDiff of "
			 << kcFormat("(%g, %g)-->(%g, %g)",
						 8*n->ll[0], 8*n->ll[1], 8*n->ur[0], 8*n->ur[1])
			 << endl;

		maxNeighborDiff(root, n);
		*/

		dw = 2*M_PI*n->cell.width()*n->cell.height();

		s += dw*n->v;

/*****/
/*****/

		if (Flag) {

			kcRGBColor<float> red(1, 0, 0);
			kcRGBColor<float> gray(.3);
			int ll_x, ll_y, ur_x, ur_y, x, y;
			
			ll_x = 499.99*n->cell.min().x();
			ll_y = 499.99*n->cell.min().y();

			ur_x = 499.99*n->cell.max().x();
			ur_y = 499.99*n->cell.max().y();

			for (x = ll_x; x <= ur_x; x++) {
				raster1[x][ll_y] = red;
				raster1[x][ur_y] = red;
				raster2[x][ll_y] = red;
				raster2[x][ur_y] = red;
			}

			for (y = ll_y; y <= ur_y; y++) {
				raster1[ll_x][y] = red;
				raster1[ur_x][y] = red;
				raster2[ll_x][y] = red;
				raster2[ur_x][y] = red;
			}
				

			for (x = ll_x + 1; x <= ur_x - 1; x++) {
				for (y = ll_y + 1; y <= ur_y - 1; y++) {
					raster1[x][y] = kcRGBColor<float>(n->v.area());
					raster2[x][y] = kcRGBColor<float>(200*n->diff);
				}
			}

			s.SetBlack();
			s[0] = .1;
			s[1] = .1;
			s[2] = .1;
			s[3] = .1;
			s[4] = .1;
			s[5] = .1;
			s[6] = .1;
			s[7] = 6;
		}
/*****/
/*****/

	}

	if (Flag) {
		cerr << "dumping data images" << endl;
		kcPicImgFile("v.pic") << raster1;
		kcPicImgFile("d.pic") << raster2;
	}

	freeTree(root);

	return s;
}

ggSpectrum
mrChiuPixelRenderer::sample(const ggPoint2 &p, const mrViewingHitRecord &VHR,
 const ggMaterialRecord &MR, double time) {

	ggVector3 rv = ggUniformVector(VHR.UVW, p);
	double cost = ggDot(rv, VHR.UVW.w());
	ggRay3 ray(VHR.p, rv);
	return cost*MR.kBRDF*MR.BRDFPointer->averageValue()
	 *chiuApproximateRadiance(ray, time, 1);
}

void
mrChiuPixelRenderer::freeTree(Node *n) {

	if (n->lc != kcNull) {
		freeTree(n->lc);
	}
	if (n->rc != kcNull) {
		freeTree(n->rc);
	}
	delete n;
}

int 
mrChiuPixelRenderer::NodeCmp::cmp(const Node *n1, const Node *n2) {

	float w1, w2;

	w1 = n1->a*n1->diff;
	w2 = n2->a*n2->diff;

	if (n1->a > Node::s_maxArea) {
		return -1;
	} else if (n2->a > Node::s_maxArea) {
		return 1;
	} else if (w1 > w2) {
		return -1;
	} else if (w1 < w2) {
		return 1;
	} else {
		if (n1->a > n2->a) {
			return -1;
		} else {
			return 1;
		}
	}
}

void
mrChiuPixelRenderer::maxNeighborDiff(Node *n, Node *m) {

	if (n != kcNull && n != m) {

		if (n->cell.incOverlap_p(m->cell)) {

			if (n->leaf) {

				float s2 = 0;

				s2 += kcAbs(n->v.data[0] - m->v.data[0]);
				s2 += kcAbs(n->v.data[1] - m->v.data[1]);
				s2 += kcAbs(n->v.data[2] - m->v.data[2]);
				s2 += kcAbs(n->v.data[3] - m->v.data[3]);
				s2 += kcAbs(n->v.data[4] - m->v.data[4]);
				s2 += kcAbs(n->v.data[5] - m->v.data[5]);
				s2 += kcAbs(n->v.data[6] - m->v.data[6]);
				s2 += kcAbs(n->v.data[7] - m->v.data[7]);

				if (s2 > m->diff) {
					m->diff = s2;
					if (m->diff > n->diff) {
						n->diff = m->diff;
						if (n->pq_node != kcNull) {
							d_queue.Update(n->pq_node);
						}
					}
				}

			} else {

				maxNeighborDiff(n->lc, m);
				maxNeighborDiff(n->rc, m);
			}
		}
	}
}

void
mrChiuPixelRenderer::UpdateNeighbors(Node *root, Node *n, const Node *m) {

	if (n != kcNull && n != m) {

		if (n->cell.incOverlap_p(m->cell)) {

			if (n->leaf) {

				if (n->pq_node != kcNull) {
					n->diff = 0;
					maxNeighborDiff(root, n);
					d_queue.Update(n->pq_node);
				}

			} else {

				UpdateNeighbors(root, n->lc, m);
				UpdateNeighbors(root, n->rc, m);
			}
		}
	}
}
