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



#ifndef GGPINHOLECAMERA_H
#define GGPINHOLECAMERA_H

#include <ggHAffineMatrix3.h>
#include <ggCamera.h>
#include <ggRay3.h>

// abstract class ggCamera


class ggPinholeCamera : public ggCamera {
public:
  virtual ggRay3 getRay(const ggPoint2& screen_coord,
                        double t = 0.0,
                        const ggPoint2& ran = ggOrigin2) const;
  ggPinholeCamera(int nx, int ny,
                  const ggPoint3& lookfrom,
                  const ggPoint3& lookat,
                  const ggVector3& vup,
                  const double fov);  // radians
protected:
  ggPoint3 pinhole;
  ggHAffineMatrix3 trans;
//  ggPinholeCamera::ggPinholeCamera() {}
  ggPinholeCamera() {}
};

#endif
