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



#include <mrDiffuseCosineZCylinderLuminaire.h>


ggBoolean mrDiffuseCosineZCylinderLuminaire::selectVisiblePoint(
             const ggPoint3& vp,  // viewpoint
             const ggVector3&,  // normal at x
             const ggPoint2& uv,  // input coordinate for warping
             const double, // time (ignored)
             ggPoint3& onLight,  // point corresponding to uv
             double& invProb ) const  //  one over probability
                                      // if uv is random in [0,1]^2
{

   double z = uv.u() * h;
   ggONB3 UVW;
   
   double dsquared = ggSquaredDistance(vp, ggPoint3(0,0,vp.z()));
   double d = sqrt(dsquared);
   double rsquared = r*r;
   if(dsquared < rsquared)
     return ggFalse;
   
   UVW.InitFromUW(ggVector3(vp.x(),vp.y(),0),ggVector3(0,0,1));
   double sintheta = uv.v() * sqrt(1 - rsquared/dsquared);
   double costheta = sqrt(1 - sintheta*sintheta);

   onLight = ggOrigin + (r*costheta*UVW.u() +
			 r*sintheta*UVW.v() +
				  z * UVW.w());

   invProb = (2*r*h*sqrt(1 - rsquared/dsquared))/costheta;
   return ggTrue;
}


