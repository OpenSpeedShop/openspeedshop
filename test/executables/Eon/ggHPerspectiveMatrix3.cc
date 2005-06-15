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
// ggHPerspectiveMatrix3.C -- perspective matrix class member functions
//
// Author:    Peter Shirley, October 1993
// Modified:  Greg Vogl, November 24, 1993
//
// Copyright 1993 by Peter Shirley and Greg Vogl
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
//

#include <ggHPerspectiveMatrix3.h>

//-----------------------------------------------------------------------------
// ggHNearFarPerspectiveMatrix3
//

ggHNearFarPerspectiveMatrix3::ggHNearFarPerspectiveMatrix3() 
 : ggHMatrix3(ggHIdentityMatrix3) {
     double neer = 1;
     double fahr = 100;
     e[2][2] = (neer + fahr)/neer;
     e[2][3] = -fahr;
     e[3][2] = 1/neer;
     e[3][3] = 0;
}

ggHNearFarPerspectiveMatrix3::ggHNearFarPerspectiveMatrix3(double neer,
                                                           double fahr)
 : ggHMatrix3(ggHIdentityMatrix3) {
#ifdef GGSAFE
     assert (neer != 0);
#endif
     e[2][2] = (neer + fahr)/neer;
     e[2][3] = -fahr;
     e[3][2] = 1/neer;
     e[3][3] = 0;
}

//-----------------------------------------------------------------------------
// ggHPerspectiveMatrix3
//

ggHPerspectiveMatrix3::ggHPerspectiveMatrix3()
 : ggHMatrix3(ggHIdentityMatrix3) {
     e[3][2] = 1;
     e[3][3] = 0;
}

ggHPerspectiveMatrix3::ggHPerspectiveMatrix3(double dist)
 : ggHMatrix3(ggHIdentityMatrix3) {
#ifdef GGSAFE
     assert (dist != 0);
#endif
     e[3][2] = 1/dist;
     e[3][3] = 0;
}
