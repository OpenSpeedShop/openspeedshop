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
#include <assert.h>
#include <ggThinLensCamera.h>
#include <ggHAffineMatrix3.h>
#include <ggHScaleMatrix3.h>
#include <ggHRotationMatrix3.h>
#include <ggHTranslationMatrix3.h>
#include <ggGeometry.h>


// x, y are in [0,nx] x [0,ny]
ggRay3 ggThinLensCamera::getRay(const ggPoint2& on_screen,
          double , const ggPoint2& uv )  const
{
     ggPoint2 xy = ggPointOnDisk(halfA, uv);
     ggPoint3 onLens(xy.x(), xy.y(), 0.0);

     double x = on_screen.x() + dx;
     double y = on_screen.y() + dy;

     ggPoint3 focus(-x *  f_over_d_minus_f_sx,
                    -y *  f_over_d_minus_f_sy,
                     f_over_d_minus_f_d);

     ggRay3 r(onLens, focus - onLens );

     return (trans * r);
}


ggThinLensCamera::ggThinLensCamera(int numx, int numy,
                                const ggPoint3& lookfrom,
                                const ggPoint3& lookat,
                                const ggVector3& vup,
                                const double vertical_fov,  // radians
                                const double A,
                                const double focal,
                                const double focus
                                ) {
    nx = numx;
    ny = numy;
    lensCenter = lookfrom;
    halfA = 0.5*A;
    f = focal;
    D = focus;
    assert (f < D);
    d = D*f / (D - f);
    f_over_d_minus_f = f / (d - f);
    
    dx = -0.5 * double(nx);
    dy = -0.5 * double(ny);
    sy = - 2.0 * d * tan(vertical_fov / 2.0) / double(ny);
    sx = -sy;
    f_over_d_minus_f_sx = f_over_d_minus_f * sx;
    f_over_d_minus_f_sy = f_over_d_minus_f * sy;
    f_over_d_minus_f_d = f_over_d_minus_f * d;


//  construct an ONB  that lines up with viewing coords.

    ggONB3 uvw;
    uvw.InitFromWV(lookat - lookfrom, vup);


// change frames from canonical to line up with viewing params

    ggHFrameToCanonicalMatrix3 frame(ggFrame3(lookfrom, uvw));

    trans = frame;
    
}
