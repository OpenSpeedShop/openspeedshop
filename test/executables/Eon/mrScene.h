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



#ifndef MRSCENE_H
#define MRSCENE_H

#include <iostream.h>
#include <stdlib.h>
#include <ggAll.h>
#include <mrObjectRecord.h>
#include <mrInstance.h>

class mrScene {
   public:

      mrScene() {
           bg.Set(0.0);
           am.Set(0.0);
           obPtr = lumPtr = lrPtr = 0;
           minGrid = 10;
           multiplier = 2.0;
      }

      ggSpectrum ambient() const { return am; }
      ggSpectrum ambient(const ggPoint3& , // point
                         const ggVector3& ,  // normal
                         double ) const { return am; }

      ggSpectrum background() const { return bg; }
      ggSpectrum background(const ggRay3&, double ) const { return bg; }
      void SetBackground(const ggSpectrum& back) { bg = back; }

      ggBoolean DeleteObject(const ggString& partName);

      ggBoolean AddObject(mrSurface*, const ggString& partName, const ggString& matName,
                          ggBoolean emits = ggFalse);

      mrSurface * objects() const { return obPtr; }
      mrSurface * luminaires() const { return lumPtr; }
      mrSurface * lowResolutionObjects() const { return lrPtr; }

      ggBoolean Read(istream& s);
      ggBoolean Setup(double, double);
      ggBoolean ChangeToDiffuseMaterial(const ggString& partName, const ggSpectrum& s);

      ggBoolean AddSpotLight(const ggString& partName, const ggString& matName,
                             const ggPoint3& location, const ggVector3& direction,
                             double radius, double maxAngle, const ggSpectrum& E); 


      ggSpectrum bg;
      ggSpectrum am;
      mrSurface * obPtr, *lumPtr, *lrPtr;
      int minGrid;
      double multiplier;

      ggDictionary< mrObjectRecord > objectRecords;

	  ggDictionary< ggRasterSurfaceTexture > surfaceTextures;
	  ggDictionary< ggSolidTexture > solidTextures;
	  ggDictionary< ggSpectrum > spectra;
	  ggDictionary< ggMaterial > materials;

      
};

#endif
