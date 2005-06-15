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
// ggONB2.C -- Orthonormal basis
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

#include <ggMacros.h>
#include <ggONB2.h>

//
// To be substituted by macros
//

#define DUMP_CORE() {int a=*(char *)1; exit(1);}

//
//---------- TESTING PROCEDURES FOR GGSAFE ----------
//

#ifdef GGSAFE

//
// Exit with error message
//
static void exitBadInputVector( char *func_name, const ggVector2 & u )
{
	cerr << "ggONB2::" << func_name << ": Cannot make an ONB from vector: " << u << "\n";
	DUMP_CORE();
}

static void exitBadInputVectors2( char *func_name, const ggVector2 & u, const ggVector2 v )
{
	cerr << "ggONB2::" << func_name << ": Cannot make an ONB from vectors: " << u << ", " << v << "\n";
	DUMP_CORE();
}

inline ggBoolean isNulVector( const ggVector2 & v, double epsilon )
{
	if(	ggIsZero( v.x(), epsilon ) &&
			ggIsZero( v.y(), epsilon )
	)
		return ggTrue;
	else
		return ggFalse;
}

#endif

//
//---------- CLASS MEMBER FUNCTIONS ----------
//


ggONB2 & ggONB2::InitFromU( const ggVector2& u )
{
	U = ggUnitVector( u );
	V = ggVector2( -U.y(), U.x() );
		// angle <u,v> = +90 degres (trigonometrical angle)

#ifdef GGSAFE
	if( !this->isONB2() )
		exitBadInputVector( "InitFromU", u );
			// The vector given as parameter was probably null
#endif

	return *this;
}
		
ggONB2 & ggONB2::InitFromV( const ggVector2& v )
{
	V = ggUnitVector( v );
	U = ggVector2( V.y(), -V.x() );
		// angle <u,v> = +90 degres (trigonometrical angle)

#ifdef GGSAFE
	if( !this->isONB2() )
		exitBadInputVector( "InitFromV", v );
			// The vector given as parameter was probably null
#endif

	return *this;
}

#ifdef GGSAFE

ggBoolean ggONB2::isONB2( )
{
	if(
		ggEqual( u().length(), 1.0, ggTinyEpsilon ) &&
		ggEqual( v().length(), 1.0, ggTinyEpsilon ) &&
		ggEqual( v().x(), -u().y(), ggTinyEpsilon ) &&
		ggEqual( v().y(),  u().x(), ggTinyEpsilon )
	)
		return ggTrue;
			// To be valid the ONB2 has to be well oriented (trigonometrical sense)
	else
		return ggFalse;
}
#endif

//
// == operator
//
ggBoolean  operator==( const ggONB2 & o1, const ggONB2 & o2 )
{
	return(
		o1.u() == o2.u() &&
		o1.v() == o2.v()
	);
}

//
// Input operator
// The ONB2 is recalculated from one vector in order to be more precise
//
istream & operator>>( istream & is, ggONB2 & t )
{
	ggVector2 new_u, new_v;

	is >> new_u >> new_v;

	t.InitFromU( new_u );

#ifdef GGSAFE
	if(  !isNulVector( t.v() - new_v, ggBigEpsilon )  )
	{
		exitBadInputVectors2( ">>", new_u, new_v );
			// The three vectors read do not form an orthonormal basis
	}
#endif

	return is;
}

//
// Output operator
//
ostream & operator<<( ostream & os, const ggONB2 & t )
{
	os << t.u() << "\n" << t.v() << "\n"; 
	return os;
}

