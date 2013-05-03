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



////////////////////////////////////////////////////////////////////////
// ggPolygon.h -- declarations for class ggPolygon
// 
// Author:     Kurt Zimmerman  November, 1993
//
// Copyright 1993 by Kurt Zimmerman
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
////////////////////////////////////////////////////////////////////////


#ifndef GGPOLYGON
#define GGPOLYGON

#include <ggBoolean.h>
#include <assert.h>
#include <ggTrain.h>
#include <ggPoint2.h>
#include <ggPlane.h>
#include <ggRay3.h>
#include <ggConstants.h>
#include <ggRanNum.h>




class ggPolygon
{
public:
  ggPolygon()
  {
    vertices.Clear();
    area = 0;
    convex = ggFalse;
  }
  ggPolygon(const ggPolygon& poly)
    {
      N = poly.normal();
      vertices = poly.getVertices();
      plane = poly.getPlane();
      area = poly.getArea();
      convex = isConvex();
      setUV();
    }
      
#ifdef  GGSAFE
  void Set(const ggTrain<ggPoint3>&  train)
  {
    assert(train.length() > 3);
    plane.Set(train);
    vertices = train;
    N = plane.normal();
    plane.Set(train);
    computeArea();
    convexivity();
    setUV();
  }
  ggPolygon(const ggTrain<ggPoint3>&  train): plane(train)
  {
    assert(train.length() > 3);
    vertices = train;
    N = plane.normal();
    computeArea();
    convexivity();
    setUV();
  }
#else
  void Set(const ggTrain<ggPoint3>&  train)
  {
    plane.Set(train);
    vertices = train;
    N = plane.normal();
    plane.Set(train);
    computeArea();
    convexivity();
    setUV();
  }
  ggPolygon(const ggTrain<ggPoint3>&  train): plane(train)
  {
    vertices = train;
    N = plane.normal();
    computeArea();
    convexivity();
    setUV(); 
  }
#endif  

  const ggPlane& getPlane() const { return plane;}
  const ggTrain<ggPoint3>& getVertices() const { return vertices;}
//  const int nVertices() const { return vertices.length();}
  int nVertices() const { return vertices.length();}
  ggPoint3 vertex(int i) const {return vertices[i];}
  ggVector3 normal() const { return N;}
  double getArea() const { return area;}
  ggBoolean isConvex() const {return convex;}
  ggPoint3 getPoint(const ggPoint2& uv) const;
  ggPoint2 getUV(const ggPoint3& p) const;
  ggPoint3 getUniformRandomPoint(const ggPoint2&) const;

  
  
protected:
  ggTrain<ggPoint3>  vertices;
  ggPlane            plane;
  double             area;
  ggBoolean          convex;
  ggVector3          N;

  // data for uv set up;
  ggPoint3 P0,P1,P2,P3;
  ggVector3 Pa, Pb, Pc, Pd;
  double Du0, Du1, Du2, Dv0, Dv1, Dv2;
  ggVector3 Na, Nb, Nc;

  // points to allow simple random point generation
  ggPoint3 R0, R1, R2, R3;
  
  double Dux,Duy,Dvx,Dvy;
  ggVector3 Qux,Quy,Qvx,Qvy;

  void computeArea();   // compute the surface area of the polygon.
  void convexivity();   // check to see if the polygon is convex.
  void setUV();         // set up variables for texture mapping.
};
  

  


// Function prototype for non-member intersection routine.
ggBoolean  ggRayPolygonIntersect(const ggRay3& r, const ggPolygon& poly,
				 double& t,
				 double tmin = ggEpsilon,
				 double tmax = ggInfinity);

// Function prototype for non-member point containment routine.
ggBoolean ggPointOnPolygon(const ggPolygon& poly, const ggPoint3& p);


// IO function prototypes
ostream &operator<<(ostream &os, const ggPolygon& p);

istream &operator>>(istream &is, ggPolygon &p);

#endif
