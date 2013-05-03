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



#ifndef GGROTATINGPINHOLECAMERA_H
#define GGROTATINGPINHOLECAMERA_H

#include <ggHAffineMatrix3.h>
#include <ggHRigidBodyMatrix3.h>
#include <ggCamera.h>
#include <ggRay3.h>



class ggRotatingPinholeCamera : public ggCamera {
public:
  virtual ggRay3 getRay(const ggPoint2& screen_coord,
                       double t, const ggPoint2& ran) const;
  ggRotatingPinholeCamera(int nx, int ny,
                  const ggPoint3& lookfrom,
                  const ggPoint3& lookat,
                  const ggVector3& vup,
                  const double fov,  // radians
                  double t1,
                  double t2,
                  double th1, // radians
                  double th2);
protected:
  double thetaAtTime(double t) const { double a = (time2 - t) / (time2 - time1);
                                 return theta1 + a*(theta2 - theta1); } 
  ggPoint3 pinhole;
  ggHAffineMatrix3 stMat;
  ggHRigidBodyMatrix3 frameMat;
  ggRotatingPinholeCamera() {}
  double time1, time2, theta1, theta2;
};

#endif
