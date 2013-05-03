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



#include <mrInstance.h>
#include <math.h>

// bbox is screwed for moving objects

mrInstance::mrInstance(
                  mrSurface *ob_ptr,
                  const ggHRigidBodyMatrix3& mat,
                  const ggHRigidBodyMatrix3& inv_mat
                      ) {
            ptr = ob_ptr;
            SetMatrix(mat, inv_mat);
}

void mrInstance::SetMatrix(const ggHRigidBodyMatrix3& mat,
                           const ggHRigidBodyMatrix3& inv_mat) {
            toworld = mat;
            tolocal = inv_mat;
            ggBox3 box;
            if (ptr->boundingBox(0, 0, box))
            {
         	ggPoint3 p[2][2][2];

         	int i, j, k;

         	for (i = 0; i < 2; i++)
            	for (j = 0; j < 2; j++)
               	for (k = 0; k < 2; k++) {
                  if (i == 0)
                      	p[i][j][k].x() = box.min().x();
                  else
                      	p[i][j][k].x() = box.max().x();
                  if (j == 0)
                      	p[i][j][k].y() = box.min().y();
                  else
                      	p[i][j][k].y() = box.max().y();
                  if (k == 0)
                      	p[i][j][k].z() = box.min().z();
                  else
                      	p[i][j][k].z() = box.max().z();

                  p[i][j][k] = toworld * p[i][j][k];
                }

         ggPoint3 min  = p[0][0][0];
         ggPoint3 max  = p[0][0][0];

         for (i = 0; i < 2; i++)
            for (j = 0; j < 2; j++)
               for (k = 0; k < 2; k++) {
                    if (p[i][j][k].x() < min.x()) min.x() = p[i][j][k].x();
                    if (p[i][j][k].x() > max.x()) max.x() = p[i][j][k].x();
                    if (p[i][j][k].y() < min.y()) min.y() = p[i][j][k].y();
                    if (p[i][j][k].y() > max.y()) max.y() = p[i][j][k].y();
                    if (p[i][j][k].z() < min.z()) min.z() = p[i][j][k].z();
                    if (p[i][j][k].z() > max.z()) max.z() = p[i][j][k].z();
               }

         pMin = min;
         pMax = max;
   }
   else {
         pMin.Set(-1.0e6, -1.0e6, -1.0e6);
         pMax.Set( 1.0e6,  1.0e6,  1.0e6);
   }
        
 }


ggBoolean mrInstance::shadowHit(        
             const ggRay3& ray,
             double time,
             double tmin,
             double tmax,
             double& t,
             ggVector3& N,
             ggBoolean& emits,
             ggSpectrum& E ) const 
{
   if (ptr->shadowHit(tolocal * ray, time, tmin, tmax, t, N, emits, E))
   {
       N = toworld * N;
       return ggTrue;
   }
   else
       return ggFalse;
}

ggBoolean mrInstance::viewingHit(        
             const ggRay3& ray,
             double time,
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord& MR
             ) const
{
   if (ptr->viewingHit(tolocal * ray, time, tmin, tmax, VHR, MR))
   {
       VHR.p = toworld * VHR.p;
	   if (MR.hasRay1)
		   MR.ray1 = toworld * MR.ray1;
	   if (MR.hasRay2)
		   MR.ray2 = toworld * MR.ray2;
       if(VHR.hasUVW) VHR.UVW = toworld * VHR.UVW;
       return ggTrue;
   }
   else
       return ggFalse;
}

ggBoolean mrInstance::boundingBox(
             double , // low end of time range
             double , // low end of time range
             ggBox3& box) const
{
   box.min() = pMin;
   box.max() = pMax;
   return ggTrue;
 }

ggBoolean mrInstance::selectVisiblePoint(
             const ggPoint3& x,
             const ggVector3& N,
             const ggPoint2& uv,
             const double time,
             ggPoint3& onObject,
             double& prob ) const
{
   if (ptr->selectVisiblePoint(tolocal * x, tolocal * N, uv, time, onObject, prob))
   {
       onObject = toworld * onObject;
       return ggTrue;
   }
   else
       return ggFalse;
 }
