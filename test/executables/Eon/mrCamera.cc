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



#include <assert.h>
#include <mrCamera.h>
#include <ggPinholeCamera.h>
#include <ggThinLensCamera.h>
#include <ggString.h>

#undef assert
#define assert(EX) (EX)

ggBoolean mrCamera::ResetSize(int width, int height) {
	 nx = width;
	 ny = height;
     if (cType == PINHOLE)
		  camPtr = new ggPinholeCamera(nx, ny, lookfrom, lookat, vup, vfov);
	else
		camPtr = new ggThinLensCamera(nx, ny, lookfrom, lookat,
											 vup, vfov, aperture, focal,focus);

  return (camPtr != 0);
}

ggBoolean mrCamera::ResetPlacement(const ggPoint3& lf,
						   const ggPoint3& la, const ggVector3& v) {
	 lookfrom =lf;
	 lookat =la;
	 vup =v;
     if (cType == PINHOLE)
		  camPtr = new ggPinholeCamera(nx, ny, lookfrom, lookat, vup, vfov);
	else
		camPtr = new ggThinLensCamera(nx, ny, lookfrom, lookat,
											 vup, vfov, aperture, focal,focus);

  return (camPtr != 0);
}

ggBoolean mrCamera::Read(istream& camera, int sizex, int sizey) {

   ggString command;
   ggString header;
   nx = sizex;
   ny = sizey;

    camera >> header;
    if (header !=  "camera") {
       cerr << "bad header: " << header << ", camera expected\n";
       return ggFalse;
   }
      ggString camType;
      camera >> camType;  // error checking later.
      
      if (camType == "pinhole") { 
		 cType = PINHOLE;
         while (camera >> command)
            if (command == "/*")
                do (camera >> command); while (command != "*/");
            else if (command == "lookfrom")
                (camera >> lookfrom);
            else if (command == "lookat")
                (camera >> lookat);
            else if (command == "vup")
                (camera >> vup);
            else if (command == "vfov") {
                (camera >> vfov);
                vfov *= (ggPi / 180.0);
            }
            else {
               cerr << "unknown command in camera file : " << command << "\n";
               return ggFalse;
            }
         camPtr = new ggPinholeCamera(nx, ny, lookfrom, lookat,
                                         vup, vfov);
      }
      else if (camType == "thinLens") { 
		 cType = THINLENS;
         while (camera >> command)
            if (command == "/*")
                do assert( (camera >> command).good() ); while (command != "*/");
            else if (command == "focus")
                (camera >> focus);
            else if (command == "focal")
                (camera >> focal);
            else if (command == "aperture")
                (camera >> aperture);
            else if (command == "lookfrom")
                (camera >> lookfrom);
            else if (command == "lookat")
                (camera >> lookat);
            else if (command == "vup")
                (camera >> vup);
            else if (command == "vfov") {
                (camera >> vfov);
                vfov *= (ggPi / 180.0);
            }
            else {
               cerr << "unknown command in camera file : " << command << "\n";
               return ggFalse;
            }
         camPtr = new ggThinLensCamera(nx, ny, lookfrom, lookat,
                                     vup, vfov, aperture, focal,focus);
      }
      else {
          cerr << "unknown camera type: " << camType << "\n";
          return ggFalse;
      }


   return (camPtr != 0);

}
