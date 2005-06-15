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



#include <ggDiffuseMaterial.h>
#include <ggGeometry.h>
#include <ggDiffuseBRDF.h>
#include <ggRanNum.h>
#include <ggRanNum.h>
#include <kai.h>

ggBoolean ggDiffuseMaterial::print(ostream& s) const {
  s << "ggDiffuseMaterial. " << PTR_TO_INTEGRAL(this) << 
        " R[400, 700] = [" << R[0] << "," <<
      "]. brdfPtr = " << rhoPtr << "\n";
  return ggTrue;
}



ggDiffuseMaterial::ggDiffuseMaterial(const ggSpectrum& c, ggBoolean hasBrdf)
{
    R = c;
    if (hasBrdf)
       rhoPtr = new ggDiffuseBRDF(R);
    else
       rhoPtr = 0;
}

ggBoolean ggDiffuseMaterial::getInfo( const ggRay3& ray,  // ray in
                                      const ggPoint3& p,
                                      const ggONB3& uvw,  
                                      const double& , // cov  
                                      ggMaterialRecord& mat) const
{


//   generate a reflected ray randomly with a cosine distrubution

     if (ggDot(ray.direction(), uvw.w()) > ggEpsilon) {
        ggONB3 myuvw = uvw;
        myuvw.FlipW();
        mat.ray1.Set(p, ggDiffuseVector(myuvw, mat.UV));  
     }
     else
        mat.ray1.Set(p, ggDiffuseVector(uvw, mat.UV));  

     // Explicitly order the calls to ggCanonicalRandom().
     // Without this ordering, the results on iA and HPPA diverge.
#if defined ORIGINAL
     mat.UV.Set(ggCanonicalRandom(), ggCanonicalRandom());
#else
     double t1=ggCanonicalRandom();
     mat.UV.Set(t1, ggCanonicalRandom());
#endif
     mat.kRay1 = R;

     mat.BRDFPointer = rhoPtr;

     mat.hasRay1 = ggTrue;
     mat.hasRay2 = ggFalse;     
     mat.CORLRay1 = (rhoPtr != 0);
     mat.CORLRay2 = ggFalse;

     return ggTrue;
}

