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



#include <ggPhongBRDF.h>
#include <ggOptics.h>
#include <math.h>

ggSpectrum ggPhongBRDF::averageValue()
const
{
   return 1.0;
}


ggSpectrum ggPhongBRDF::value(const ggVector3& v_in,
                                const ggVector3& v_out,
                                const ggONB3& uvw) const {
   ggVector3 n=uvw.w();
   ggVector3 v=ggUnitVector(v_in);


   if (ggDot(n,v)>= 0.0) n=-uvw.w(); // to be sure about the n direction
   else n=uvw.w();

   v = ggReflection(v, n);
   double cs = ggDot(ggUnitVector(v_out), v);
   double cs_half = sqrt(0.5 * fabs(cs+1.0)); // fabs to avoid domain error
   double cs_theta = ggDot(v,n);
 
   return ggSpectrum( (phongN + 2.0) * pow(cs_half, phongN) / (8.0 * ggPi * (0.0000001+cs_theta)) );

}
// returns the value of the probability scattering function divided by cos theta


