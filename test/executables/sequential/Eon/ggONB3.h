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
// ggONB3.h -- Orthonormal basis
//
// Author:   Peter Shirley,    Feb 6, 1993
// Modified: Christophe Meyer, Nov 1, 1993
//

#ifndef GGONB3_H
#define GGONB3_H

#include <iostream.h>
#include <math.h>
#include <assert.h>
#include <ggVector3.h>


class ggONB3 {

public:
    ggONB3() { U.Set(1, 0, 0); V.Set(0, 1, 0); W.Set(0, 0, 1); }

    ggONB3(ggBoolean b) : U(b), V(b), W(b) { }

    ggONB3(const ggVector3& a, const ggVector3& b, const ggVector3& c) {
         U = a; V = b; W = c;
#ifdef GGSAFE
         assert(isONB3());
#endif
    }

    //
    // Calculate the ONB from one vector only
    //
    ggONB3 & InitFromU( const ggVector3& u );
    ggONB3 & InitFromV( const ggVector3& v );
    ggONB3 & InitFromW( const ggVector3& w );

    void Set(const ggVector3& a,
             const ggVector3& b,
             const ggVector3& c) { U = a; V = b; W = c; }

    //
    // Calculate the ONB from two vectors
    // The first one is the Fixed vector (it is just normalized)
    // The second is normalized and its direction can be ajusted
    //
    ggONB3 & InitFromUV( const ggVector3& u, const ggVector3& v );
    ggONB3 & InitFromVU( const ggVector3& v, const ggVector3& u );

    ggONB3 & InitFromUW( const ggVector3& u, const ggVector3& w );
    ggONB3 & InitFromWU( const ggVector3& w, const ggVector3& u );

    ggONB3 & InitFromVW( const ggVector3& v, const ggVector3& w );
    ggONB3 & InitFromWV( const ggVector3& w, const ggVector3& v );

    //
    // Get a component from the ONB basis
    //
    ggVector3 u() const { return U; }
    ggVector3 v() const { return V; }
    ggVector3 w() const { return W; }

    void FlipW() { W = -W; U = -U; }

    //
    // For debugging purposes
    //
#ifdef GGSAFE
    ggBoolean isONB3( void );
#endif

private:
    ggVector3 U,V,W;
};


const ggONB3 ggCanonicalBasis3(ggXAxis3, ggYAxis3, ggZAxis3);


//
// == operator
//
ggBoolean  operator==(const ggONB3& o1, const ggONB3 &o2);

//
// Input / Output Operator
//
istream &operator>>(istream &is, ggONB3 &t);
ostream &operator<<(ostream &os, const ggONB3 &t);


#endif
