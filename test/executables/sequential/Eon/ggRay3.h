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
// ggRay3.h-- declarations for class ggRay3
//
// Author:    Peter Shirley
//
// Modified:  Jai-hoon Kim
//            Nov. 1, 1993
//            Pete Shirley 6/94 to remove refractive index
//
#ifndef GGRAY3_H
#define GGRAY3_H

#include <ggMacros.h>
#include <ggPoint3.h>
#include <ggVector3.h>

class ggRay3 {
public :
   ggRay3() {}
   ggRay3(ggBoolean b) : o(b), v(b)  {}
   ggRay3(const ggPoint3& a, const ggVector3& b); 
   void Set(const ggPoint3&, const ggVector3& v);
   ggPoint3 origin() const { return o; }
   ggPoint3& origin() { return o; }
   ggVector3 direction() const { return v; }
   ggVector3& direction() { return v; }
   ggPoint3 pointAtParameter(double t) const { return o + t * v ;}
   void SetOrigin(const ggPoint3 p) { o = p; }
   void SetDirection(const ggVector3 a) { v = a; }
   void Advance(double t) { o += t*v; }
   ggRay3 &operator=(const ggRay3& r) { o = r.origin();
                                        v = r.direction();
                                        return *this; }
   friend ostream & operator<<(ostream &os, const ggRay3 &r);
   friend istream & operator>>(istream &is, ggRay3 &r);
private:
   ggPoint3 o;
   ggVector3 v;
};



inline ggRay3::ggRay3(const ggPoint3& a,
                      const ggVector3& b) {
   o = a;
   v = b;
}


inline void ggRay3::Set(const ggPoint3& a,
                        const ggVector3& b) {
   o = a;
   v = b;
}

inline ostream & operator<<(ostream &os, const ggRay3 &r) {
   os << r.origin() << " " << r.direction();
   return os;
}

inline istream & operator<<(istream &is, ggRay3 &r) {
   is >> r.origin() >>  r.direction();
   return is;
}

ggBoolean operator==(const ggRay3& r1, const ggRay3& r2);

#endif
