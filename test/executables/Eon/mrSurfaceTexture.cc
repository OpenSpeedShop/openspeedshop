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



#include <mrSurfaceTexture.h>
#include <assert.h>
#include <kai.h>


ggBoolean mrSurfaceTexture::print( ostream& s) const {
   s << "mrSurfaceTexture " << PTR_TO_INTEGRAL(this) << ", object = ";
   assert (objPtr != 0);
   assert (texPtr != 0);
   objPtr->print(s);
   return  ggTrue;
}
       
ggBoolean mrSurfaceTexture::shadowHit(        
             const ggRay3& ray,
             double time,
             double tmin,
             double tmax,
             double& t,
             ggVector3& N,
             ggBoolean& emits,
             ggSpectrum& E ) const {

   return (objPtr->shadowHit(ray, time, tmin, tmax, t, N, emits, E));
}

ggBoolean mrSurfaceTexture::viewingHit(        
             const ggRay3& ray,
             double time,
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord& MR
             ) const
{
   if (objPtr->viewingHit(ray, time, tmin, tmax, VHR, MR)) {
       ggPoint2 localUV(repeatX * VHR.UV.u(), repeatY * VHR.UV.v());
       localUV.Set(localUV.u() - int(localUV.u()),
                   localUV.v() - int(localUV.v()) );
       ggRGBFPixel ftex = texPtr->bilinearColor(localUV);
       ggSpectrum tex = rPrimary * ftex.r() +
                        gPrimary * ftex.g() +
                        bPrimary * ftex.b();
       MR.kBRDF *= tex;
       if (MR.hasRay1 && MR.CORLRay1)
           MR.kRay1 *= tex;
       if (MR.hasRay2 && MR.CORLRay2)
           MR.kRay2 *= tex;

       return ggTrue;
   }
   else
       return ggFalse;
}

ggBoolean mrSurfaceTexture::boundingBox(
             double t1, // low end of time range
             double t2, // low end of time range
             ggBox3& box) const
{
   return (objPtr->boundingBox(t1, t2, box));
 }


ggBoolean mrSurfaceTexture::selectVisiblePoint(
             const ggPoint3& x,
             const ggVector3& N,
             const ggPoint2& uv,
             const double time,
             ggPoint3& onObject,
             double& prob ) const
{
   return (objPtr->selectVisiblePoint(x, N, uv, time, onObject, prob));
 }
