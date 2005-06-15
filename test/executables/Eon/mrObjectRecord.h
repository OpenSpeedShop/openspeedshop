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



#ifndef MROBJECTRECORD_H
#define MROBJECTRECORD_H



#include <ggMaterial.h>
#include <ggDiffuseBRDF.h>
#include <gg.h>
#include <ggRasterSurfaceTexture.h>
#include <ggSolidTexture.h>
#include <mrSurfaceList.h>


class mrObjectRecord {
public:

     const ggHRigidBodyMatrix3& toWorld() const { return toworld; }
     const ggHRigidBodyMatrix3& toLocal() const { return tolocal; }
     void SetMatrix(const ggHRigidBodyMatrix3& mat, const ggHRigidBodyMatrix3& inv_mat);
     ggMaterial * material() { return matPtr; }
     ggBoolean identityTransform() const { return tolocal.isIdentity();}

     void AddMove(double dx, double dy, double dz);

     void AddXRotation(double angle);

     void AddYRotation(double angle);

     void AddZRotation(double angle);

     void AddRotation(const ggVector3& axis, double angle);

     void AddCanonicalToBasisRotation(const ggONB3& uvw);
     void AddBasisToCanonicalRotation(const ggONB3& uvw);

// sets transform to identity
     void Reset();

     void SetMaterial(ggMaterial *newMatPtr) { matPtr = newMatPtr; }

     void AddSurface(mrSurface *surfPtr) { surfaces.Add(surfPtr); }

     mrObjectRecord(ggMaterial  *newMatPtr) {
            surfaceTexturePtr = 0;
             solidTexturePtr = 0;
            matPtr = newMatPtr;
            emits = ggFalse;
       }
       int length() const { return surfaces.length(); }

       ggBoolean isLuminaire() const { return emits; }

       void SetLuminaire(ggBoolean flag = ggTrue) { emits = flag; }

       ggRasterSurfaceTexture *surfaceTexture() const { return surfaceTexturePtr; }

       ggSolidTexture *solidTexture() const { return solidTexturePtr; }

       void SetSolidTexture(ggSolidTexture *ptr)  {
            solidTexturePtr = ptr; }
                 
       void SetSurfaceTexture(ggRasterSurfaceTexture *ptr, int rx=1, int ry=1)  {
            surfaceTexturePtr = ptr; repeatX = rx; repeatY = ry; }
                 


     ggHRigidBodyMatrix3 toworld, tolocal;
     mrSurfaceList surfaces;
     ggMaterial *matPtr;
     ggBoolean emits;
     ggRasterSurfaceTexture *surfaceTexturePtr;
     ggSolidTexture *solidTexturePtr;
     int repeatX, repeatY;

protected:
     mrObjectRecord() { }
};


#endif
