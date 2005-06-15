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



#include <ggSpecularMaterial.h>
#include <ggGeometry.h>
#include <ggOptics.h>
#include <ggRanNum.h>
#include <assert.h>



ggSpecularMaterial::ggSpecularMaterial(const ggSpectrum& c1, double e) {
    R = c1;
    phongN = e;
    rhoPtr = 0;
}

ggBoolean ggSpecularMaterial::getInfo(const ggRay3& r_in,
                                      const ggPoint3& p,
                                      const ggONB3& uvw,  
                                      const double& , // cov  
                                      ggMaterialRecord& mat) const 
{

    ggVector3 an = uvw.w();
    ggVector3 rv,rn;

    mat.BRDFPointer = rhoPtr;
    mat.CORLRay1 = (rhoPtr != 0);

    rv = ggReflection(r_in.direction(), an);

    //  generate one reflected ray 

    if (phongN < 10000.0) {
       rv = ggPhongVector(rv, mat.UV, phongN);
#if defined ORIGINAL
       mat.UV.Set(ggCanonicalRandom(),ggCanonicalRandom());
#else
       double t1=ggCanonicalRandom();
       mat.UV.Set(t1, ggCanonicalRandom());
#endif
    }

    mat.ray1.Set(p, rv);

    if (ggDot(mat.ray1.direction(), an) <= 0.0)
        mat.hasRay1 = ggFalse; 
    else
    {
        mat.hasRay1 = ggTrue;
        mat.kRay1 = R;
    }

    mat.hasRay2 = ggFalse; 

    return ggTrue;
}

