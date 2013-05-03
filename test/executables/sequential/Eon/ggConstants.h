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



#ifndef GGCONSTANTS_H
#define GGCONSTANTS_H


const double ggFourPi = 12.566371;  // needs more digits
const double ggTwoPi = 6.2831853;  // needs more digits
const double ggPi = 3.14159265358979323846;
const double ggHalfPi = 1.57079632679489661923;
const double ggThirdPi = 1.0471976; // needs more digits
const double ggQuarterPi = 0.78539816; // needs more digits
const double ggInversePi = 0.31830989;
const double ggSqrtTwo = 1.4142135623730950488;
const double ggInverseSqrtTwo = 0.70710678;
const double ggSqrtThree = 1.7320508075688772935;
const double ggSqrtFive = 2.2360679774997896964;
const double ggE = 2.718281828459045235360287;

const double ggRad = 57.29577951308232;

#ifdef sun
const double ggInfinity = 1.0e10;
#else
#include <float.h>
const double ggInfinity = DBL_MAX;
#endif

const double ggBigEpsilon = 0.0001;
const double ggEpsilon = 0.000001;
const double ggSmallEpsilon = 0.000000001;
const double ggTinyEpsilon = 0.000000000001;


const double ggColorRatio = 0.0039215686274509803;

#endif

