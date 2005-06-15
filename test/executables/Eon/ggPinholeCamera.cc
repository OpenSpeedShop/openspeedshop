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



#include <math.h>
#include <ggPinholeCamera.h>
#include <ggHAffineMatrix3.h>
#include <ggHScaleMatrix3.h>
#include <ggHRotationMatrix3.h>
#include <ggHTranslationMatrix3.h>


// x, y are in [0,nx] x [0,ny]
ggRay3 ggPinholeCamera::getRay(const ggPoint2& on_screen,
                               double , const ggPoint2& )  const
// In case of a pin-hole camera, the transformation from point at 
// the screen to the point in the general coordinate system is linear. 
// So, we use precomputed matrix 'trans'. About computing this matrix 
// see constructor 'ggPinholeCamera::ggPinholeCamera'
{
     ggPoint3 p(on_screen.x(), on_screen.y(), 0.0);

// p is actually on the film plane-- we calculate the direction from
// p to the pinhole, but let the ray start at the pinhole in that
// direction
     return ggRay3(pinhole, pinhole - trans*p);
}



ggPinholeCamera::ggPinholeCamera(int numx, int numy,
                                const ggPoint3& lookfrom,
                                const ggPoint3& lookat,
                                const ggVector3& vup,
                                const double vertical_fov) {
   
    nx = numx;
    ny = numy;
    pinhole = lookfrom;

// Now, we construct an Ortho Normal Basis 'uvw' so that its origin is in 
// 'lookfrom' point, Z-axis is directed to 'lookat' point, Y-axis is parallel 
// to 'vup'. XYZ are called UVW here. 

// What we need are
//   M will be M3 M2 M1:
//   M1 translate (-nx/2, -ny/2, -d)  (d = 1)
//   M2 scale by (+h/ny, h/ny, 1)
//   M3 coordinate frame transform
 
    ggONB3 uvw;
    uvw.InitFromWV(lookat - lookfrom, vup);

    ggHTranslationMatrix3 M1(-0.5 * double(nx), -0.5 * double(ny), -1.0);

// scale the screen so it subtends fov radians as seen from the
// origin in the yz plane.
  
   double h = 2 * tan( vertical_fov / 2.0 );
   ggHScaleMatrix3 M2(h/double(ny), -h/double(ny), 1);

// change frames from canonical to line up with viewing params

    ggHFrameToCanonicalMatrix3 M3(ggFrame3(lookfrom, uvw));

    trans = M3 * M2 * M1;
    
}
