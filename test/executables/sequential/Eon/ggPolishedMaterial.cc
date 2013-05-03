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



#include <ggPolishedMaterial.h>
#include <ggDiffuseBRDF.h>
#include <ggGeometry.h>
#include <ggOptics.h>
#include <ggRanNum.h>
#include <assert.h>

ggPolishedMaterial::ggPolishedMaterial(const ggSpectrum& ind,
                      const ggSpectrum& ref,
                      double exp)
 
{ 
   #ifdef GGSAFE
         assert(ind[0] >= 1.0);
   #endif
	
   Nt = ind; R = ref; phongN = exp;
   rhoPtr = new ggDiffuseBRDF(R);
}



ggBoolean ggPolishedMaterial::getInfo(const ggRay3& r_in,
                                      const ggPoint3& p,
                                      const ggONB3& uvw,  
                                      const double& cov,  
                                      ggMaterialRecord& mat) const
{

    double R_s;
    ggVector3 refracted_v;
    ggVector3 v_in;
    ggVector3 an;
    ggVector3 n = uvw.w();
    ggONB3 myuvw = uvw;


    #ifdef GGSAFE
        ggVector3 r=r_in.direction();
	assert(ggEqual(ggDot(r,r),1.0,1E-10));	
    #endif

    refracted_v = r_in.direction();
    v_in = refracted_v;
    if (ggDot(n, v_in) >= 0.0) {
        an = -n;
        myuvw.FlipW();
     }
     else {
        an = n;
     }

    if (ggVacuumDielectricRefract(an, Nt[0], refracted_v))
         R_s = ggVacuumDielectricReflectance(v_in, refracted_v, an, Nt[0]);
    else
         R_s = 1.0;



    double k = R_s * cov;
    mat.kRay1.Set(k);

    ggVector3 rv = ggReflection(r_in.direction(), an);


    // generate one reflected ray from polish

    if (phongN < 10000.0) { // perturb randomly
       rv = ggPhongVector(rv, mat.UV, phongN);
#if defined ORIGINAL
       mat.UV.Set(ggCanonicalRandom(),ggCanonicalRandom());
#else
       double t1=ggCanonicalRandom();
       mat.UV.Set(t1, ggCanonicalRandom());
#endif
    }

    mat.ray1.Set(p, rv);

    mat.kBRDF *= (1.0 - k);
    mat.BRDFPointer = rhoPtr;

    mat.hasRay1 = ggTrue;
    mat.CORLRay1 = ggFalse;

    //  the substrate

    if (k < 1) {
       // generate a reflected ray with a cosine distribution	
       
       mat.hasRay2 = ggTrue;
       mat.kRay2 = R * (1 - k);
       mat.CORLRay2 = ggTrue;

       ggVector3 v = ggDiffuseVector(myuvw, mat.UV);

       mat.ray2.Set(p, v);
#if defined ORIGINAL
       mat.UV.Set(ggCanonicalRandom(),ggCanonicalRandom());
#else
       double t1=ggCanonicalRandom();
       mat.UV.Set(t1, ggCanonicalRandom());
#endif
    }
    else
       mat.hasRay2 = ggFalse;

    return ggTrue;
}

