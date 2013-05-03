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
// ggHPoint3.C-- declarations for class ggHPoint3
//
// Author:    Pete Shirley
//            10/30/93
// Modified:  Kwansik Kim
//
//
// Copyright 1993 by Peter Shirley, Kwansik Kim
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
//

#include <ggHPoint3.h>


ostream &operator<<(ostream &os, const ggHPoint3 &t)
{ os << t.x() << " " << t.y() << " " << t.z() << " " << t.h();  return os; }

istream &operator>>(istream &is, ggHPoint3 &t)
  { is >> t.x() >> t.y() >> t.z() >> t.h();  return is; }



// Equality/inequality

ggBoolean operator==(const ggHPoint3 &p1, const ggHPoint3 &p2) {
    double p1h = p1.h(), p2h = p2.h();

#ifdef GGSAFE
    if (ggAbs(p1h) <= 0.0 || ggAbs(p2h) <= 0.0) {
	fprintf(stderr, "ggHPoint3::invalid homogeneous coordinate\n");
	exit(1);
    }	
#endif

    ggPoint3 a1(p1.x()/p1h, p1.y()/p1h, p1.z()/p1h);
    ggPoint3 a2(p2.x()/p2h, p2.y()/p2h, p2.z()/p2h);
    return (a1 == a2);
}

ggBoolean operator!=(const ggHPoint3 &p1, const ggHPoint3 &p2) {
    return !(p1 == p2);
}






