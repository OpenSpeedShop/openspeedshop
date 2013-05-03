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
// ggFrame2.h -- Frames defined by a point and an orthonormal basis
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

#ifndef GGFRAME2_H
#define GGFRAME2_H

#include <iostream.h>
#include <math.h>
#include <ggMacros.h>
#include <ggPoint2.h>
#include <ggONB2.h>

#define DUMP_CORE() {int a=*(char *)1; exit(1);}

//
// ggFrame2 interface
//

class ggFrame2 {
public:

	ggFrame2() {}
		// origin and basis set respectively
		// to default ggPoint2 and default ggFrame2

	ggFrame2( const ggFrame2& f ) { Set(f.origin(), f.basis()); }
	ggFrame2( const ggPoint2& p, const ggONB2& basis ) { Set( p, basis ); }

	void Set( const ggPoint2& p, const ggONB2& bas ) { o = p; uv = bas; }
	void SetOrigin( const ggPoint2& p ) { o = p; }
	void SetBasis( const ggONB2& bas )  { uv = bas; }

	ggPoint2 origin() const { return o; }
	ggONB2 basis() const    { return uv; }

protected:
	ggPoint2 o;
	ggONB2 uv;
};

//
// Declarations
//

const ggFrame2 ggCanonicalFrame2(ggOrigin2, ggCanonicalBasis2);


ggBoolean operator==( const ggFrame2 &f1, const ggFrame2 &f2 );

istream & operator>>( istream & is, ggFrame2 & t );
ostream &operator<<( ostream &os, const ggFrame2 &t );

//
// Friend
//

//
// Non-friends
//

#endif
