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
// ggPolygon.C -- Area and Intersection Code for ggPolygon
//
// Author:    Kurt Zimmerman  November, 1993
//
// Copyright 1993 by <Kurt Zimmerman>
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
////////////////////////////////////////////////////////////////////////


#include <ggPolygon.h>
#include <ggRay3.h>

void ggPolygon::setUV()
{

  int maxIndex = (N).indexOfMaxAbsComponent();
  int axis1, axis2;

  if(0 == maxIndex) { axis1 = 1; axis2 = 2;}
  else if ( 1 == maxIndex ){ axis1 = 0; axis2 = 2;}
  else { axis1 = 0; axis2 = 1;}

  R0 =  R2 = vertex(0);
  for(int i = 1; i < nVertices(); i++)
    {
      if(R0[axis1] > (vertex(i))[axis1])
	R0[axis1] = (vertex(i))[axis1];
      if(R0[axis2] > (vertex(i))[axis2])
	R0[axis2] = (vertex(i))[axis2];
      if(R2[axis1] < (vertex(i))[axis1])
	R2[axis1] = (vertex(i))[axis1];
      if(R2[axis2] < (vertex(i))[axis2])
	R2[axis2] = (vertex(i))[axis2];
    }

  R1 = R0; R1[axis1] = R2[axis1];
  R3 = R0; R3[axis2] = R2[axis2];

  R0[maxIndex] = (plane.returnD() - R0[axis1]*(N)[axis1]
		  - R0[axis2]*(N)[axis2])/(N)[maxIndex];
  R1[maxIndex] = (plane.returnD() - R1[axis1]*(N)[axis1]
		  - R1[axis2]*(N)[axis2])/(N)[maxIndex];
  R2[maxIndex] = (plane.returnD() - R2[axis1]*(N)[axis1]
		  - R2[axis2]*(N)[axis2])/(N)[maxIndex];
  R3[maxIndex] = (plane.returnD() - R3[axis1]*(N)[axis1]
		  - R3[axis2]*(N)[axis2])/(N)[maxIndex];

  
  if(nVertices() == 4 && convex)
    {
      P0 = vertex(0);
      P1 = vertex(1);
      P2 = vertex(2);
      P3 = vertex(3);
    }
  else
    {
      P0 = R0;
      P1 = R1;
      P2 = R2;
      P3 = R3;
    }
  Pa = (P0 - P1)
    + (P2 - ggOrigin) - (P3 - ggOrigin);
  Pb = P1 - P0;
  Pc = P3 - P0;
  Pd = P0 - ggOrigin;

  Na = ggCross(Pa, N);
  Nc = ggCross(Pc, N);
  Du0 = ggDot(Nc, Pd);
  Du1 = ggDot(Na, Pd) + ggDot(Nc, Pb);
  Du2 = ggDot(Na, Pb);
  
  Qux = Na/(2*Du2);
  Dux = -Du1/(2*Du2);
  Quy = -Nc/Du2;
  Duy = Du0/Du2;

  Nb = ggCross(Pb, N);
  Dv0 = ggDot(Nb, Pd);
  Dv1 = ggDot(Na, Pd) + ggDot(Nb, Pc);
  Dv2 = ggDot(Na, Pc);
  
  Qvx = Na/(2*Dv2);
  Dvx = -Dv1/(2*Dv2);
  Qvy = -Nb/Dv2;
  Dvy = Dv0/Dv2;
}

ggPoint3 ggPolygon::getPoint(const ggPoint2& uv) const
{
  return ( P0 + uv.x() * (P1 - P0)
	  + uv.y()*((P3 + uv.x() * (P2 - P3))
		    -(P0 + uv.x() * (P1 - P0))));
}

ggPoint2 ggPolygon::getUV(const ggPoint3& p) const
{
  ggVector3 R = p - ggOrigin;
  ggPoint2 p2(0,0);
  double d;
  
  if(fabs(Du2)< ggEpsilon)
    {
      p2[0] = (ggDot(Nc, R) - Du0)/(Du1 - ggDot(Na, R));
    }
  else
    {
      double Ka = Dux + ggDot(Qux, R);
      double Kb = Duy + ggDot(Quy, R);

      d = sqrt((Ka * Ka) - Kb);
      if(0 <= Ka + d && Ka + d <=1.0)
	{
	  p2[0] = Ka + d;
	}
      else
	{
	  p2[0] = Ka - d;
	}
    }

  if(fabs(Dv2) < ggEpsilon)
    {
      p2[1] = (ggDot(Nb, R) - Dv0)/(Dv1 - ggDot(Na, R));
    }
  else
    {
      double Ca = Dvx + ggDot(Qvx,  R);
      double Cb = Dvy + ggDot(Qvy,  R);

      d = sqrt((Ca * Ca) - Cb);
      if(0 <= Ca - d && Ca - d <= 1.0)
	p2[1] = Ca - d;
      else
	p2[1] = Ca + d;
    }
  return p2;
}

void ggPolygon::computeArea()
{
  ggVector3 sum(0,0,0) , v1, v2;
  ggPoint3   p1,p2;
  int j = 0, i = nVertices() - 1;

  do
    {
      p1 = vertex(i);
      p2 = vertex(j);

      i = j++;  // ;-)

     v1 = p1 - ggOrigin;  // set v1 = p1
     v2 = p2 - ggOrigin;  // set v2 = p2

     sum += ggCross(v1,v2);

    } while (j < nVertices());

  area = 0.5 * ggDot(N,sum);
}
  
void ggPolygon::convexivity()
{
  ggVector3 v, v1,v2;
  double c;
  int k = 0, j = nVertices() - 1, i = nVertices() - 2;
  int normIndex = N.indexOfMaxAbsComponent();
  int sumMinus = 0, sumPlus = 0;

  do
    {
      v1 = vertex(j) - vertex(i);
      v2 = vertex(k) - vertex(i);
      i = j; j = k++;
      v = ggCross(v1, v2);

      c = ggDot(N,v);

      if( c < 0 )
	sumMinus++;
      else
	sumPlus++;
    } while(k < nVertices());
  if(sumPlus < sumMinus)
    {
      N = -N;
    }
  if(sumMinus == 0 || sumPlus == 0)
    convex = ggTrue;
  else
    convex = ggFalse;
}

ggPoint3 ggPolygon::getUniformRandomPoint(const ggPoint2&) const
{
  ggPoint2     uv;
  ggPoint3     p;
  int          i = 0;
  
  do
    {
      uv[0] = ggCanonicalRandom();
      uv[1] = ggCanonicalRandom();

      p = R0 + uv.x() * (R1 - R0)
 	  + uv.y()*((R3 + uv.x() * (R2 - R3))
		    -(R0 + uv.x() * (R1 - R0)));
    }while( !ggPointOnPolygon(*this, p) && 20 > i++ );

  if(i >= 20)
    return vertex(0);
  else
    return p;
}
  
ggBoolean operator==(const ggPolygon& p1, const ggPolygon& p2)
{
  if(p1.nVertices() != p2.nVertices())
    return ggFalse;
  else{
    for(int i = 0; i < p1.nVertices(); i++)
      {
	if(p1.vertex(i) != p2.vertex(i))
	  return ggFalse;
      }
    return ggFalse;
  }
}

ostream &operator<<(ostream &os, const ggPolygon& p)
{
  os<< p.nVertices()<<endl;
  for(int i = 0; i < p.nVertices(); i++)
    os<<p.vertex(i)<<endl;
  return os;
}

istream &operator>>(istream &is, ggPolygon &p)
{
  int n;
  ggTrain<ggPoint3> t;
  ggPoint3 vert;
  is >> n;
  for(int i = 0; i < n; i++)
    {
      is >> vert;
      t.Append(vert);
    }
  p.Set(t);

  return is;
}

ggBoolean  ggRayPolygonIntersect(const ggRay3& r, const ggPolygon& poly,
				 double& t,
				 double tmin, double tmax)
{
  double tPlane;
  if(!ggRayPlaneIntersect(r, poly.getPlane(), tPlane, tmin, tmax))
    {
      return ggFalse;
    }
  else
    {
      t = tPlane;
      if(ggPointOnPolygon(poly, r.pointAtParameter(tPlane)))
	return ggTrue;
      else
	return ggFalse;
    }
}

ggBoolean ggPointOnPolygon(const ggPolygon& poly, const ggPoint3& p)
{
 
      int maxIndex = ((poly.normal()).indexOfMaxAbsComponent());

      int axis1, axis2;

      if(0 == maxIndex) { axis1 = 1; axis2 = 2;}
      else if ( 1 == maxIndex ){ axis1 = 0; axis2 = 2;}
      else { axis1 = 0; axis2 = 1;}

      int j = 0, i = poly.nVertices() - 1, intersections = 0;
      ggPoint3 p1, p2;
      
      do
	{
	  p1 = poly.vertex(i);
	  p2 = poly.vertex(j);

	  i = j++; 

	  // our point is a vertex
	  if(p2[axis1] == p[axis1] && p2[axis2] == p[axis2])
	    {
	      intersections = 1;
	      break;
	    }
	  // our point resides on a horizontal edge or on a vertex
	  else if(p1[axis2] == p[axis2] && p2[axis2] == p[axis2])
	   {
	     if((p1[axis1] < p[axis1] && p2[axis1] > p[axis1])
		|| (p1[axis1] > p[axis1] && p2[axis1] < p[axis1]))
	       {
		 intersections = 1;
		 break;
	       }
	   }
	  // our point resides on a vertical edge
	  else if(p1[axis1] == p[axis1] && p2[axis1] == p[axis1])
	   {
	     if((p1[axis2] < p[axis2] && p2[axis2] > p[axis2])
		|| (p1[axis2] > p[axis2] && p2[axis2] < p[axis2]))
	       {
		 intersections = 1;
		 break;
	       }
	   }
          else if(p1[axis1] < p[axis1] && p2[axis1] < p[axis1])
	    continue; // trivially behind
	  else if(p1[axis2] > p[axis2] && p2[axis2] > p[axis2])
	    continue; // trivially above
	  else if(p1[axis2] <= p[axis2] && p2[axis2] <= p[axis2])
	    continue; // trivially below
	  
	  
          else if(p1[axis1] > p[axis1] && p2[axis1] > p[axis1] &&
		  ((p1[axis2] > p[axis2] && p2[axis2] <= p[axis2]) ||
		   (p1[axis2] <= p[axis2] && p2[axis2] > p[axis2])))
	    {
	      intersections++;
	      continue;
	    }
	  
	  else
	    { double tt;
	      double n1 = -(p1[axis2] - p2[axis2]);
	      double n2 = p1[axis1] - p2[axis1];
	      
	      tt = (n1 * (p1[axis1] - p[axis1])
		    + n2 * (p1[axis2] - p[axis2])) / n1;
	      
	      if(tt < 0.0) continue;
	      else intersections++;
	    }
	} while(j < poly.nVertices());
      
      return ggBoolean(intersections & 1);
    }

