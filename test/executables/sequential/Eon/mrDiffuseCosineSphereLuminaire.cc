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



#include <mrDiffuseCosineSphereLuminaire.h>
#include <ggConstants.h>
#include <math.h>


ggBoolean mrDiffuseCosineSphereLuminaire::selectVisiblePoint(
             const ggPoint3& x,  // viewpoint
             const ggVector3&,  // normal at x
             const ggPoint2& uv,  // input coordinate for warping
             const double, // time (ignored)
             ggPoint3& on_light,  // point corresponding to uv
             double& invProb ) const  // probability if uv is random in [0,1]^2
{
     ggVector3 Nprime;
     ggONB3 UVW;
     double d, dsquared, rsquared, frac;
     double phi, costheta;

     dsquared = ggSquaredDistance(x,center());
     d = sqrt(dsquared);
     rsquared = radius()*radius();
     frac = rsquared/dsquared;

     // Check if x is on the luminaire.
     if(radius() + ggEpsilon >= d) return ggFalse;

     // Construct the local coordinate system UVW where the sphere center
     // is at the origin and x is at (0,0,d) in UVW.
     UVW.InitFromW(x - center());

     
     // Find the point on_light
     costheta = sqrt(1 - uv.u() + uv.u() * frac);
     double sintheta = sqrt(1-costheta*costheta);
     phi = ggTwoPi * uv.v();
     
     on_light = center() + 
                 ggVector3(radius()*sintheta*cos(phi) * UVW.u()  +
		 radius()*sintheta*sin(phi) * UVW.v()  +
		 radius()*costheta * UVW.w());

     // Find prob.
     invProb = ( ggPi * rsquared * ( 1 - frac)) / costheta;
     return ggTrue;
}
