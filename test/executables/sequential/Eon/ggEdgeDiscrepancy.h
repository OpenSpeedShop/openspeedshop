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



#ifndef GGEDGEDISCREPANCY_H
#define GGEDGEDISCREPANCY_H

#include <iostream.h>
#include <ggConstants.h>
#include <ggPoint2.h>
#include <ggPoint3.h>
#include <ggVector2.h>
#include <ggVector3.h>
#include <math.h>

const ggPoint2 ggCenter2= ggPoint2(0.5, 0.5);
const ggPoint3 ggCenter3= ggPoint3(0.5, 0.5, 0.5);

const double ggRadius3 =0.866025403784439;// ggSqrtThree * 0.5;
const double ggRadius2 =0.707106781186548;// ggInverseSqrtTwo;

ggVector2 getClosestDiagonal2( int );
ggVector3 getClosestDiagonal3( int, int);

double volumeUnderPlane( double, double, double, double);
double areaUnderLine( double, double, double);

double f(double);		// Inverse Mapping function of old

class Face {
public:
  Face() {area = 0.0;}
				// Destructor not needed
  void Set(double i, double j, double k, int off)
  {
    offset = off;
    area = 1.0;
    norm = ggVector3(i,j,k)*(off+off-1);
    ranPt = ggPoint3(i*off,j*off,k*off);
  }
  
  ggBoolean PlaneIntersect( const double&, const double&, const double&,
			   const double&);
  
  double  getSurfaceTerm(ggPoint3 p)
  {
    ranPt -= ggVector3(p-ggOrigin);
    return( area * ggDot( ranPt-ggOrigin,norm));
  }

  inline
  double getArea() { return area; }
  

protected:
  ggVector3 norm;
  ggPoint3 ranPt;
  double area;
  int offset;

};

#endif
