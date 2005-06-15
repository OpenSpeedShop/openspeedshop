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
#include <ggRotatingPinholeCamera.h>
#include <ggHAffineMatrix3.h>
#include <ggHScaleMatrix3.h>
#include <ggHRotationMatrix3.h>
#include <ggHTranslationMatrix3.h>


// x, y are in [0,nx] x [0,ny]
ggRay3 ggRotatingPinholeCamera::getRay(const ggPoint2& on_screen,
                               double time , const ggPoint2& )  const
// In case of a pin-hole camera, the transformation from point at 
// the screen to the point in the general coordinate system is linear. 
// So, we use precomputed matrix 'trans'. About computing this matrix 
// see constructor 'ggRotatingPinholeCamera::ggRotatingPinholeCamera'
{
     ggPoint3 p(on_screen.x(), on_screen.y(), 0.0);

     ggHZRotationMatrix3 rz(thetaAtTime(time));

     return ggRay3(pinhole, pinhole - frameMat * rz * stMat * p);
}



ggRotatingPinholeCamera::ggRotatingPinholeCamera(int numx,
                                              int numy,
                                const ggPoint3& lookfrom,
                                const ggPoint3& lookat,
                                const ggVector3& vup,
                                const double vertical_fov,
                                double t1, double t2,
                                double th1, double th2
                                ) {
// The following assigments are neccessary by definition of the approriate fields.
// They should not be modified later.
   
    nx = numx;
    ny = numy;
    pinhole = lookfrom;
    theta1 = th1;
    theta2 = th2;
    time1 = t1;
    time2 = t2;

// Now, we construct an Ortho Normal Basis 'uvw' so that its origin is in 
// 'lookfrom' point, Z-axis is directed to 'lookat' point, Y-axis is parallel 
// to 'vup'. XYZ are called UVW here. 

// What we need are
//   1) to direct the basis
//   2) to flip X coordinates
//   3) to put the center of the screen in the origin
 


    ggONB3 uvw;
    uvw.InitFromWV(lookat - lookfrom, vup);

// We need to inverse the image in X and Y direction, which follows from 
// the geometry of a pin-hole. Because the Y-axis is directed from top to bottom, 
// we don't to flip Y-coordinates. So, a matrix to flip X-coordinates is created as
// a rotation matrix with the angle of rotation equal Pi.

    ggHXRotationMatrix3 flip(ggPi);

// get the nx by ny "screen" centered on the z axis, and moved to 
// z = -1

    ggHTranslationMatrix3 t(-0.5 * double(nx), 0.5 * double(ny), -1.0);

// scale the screen so it subtends fov radians as seen from the
// origin in the yz plane.
  
   double film_height = 2 * tan( vertical_fov / 2.0 );
   ggHScaleMatrix3 s(film_height/double(ny), film_height/double(ny), 1);

// change frames from canonical to line up with viewing params

    ggHFrameToCanonicalMatrix3 frame(ggFrame3(lookfrom, uvw));

    stMat = s * t * flip;
    frameMat = frame;
    
}
