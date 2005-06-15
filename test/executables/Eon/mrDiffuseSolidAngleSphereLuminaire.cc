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



#include <mrDiffuseSolidAngleSphereLuminaire.h>
#include <math.h>



// Selects a point visible from x given a uv-pair.  Sometimes
// returning a non-visible point is allowed, but not desirable.
// Here, visible means not SELF-shadowed.
ggBoolean mrDiffuseSolidAngleSphereLuminaire::selectVisiblePoint(
             const ggPoint3& x,  // viewpoint
             const ggVector3&,  // normal at x
             const ggPoint2& uv,  // input coordinate for warping
             const double, // time (ignored)
             ggPoint3& on_light,  // point corresponding to uv
             double& invProb ) const  // probability if uv is random in [0,1]^2
{
     ggVector3 psi, Nprime;
     ggONB3 UVW;
     ggRay3 ray;
     double t, d, theta, phi, tolerance = 0.00000001;

     d = ggDistance(x,center());
     // Check if x is on the luminaire.
     if(radius() + tolerance >= d) return ggFalse;

     // Construct the local coordinate system UVW where the x is at
     // the origin and the sphere is at (0,0,d) in UVW.
     UVW.InitFromW(center() - x);

     // Find the direction psi in the world coordinate system.
     theta = acos(1 - uv.u() + uv.u()*sqrt(d*d - radius()*radius())/d);
     phi = 2 * ggPi * uv.v();
     psi = sin(theta)*cos(phi)*UVW.u() + sin(theta)*sin(phi)*UVW.v()  
			               + cos(theta)*UVW.w();
     // Find on_light.
     ray = ggRay3(x,psi);
     ggRaySphereIntersect(ray, sphere, ggEpsilon, ggInfinity, t);
     on_light = ray.pointAtParameter(t);

     // Find invProb.
     invProb = (2 * ggPi * (1 - sqrt(d*d - radius()*radius())/d));		// prob of psi.
     Nprime = ggUnitVector(on_light - center());
     psi = -psi;
     invProb *=  ggSquaredDistance(x,on_light) /( ggDot(Nprime,psi) + ggEpsilon);
  	// prob of on_light.
     return ggTrue;
}

