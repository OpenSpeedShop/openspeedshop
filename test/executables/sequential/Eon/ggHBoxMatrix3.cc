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
// ggHBoxMatrix3.C -- box matrix class member functions
//
// Author:  Greg Vogl, November 24, 1993
//
// Copyright 1993 by Greg Vogl
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
//

#include <iostream.h>
#include <math.h>
#include <ggHBoxMatrix3.h>
#include <ggHScaleMatrix3.h>
#include <ggHTranslationMatrix3.h>

//-----------------------------------------------------------------------------
// ggHBoxMatrix3
//

//
// Constructors
//

ggHBoxMatrix3::ggHBoxMatrix3(const ggBox3& b1, const ggBox3& b2)
{
    // Translate b1 from b1.min() to the origin
    ggHTranslationMatrix3 t1(b1.min() - ggOrigin);

    // Scale b1 to the unit cube
    ggHScaleMatrix3 s1(b1.max() - b1.min());

    // Scale the unit cube to b2
    ggHScaleMatrix3 s2(b2.max() - b2.min());

    // Translate b2 from the origin to b2.min()
    ggHTranslationMatrix3 t2(b2.min() - ggOrigin);

    // combine the transformations into a temporary matrix
    // (inverse() for scale and translation is relatively efficient)
    ggHAffineMatrix3 m = t2 * (s2 * s1.inverse()) * t1.inverse();

    // copy the matrix elements from the temporary matrix
    for (int r = 0; r < 4; r++)
      for (int c = 0; c < 4; c++)
	e[r][c] = m.e[r][c];
}
