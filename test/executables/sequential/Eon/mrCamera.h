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



#ifndef MRCAMERA_H
#define MRCAMERA_H

#include <iostream.h>
#include <ggCamera.h>

enum cameraType { PINHOLE, THINLENS };


class mrCamera {
public:
  mrCamera() {camPtr = 0;
			   lookfrom = ggOrigin3;
			   lookat = ggOrigin3 + ggXAxis3;
			   time1 = time2 = 0.0;
			   vup = ggYAxis3;
			   aperture = 0.0;
			   focus =  10000;
			   focal = 0.035;
			   cType = PINHOLE;
  }

  ggRay3 getRay(const ggPoint2& screenCoord,
                double time, const ggPoint2& lensCoord) const {
					 return camPtr->getRay(screenCoord, time, lensCoord);
				}

  ggBoolean ResetPlacement(const ggPoint3& lf,
						   const ggPoint3& la, const ggVector3& vup);

  ggBoolean ResetSize(int width, int height);


  ggBoolean Read(istream& s, int width, int height);

  int width() const { return camPtr->width(); }
  int height() const { return camPtr->height(); }

protected:
  ggCamera *camPtr;
  double time1, time2;
  ggPoint3 lookfrom;
  ggPoint3 lookat;
  ggVector3 vup;
  double vfov;
  double aperture;
  double focal;
  double focus;
  cameraType cType;
  int nx, ny;
};

#endif
