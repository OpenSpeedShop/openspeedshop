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



#include <ggConstantMaterial.h>
#include <kai.h>


ggBoolean ggConstantMaterial::print(ostream& s) const
{
      s << "ggConstantMaterial. " << PTR_TO_INTEGRAL(this) <<
        " E[400, 700] = [" << E.component(0) << "," <<
                              E.component(20) << "\n";
  return ggTrue;

}



ggBoolean ggConstantMaterial::getInfo(const ggRay3& ,
                                      const double& ,  
                                      const ggSpectrum& ,
                                      const ggPoint3& ,
                                      const ggONB3& ,  
                                      ggMaterialRecord& mat) const 
{

     mat.new_uv = mat.uv;
     mat.has_brdf_ptr = ggFalse;
     mat.has_scat_ray1 = ggFalse;
     mat.has_scat_ray2 = ggFalse;
     mat.corl1=mat.corl2= ggFalse;
     mat.brdf_ptr=0;
     
     mat.emits = ggTrue;
     mat.k_emit = E;

     return ggTrue;
}


