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
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <math.h>
#include <gg.h>
#include <ggRotatingPinholeCamera.h>
#include <mrBox.h>
#include <mrLinkedObjects.h>
#include <mrEmitter.h>
#include <mrImposter.h>
#include <mrPolygon.h>
#include <mrXCylinder.h>
#include <mrYCylinder.h>
#include <mrZCylinder.h>
#include <mrXEllipticalCylinder.h>
#include <mrYEllipticalCylinder.h>
#include <mrZEllipticalCylinder.h>
#include <mrSurfaceList.h>
#include <mrSphere.h>
#include <mrPhongAreaTriangleLuminaire.h>
#include <mrPhongAreaXYRectangleLuminaire.h>
#include <mrPhongAreaXZRectangleLuminaire.h>
#include <mrPhongAreaYZRectangleLuminaire.h>
#include <mrDiffuseAreaXYRectangleLuminaire.h>
#include <mrDiffuseAreaXZRectangleLuminaire.h>
#include <mrDiffuseAreaYZRectangleLuminaire.h>
#include <mrShellLuminaire.h>
#include <mrDiffuseCosineSphereLuminaire.h>
#include <mrDiffuseAreaZCylinderLuminaire.h>
#include <mrDiffuseVisibleAreaZCylinderLuminaire.h>
#include <mrDiffuseCosineZCylinderLuminaire.h>
#include <mrDiffuseAreaTriangleLuminaire.h>
#include <mrDiffuseSolidAngleSphereLuminaire.h>
#include <mrTriangle.h>
#include <mrFastTriangle.h>
#include <mrBruteForcePixelRenderer.h>
#include <mrRushmeierPixelRenderer.h>
#include <mrCookPixelRenderer.h>
#include <mrKajiyaPixelRenderer.h>
#include <mrIndirectPixelRenderer.h>
#include <mrCoarsePixelRenderer.h>
#include <mrInstance.h>
#include <mrSurfaceTexture.h>
#include <mrSolidTexture.h>
#include <mrMaterial.h>
#include <mrXYRectangle.h>
#include <mrXZRectangle.h>
#include <mrYZRectangle.h>
#include <mrGrid.h>
#include <mrXYDisk.h>
#include <mrYZDisk.h>
#include <mrXZDisk.h>
#include "eonImageCalculator.h"

#undef assert
#define assert(EX) (EX)

extern int DumpX, DumpY;

eonImageCalculator::eonImageCalculator() {
         nx = 100; ny = 200;
		 ggSpectrum cieX, cieY, cieZ,  vPrime;
		 ifstream s("eon.dat");
                 if (!s) {
                      cerr << "file eon.dat required for eon to run\n";
                      exit (-1);
                  }
		 ggString name;
		 while (s >> name) {
			  if (name == "x")
				 s >> cieX;
              else if (name == "y")
				 s >> cieY;
              else if (name == "z")
				 s >> cieZ;
              else if (name == "rPrimary")
				 s >> redPrimary;
              else if (name == "gPrimary")
				 s >> greenPrimary;
              else if (name == "bPrimary")
				 s >> bluePrimary;
	  else if (name == "vPrime")
				 s >> vPrime;
              else { 
				 cerr << "unkown spectrum " << name << " in eon.dat\n";
				 exit(-1);
			   }
		 }

		 photometer = ggPhotometer(redPrimary, greenPrimary, bluePrimary,
		     						cieX, cieY, cieZ, vPrime);
}


float eonImageCalculator::scotopicLuminance(const ggSpectrum& s) const {
      return photometer.scotopicLuminance(s);
}

ggRGBFPixel eonImageCalculator::tristimulusValue(const ggSpectrum& s) const {
      return photometer.tristimulusValue(s);
}

ggRGBFPixel eonImageCalculator::metamer(const ggSpectrum& s) const {
      return photometer.metamer(s);
}




ggSpectrum eonImageCalculator::pixelRadiance(int i, int j, int level) {
    ggRay3 r;
    int lev = ggMin(level, nLevels());
    int n = nSamples[level];
	sampleValues.Clear();

    renderer[level]->samplePixel(i, j, n, sampleValues);

    return ggAverage(sampleValues);

}



ggBoolean eonImageCalculator::readData(istream& control,
                                    istream& cam,
                                    istream& surfaces,
                                    int& xPixels, int &yPixels) {

   ggString command;
   ggSpectrum E;
   ggSpectrum R;
   ggString header;
   ggString fileName;
   ggString partName;
   ggPoint3 center;
   ggPoint3 p0, p1, p2;
   ggString newPartName, oldPartName;

   time1 = 0.0;
   time2 = 0.0;

   scene.Read(surfaces);

   cerr << "reading control stream\n";
      int maxRayDepth;
      control >> header;
      if (header !=  "control") {
          cerr << "bad header: " << header << ", control expected\n";
          return ggFalse;
      }
      while (control >> command)
         if (command == "/*")
             do assert(control >> command); while (command != "*/");
         else if (command == "pixels") {
             assert(control >> nx >> ny);
             if (nx <= 0 || ny <= 0 || nx > 100000 || ny > 100000) {
                 cerr << "nx ny must be in range [0,100000]\n";
                 cerr << "input values: " << nx << " " << ny << "\n";
                 return ggFalse;
             } 
         }
         else if (command == "bruteForce") {
             int nSamp;
             assert(control >> nSamp >> maxRayDepth);
             if (nSamp < 1 || nSamp > 100000000) {
                 cerr << "nSamp  must be in range [0,10000000]\n";
                 cerr << "input value: " << nSamp << "\n";
                 return ggFalse;
             }
             nSamp = int ( sqrt (nSamp + 0.001) );
             nSamples.Append(nSamp);
             renderer.Append(new mrBruteForcePixelRenderer(&scene, &camera,
                             maxRayDepth, time1, time2));
         }
         else if (command == "rushmeier") {
             int nSamp;
             assert(control >> nSamp >> maxRayDepth);
             if (nSamp < 1 || nSamp > 100000000) {
                 cerr << "nSamp  must be in range [0,10000000]\n";
                 cerr << "input value: " << nSamp << "\n";
                 return ggFalse;
             }
             nSamp = int ( sqrt (nSamp + 0.001) );
             nSamples.Append(nSamp);
             renderer.Append(new mrRushmeierPixelRenderer(&scene, &camera,
                             maxRayDepth, time1, time2, 0.005));
         }
         else if (command == "indirect") {
             int nSamp;
             assert(control >> nSamp >> maxRayDepth);
             if (nSamp < 1 || nSamp > 100000000) {
                 cerr << "nSamp  must be in range [0,10000000]\n";
                 cerr << "nSamp  must be in range [0,100000]\n";
                 cerr << "input value: " << nSamp << "\n";
                 return ggFalse;
             }
             nSamp = int ( sqrt (nSamp + 0.001) );
             nSamples.Append(nSamp);
             renderer.Append(new mrIndirectPixelRenderer(&scene, &camera, maxRayDepth,
				 time1, time2, 0.005));
         }
         else if (command == "kajiya") {
             int nSamp;
             assert(control >> nSamp >> maxRayDepth);
             if (nSamp < 1 || nSamp > 100000000) {
                 cerr << "nSamp  must be in range [0,10000000]\n";
                 cerr << "nSamp  must be in range [0,100000]\n";
                 cerr << "input value: " << nSamp << "\n";
                 return ggFalse;
             }
             nSamp = int ( sqrt (nSamp + 0.001) );
             nSamples.Append(nSamp);
             renderer.Append(new mrKajiyaPixelRenderer(&scene, &camera, maxRayDepth,
				 time1, time2, 0.005));
         }
         else if (command == "coarse") {
             int nSamp;
             assert(control >> nSamp);
             if (nSamp < 1 || nSamp > 100000000) {
                 cerr << "nSamp  must be in range [0,10000000]\n";
                 cerr << "input value: " << nSamp << "\n";
                 return ggFalse;
             }
             nSamp = int ( sqrt (nSamp + 0.001) );
             nSamples.Append(nSamp);
             renderer.Append(new mrCoarsePixelRenderer(
                                   &scene, &camera, time1, time2));
         }
         else if (command == "cook") {
             int nSamp;
             assert(control >> nSamp >> maxRayDepth);
             if (nSamp < 1 || nSamp > 100000000) {
                 cerr << "nSamp  must be in range [0,10000000]\n";
                 cerr << "input value: " << nSamp << "\n";
                 return ggFalse;
             }
             nSamp = int ( sqrt (nSamp + 0.001) );
             nSamples.Append(nSamp);
             renderer.Append(new mrCookPixelRenderer(&scene, &camera, maxRayDepth,
				 time1, time2));
         }
         else {
            cerr << "unknown command in control file : " << command << "\n";
            return ggFalse;
         }

       assert(nLevels() > 0);
   xPixels = nx;
   yPixels = ny;

   cerr << "reading camera stream\n";
	  camera.Read(cam, nx, ny);


   return ggTrue;

}
