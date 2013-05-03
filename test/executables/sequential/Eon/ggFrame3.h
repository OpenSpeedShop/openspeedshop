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
// ggFrame3.h -- Frames defined by a point and an orthonormal basis
//
// Author:   Peter Shirley,    Feb 6, 1993
// Modified: Christophe Meyer, Nov 1, 1993
//

#ifndef GGFRAME3_H
#define GGFRAME3_H

#include <iostream.h>
#include <math.h>
#include <ggMacros.h>
#include <ggPoint3.h>
#include <ggONB3.h>

#define DUMP_CORE() {int a=*(char *)1; exit(1);}

//
// ggFrame3 interface
//

class ggFrame3 {
public:

	ggFrame3() {}
		// origin and basis set respectively
		// to default ggPoint3 and default ggFrame3

	ggFrame3( const ggFrame3& f ) { Set(f.origin(), f.basis()); }
	ggFrame3( const ggPoint3& p, const ggONB3& basis ) { Set( p, basis ); }

	void Set( const ggPoint3& p, const ggONB3& bas ) { o = p; uvw = bas; }
	void SetOrigin( const ggPoint3& p ) { o = p; }
	void SetBasis( const ggONB3& bas )  { uvw = bas; }

	ggPoint3 origin() const { return o; }
	ggONB3 basis() const    { return uvw; }

protected:
	ggPoint3 o;
	ggONB3 uvw;
};

const ggFrame3 ggCanonicalFrame3(ggOrigin3, ggCanonicalBasis3);


//
// Declarations
//

ggBoolean operator==( const ggFrame3 &f1, const ggFrame3 &f2 );

//
// Input / Output operators
//

istream & operator>>( istream & is, ggFrame3 & t );
ostream & operator<<( ostream &os, const ggFrame3 &t );

//
// Friend
//

//
// Non-friends
//

#endif
