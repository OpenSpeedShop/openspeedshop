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
// ggRay2.h-- declarations for class ggRay2
//
// Author:    Peter Shirley
//
// Modified:  Jai-hoon Kim
//            Nov. 1, 1993
//
#ifndef GGRAY2_H
#define GGRAY2_H

#include <ggMacros.h>
#include <ggPoint2.h>
#include <ggVector2.h>

class ggRay2 {
public :
   ggRay2() {}
   ggRay2(const ggPoint2& a, const ggVector2& b); 
   void Set(const ggPoint2&, const ggVector2& v);
   ggPoint2 origin() const { return o; }
   ggPoint2& origin() { return o; }
   ggVector2 direction() const {return v; }
   ggVector2& direction() {return v; }
   ggPoint2 pointAtParameter(double t) const { return o + t*v; }
   void SetOrigin(const ggPoint2 p) { o = p; }
   void SetDirection(const ggVector2 a) { v = a; }
   void Advance(double t) { o += t * v; }
   ggRay2 &operator=(const ggRay2&);
   friend ostream & operator<<(ostream &os, const ggRay2 &r);
   friend istream & operator>>(istream &is, ggRay2 &r);
private:
   ggPoint2 o;
   ggVector2 v;
};



inline ggRay2::ggRay2(const ggPoint2& a,
                      const ggVector2& b) 
{
   o = a;
   v = b;
}

inline void ggRay2::Set(const ggPoint2& a, const ggVector2& b)
{
   o = a;
   v = b;
}


inline ggRay2& ggRay2::operator=(const ggRay2& r)
{
   o = r.origin();
   v = r.direction();
   return *this;
}


inline ostream & operator<<(ostream &os, const ggRay2 &r) {
   os << r.origin() << " " << r.direction();
   return os;
}

inline istream & operator>>(istream &is, ggRay2 &r) {
   is >> r.origin() >> r.direction() ;
   return is;
}

ggBoolean operator==(const ggRay2& r1, const ggRay2& r2);

#endif
