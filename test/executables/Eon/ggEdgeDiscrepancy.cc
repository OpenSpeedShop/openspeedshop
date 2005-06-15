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



#include <ggEdgeDiscrepancy.h>


// The following function returns the half diagonal vector of the unit
// square appropriate to the quadrant in which the point lies.
ggVector2 getClosestDiagonal2(int xy)
{
  double x = (( xy == 0 || xy == 3)? 1.0 : -1.0);
  double y = ( xy < 2 ? 1.0 : -1.0);

  return( ggVector2( x,y ) * ggInverseSqrtTwo);

}

// Given one of four XY quadrants and a +/- Z specification, this
// function returns the closest half diagonal (in the same octant)
ggVector3 getClosestDiagonal3( int xy, int z)
{
  double vx = ((xy==0||xy==3)? 1.0 : -1.0);
  double vy = ((xy<2)? 1.0 : -1.0);
  double vz = ( z==1? 1.0 : -1.0);

  ggVector3 v = ggVector3(vx,vy,vz);
  v.MakeUnitVector();
  return v;
}

// A Stokes's Theorem implementation for calculating the volume of a polyhedron
// The higher level function which puts together all the area data from the
// faces of the cube.
//           -   Graphics Gems (Glassner)
double volumeUnderPlane(double a, double b, double c, double d)
{
  Face faces[6];

  int cnt = 6;
  faces[0].Set(0,0,1,0);	// xy , offset = 0
  faces[1].Set(0,0,1,1);	// xy , offset = 1
  faces[2].Set(0,1,0,0);	// zx, 0
  faces[3].Set(0,1,0,1);	// zx, 1
  faces[4].Set(1,0,0,0);	// yz, 0
  faces[5].Set(1,0,0,1);	// yz, 1

  ggPoint3 neworigin;
  double volume=0.0;
				// Find a point on the plane to which
				// we can move the origin. This eliminates
				// the face newly created (by the plane inter
				// secting the cube) from the area sum calcu-
				// lations.
 if(a)				// Try a point on the X axis
   neworigin = ggPoint3( -d/a, 0, 0 );
 else
   if(b)			// Y axis
     neworigin = ggPoint3(0, -d/b, 0);
   else
     if(c)			// Z axis
       neworigin = ggPoint3(0,0, -d/c);
 while(cnt--)
   {
     faces[cnt].PlaneIntersect(a,b,c,d);
     volume += faces[cnt].getSurfaceTerm(neworigin);
   }
  return (fabs(volume)/3.0);	// 1/3 required by Stokes Theorem
}



// Compute the intersection of the plane ax+by+cz+d=0 with
// the given face
ggBoolean Face::PlaneIntersect( const double& a,
			   const double& b,
			   const double& c,
			   const double& d
			   )
{
  double areaUnderLine (double, double, double);

 if((int)norm.x())
     area = ( areaUnderLine( b, c, d + (offset*a)));
  else
    if((int)norm.y())
      area = ( areaUnderLine( a, c, d + (offset*b)));
    else
      if((int)norm.z())
	area = ( areaUnderLine( a, b, d + (offset*c)));
 return ggTrue;
}
      



// Calculate area under ax + by + c = 0, in a unit square
double areaUnderLine ( double a, double b, double c)
{
  double x0, x1, y0, y1;
  int type = 0;
  double ar;

  if(b==0.0){			// Line parallel to y axis
      x0 = -c/a;
      return( x0 > 1.0? 1.0 : ( x0 < 0.0 ? 0.0: x0));
    }
  if(a==0.0) {			// Line parallel to x axis
      y0 = -c/b;
      return( y0 > 1.0? 1.0 : ( y0 < 0.0 ? 0.0: y0));
    }
				// Compute 4 points of intersection
				// with x=0, x=1, y=0, y=1
  x0 = -c/a; x1 = -(c+b)/a;
  y0 = -c/b; y1 = -(c+a)/b;

  type += ((y0>=0.0) && (y0<=1.0));   // Left
  type += 2*((y1>=0.0) && (y1<=1.0)); // Right
  type += 4*((x0>0.0) && (x0<1.0));   // Bottom
  type += 8*((x1>0.0) && (x1<1.0));   // Top
				      // horrible hack to take care
				      // of special case when line
				      // passes through top corner
  type =  ((y0>=1.0)&&(y1>=1.0) ? 16 : type);

  switch(type){
  case 3:  ar = (0.5*(y0+y1)); break;         // left, right
  case 5:  ar = (0.5*y0*x0); break;           // bottom, left
  case 9:  ar = (1.0 - 0.5*(1-y0)*x1); break; // top, left
  case 6:  ar = (0.5*(1-x0)*y1); break;	      // bottom, right
  case 10: ar = (1.0 - 0.5*(1-y1)*(1-x1)); break;
				              // top, right
  case 12:				      // top, bottom
    if(x0>x1)
      ar = (0.5*(x0+x1));
    else
      ar = (1 - 0.5*(x0+x1));
    break;
  case 16: ar = (1.0); break;	              // totally above
  default: ar = (0.0); break;                 // totally below
  }

  if(b<0.0)			// Depending upon the sense of the 
 return(1.0-ar);		// normal of the cutting edge
				// (probably will never occur, just in case)
   else				// choose appropriate area
   return ar;	       

}


// Old inverse mapper to generate random edges
double f(double x) {
   double num, denom;
   double y;

   y = 4*x*(1-x);

   num = sqrt(1 - y*y);
   denom = y;

   if (denom < 0.000000001) denom = 0.000000001;
   if (x > 0.5)
       return (1 + num) / denom;
   else
       return (1 - num) / denom;
 }


