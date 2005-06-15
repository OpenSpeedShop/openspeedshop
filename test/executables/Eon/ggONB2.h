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
// ggONB2.h -- Orthonormal basis
//
// Author:   Peter Shirley,    Feb 6, 1993
// Modified: Christophe Meyer, Nov 1, 1993
//
// Copyright 1993 by Peter Shirley, Christophe Meyer
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
//

#ifndef GGONB2_H
#define GGONB2_H

#include <iostream.h>
#include <math.h>
#include <assert.h>
#include <ggVector2.h>


class ggONB2 {

public:
    ggONB2() { U.Set(1, 0); V.Set(0, 1); }

    ggONB2(const ggVector2& a, const ggVector2& b) {
         U = a; V = b;
#ifdef GGSAFE
         assert(isONB2());
#endif
    }

    //
    // Calculate the ONB2
    //
    ggONB2 & InitFromU( const ggVector2& u );
    ggONB2 & InitFromV( const ggVector2& v );

    //
    // Get a component from the ONB basis
    //
    ggVector2 u() const { return U; }
    ggVector2 v() const { return V; }

    //
    // For debugging purposes
    //
#ifdef GGSAFE
    ggBoolean isONB2( void );
#endif

private:
    ggVector2 U,V;
};

const ggONB2 ggCanonicalBasis2(ggXAxis2, ggYAxis2);

//
// == operator
//
ggBoolean  operator==(const ggONB2& o1, const ggONB2 &o2);

//
// Input / Output Operator
//
istream &operator>>(istream &is, ggONB2 &t);
ostream &operator<<(ostream &os, const ggONB2 &t);

#endif
