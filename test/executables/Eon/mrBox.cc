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



#include <mrBox.h>
#include <ggGeometry.h>
#include <ggHRotationMatrix3.h>
#include <math.h>
#include <kai.h>

ggBoolean  mrBox::print(ostream& os ) const {
    os << "mrBox " << PTR_TO_INTEGRAL(this) << box << "\n";
    return ggTrue;
}

ggBoolean mrBox::boundingBox( double, // low end of time range
                                 double, // low end of time range
                                 ggBox3& bbox ) const {
    ggVector3 v(ggBigEpsilon, ggBigEpsilon, ggBigEpsilon);
    bbox =  ggBox3(box.min() - v, box.max() + v);
    return ggTrue;
}

ggBoolean mrBox::shadowHit( const ggRay3& ray,
			    double,
			    double tmin,
			    double tmax,
			    double& t,
			    ggVector3& N,
			    ggBoolean& emits,
			    ggSpectrum& ) const 
{

  ggPoint3 p;
  ggONB3 uvw;

  if ( ggRayBoxIntersect(ray, box, tmin, tmax, uvw, p, t)) {
      N = uvw.w();
      emits = ggFalse;
      return ggTrue;
    }
  else
    return ggFalse; 
}

ggBoolean mrBox::getParameters( const ggPoint3& p,
				ggPoint2& uv,
				ggONB3& uvw ) const
{
  double U = .5, V = .5;

  if ( ggEqual(p.x(), box.min().x()) ) {
    uvw = ggONB3(ggYAxis, -ggZAxis, -ggXAxis);
    U = (box.max().z() - p.z()) * inverseRangeZ;
    V = (box.max().y() - p.y()) * inverseRangeY;
  }
  else if ( ggEqual(p.x(), box.max().x()) ) {
    uvw = ggONB3(ggYAxis, ggZAxis, ggXAxis);
    U = (p.z() - box.min().z()) * inverseRangeZ;
    V = (box.max().y() - p.y()) * inverseRangeY;
  }
  else if ( ggEqual(p.y(), box.min().y()) ) {
    uvw = ggONB3(ggZAxis, -ggXAxis, -ggYAxis);
    U = (p.x() - box.min().x()) * inverseRangeX;
    V = (p.z() - box.min().z()) * inverseRangeZ;
  }
  else if ( ggEqual(p.y(), box.max().y()) ) { 
    uvw = ggONB3(ggZAxis, ggXAxis, ggYAxis);
    U = (box.max().x() - p.x()) * inverseRangeX;
    V = (box.max().z() - p.z()) * inverseRangeZ;
  }
  else if ( ggEqual(p.z(), box.min().z()) ) {
    uvw = ggONB3(ggXAxis, -ggYAxis, -ggZAxis);
    U = (p.x() - box.min().x()) * inverseRangeX;
    V = (p.y() - box.min().y()) * inverseRangeY;
  }
  else {
#ifdef GGSAFE
   assert( ggEqual(p.z(), box.max().z()) );
#endif
    uvw = ggONB3(ggXAxis, ggYAxis, ggZAxis);
    U = (box.max().x() - p.x()) * inverseRangeX;
    V = (box.max().y() - p.y()) * inverseRangeY;
  }

#ifdef GGSAFE
  assert( (U >= 0.0) && (U <= 1.0) );
  assert( (V >= 0.0) && (V <= 1.0) );
#endif

  uv.Set(U, V);
  return ggTrue;
}

ggBoolean mrBox::viewingHit( const ggRay3& ray,
			     double,
			     double tmin,
			     double tmax,
			     mrViewingHitRecord& VHR,
			     ggMaterialRecord& ) const
{
  if ( ggRayBoxIntersect(ray, box, tmin, tmax, VHR.UVW, VHR.p, VHR.t)) {
      VHR.hasUVW = ggTrue;
      VHR.hasUV = ggTrue;
      return getParameters(VHR.p, VHR.UV, VHR.UVW);
    }
  else
    return ggFalse;
}


