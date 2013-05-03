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



#include <ggNA.h>
#include <ggConstants.h>
#include <math.h>

void ggInvertMatrix(double data[3][3])
{
   double a = data[0][0];
   double b = data[0][1];
   double c = data[0][2];
   double d = data[1][0];
   double e = data[1][1];
   double f = data[1][2];
   double g = data[2][0];
   double h = data[2][1];
   double i = data[2][2];


   double denom = -c*e*g + b*f*g + c*d*h - a*f*h - b*d*i + a*e*i;

   data[0][0] = (-f*h + e*i) / denom;
   data[0][1] = ( c*h - b*i) / denom;
   data[0][2] = (-c*e + b*f) / denom;
   data[1][0] = ( f*g - d*i) / denom;
   data[1][1] = (-c*g + a*i) / denom;
   data[1][2] = ( c*d - a*f) / denom;
   data[2][0] = (-e*g + d*h) / denom;
   data[2][1] = ( b*g - a*h) / denom;
   data[2][2] = (-b*d + a*e) / denom;
}



// finds smallest real root of ax^2 + bx + c in [xmin,xmax].  returns
// ggFalkse if no such root exists
ggBoolean ggSmallestQuadraticRoot(double a, double b, double c,
                                  double xmin, double xmax, double& x) {

    if (a < ggSmallEpsilon) return ggFalse;

    a *= 2;
    double rad = b*b - 2*(a*c);
    if (rad < ggEpsilon) {   // single or no root
        if (rad > 0.0) {
            x = -b / a;
            return ggTrue;
        }
        else
            return ggFalse;
    }
    rad = sqrt(rad);
    a = 1/a;
    x = a * (-b - rad);
    if (x > xmax) return ggFalse;
    if (x < xmin) x = a * (-b + rad);
    return (x >= xmin && x <= xmax);
}
