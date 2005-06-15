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



//
// ggFineSpectrum.C-- definitions for class ggFineSpectrum
//
// Author:    Peter Shirley
//            10/20/93
// Modified:
//
//
// Copyright 1993 by Peter Shirley
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
//

#include <math.h>
#include <ggFineSpectrum.h>


istream& operator>>(istream& s, ggFineSpectrum& c) {
    int lambda1, lambda2;
    float amp1, amp2;
    s >> lambda1;
    if (lambda1 < 0) { c.SetRange(0, 0.0, 10000, 0.0); return s; }
    s >> amp1;
    c.SetRange(0, amp1, lambda1, amp1);
    do {
        s >> lambda2;
        if (lambda2 < 0) { 
            c.SetRange(lambda1, amp1, 10000, amp1);
         } 
         else {
             s >> amp2; 
             c.SetRange(lambda1, amp1, lambda2, amp2);
             amp1 = amp2;
             lambda1 = lambda2;
         }
    } while (lambda2 >= 0);
    c.SetRange(lambda1, amp1, 10000, amp1);
    return s;
}

ostream& operator<<(ostream& s, const ggFineSpectrum& c) {
   s << c.nComponents() << "\n";
   for (int i = 0; i < c.nComponents(); i++) 
      s << c.wavelength(i) << " " << c[i] << "\n";
    return s;
}


double  ggFineSpectrum::area(double lambda1, double lambda2) const {
#ifdef GGSAFE
    assert (lambda1 <= lambda2);
#endif
    double f1 = lambda1 - minLambda() + 0.5;
    double f2 = lambda2 - minLambda() + 0.5;
    if (f1 >= (nComponents() - 0.00001) || f2 <= 0.00001) return 0.0;

    f1 = ggMax(0.00001, f1);
    f2 = ggMin(nComponents() - 0.1, f2);

    int i1 = int(f1) + 1;  // poor man's ceiling-- not quite right for
                           // perfect integers-- wont cause us trouble
    int i2 = int(f2) - 1;  // poor man's floor
    double frac1 = i1 - f1;
    double frac2 = f2 - i2 - 1;
    return frac1*data[i1-1] + frac2*data[i2+1] + sum(i1,i2);
}

double  ggFineSpectrum::sum(int i1, int i2) const {
#ifdef GGSAFE
    assert (i1 < i2 && i1 >= 0 && i2 < nComponents());
#endif
   double sum = 0.0;
   for (int i = i1; i <= i2; i++)
        sum += data[i];
   return sum;
}

ggBoolean operator==(const ggFineSpectrum& s1, const ggFineSpectrum& s2) {
     for (int i = 0; i < s1.nComponents(); i++) 
         if (s1[i] == s2[i]) ; else return ggFalse;
     return ggTrue;
}

ggBoolean operator!=(const ggFineSpectrum& s1, const ggFineSpectrum& s2) {
    return !(s1 == s2);
}

void ggFineSpectrum::SetRange(int lambda1, float amp1,
                              int lambda2, float amp2) {
    int i1 = this->index(lambda1);
    int i2 = this->index(lambda2);
    if (i2 < 0 || i1 >= nComponents()) return;

#ifdef GGSAFE
    assert (i1 <= i2);
#endif
    if (i1 == i2) {
         data[i1] = 0.5*(amp1 + amp2);
         return;
    }

    int start = ggMax(i1, 0);
    int stop =  ggMin(i2, nComponents() - 1);
    double t;
    double scale = 1.0 / (i2 - i1);
    for (int i = start; i <= stop; i++) {
       t = (i - i1) * scale;
       data[i] = t*amp2 + (1-t)*amp1;
    }
}
