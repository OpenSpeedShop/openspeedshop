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



#ifndef GGMATERIAL_H
#define GGMATERIAL_H


#include <ggMacros.h>
#include <ggPoint2.h>
#include <ggPoint3.h>
#include <ggVector3.h>
#include <ggONB3.h>
#include <ggSpectrum.h>
#include <ggBRDF.h>
#include <ggRay3.h>


class ggMaterialRecord {
public:        
     ggPoint2 UV;
     ggRay3 ray1;
     ggRay3 ray2;
     ggSpectrum kRay1;
     ggSpectrum kRay2;
     ggSpectrum kBRDF;
     ggBRDF* BRDFPointer;
     ggBoolean hasRay1;
     ggBoolean CORLRay1;
     ggBoolean hasRay2;
     ggBoolean CORLRay2;
             
     ggMaterialRecord() { BRDFPointer = 0; kBRDF.Set(1.0);
                           hasRay1 = hasRay2 = ggFalse; }

     ggMaterialRecord(ggBoolean b) : UV(b), ray1(b), ray2(b) 
                  { BRDFPointer = 0; hasRay1 = hasRay2 = ggFalse;}

};
             


class ggMaterial {
protected:
  ggBRDF *rhoPtr;
public:

  virtual ggBoolean print(ostream& ) const;

  virtual ggBoolean getInfo(const ggRay3&, // rayIn,
                            const ggPoint3&, //  p,
                            const ggONB3&, // uvw,  
                            const double&, // cover,  
                            ggMaterialRecord& ) const;

};


#endif
