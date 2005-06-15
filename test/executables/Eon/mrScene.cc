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
#include <mrScene.h>
#include <ggCoverageSolidTexture.h>
#include <mr.h>

#undef assert
#define assert(EX) (EX)

#if defined( _WIN32 )
#  undef min
#  undef max
#endif

mrSurface* makeGrid(mrSurfaceList* l, int minGrid, double multi, int go, int gd,
                   double t1, double t2) {

     if (l->length() == 1)
            return (*l)[0];
    else if (l->length() <  minGrid)
            return l;
    else {
        ggBox3 box;
      l->boundingBox(t1,t2,box);
         gd = 0;
         return new mrGrid(t1, t2, l, box.min(), box.max(), multi, go, gd);
     }
}

ggBoolean mrScene::Setup(double time1, double time2) {

   mrSurface* makeGrid(mrSurfaceList* l, int minGrid, double multi, int go, int gd,
                   double t1, double t2);

    ggTrain <mrObjectRecord *> objectTrain;

    objectRecords.fillTrain(objectTrain);

	mrSurfaceList* sList = new mrSurfaceList ;
	mrSurfaceList* lList = new mrSurfaceList ;

	int i, j;

             ggSpectrum rPrimary = *(spectra.lookUp("rPrimary"));
             ggSpectrum gPrimary = *(spectra.lookUp("gPrimary"));
             ggSpectrum bPrimary = *(spectra.lookUp("bPrimary"));

        cerr << "processing " << objectTrain.length() << "parts\n";
	for (i = 0; i < objectTrain.length(); i++) {
                ggMaterial *matPtr = objectTrain[i]->material();
                ggRasterSurfaceTexture* surfaceTexturePtr = objectTrain[i]->surfaceTexture();
                ggSolidTexture* solidTexturePtr = objectTrain[i]->solidTexture();
		if (objectTrain[i]->identityTransform())
                   for (j = 0; j < objectTrain[i]->length(); j++) {
                       mrSurface *geomPtr = objectTrain[i]->surfaces[j];
                       mrSurface *surfPtr = geomPtr;
                       if (surfaceTexturePtr) { 
                             int repeatX = objectTrain[i]->repeatX;
                             int repeatY = objectTrain[i]->repeatY;
                             surfPtr = new mrSurfaceTexture(surfPtr, surfaceTexturePtr,
                                            rPrimary, gPrimary, bPrimary,
                                            repeatX, repeatY);
                       }
                       if (solidTexturePtr) 
                             surfPtr = new mrSolidTexture(surfPtr,
                                  solidTexturePtr);
                       surfPtr = new mrMaterial(surfPtr, matPtr);

		       sList->Add( surfPtr );
                       if (objectTrain[i]->isLuminaire())
                          lList->Add( surfPtr );
                   }
                else {
                   ggHRigidBodyMatrix3 m  = objectTrain[i]->toWorld();
                   ggHRigidBodyMatrix3 im = objectTrain[i]->toLocal();
                   for (j = 0; j < objectTrain[i]->length(); j++) {
                       mrSurface *geomPtr = objectTrain[i]->surfaces[j];
                       mrSurface *surfPtr = new mrInstance(geomPtr, m, im);
                       if (surfaceTexturePtr)  {
                             int repeatX = objectTrain[i]->repeatX;
                             int repeatY = objectTrain[i]->repeatY;
                             surfPtr = new mrSurfaceTexture(surfPtr, surfaceTexturePtr,
                                            rPrimary, gPrimary, bPrimary,
                                            repeatX, repeatY);
                        }
                       if (solidTexturePtr) 
                             surfPtr = new mrSolidTexture(surfPtr,
                                  solidTexturePtr);
                       surfPtr = new mrMaterial(surfPtr, matPtr);
                       sList->Add( surfPtr );
                       if (objectTrain[i]->isLuminaire())
                          lList->Add( surfPtr );
                   }
                }
	}

    obPtr = makeGrid(sList, minGrid, multiplier, 70, 0, time1, time2);
    if (lList->length()  == 1)
       lumPtr = (*lList)[0];
    else
       lumPtr = lList;
     return ggTrue;  // should check for mem access problems!
}

ggBoolean mrScene::ChangeToDiffuseMaterial(const ggString& partName, const ggSpectrum& s) {
      mrObjectRecord * rPtr = objectRecords.lookUp(partName);
      if (rPtr) {
         rPtr->SetMaterial (new ggDiffuseMaterial(s, ggTrue));
         return ggFalse;  
      }
      else
         return ggTrue;  
}

ggBoolean mrScene::AddSpotLight(const ggString& partName,  const ggString& matName,
                                const ggPoint3& location, const ggVector3& direction, 
                                double radius, double maxAngle, const ggSpectrum& E) {
      mrSurface *temp = new mrSpotAreaXYDiskLuminaire(radius, maxAngle, E);
      AddObject(temp, partName,  matName, ggTrue);
      mrObjectRecord *recPtr = objectRecords.lookUp(partName);
      ggONB3 uvw;
      uvw.InitFromW(direction);
      recPtr->AddBasisToCanonicalRotation(uvw);
      recPtr->AddMove(location.x(), location.y(), location.z());
      return ggTrue;
}

ggBoolean mrScene::AddObject(mrSurface* surfPtr,
                                const ggString& partName,
                                const ggString& matName,
                                ggBoolean emits) {
    ggMaterial *matPtr = materials.lookUp(matName);
    if (!matPtr) {
        cerr << "material " << matName << " not found\n";
        exit(-1);
    }
    mrObjectRecord *recPtr = objectRecords.lookUp(partName);
    if (!recPtr) {
        objectRecords.Add( partName, recPtr = new mrObjectRecord(matPtr)  );
    }
    recPtr->AddSurface(surfPtr);
    if (emits) recPtr->SetLuminaire();
    return ggTrue;
}


ggBoolean mrScene::DeleteObject(const ggString& partName) {
    return objectRecords.Delete( partName );
}

ggBoolean mrScene::Read(istream& surfaces) {

   ggString command;
   ggSpectrum E;
   ggSpectrum R;
   ggString header;
   ggString fileName;
   ggString partName;
   ggString matName;
   ggPoint3 center;
   double radius, height;
   ggPoint3 p0, p1, p2;
   ggString newPartName, oldPartName;

   mrSurface* makeGrid(mrSurfaceList* l, int minGrid, double, int go, int gd,
                   double t1, double t2);

   double time1 = 0.0; double time2 = 0.0;
   mrSurfaceList *isList = new mrSurfaceList; // for indirect objects
   double matrixMultiplier = 100.0;
   double matrixConstant = matrixMultiplier;

   surfaces >> header;
   if (header !=  "surfaces") {
       cerr << "bad header: " << header << ", surfaces expected\n";
        return ggFalse;
   }
      
   while (surfaces >> command) { 
         if (command == "/*") {
             do {
                surfaces >> command;
             } while (command != "*/");
         } else if (command == "white") {
             surfaces >> matrixConstant;
             matrixConstant *= matrixMultiplier;
         } else if (command == "gridMultiplier") {
             surfaces >> multiplier;
          }
           else if (command == "minGrid") {
             surfaces >> minGrid;
         } else if (command == "triangle") {
             surfaces >> partName >> matName >> p0 >> p1 >> p2;
             mrSurface *temp = new mrTriangle(p0, p1, p2);
               this->AddObject(temp,partName, matName);
         } else if (command == "indirectTriangles") {
             surfaces >> fileName;
             ifstream fin(fileName);
             if (!fin) {
                 cerr << "Could not open triangle file " << fileName << "\n";
                 return ggFalse;
             }
             fin >> header;
             if (header != "ITRIANGLES") {
               cerr << "Bad header on file " << fileName << ",use ITRIANGLES\n";
                return ggFalse;
             }
             while (fin >> p0 >> p1 >> p2 >> E) 
                 isList->Add(new mrDiffuseAreaTriangleLuminaire(p0,p1,p2,E));

         } else if (command == "scaleTriangles") {
             double scale;
             surfaces >> partName >> matName >> fileName >> scale;
             ifstream trifile(fileName);
             if (!trifile) {
                  cerr << "could not open triangle file " << fileName << "\n";
                  exit(-1);
             }
             trifile >> header;
             if (header != "TRIANGLES") {
               cerr << "Bad header on file " << fileName << ",use TRIANGLES\n";
                return ggFalse;
             }
             while (trifile >> p0 >> p1 >> p2 ) {
                 p0 = ggOrigin3 + scale * (p0 - ggOrigin3);
                 p1 = ggOrigin3 + scale * (p1 - ggOrigin3);
                 p2 = ggOrigin3 + scale * (p2 - ggOrigin3);
                 mrSurface *temp = new mrTriangle(p0, p1, p2);
                 this->AddObject(temp,partName, matName);
             }
         } else if (command == "triangles") {
             surfaces >> partName >> matName >> fileName;
             ifstream trifile(fileName);
             if (!trifile) {
                  cerr << "could not open triangle file " << fileName << "\n";
                  exit(-1);
             }
             trifile >> header;
             if (header != "TRIANGLES") {
               cerr << "Bad header on file " << fileName << ",use TRIANGLES\n";
                return ggFalse;
             }
             while (trifile >> p0 >> p1 >> p2 ) {
                 mrSurface *temp = new mrTriangle(p0, p1, p2);
                 this->AddObject(temp,partName, matName);
             }
         } else if (command == "fastTriangles") {
             surfaces >> partName >> matName >> fileName;
             ifstream trifile(fileName);
             if (!trifile) {
                  cerr << "could not open triangle file " << fileName << "\n";
                  exit(-1);
             }
             trifile >> header;
             if (header != "TRIANGLES") {
               cerr << "Bad header on file " << fileName << ",use TRIANGLES\n";
                return ggFalse;
             }
             while (trifile >> p0 >> p1 >> p2 ) {
                 mrSurface *temp = new mrFastTriangle(p0, p1, p2);
                 this->AddObject(temp,partName, matName);
             }
         } else if (command == "boxes") {
             surfaces >> partName >> matName >> fileName;
             ifstream boxfile(fileName);
             if (!boxfile) {
                  cerr << "could not open box file " << fileName << "\n";
                  exit(-1);
             }
             boxfile >> header;
             if (header != "BOXES") {
               cerr << "*****BAD header on file " << fileName << ",use BOXES\n";
               cerr << header << " was used\n";
                return ggFalse;
             }
             ggPoint3 pmin, pmax;
             while (boxfile >> pmin >> pmax ) {
                 mrSurface *temp = new mrBox(pmin, pmax);
                 this->AddObject(temp,partName, matName);
             }
         } else if (command == "polygon") {
             ggPolygon polygon;
             surfaces >> partName >> matName >> polygon;
             mrSurface *temp = new mrPolygon(polygon );
               this->AddObject(temp,partName, matName);
         }
         else if (command == "spotLuminaire") {
             ggPoint3 location;
             ggVector3 direction;
              double maxAngle;
             surfaces >> partName >> matName >> location >> direction >> radius >> maxAngle >> E;
             AddSpotLight(partName,   matName, location,  direction,
                          radius, ggDegreesToRadians(maxAngle),  E);
         }
         else if (command == "spotXYDiskLuminaire") {
             ggSpectrum E;
             double maxAngle;
             surfaces >> partName >> matName >> radius >> maxAngle >> E;
             mrSurface *temp = new mrSpotAreaXYDiskLuminaire(radius, ggDegreesToRadians(maxAngle), E);
             this->AddObject(temp,partName, matName, ggTrue);
         }
         else if (command == "diffuseTriangleLuminaire") {
             ggSpectrum E;
             surfaces >> partName >> matName >> p0 >> p1 >> p2 >> E;
             mrSurface *temp = new mrDiffuseAreaTriangleLuminaire(
                                 p0, p1, p2, E);
              this->AddObject(temp,partName, matName, ggTrue);
         }
         else if (command == "XYDisk") {

             surfaces >> partName >> matName >> center >> radius;
             mrSurface *temp = new mrXYDisk(center, radius);
               this->AddObject(temp,partName, matName);
         }
         else if (command == "YZDisk") {

             surfaces >> partName >> matName >> center >> radius;
             mrSurface *temp = new mrYZDisk(center, radius);
               this->AddObject(temp,partName, matName);

         }
         else if (command == "XZDisk") {

             surfaces >> partName >> matName >> center >> radius ;
             mrSurface *temp = new mrXZDisk(center, radius);
               this->AddObject(temp,partName, matName);

         }
         else if (command == "phongXYRectangleLuminaire") {
             double xMin, xMax, yMin, yMax, z, expo;
             surfaces >> partName >> matName >> xMin >> xMax
                                 >> yMin >> yMax >> z >>  E >> expo;
             mrSurface *temp = 
                new mrPhongAreaXYRectangleLuminaire(xMin, xMax, yMin, yMax,z, E, expo);
               this->AddObject(temp,partName, matName,ggTrue);

         }
         else if (command == "diffuseXYRectangleLuminaire") {
             double xMin, xMax, yMin, yMax, z;
             assert( (surfaces >> partName >> matName>>  xMin >> xMax
                                       >> yMin >> yMax >> z >>  E).good() );
             mrSurface *temp = 
                new mrDiffuseAreaXYRectangleLuminaire(xMin, xMax, yMin, yMax,z, E);
             this->AddObject(temp,partName, matName,ggTrue);
         }
         else if (command == "phongXZRectangleLuminaire") {
             double xMin, xMax, zMin, zMax, y, expo;
             assert( (surfaces >> partName >> matName >> xMin >> xMax
                                  >> y >> zMin >> zMax >> E >> expo).good() );
             mrSurface *temp = new
         mrPhongAreaXZRectangleLuminaire(xMin, xMax,y, zMin, zMax,E, expo);
             this->AddObject(temp,partName, matName,ggTrue);
         }
         else if (command == "diffuseXZRectangleLuminaire") {
             double xMin, xMax, zMin, zMax, y;
             assert( (surfaces >> partName >> matName >> xMin >> xMax
                                       >> y >> zMin >> zMax >> E).good() );
             mrSurface *temp = new
         mrDiffuseAreaXZRectangleLuminaire(xMin, xMax,y, zMin, zMax,E);
             this->AddObject(temp,partName, matName,ggTrue);
         }
         else if (command == "phongYZRectangleLuminaire") {
             double yMin, yMax, zMin, zMax, x, expo;
             assert( (surfaces >> partName >> matName>>  x >> yMin
                                 >> yMax >> zMin >> zMax >> E >> expo).good() );
             mrSurface *temp = new
           mrPhongAreaYZRectangleLuminaire(x, yMin, yMax,zMin,zMax,E,expo);
             this->AddObject(temp,partName, matName,ggTrue);
         }
         else if (command == "diffuseYZRectangleLuminaire") {
             double yMin, yMax, zMin, zMax, x;
             assert( (surfaces >> partName >> matName >> x >> yMin
                                       >> yMax >> zMin >> zMax >> E).good() );
             mrSurface *temp = new
               mrDiffuseAreaYZRectangleLuminaire(x, yMin, yMax,zMin,zMax,E);
              this->AddObject(temp,partName, matName,ggTrue);

         }
          else if (command == "box") {
                ggPoint3 pmin, pmax;
                surfaces >> partName >> matName >> pmin >> pmax;
                mrSurface *temp = new mrBox(pmin, pmax);
                this->AddObject(temp,partName, matName);
         }
         else if (command == "XYRectangle") {
             double xMin, xMax, yMin, yMax, z;
             surfaces >> partName  >> matName >> xMin >> xMax
                                       >> yMin >> yMax >> z ;
             mrSurface *temp = new mrXYRectangle(xMin, xMax, yMin, yMax,z);
             this->AddObject(temp,partName, matName);
         }
         else if (command == "XZRectangle") {
             double xMin, xMax, zMin, zMax, y;
             assert((surfaces >> partName >> matName >> xMin >> xMax
                                       >> y >> zMin >> zMax ).good() );
             mrSurface *temp = new mrXZRectangle(xMin, xMax,y, zMin,zMax);
             this->AddObject(temp,partName, matName);
         }
         else if (command == "YZRectangle") {
             double yMin, yMax, zMin, zMax, x;
             assert( (surfaces >> partName >> matName >> x >> yMin
                                       >> yMax >> zMin >> zMax ).good() );
             mrSurface *temp = new mrYZRectangle(x, yMin, yMax,zMin, zMax);
             this->AddObject(temp,partName, matName);
         }
         else if (command == "XCylinder") {
             double r, xMin, xMax, y, z;
             assert( (surfaces >> partName >> xMin >> xMax >> y >> z >> r ).good() );
             mrSurface *temp = new mrXCylinder(xMin, xMax, y, z, r);
              this->AddObject(temp,partName, matName);
         }
         else if (command == "YCylinder") {
             double r, yMin, yMax, z, x;
             assert( (surfaces >> partName >> x>> yMin >> yMax >> z >> r ).good() );
             mrSurface *temp = new mrYCylinder(x, yMin, yMax, z, r);
              this->AddObject(temp,partName, matName);
         }
         else if (command == "ZCylinder") {
             double r, zMin, zMax, x, y;
             assert( (surfaces >> partName >> x >> y >> zMin >> zMax >> r ).good() );
             mrSurface *temp = new mrZCylinder(x, y, zMin, zMax, r);
               this->AddObject(temp,partName, matName);
         }
         else if (command == "XEllipticalCylinder") {
             double rY, rZ, xMin, xMax, y, z;
             assert( (surfaces >> partName >>  xMin 
                                 >> xMax >> y >> z >> rY >> rZ ).good() );
             mrSurface *temp = new mrXEllipticalCylinder(xMin, xMax, y, z, rY, rZ);
               this->AddObject(temp,partName, matName);
         }
         else if (command == "YEllipticalCylinder") {
             double rZ, rX, yMin, yMax, z, x;
             assert( (surfaces >> partName >> x >> yMin 
                                      >> yMax >> z >> rX >> rZ).good() );
             mrSurface *temp = new mrYEllipticalCylinder( x,
                                          yMin, yMax, z, rX, rZ);
               this->AddObject(temp,partName, matName);
         }
         else if (command == "ZEllipticalCylinder") {
             double rX, rY, zMin, zMax, x, y;
             assert( (surfaces >> partName >> x >> y >> zMin 
                                     >> zMax >> rX >> rY ).good() );
             mrSurface *temp = new mrZEllipticalCylinder(x, y, 
                                            zMin, zMax, rX, rY);
               this->AddObject(temp,partName, matName);
         }
         else if (command == "sphere") {
             surfaces >> partName >> matName >> center >> radius;
             assert (radius > 0.0);
             mrSurface *temp = new mrSphere(center, radius);
             this->AddObject(temp,partName, matName);
         }
         else if (command == "background") {
             ggString bag;
             assert( (surfaces >> bag).good() );
             ggSpectrum *bgPtr;
             bgPtr =  spectra.lookUp(bag) ;
             if (bgPtr == 0) {
                 cerr << "Bad background name " << bg << "\n";
                 exit(1);
             }
             bg = *bgPtr;
         }
         else if (command == "ambient") {
             ggString amb;
             surfaces >> amb;
             ggSpectrum *amPtr;
             amPtr =  spectra.lookUp(amb) ;
             if (amPtr == 0) {
                     cerr << "Bad ambient name " << amb << "\n";
                 exit(1);
             }
             am = *amPtr;
         }
         else if (command == "spectra") {
             ggString spectrumName;
             ggSpectrum s;
             surfaces >> fileName;
             ifstream fin(fileName);
             if (!fin) {
                   cerr << "could not open file " << fileName << "\n";
                   return ggFalse;
             }
             fin >> header;
             if (header != "spectra") {
                 cerr << "Bad header:  " << fileName << ", use spectra\n";
                 exit(1);
             }
             while (fin >> spectrumName) {
                 assert ( (fin >> s).good() );
                 ggSpectrum *sp = new ggSpectrum;
                 *sp = s;
                 spectra.Add(spectrumName, sp);
                 materials.Add(spectrumName, new ggDiffuseMaterial(s, ggTrue));
             }
         }
         else if (command == "echo") {
             ggString s;
             surfaces >> s;
             cerr << s << "\n";
         }
         else if (command == "materials") {
             ggString materialName;
             ggString materialType;
             ggString brdfFlag;
             surfaces >> fileName;
             ifstream fin(fileName);
             fin >> header;
             if (header != "materials") {
                 cerr << "Bad header on file " << fileName << 
                          ", use materials\n";
                 exit(1);
             }
             while (fin >> materialName) {
                 assert( (fin >>  materialType).good() );
                 if (materialType == "diffuse") {
                    assert( (fin >> brdfFlag >> R).good() );
                    if (brdfFlag == "brdf")
                      materials.Add(materialName, 
                                    new ggDiffuseMaterial(R, ggTrue));
                    else
                      materials.Add(materialName, 
                                    new ggDiffuseMaterial(R, ggFalse));
                 }
                 else if (materialType == "specular") {
                    double e;
                    assert( (fin >> R >>  e).good() );
                   materials.Add(materialName, new ggSpecularMaterial(R,e));
                 }
                 else if (materialType == "conductor") {
                    ggSpectrum N, k;
                    double e;
                    assert( (fin >> N >> k >> e).good() ); 
                   materials.Add(materialName, new ggConductorMaterial(N,k,e));
                 }
                 else if (materialType == "polished") {
                    ggSpectrum N;
                    double e;
                    assert( (fin >> N >> R >> e).good() ); 
                    materials.Add(materialName, new ggPolishedMaterial(N,R,e));
                 }
                 else if (materialType == "pathDielectric") {
                    ggSpectrum N, k;
                    assert( (fin >> N >> k).good() ); 
                   materials.Add(materialName, 
                           new ggPathDielectricMaterial(N,ggInfinity));
                 }
                 else if (materialType == "dielectric") {
                    ggSpectrum N, k;
                    assert( (fin >> N >> k).good() ); 
                   materials.Add(materialName, 
                           new ggDielectricMaterial(N,ggInfinity));
                 }
                 else {
                     cerr << "Unknown material type " << materialType << "\n";
                 }
             }
         }
         else if (command == "move") {
             double dx, dy, dz;
             assert( (surfaces >> oldPartName >> dx >> dy >> dz).good() );
             mrObjectRecord * obj = (mrObjectRecord *) objectRecords.lookUp(oldPartName);
             assert(obj);
             obj->AddMove(dx, dy, dz);
         }
         else if (command == "rotateX") {
             double rx;
             surfaces >> oldPartName >> rx;
	     rx = ggDegreesToRadians(rx);
             mrObjectRecord * obj = (mrObjectRecord *) objectRecords.lookUp(oldPartName);
             assert(obj);
             obj->AddXRotation(rx);
         }
         else if (command == "rotateY") {
             double ry;
             surfaces >> oldPartName >> ry;
			 ry = ggDegreesToRadians(ry);
             mrObjectRecord * obj = (mrObjectRecord *) objectRecords.lookUp(oldPartName);
             assert(obj);
             obj->AddYRotation(ry);
         }
         else if (command == "rotateZInstance") {
             double rz;
             surfaces >> oldPartName >> rz;
			 rz = ggDegreesToRadians(rz);
             mrObjectRecord * obj = (mrObjectRecord *) objectRecords.lookUp(oldPartName);
             assert(obj);
             obj->AddZRotation(rz);
         }
         else if (command == "diffuseCosineSphereLuminaire") {
             surfaces >> partName >> matName >> center >> radius >> E;
              mrSurface *temp = 
                new mrDiffuseCosineSphereLuminaire(center, radius, E);
             this->AddObject(temp,partName, matName,ggTrue);
         }
         else if (command == "diffuseSolidAngleSphereLuminaire") {
             surfaces >> partName >> matName >> center >> radius >> E;
             mrSurface *temp = new mrDiffuseSolidAngleSphereLuminaire(center, radius, E);
             this->AddObject(temp,partName, matName,ggTrue);
         }
         else if (command == "diffuseAreaZCylinderLuminaire") {
         ggSpectrum E;
             surfaces >> partName >> matName >> radius >> height >> E;
             mrSurface *temp = 
                new mrDiffuseAreaZCylinderLuminaire(radius, height, E);
             this->AddObject(temp,partName, matName,ggTrue);
     }
         else if (command == "diffuseVisibleAreaZCylinderLuminaire") {
             surfaces >> partName >> matName >> radius >> height >> E;
              mrSurface *temp = 
               new mrDiffuseVisibleAreaZCylinderLuminaire(radius,
                                      height, E);
             this->AddObject(temp,partName, matName,ggTrue);
     }
         else if (command == "diffuseCosineZCylinderLuminaire") {
             surfaces >> partName >> matName >> radius >> height >> E;
              mrSurface *temp = 
                new mrDiffuseCosineZCylinderLuminaire(radius, height, E);
             this->AddObject(temp,partName, matName,ggTrue);
     }
         else if (command == "phongAreaTriangleLuminaire") {
             double N;
             surfaces >> partName >> matName >> p0 >> p1 >> p2 >> E >> N;
              mrSurface *temp =  
                       new mrPhongAreaTriangleLuminaire(p0,p1,p2,E,N);
             this->AddObject(temp,partName, matName,ggTrue);
         }
         else if (command == "mapSurfaceTexture") {
             ggString textureName;
             double repeatX, repeatY;
             surfaces >> oldPartName >> textureName >>
                    repeatX >> repeatY;

             ggRasterSurfaceTexture *tex = surfaceTextures.lookUp(textureName);
             if (tex == 0) { 
                 cerr << "No texture " << textureName << " defined\n";
                 exit(1);
             }

             mrObjectRecord *obj = objectRecords.lookUp(oldPartName);
             assert(obj != 0);
// We seem to need to explicitly convert the second and third args
	     // to avoid a runtime error on NT
#if 0
             obj->SetSurfaceTexture(tex, repeatX, repeatY);
#else
             obj->SetSurfaceTexture(tex, (int)repeatX, (int)repeatY);
#endif
         }
         else if (command == "mapSolidTexture") {
             ggString textureName;
             surfaces >> oldPartName >> textureName; 

             ggSolidTexture *tex = solidTextures.lookUp(textureName);
             if (tex == 0) { 
                 cerr << "No texture " << textureName << " defined\n";
                 exit(1);
             }

             mrObjectRecord *obj = objectRecords.lookUp(oldPartName);
             assert(obj != 0);
             obj->SetSolidTexture(tex);
         }
         else if (command == "solidTexture") {
             ggString textureName;
             ggString textureType;
             surfaces >> textureName >>  textureType;
             ggSolidTexture *tex;
             if (textureType == "wood") {
                 ggString s1, s2;
                 ggSpectrum *sLight, *sDark;
                 double bR, tR, h, w0, w1, ra;
                 surfaces >> s1 >> s2;
                 sLight = spectra.lookUp(s1);
                 sDark = spectra.lookUp(s2);
                 surfaces >>  bR >>  tR >> h >> w0 >> w1 >> ra;
                 if (sLight == 0) {
                     cerr << "Spectrum " << s1 << " not in library\n";
                     return ggFalse;
                 }
                 if (sDark == 0) {
                     cerr << "Spectrum " << s2 << " not in library\n";
                     return ggFalse;
                 }
             tex =  new ggOakSolidTexture(*sLight, *sDark, bR, tR, h,w0,w1,ra);
             }
             else if (textureType == "coverage") {
                 double baseline, scale;
                 int num;
                 surfaces >> baseline >> scale >> num;
                tex =  new ggCoverageSolidTexture(baseline, scale, num);
             }
             else if (textureType == "boardFloor") {
                 double bl, bw, bd;
                 ggString axisName;
                 ggString tName;
                 int axis;
                 surfaces >> tName >> bl >> bw >> bd >> axisName;

                 ggSolidTexture *tPtr = solidTextures.lookUp(tName);
                 if (tPtr == 0) {
                    cerr << "No texture " << tName << " defined\n";
                    exit(1);
                 }
                 if (axisName == "x" || axisName == "X")
                    axis = 0;
                 else if (axisName == "y" || axisName == "Y")
                    axis = 1;
                 else if (axisName == "z" || axisName == "Z")
                    axis = 2;
                 else {
                    cerr << "Unknown axis " << axisName << "\n";
                    return ggFalse;
                 }
                 tex =  new ggBoardFloorSolidTexture(tPtr, bl, bw, bd, axis);
             }
             else {
                 cerr << "Unknown solid texture type " << textureName << "\n";
                 return ggFalse;
             }
             solidTextures.Add(textureName, tex );
         }
         else if (command == "surfaceTexture") {
             ggString textureName;
             surfaces >> textureName >>  fileName;
             ggRaster<ggRGBPixel<unsigned char> > *r1 =
                   new ggRaster<ggRGBPixel<unsigned char> >;

             ggIO<ggRGBPixel<unsigned char> > fin(fileName, GGPPM);
             fin.SetInputProcessingType(GGRAW);
             fin.SetOutputProcessingType(GGRAW);
             fin >> (*r1);
             ggRasterSurfaceTexture *tex = new ggRasterSurfaceTexture(r1);
             surfaceTextures.Add(textureName, tex );
         }
         else {
            cerr << "****UNKNOWN command in surfaces file : " << command << "\n";
            exit(-1);
            // return ggFalse;
         }
       }

       lrPtr = isList;

	this->Setup(time1, time2);

   return ggTrue;

}
