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



#include <mrLinkedObjects.h>
#include <ggMacros.h>
#include <ggGeometry.h>
#include <math.h>

ggBoolean
mrLinkedObjects::print(ostream& s) const
{
  s << "mrLinkedObjects:" << endl;
  s << "Object1:" << endl << obj1->print(s) << endl;
  s << "Object2:" << endl << obj2->print(s) << endl;
  s << "End of mrLinkedObjects" << endl;

  return ggTrue;
}

ggBoolean
mrLinkedObjects::shadowHit(const ggRay3& ray,
			   double time,
			   double tmin,
			   double tmax,
			   double& t,
			   ggVector3& N,
			   ggBoolean& emits,
			   ggSpectrum& kEmit) const
{
  if(obj1->shadowHit(ray,time,tmin,tmax,t,N,emits,kEmit))
  {
    ggVector3 tN;
    ggBoolean temits;
    ggSpectrum tkEmit;
    double tt;
    
    if(obj2->shadowHit(ray,time,tmin,tmax,tt,tN,temits,tkEmit))
      return ggFalse;    
    return ggTrue;
  }
  else
     return ggFalse;
}

ggBoolean
mrLinkedObjects::viewingHit(const ggRay3& ray,
			    double time,
			    double tmin,
			    double tmax,
			    mrViewingHitRecord& VHR,
			    ggMaterialRecord& rec) const
{
  if(obj1->viewingHit(ray,time,tmin,tmax,VHR,rec))
  {
    mrViewingHitRecord TVHR;
    ggMaterialRecord trec;
    if(obj2->viewingHit(ray,time,tmin,tmax,TVHR,trec))
      return ggFalse;
    return ggTrue;
  }
  return ggFalse;
}

ggBoolean
mrLinkedObjects::boundingBox(double time1, double time2, ggBox3& bbox) const
{
  return obj1->boundingBox(time1, time2, bbox);
}









