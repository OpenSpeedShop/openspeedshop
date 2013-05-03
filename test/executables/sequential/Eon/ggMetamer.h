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



#ifndef GGPHOTOMETER_H
#define GGPHOTOMETER_H

#include <stdlib.h>
#include <ggRGBFPixel.h>
#include <ggSpectrum.h>

class ggPhotometer {
   public:
      ggPhotometer(const ggSpectrum& rPrimary,
                   const ggSpectrum& gPrimary,
                   const ggSpectrum& bPrimary,
                   const ggSpectrum& x,
                   const ggSpectrum& y,
                   const ggSpectrum& z,
                   const ggSpectrum& vPrime) {

				  cieX = x;
				  cieY = y;
				  cieZ = z;

                  colorMatrix[0][0] = (cieX * rPrimary).area();
                  colorMatrix[0][1] = (cieX * gPrimary).area();
                  colorMatrix[0][2] = (cieX * bPrimary).area();

                  colorMatrix[1][0] = (cieY * rPrimary).area();
                  colorMatrix[1][1] = (cieY * gPrimary).area();
                  colorMatrix[1][2] = (cieY * bPrimary).area();

                  colorMatrix[2][0] = (cieZ * rPrimary).area();
                  colorMatrix[2][1] = (cieZ * gPrimary).area();
                  colorMatrix[2][2] = (cieZ * bPrimary).area();
       }
      
       float scotopicLuminance(const ggSpectrum& s) const {
               return 1700.0 * (s * vPrime).area(); }

       float photopicLuminance(const ggSpectrum& s) const {
               return 683.0 * (s * cieY).area(); }

       ggRGBFPixel tristimulusValue(const ggSpectrum& s) const {
            float X = (s * cieX).area();
            float Y = (s * cieY).area();
            float Z = (s * cieZ).area();
            return 683.0 * ggRGBFPixel(X, Y, Z);
       }


       ggRGBFPixel metamer(const ggSpectrum& s) const {
              double X = (s * cieX).area();
              double Y = (s * cieY).area();
              double Z = (s * cieZ).area();
              double red =   colorMatrix[0][0] * X +
                             colorMatrix[0][1] * Y +
                             colorMatrix[0][2] * Z;
              double green = colorMatrix[1][0] * X +
                             colorMatrix[1][1] * Y +
                             colorMatrix[1][2] * Z;
              double blue =  colorMatrix[2][0] * X +
                             colorMatrix[2][1] * Y +
                             colorMatrix[2][2] * Z;
              return ggRGBFPixel(red, green, blue);
  }

   protected:

      double colorMatrix[3][3];

      ggSpectrum cieX, cieY, cieZ;
      ggSpectrum vPrime;
};

#endif
