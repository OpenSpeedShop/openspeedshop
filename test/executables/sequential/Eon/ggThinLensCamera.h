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



#ifndef GGTHINLENSCAMERA_H
#define GGTHINLENSCAMERA_H

#include <ggHAffineMatrix3.h>
#include <ggCamera.h>
#include <ggRay3.h>



class ggThinLensCamera : public ggCamera {
public:
  virtual ggRay3 getRay(const ggPoint2& screen_coord,
                       double t, const ggPoint2& ran) const;
  ggThinLensCamera(int nx, int ny,
                  const ggPoint3& lookfrom,
                  const ggPoint3& lookat,
                  const ggVector3& vup,
                  const double fov,  // radians
                  const double A = 0.0,
                  const double focal = 0.035,
                  const double focus = ggInfinity);
protected:
  double halfA;  // half aperture
  double f;      // focal distance
  double D;      // focus distance
  double d;      // film distance
  double sx, sy, dx, dy;
  double  f_over_d_minus_f;
  double  f_over_d_minus_f_sx;
  double  f_over_d_minus_f_sy;
  double  f_over_d_minus_f_d;

  ggPoint3 lensCenter;
  ggHAffineMatrix3 trans;
//  ggThinLensCamera::ggThinLensCamera() {}
  ggThinLensCamera() {}
};

#endif
