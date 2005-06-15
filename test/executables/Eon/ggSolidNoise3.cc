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



#include <ggSolidNoise3.h>
#include <ggPermute.h>


ggSolidNoise3::ggSolidNoise3()
{
   ggVector3 v;
   int accept;

   n = 256;
   for (int i = 0; i < n; i++)
   {
      phi[i] = i;
      accept = 0;
      while (!accept)
      {
#if defined ORIGINAL
	v.Set( ggCanonicalRandom(),ggCanonicalRandom(),ggCanonicalRandom());
#else
	double t0=ggCanonicalRandom();
	double t1=ggCanonicalRandom();
	v.Set(t0, t1, ggCanonicalRandom());
#endif
         if (ggDot(v , v) <= 1.0) accept = 1;
      }
      v.MakeUnitVector();
      grad[i] = v;
   }
   ggPermute(phi, n);
}


double ggSolidNoise3::turbulence(const ggPoint3& p, int depth)
const
{
   double sum;
   double weight;
   ggPoint3 ptemp;

   sum = 0.0;
   weight = 1.0;
   ptemp = p;

   for (int i = 0; i < depth; i++)
   {
       sum += fabs(noise(ptemp)) / weight;
       weight = weight * 2.0;

       ptemp.x() = (ptemp.x() * weight);
       ptemp.y() = (ptemp.y() * weight);
       ptemp.z() = (ptemp.z() * weight);
   }
   return sum;
}

double ggSolidNoise3::dturbulence(const ggPoint3& p, int depth, double d)
const {
   double sum;
   double weight;
   ggPoint3 ptemp;

   sum = 0.0;
   weight = 1.0;
   ptemp = p;

   for (int i = 0; i < depth; i++)
   {
       sum += fabs(noise(ptemp)) / d;
       weight = weight * d;
       ptemp.x() = (ptemp.x() * weight);
       ptemp.y() = (ptemp.y() * weight);
       ptemp.z() = (ptemp.z() * weight);
   }
   return sum;
}

ggVector3 ggSolidNoise3::vectorTurbulence(const ggPoint3& p, int depth)
const {
   ggVector3 sum;
   double weight;
   ggPoint3 ptemp;

   sum.Set(0.0, 0.0, 0.0);
   weight = 1.0;
   ptemp = p;

   for (int i = 0; i < depth; i++)
   {
       sum += vectorNoise(ptemp) * weight;
       weight = weight / 2.0;
       ptemp.x() = (ptemp.x() / weight);
       ptemp.y() = (ptemp.y() / weight);
       ptemp.z() = (ptemp.z() / weight);
   }
   return sum;
}

ggVector3 ggSolidNoise3::vectorNoise(const ggPoint3& p) const {
    int i, j, k;
    int fi, fj, fk;
    ggVector3 sum, v;

    sum.Set(0.0, 0.0, 0.0);
    fi = int(floor(p.x()));
    fj = int(floor(p.y()));
    fk = int(floor(p.z()));
    for (i = fi; i <= fi+1; i++)
       for (j = fj; j <= fj+1; j++)
          for (k = fk; k <= fk+1; k++) {
               v.Set(p.x() - i, p.y() - j, p.z() - k);
               sum += vectorKnot(i, j, k, v);
          }
    return sum;
    
}

double ggSolidNoise3::noise(const ggPoint3& p) const {
    int fi, fj, fk;
    double sum;
    ggVector3 v;

    fi = int(floor(p.x()));
    fj = int(floor(p.y()));
    fk = int(floor(p.z()));
/*
    sum = 0.0;
    for (int i = fi; i <= fi+1; i++)
       for (int j = fj; j <= fj+1; j++)
          for (int k = fk; k <= fk+1; k++)
          {
               v.Set(p.x() - i, p.y() - j, p.z() - k);
               sum += knot(i, j, k, v);
          }
*/
    sum = 0.0;

    v.Set(p.x() - fi, p.y() - fj, p.z() - fk);
    sum += knot(fi, fj, fk, v);

    v.Set(p.x() - fi - 1, p.y() - fj, p.z() - fk);
    sum += knot(fi + 1, fj, fk, v);

    v.Set(p.x() - fi, p.y() - fj - 1, p.z() - fk);
    sum += knot(fi, fj + 1, fk, v);

    v.Set(p.x() - fi, p.y() - fj, p.z() - fk -1);
    sum += knot(fi, fj, fk + 1, v);

    v.Set(p.x() - fi -1, p.y() - fj -1, p.z() - fk);
    sum += knot(fi + 1, fj + 1, fk, v);

    v.Set(p.x() - fi -1, p.y() - fj, p.z() - fk -1);
    sum += knot(fi + 1, fj, fk + 1, v);

    v.Set(p.x() - fi, p.y() - fj -1, p.z() - fk -1);
    sum += knot(fi, fj + 1, fk + 1, v);

    v.Set(p.x() - fi -1, p.y() - fj -1, p.z() - fk -1);
    sum += knot(fi + 1, fj + 1, fk + 1, v);
    
    return sum;
    
}
