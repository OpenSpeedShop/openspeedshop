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



#ifndef EONIMAGECALCULATOR_H
#define EONIMAGECALCULATOR_H

#include <iostream.h>
#include <stdlib.h>
#include <gg.h>
#include <mrScene.h>
#include <mrCamera.h>
#include <mrSurface.h>
#include <mrPixelRenderer.h>

class eonImageCalculator {
   public:
      eonImageCalculator();

// number of accuracy levels
      int nLevels() const { assert(nSamples.length() == renderer.length());
                            return nSamples.length();
                          }

// pixel radiance with desired sample density
      ggSpectrum pixelRadiance(int x, int y, int level) ;

      ggBoolean readData(istream& control, istream& camera,
                         istream& world, int& xPixels, int& yPixels);

      ggTrain< int > nSamples;
      ggTrain< mrPixelRenderer * > renderer;

      ggRGBFPixel metamer(const ggSpectrum& s) const;
      ggRGBFPixel tristimulusValue(const ggSpectrum& s) const;
      float scotopicLuminance(const ggSpectrum& s) const;


      ggSpectrum spectrumFromRGB( const ggRGBFPixel& rgb) const {
             return rgb.r() * redPrimary + rgb.b() * bluePrimary + rgb.g() * greenPrimary;
      }

   protected:

      ggSpectrum redPrimary, bluePrimary, greenPrimary;

      int nx;
      int ny;
	  ggPhotometer photometer;

	  mrScene scene;
	  mrCamera camera;

      double time1;
      double time2;

      ggTrain<ggSpectrum> sampleValues;

};

#endif
