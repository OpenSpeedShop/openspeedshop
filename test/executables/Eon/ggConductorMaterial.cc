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



#include <ggConductorMaterial.h>
#include <ggGeometry.h>
#include <ggOptics.h>
#include <ggRanNum.h>
#include <assert.h>



ggConductorMaterial::ggConductorMaterial(const ggSpectrum& c1,
                                         const ggSpectrum& c2, double e) {
    N = c1;
    k = c2;
    phongN = e;
    rhoPtr = 0;
}

ggBoolean ggConductorMaterial::getInfo(const ggRay3& r_in,
                                      const ggPoint3& p,
                                      const ggONB3& uvw,  
                                      const double& , // cov  
                                      ggMaterialRecord& mat) const {

    #ifdef GGSAFE
         ggVector3 r = r_in.direction();
         assert(ggEqual(ggDot(r,r),1.0,1e-10));
    #endif

    ggVector3 an = uvw.w();
    ggVector3 rv,rn;
    ggSpectrum S = mat.kBRDF;

    mat.BRDFPointer = rhoPtr;
    mat.CORLRay1 = (rhoPtr != 0);

    rv = ggReflection(r_in.direction(), an);

    //  generate one reflected ray 

    if (phongN < 10000.0) { //  perturb randomly
       rv = ggPhongVector(rv, mat.UV, phongN);
#if defined ORIGINAL
       mat.UV.Set(ggCanonicalRandom(), ggCanonicalRandom());
#else
       double t = ggCanonicalRandom();
       mat.UV.Set(t, ggCanonicalRandom());
#endif
    }

    mat.ray1.Set(p, rv);

        mat.hasRay1 = ggTrue;
	rn = r_in.direction();
	for (int i = 0; i < mat.kRay1.nComponents(); i++)
        {
         //attenuation determined by Fresnel Equations

	 mat.kRay1[i] = S[i] * ggVacuumConductorReflectance(r_in.direction(), an,
                          N[i], k[i]);
        }
        mat.kBRDF *= mat.kRay1;

    mat.hasRay2 = ggFalse; 

    return ggTrue;
}

