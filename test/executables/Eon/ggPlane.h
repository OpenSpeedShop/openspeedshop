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



// 
// ggPlane.h -- declarations for class ggPlane
// Author:     Kurt Zimmerman
//             10/8/93
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
//

#ifndef  GGPLANE
#define  GGPLANE

#include <ggBoolean.h>
#include <assert.h>
#include <ggPoint3.h>
#include <ggVector3.h>
#include <ggTrain.h>
#include <ggRay3.h>
#include <ggConstants.h>



class ggPlane
{
public:


  void coefficients(double& a, double& b, double& c, double& d) const
  { a = N.x(); b = N.y(); c = N.z(); d = D; }
  
  ggVector3 normal() const { return N;}

  ggPoint3  point() const { return P;}

  double returnD() const { return D;}  

  ggPlane()
  {
    D = 0; N.Set(0,0,0); P.Set(0,0,0);
  }

  void Set(const ggTrain<ggPoint3>& t){initialize(t);}
  ggPlane(const ggTrain<ggPoint3>& t){initialize(t);}
  
  void Set(const ggPlane& p)
  {
    P = p.point();
    p.coefficients(N.x(), N.y(),N.z(), D);
  }
  ggPlane(const ggPlane& p)
  {
    P = p.point();
    p.coefficients(N.x(), N.y(),N.z(), D);
  }
  
#   ifdef GGSAFE
  void Set(const ggPoint3& p1, const ggPoint3& p2, const ggPoint3& p3)
  {
    ggVector3 v1(p1 - p3), v2(p2 - p3);

    assert(p1 != p3 && p2 != p3);
    N = ggCross(v1,v2);
    D = N.x()*p1.x() + N.y()*p1.y() + N.z()*p1.z();
    D = D/N.length();
    N.MakeUnitVector();
    P = p1;
  }
  ggPlane(const ggPoint3& p1, const ggPoint3& p2, const ggPoint3& p3)
  {
    ggVector3 v1(p1 - p3), v2(p2 - p3);

    assert(p1 != p3 && p2 != p3);
    N = ggCross(v1,v2);
    D = N.x()*p1.x() + N.y()*p1.y() + N.z()*p1.z();
    D = D/N.length();
    N.MakeUnitVector();
    P = p1;
  }
void Set(const double& a, const double& b, const double& c, const double& d)
  {
    assert(!(a == b && b == c && c == d));
    ggPoint3 p(0,0,0);
    N.Set(a,b,c); D = d;
    D = D/N.length();
    N.MakeUnitVector();
    setPointInPlane(p);
  }

  ggPlane(const double& a, const double& b, const double& c, const double& d)
  {
    assert(!(a == b && b == c && c == d));    
    ggPoint3 p(0,0,0);
    N.Set(a,b,c); D = d;
    D = D/N.length();
    N.MakeUnitVector();
    setPointInPlane(p);
  }

  void Set(const ggPoint3& point, const ggVector3& normal)
    {
      P = point;
      N = normal;
      D = N[0]*P[0] + N[1]*P[1] + N[2]*P[2];
      assert(!(N[0]==0.0 && N[1]==0.0 && N[2]==0.0));
    }

  ggPlane(const ggPoint3& point, const ggVector3& normal)
    {
      P = point;
      N = normal;
      D = N[0]*P[0] + N[1]*P[1] + N[2]*P[2];
      assert(!(N[0] == 0.0 && N[1]==0.0 && N[2]==0.0));
    }
#   else
  void Set(const ggPoint3& p1, const ggPoint3& p2, const ggPoint3& p3)
  {
    ggVector3 v1, v2;
    v1 = p1 - p3;
    v2 = p2 - p3;
    N = ggCross(v1,v2);
    D = N.x()*p1.x() + N.y()*p1.y() + N.z()*p1.z();
    D = D/N.length();
    N.MakeUnitVector();
    P = p1;
  }
  ggPlane(const ggPoint3& p1, const ggPoint3& p2, const ggPoint3& p3)
  {
    ggVector3 v1, v2;
    v1 = p1 - p3;
    v2 = p2 - p3;
    N = ggCross(v1,v2);
    D = N.x()*p1.x() + N.y()*p1.y() + N.z()*p1.z();
    D = D/N.length();
    N.MakeUnitVector();
    P = p1;
  }

  void Set(const double& a, const double& b, const double& c, const double& d)
  {
    ggPoint3 p(0,0,0);
    N.Set(a,b,c); D = d;
    D = D/N.length();
    N.MakeUnitVector();
    setPointInPlane(p);
  }

  ggPlane(const double& a, const double& b, const double& c, const double& d)
  {
    ggPoint3 p(0,0,0);
    N.Set(a,b,c); D = d;
    D = D/N.length();
    N.MakeUnitVector();
    setPointInPlane(p);
  }

  void Set(const ggPoint3& point, const ggVector3& normal)
    {
      P = point;
      N = normal;
      D = N[0]*P[0] + N[1]*P[1] + N[2]*P[2];
    }

  ggPlane(const ggPoint3& point, const ggVector3& normal)
    {
      P = point;
      N = normal;
      D = N[0]*P[0] + N[1]*P[1] + N[2]*P[2];
    }
#   endif
protected:
  
  double  D;
  ggVector3 N;
  ggPoint3 P;

  void initialize(const ggTrain<ggPoint3>&);

# ifdef  GGSAFE
  // this function takes a point and projects it into the plane
  // then sets the data member P to that point.
  void setPointInPlane(const ggPoint3& p)
  {
    int i = N.indexOfMaxAbsComponent();
    assert(N[i]>0);
    if(i == 2)
      {
	assert(N[2] != 0);
	P.Set(p[0],p[1],(D - (N[0] + N[1]))/N[2]);
      }
    else if(i == 0)
      {
	assert(N[0] != 0);
	P.Set((D - (N[1] + N[2]))/N[0],	p[1], p[2]);
      }
    else
      {
	assert(N[1] != 0);
	P.Set(p[0],(D - (N[0] + N[2]))/N[1],p[2]);
      }
  }
# else
 void  setPointInPlane(const ggPoint3& p)
  {
    int i = N.indexOfMaxAbsComponent();
    if(i == 2)
      P.Set(p[0],p[1],(D - (N[0] + N[1]))/N[2]);
    else if(i == 0)
      P.Set((D - (N[1] + N[2]))/N[0],p[1], p[2]);
    else
      P.Set(p[0],(D - (N[0] + N[2]))/N[1],p[2]);
  } 
# endif
 
};



// Function prototype for non-member intersection utility
ggBoolean ggRayPlaneIntersect(const ggRay3& r,const ggPlane& plane,
			      double& t,
			      double tmin = ggEpsilon,
			      double tmax = ggInfinity);

// IO function prototypes
inline ostream &operator<<(ostream &os, const ggPlane& p)
{
  os<<p.point()<<endl;
  os<<p.normal()<<endl;
  
  return os;
}

inline istream &operator>>(istream &is, ggPlane& p)
{
  ggPoint3 point;
  ggVector3 normal;

  is >> point >> normal;

  p.Set(point, normal);
  
  return is;
}

#endif
