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



//////////////////////////////////////////////////////////////////////////
// ggPlane.C -- functions for class ggPlane
// Author:     Kurt Zimmerman
//             12/1/93
// Modified:
//
//
// Copyright 1993 by Kurt Zimmerman
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
//////////////////////////////////////////////////////////////////////////

#include <ggPlane.h>

void ggPlane::initialize(const ggTrain<ggPoint3>&  train)
{
  // Because of floating point arithematic problems we must compute a 
  // plane of best fit for the points which will not be coplanar.  To
  // do this we can set a Matrix A, a vector x, and a vector b as 
  //    follows: 
  //
  //  A = | x1  y1  1 |   x = |a|   b = | z1 |
  //      | x2  y2  1 |	      |b|	| z2 |
  //      | x3  y3  1 |	      |c|	| z3 |
  //      |  .   .  . |		        |  . |
  //      |  .   .  . |		        |  . |
  //      |  .   .  . |		        |  . |
  //      | xn  yn  1 |		        | zn |
  //
  //   We then want to find a vector x that minimizes the square of the
  //   2-norm | Ax - b |.  This can be done by simply multiplying the
  //   transpose of A, A', as such:
  //
  //	A'Ax = A'b
  //
  //   A'A is a symmetric 3x3 matrix, and A'b is a 3x1 vector as is x.
  //   We can then use Cramer's Rule to solve the system and thus find
  //   a least squares solution for the plane of best fit.
 
  // First we must make sure that the plane is not axis aligned or
  // near axis aligned. Let's iterate through the points and determine
  // component of greatest variance

  int   i=0, j = 0;
  double x = 0,y = 0,z = 0;
    do
      {
	j++;
	x += fabs((train[j])[0] - (train[i])[0]);
	y += fabs((train[j])[1] - (train[i])[1]);
	z += fabs((train[j])[2] - (train[i])[2]);
      } while(j < train.length());
  
  if(x < ggEpsilon){N.Set(1,0,0); D = (train[0])[0];}
  else if(y < ggEpsilon) {N.Set(0,1,0); D = (train[0])[1];}
  else if(z < ggEpsilon) {N.Set(0,0,1); D = (train[0])[2];}
  else
    {
      // We have a non axis aligned plane
      // since A'A is 3x3 symmetric matrix we only need to
      // store  m11, m12, m13, m22, m23, m33
      double m11, m12, m13, m22, m23, m33;

      double v1, v2, v3; // space for A'b

      m11 = m12 = m13 = m22 = m23 = m33 = 0;
      v1 = v2 = v3 = 0;
      
      for(i = 0 ; i < train.length(); i++)
	{
	  m11 +=  (train[i])[0] * (train[i])[0];
	  m12 +=  (train[i])[0] * (train[i])[1];
	  m13 +=  (train[i])[0];
	  m22 +=  (train[i])[1] * (train[i])[1];
	  m23 +=  (train[i])[1];
	  m33 +=  1;

	  v1 +=  (train[i])[0] * (train[i])[2];
	  v2 +=  (train[i])[1] * (train[i])[2];
	  v3 +=  (train[i])[2];
	}
      
      // Now A'A is represented as:   m11 m12 m13
      //                              m12 m22 m23
      //                              m13 m23 m33
      
      // Performing Cramer's Rule
      
      double Det = m11*(m22*m33 - m23*m23)     // determinant m11 m12 m13
	+ m12*(m23*m13 - m12*m33)	       //      of     m12 m22 m23
	  + m13*(m12*m23 - m13*m22);	       //             m13 m23 m33

      double d1 = v1*(m22*m33 - m23*m23)       // determinant v1 m12 m13
	+ m12*(m23*v3 - v2*m33)	               //      of     v2 m22 m23
	  + m13*(v2*m23 - v3*m22);	       //             v3 m23 m33

      double d2 = m11*(v2*m33 - v3*m23)     // determinant m11 v1 m13
	+ v1 * (m23*m13 - m12*m33)	    //      of     m12 v2 m23
	  + m13*(m12*v3 - m13*v2);	    //             m13 v3 m33
      
      double d3 = m11*(m22*v3 - m23*v2) // determinant m11 m12 v1
	+ m12*(v2*m13 - m12*v3)	    //      of     m12 m22 v2
	  + v1*(m12*m23 - m13*m22);	    //             m13 m23 v3
      
      // Now we can get the coefficients to the plane equation
      // z = ax + by + d  or ax + by - z = -d

      double a = d1/Det;
      double b = d2/Det;
      double d = d3/Det;

      // set the normal vector  
      N[0] = a; N[1]= b; N[2] = -1.0;
      D = -d/N.length();
      N.MakeUnitVector();
    }


  // We have found a plane equation but the normal may be pointed 
  // in the wrong direction.  You may wish to use the opposite normal 
  // depending on your application. See the function convexivity() in
  // ggPolygon.C for an example when this is neededs.

  // store a point in the plane
  ggPoint3 p;
  int axis1, axis2;
  i = N.indexOfMaxAbsComponent();
  
  if(0 == i) { axis1 = 1; axis2 = 2;}
  else if ( 1 == i ){ axis1 = 0; axis2 = 2;}
  else { axis1 = 0; axis2 = 1;}

  p[axis1] = (train[0])[axis1];
  p[axis2] = (train[0])[axis2];
  p[i] = (D - p[axis1] * N[axis1] - p[axis2] * N[axis2])/N[i];

  setPointInPlane(p);
    
}



ggBoolean ggRayPlaneIntersect(const ggRay3& r,const ggPlane& plane, double& t,
			      double tmin, double tmax)
{
#ifdef GGSAFE
  assert((plane.normal()).maxAbsComponent() > 0);
#endif
  t =  ggDot(plane.normal(), (plane.point() - r.origin()))
    / ggDot(plane.normal(), r.direction());
  
return ((t > ggEpsilon) && (tmin < t) && (t < tmax));
}
