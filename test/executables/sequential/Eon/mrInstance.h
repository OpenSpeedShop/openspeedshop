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



#ifndef MRINSTANCE_H
#define MRINSTANCE_H


#include <mrSurface.h>
#include <ggHRigidBodyMatrix3.h>
#include <ggHTranslationMatrix3.h>
#include <ggHRotationMatrix3.h>

class mrInstance : public mrSurface {
public:


  virtual ggBoolean shadowHit(        
             const ggRay3& r,    // ray being sent
             double time,     // time ray is sent
             double tmin, // minimum hit parameter to be searched for
             double tmax, // maximum hit parameter to be searched for
             double& t,
             ggVector3& N,  
             ggBoolean& emits,
             ggSpectrum& kEmit
             ) const;

  virtual ggBoolean viewingHit(        
             const ggRay3& r,    // ray being sent
             double time,     // time ray is sent
             double tmin, // minimum hit parameter to be searched for
             double tmax, // maximum hit parameter to be searched for
             mrViewingHitRecord&,
             ggMaterialRecord& rec
             ) const;

  virtual ggBoolean boundingBox(
             double time1, // low end of time range
             double time2, // low end of time range
             ggBox3& bbox)
             const;

     // Selects a point visible from x given a uv-pair.  Sometimes
     // returning a non-visible point is allowed, but not desirable.
     // Here, visible means not SELF-sahdowed.
     virtual ggBoolean selectVisiblePoint(
             const ggPoint3& x,   // viewpoint
             const ggVector3& Nx, // unit vector at x
             const ggPoint2& uv,   // input coordinate
             const double time,// time of query
             ggPoint3& on_light,  // point corresponding to uv
             double& prob)     // probability of selecting on_light
             const;
                               // assuming random uv
     mrInstance(
                  mrSurface *ob_ptr,
                  const ggHRigidBodyMatrix3& mat,
                  const ggHRigidBodyMatrix3& inv_mat
                 );

     const ggHRigidBodyMatrix3& toWorld() const { return toworld; }
     const ggHRigidBodyMatrix3& toLocal() const { return tolocal; }
     void SetMatrix(const ggHRigidBodyMatrix3& mat, 
    const ggHRigidBodyMatrix3& inv_mat);

     mrSurface * localSurface() { return ptr; }
     ggBoolean identityTransform() const { return tolocal.isIdentity();}

   void AddMove(double dx, double dy, double dz) {
      ggHTranslationMatrix3  t( dx,  dy,  dz);
      ggHTranslationMatrix3 it(-dx, -dy, -dz);
      SetMatrix(t * toWorld(), toLocal() * it);
   }

   void AddXRotation(double angle) {
       ggHXRotationMatrix3  r( angle);
       ggHXRotationMatrix3 ir(-angle);
       SetMatrix(r * toWorld(), toLocal() * ir);
    }

    void AddYRotation(double angle) {
       ggHYRotationMatrix3  r( angle);
       ggHYRotationMatrix3 ir(-angle);
       SetMatrix(r * toWorld(), toLocal() * ir);
    }

    void AddZRotation(double angle) {
       ggHZRotationMatrix3  r( angle);
       ggHZRotationMatrix3 ir(-angle);
       SetMatrix(r * toWorld(), toLocal() * ir);
    }

    void AddRotation(const ggVector3& axis, double angle) {
        ggONB3 uvw;
        uvw.InitFromW(axis);
        ggHCanonicalToBasisRotationMatrix3 Mto(uvw);
        ggHBasisToCanonicalRotationMatrix3 Mfrom(uvw);
        ggHZRotationMatrix3 Mr(angle);
        ggHZRotationMatrix3 Mri(-angle);
		ggHRotationMatrix3 r = Mfrom * Mr * Mto;
		ggHRotationMatrix3 ir = Mto * Mri * Mfrom;
		SetMatrix(r * toWorld(), toLocal() * ir);
    }

	void Reset() {
		 ggHRigidBodyMatrix3  m;
		 SetMatrix(m, m);  // sets to Identity
	}

  protected:
     mrInstance() { }
     ggHRigidBodyMatrix3 toworld, tolocal;
     mrSurface *ptr;
     ggPoint3 pMin, pMax;
};


#endif



