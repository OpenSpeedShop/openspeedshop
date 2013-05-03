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



#include <mrObjectRecord.h>
#include <math.h>


     void mrObjectRecord::AddMove(double dx, double dy, double dz) {
         ggHTranslationMatrix3  t( dx,  dy,  dz);
         ggHTranslationMatrix3 it(-dx, -dy, -dz);
         SetMatrix(t * toWorld(), toLocal() * it);
      }

      void mrObjectRecord::AddXRotation(double angle) {
          ggHXRotationMatrix3  r( angle);
          ggHXRotationMatrix3 ir(-angle);
          SetMatrix(r * toWorld(), toLocal() * ir);
       }

       void mrObjectRecord::AddYRotation(double angle) {
          ggHYRotationMatrix3  r( angle);
          ggHYRotationMatrix3 ir(-angle);
          SetMatrix(r * toWorld(), toLocal() * ir);
       }

       void mrObjectRecord::AddZRotation(double angle) {
           ggHZRotationMatrix3  r( angle);
           ggHZRotationMatrix3 ir(-angle);
           SetMatrix(r * toWorld(), toLocal() * ir);
        }

        void mrObjectRecord::AddBasisToCanonicalRotation(const ggONB3& uvw) {
            ggHBasisToCanonicalRotationMatrix3 Mto(uvw);
            ggHCanonicalToBasisRotationMatrix3 Mfrom(uvw);
            SetMatrix(Mto * toWorld(), toLocal() * Mfrom);
        }

        void mrObjectRecord::AddCanonicalToBasisRotation(const ggONB3& uvw) {
            ggHCanonicalToBasisRotationMatrix3 Mto(uvw);
            ggHBasisToCanonicalRotationMatrix3 Mfrom(uvw);
            SetMatrix(Mto * toWorld(), toLocal() * Mfrom);
        }

        void mrObjectRecord::AddRotation(const ggVector3& axis, double angle) {
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

        void mrObjectRecord::Reset() {
                 ggHRigidBodyMatrix3  m;
                 SetMatrix(m, m);  // sets to Identity
        }


void mrObjectRecord::SetMatrix(const ggHRigidBodyMatrix3& mat,
                           const ggHRigidBodyMatrix3& inv_mat) {
            toworld = mat;
            tolocal = inv_mat;

}
