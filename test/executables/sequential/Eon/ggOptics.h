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



#ifndef GGOPTICS_H
#define GGOPTICS_H


#include <ggPoint2.h>
#include <ggONB3.h>


double ggMaterialReflectance(const double Re);
// returns reflectance defined by the top level

double ggVacuumConductorReflectance(const ggVector3& v,
                              const ggVector3& n,
                              double Nt, double kt);

double ggConductorReflectance(const ggVector3& v,
                              const ggVector3& n,
                              double Ni, double Nt, double kt);
// returns the reflectance using Fresnel Equations

double ggDielectricReflectance(const ggVector3& v,
                               const ggVector3& tv, 
                               const ggVector3& n,
                               double Ni, double Nt);
double ggVacuumDielectricReflectance(const ggVector3& v,
                               const ggVector3& tv, 
                               const ggVector3& n,
                               double Nt);
double ggDielectricVacuumReflectance(const ggVector3& v,
                               const ggVector3& tv, 
                               const ggVector3& n,
                               double Ni);
// returns the reflectance using Fresnel equations with k=0

ggVector3 ggReflection(const ggVector3& v, const ggVector3& n);
// returns the reflection vector 

ggBoolean ggVacuumDielectricRefract(const ggVector3& n, double Nt, ggVector3& v);

ggBoolean ggDielectricVacuumRefract(const ggVector3& n, double Ni, ggVector3& v);

ggBoolean ggRefract(const ggVector3& n, double Ni, double Nt, ggVector3& v);
// returns transmitted vector using Snell's law



#endif
