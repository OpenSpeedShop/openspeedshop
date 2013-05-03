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



#include <math.h>
#include <ggOakSolidTexture.h>



ggBoolean ggOakSolidTexture::getTexture(const ggPoint3& p,
                                        ggSpectrum& tex,
                                        double& cov) const
{
     double r, R;

     double b0,b1;

     if (p.z() > height) R = topR;
     else if (p.z() < 0) R = baseR;
     else {
         double s = (height - p.z() ) / height;
         R = s * baseR + (1-s) * topR;
     }

     r = sqrt(p.x() * p.x() + p.y() * p.y());


// double d = sin(()) + 100*noise.turbulence(p, 5));

     ggPoint3 sp(20*p.x(), 20*p.y(), p.z());

     b0 = c0/baseR;
     b1 = c1/baseR;

     r += randomness * noise.turbulence(sp,5);
     //double d = ((r + 0.1*noise.turbulence(sp, 3)) * age) / R;
     r /= R;
     double d = r*r * (b1 - b0) / (2.0) + b0*r;
     d = d - int(d);
     d = d*d;
     d = sin(ggPi*0.5*d);

     tex = d * dark + (1-d)*light;
     cov = 1-d;
     return ggTrue;
}
