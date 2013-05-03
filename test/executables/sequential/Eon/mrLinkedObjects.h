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



#ifndef MRLINKEDOBJECTS_H
#define MRLINKEDOBJECTS_H

#include <mrSurface.h>


class mrLinkedObjects : public mrSurface {
 public:

  virtual ggBoolean shadowHit(const ggRay3& ray,
			      double time,
			      double tmin,
			      double tmax,
			      double& t,
			      ggVector3& N,
			      ggBoolean& emits,
			      ggSpectrum& kEmit
			      ) const;

  virtual ggBoolean viewingHit(const ggRay3& r,
			       double time,
			       double tmin,
			       double tmax,
			       mrViewingHitRecord&,
			       ggMaterialRecord& rec
			       ) const;

  virtual ggBoolean boundingBox(double time1,
				double time2,
				ggBox3& bbox
				) const;

  virtual ggBoolean print(ostream& s) const;

  mrLinkedObjects(mrSurface *object1, 
		  mrSurface *object2 
		  ) {obj1 = object1; obj2 = object2;}

 protected:
  mrLinkedObjects() {}

  mrSurface *obj1, *obj2;
};

#endif
