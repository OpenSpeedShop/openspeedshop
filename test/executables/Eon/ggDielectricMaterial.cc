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



#include <ggDielectricMaterial.h>
#include <ggGeometry.h>
#include <ggOptics.h>
#include <ggRanNum.h>
#include <assert.h>


ggDielectricMaterial::ggDielectricMaterial(const ggSpectrum& c1,
                                           const double& c2)
{
    #ifdef GGSAFE
        assert(c1[0] >= 1.0);
    #endif

    Nt = c1;
    phongN = c2;
    rhoPtr = 0;
}

ggBoolean ggDielectricMaterial::getInfo(const ggRay3& r_in,
                                        const ggPoint3& p,
                                        const ggONB3& uvw,  
                                        const double& ,  
                                        ggMaterialRecord& mat) const 
{

    
    #ifdef GGSAFE
         ggVector3 r=r_in.direction();
         assert(ggEqual(ggDot(r,r),1.0,1E-10));        
    #endif	

    double Rs;
    ggVector3 refracted_v,v_in;
    ggVector3 an;
    ggVector3 n = uvw.w();


    refracted_v = r_in.direction();
    v_in = refracted_v;
    if (ggDot(n, v_in) >= 0.0) { // ray is in material  
        an = -n;
        ggVector3 rv=ggReflection(r_in.direction(), an);
        if (ggDielectricVacuumRefract(an, Nt[0], refracted_v)) {
             //  generate one reflected ray and one refracted ray 
             mat.hasRay1 = ggTrue;
             mat.hasRay2 = ggTrue;
             Rs = ggDielectricVacuumReflectance(v_in, refracted_v, an, Nt[0]);
             if (phongN < 10000.0) { //  perturb randomly 
               rv = ggPhongVector(rv, mat.UV, phongN);
               refracted_v = ggPhongVector(rv, mat.UV, phongN);
	       //explicitly order the calls to ggCanonicalRandom
#if defined ORIGINAL
               mat.UV.Set(ggCanonicalRandom(),ggCanonicalRandom());
#else
	       double t = ggCanonicalRandom();
               mat.UV.Set(t,ggCanonicalRandom());
#endif
             }
             mat.ray1.Set(p, rv);
             mat.ray2.Set(p, refracted_v);
             mat.kRay1.Set(Rs);
             mat.kRay2.Set(1-Rs);
        }
        else { //  only a reflected ray
            if (phongN < 10000.0) { // perturb randomly
              rv = ggPhongVector(rv, mat.UV, phongN);
#if defined ORIGINAL
               mat.UV.Set(ggCanonicalRandom(),ggCanonicalRandom());
#else
	       double t = ggCanonicalRandom();
               mat.UV.Set(t,ggCanonicalRandom());
#endif
             }
     
             mat.ray1.Set(p, rv);
             mat.kRay1.Set(1.0);
     
             mat.hasRay1 = ggTrue;
             mat.hasRay2 = ggFalse;
         }
     }
     else {  // ray is coming from air
         an = n;
         ggVector3 rv=ggReflection(r_in.direction(), an);
        if (ggVacuumDielectricRefract(an, Nt[0], refracted_v)) {
             //  generate one reflected ray and one refracted ray 
             mat.hasRay1 = ggTrue;
             mat.hasRay2 = ggTrue;
             Rs = ggVacuumDielectricReflectance(v_in, refracted_v, an, Nt[0]);
             if (phongN < 10000.0) { //  perturb randomly 
               rv = ggPhongVector(rv, mat.UV, phongN);
               refracted_v = ggPhongVector(rv, mat.UV, phongN);
	       //explicitly order the calls to ggCanonicalRandom
#if defined ORIGINAL
               mat.UV.Set(ggCanonicalRandom(),ggCanonicalRandom());
#else
	       double t = ggCanonicalRandom();
               mat.UV.Set(t,ggCanonicalRandom());
#endif
             }
             mat.ray1.Set(p, rv);
             mat.ray2.Set(p, refracted_v);
             mat.kRay1.Set(Rs);
             mat.kRay2.Set(1-Rs);
        }
        else { //  only a reflected ray
            if (phongN < 10000.0) { // perturb randomly
              rv = ggPhongVector(rv, mat.UV, phongN);
#if defined ORIGINAL
               mat.UV.Set(ggCanonicalRandom(),ggCanonicalRandom());
#else
	       double t = ggCanonicalRandom();
               mat.UV.Set(t,ggCanonicalRandom());
#endif
             }
     
             mat.ray1.Set(p, rv);
             mat.kRay1.Set(1.0);
     
             mat.hasRay1 = ggTrue;
             mat.hasRay2 = ggFalse;
         }
     }


    mat.CORLRay1 = mat.CORLRay2 = (rhoPtr != 0);
    mat.BRDFPointer = rhoPtr;

    return ggTrue;
}


