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
// ggPoint2.C-- declarations for class ggPoint2
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

#include <ggPoint2.h>


double ggPoint2::tol = 1E-8;


ggBoolean operator==(const ggPoint2 &t1, const ggPoint2 &t2) {
return ggEqual(t1.x() ,  t2.x() , t1.tolerance())
     && ggEqual(t1.y() , t2.y() , t1.tolerance()); }

ggBoolean operator!=(const ggPoint2 &t1, const ggPoint2 &t2)
    { return !(t1 == t2); }

ostream &operator<<(ostream &os, const ggPoint2 &t)
  { os << t.x() << " " << t.y() ;  return os; }

istream &operator>>(istream &is, ggPoint2 &t)
  { is >> t.x() >> t.y() ;  return is; }



