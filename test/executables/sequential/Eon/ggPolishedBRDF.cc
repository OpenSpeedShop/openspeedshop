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



#include <ggPolishedBRDF.h>
#include <ggOptics.h>




ggSpectrum ggPolishedBRDF::averageValue()
const
{
   return R * 0.8; // arbitrary value
}


ggSpectrum ggPolishedBRDF::value(const ggVector3& v_in,
                                  const ggVector3&,
                                  const ggONB3& uvw)
const
{
   double Rs;
  
   ggVector3 v= -ggUnitVector(v_in); // vector comes from the substrate
   ggVector3 t= v;
   ggVector3 n=uvw.w();
   

   #ifdef GGSAFE
     if (ggDot(n, v) <= 0.0)  n = -n;
   #endif 

   if (ggRefract(n, 1.0, Nt[0], t)) //  t: refracted vector
         Rs = ggDielectricReflectance(v, t, n, 1.0, Nt[0]);
   else
         Rs = 1.0;
   return (1.0 - Rs) * R;

}

// returns the diffuse component of Polish
// the specular component is calculated in ggPolishedMaterial.C
