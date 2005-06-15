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



#include <ggSolidNoise2.h>


ggSolidNoise2::ggSolidNoise2()
{
   ggVector2 v;
   int accept;

   n = 256;
   for (int i = 0; i < n; i++)
   {
      phi[i] = i;
      accept = 0;
      while (!accept) {
#if defined ORIGINAL
	v.Set(ggCanonicalRandom(),ggCanonicalRandom());
#else
       double t1=ggCanonicalRandom();
       v.Set(t1, ggCanonicalRandom());
#endif
         if (ggDot(v , v) <= 1.0) accept = 1;
      }
      double l = sqrt(v.x()*v.x() + v.y()*v.y());
      v.Set(v.x() / l, v.y()/l);
   //  compilation trouble.
   //   v = v.normal();
      grad[i] = v;
   }
   ggPermute(phi, n);
}


double ggSolidNoise2::turbulence(const ggPoint2& p, int depth) {
   double sum;
   double weight;
   ggPoint2 ptemp;

   sum = 0.0;
   weight = 1.0;
   ptemp = p;

   for (int i = 0; i < depth; i++) {
       sum += fabs(noise(ptemp)) / weight;
       weight = weight * 2.0;
       ptemp.x() = (ptemp.x() * weight);
       ptemp.y() = (ptemp.y() * weight);
   }
   return sum;
}

double ggSolidNoise2::dturbulence(const ggPoint2& p, int depth, double d) {
   double sum;
   double weight;
   ggPoint2 ptemp;

   sum = 0.0;
   weight = 1.0;
   ptemp = p;

   for (int i = 0; i < depth; i++) {
       sum += fabs(noise(ptemp)) / d;
       weight = weight * d;
       ptemp.x() = (ptemp.x() * weight);
       ptemp.y() = (ptemp.y() * weight);
   }
   return sum;
}

ggVector2 ggSolidNoise2::vectorTurbulence(const ggPoint2& p, int depth) {
   ggVector2 sum;
   double weight;
   ggPoint2 ptemp;

   sum.Set(0.0, 0.0);
   weight = 1.0;
   ptemp = p;

   for (int i = 0; i < depth; i++)
   {
       sum += vectorNoise(ptemp) * weight;
       weight = weight / 2.0;
       ptemp.x() = (ptemp.x() / weight);
       ptemp.y() = (ptemp.y() / weight);
   }
   return sum;
}

ggVector2 ggSolidNoise2::vectorNoise(const ggPoint2& p) {
    int i, j;
    int fi, fj;
    ggVector2 sum, v;

    sum.Set(0.0, 0.0);
    fi = int(floor(p.x()));
    fj = int(floor(p.y()));
    for (i = fi; i <= fi+1; i++)
       for (j = fj; j <= fj+1; j++) {
               v.Set(p.x() - i, p.y() - j);
               sum += vectorKnot(i, j,  v);
        }
    return sum;
    
}

double ggSolidNoise2::noise(const ggPoint2& p)
{
    int fi, fj;
    double sum;
    ggVector2 v;

    fi = int(floor(p.x()));
    fj = int(floor(p.y()));
    sum = 0.0;
    for (int i = fi; i <= fi+1; i++)
       for (int j = fj; j <= fj+1; j++)
          {
               v.Set(p.x() - i, p.y() - j);
               sum += knot(i, j, v);
          }
    
    return sum;
    
}
