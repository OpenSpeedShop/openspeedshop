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
// ggONB3.C -- Orthonormal basis
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
#include <ggGeometry.h>
#include <ggONB3.h>

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
static void exitBadInputVector( char *func_name, const ggVector3 & u )
{
	cerr << "ggONB3::" << func_name << ": Cannot make an ONB from vector: " << u << "\n";
	DUMP_CORE();
}

static void exitBadInputVectors2( char *func_name, const ggVector3 & u, const ggVector3 v )
{
	cerr << "ggONB3::" << func_name << ": Cannot make an ONB from vectors: " << u << ", " << v << "\n";
	DUMP_CORE();
}

static void exitBadInputVectors3( char *func_name, const ggVector3 & u, const ggVector3 v, const ggVector3 w )
{
	cerr << "ggONB3::" << func_name << ": Cannot make an ONB from vectors: " << u << ", " << v << ", " << w << "\n";
	DUMP_CORE();
}

inline ggBoolean isNulVector( const ggVector3 & v, double epsilon )
{
	if(	ggIsZero( v.x(), epsilon ) &&
			ggIsZero( v.y(), epsilon ) &&
			ggIsZero( v.z(), epsilon )
	)
		return ggTrue;
	else
		return ggFalse;
}

#endif

//
//---------- CLASS MEMBER FUNCTIONS ----------
//


ggONB3 & ggONB3::InitFromU( const ggVector3& u )
{
	U = ggUnitVector( u );
        V = ggUnitVector(ggPerpendicularVector(U));
	W = ggCross( U, V );

#ifdef GGSAFE
	if( !this->isONB3() )
		exitBadInputVector( "InitFromU", u );
			// u is probably the null vector
#endif

	return *this;
}
		
ggONB3 & ggONB3::InitFromV( const ggVector3& v )
{
	V = ggUnitVector( v );
        U = ggUnitVector(ggPerpendicularVector(V));
	W = ggCross( U, V );

#ifdef GGSAFE
	if( !this->isONB3() )
		exitBadInputVector( "InitFromV", v );
#endif

	return *this;
}

ggONB3 & ggONB3::InitFromW( const ggVector3& w )
{   
	W = ggUnitVector( w );

        U = ggUnitVector(ggPerpendicularVector(W));

	V = ggCross( W, U );

#ifdef GGSAFE
	if( !this->isONB3() )
		exitBadInputVector( "InitFromW", w );
#endif

	return *this;
}

//
// Calculate the ONB from two vectors
// The first one is the Fixed vector (it is just Normalized)
// The second is Normalized and its direction can be ajusted
//

ggONB3 & ggONB3::InitFromUV( const ggVector3& u, const ggVector3& v )
{
   U = ggUnitVector( u );
   W = ggUnitVector( ggCross(U, v) );
   V = ggCross( W, U);

#ifdef GGSAFE
	if( !this->isONB3() )
		exitBadInputVectors2( "InitFromUV", u, v );
			// 3 types of errors:
			//		- u is the null vector
			//		- v is the null vector
			//		- u // v
#endif

	return *this;
}

ggONB3 & ggONB3::InitFromVU( const ggVector3& v, const ggVector3& u )
{
   V = ggUnitVector( v );
   W = ggUnitVector( ggCross(u, V) );
   U = ggCross( V, W );

#ifdef GGSAFE
	if( !this->isONB3() )
		exitBadInputVectors2( "InitFromVU", v, u );
#endif

	return *this;
}

ggONB3 & ggONB3::InitFromUW( const ggVector3& u, const ggVector3& w )
{
   U = ggUnitVector( u );
   V = ggUnitVector( ggCross(w, U) );
   W = ggCross( U, V );

#ifdef GGSAFE
	if( !this->isONB3() )
		exitBadInputVectors2( "InitFromUW", u, w );
#endif

	return *this;
}

ggONB3 & ggONB3::InitFromWU( const ggVector3& w, const ggVector3& u )
{
   W = ggUnitVector( w );
   V = ggUnitVector( ggCross(W, u) );
   U = ggCross(V, W);

#ifdef GGSAFE
	if( !this->isONB3() )
		exitBadInputVectors2( "InitFromWU", w, u );
#endif

	return *this;
}

ggONB3 & ggONB3::InitFromVW( const ggVector3& v, const ggVector3& w )
{
   V = ggUnitVector( v );
   U = ggUnitVector( ggCross(V, w) );
   W = ggCross( U, V );

#ifdef GGSAFE
	if( !this->isONB3() )
		exitBadInputVectors2( "InitFromVW", v, w );
#endif

	return *this;
}

ggONB3 & ggONB3::InitFromWV( const ggVector3& w, const ggVector3& v )
{
   W = ggUnitVector( w );
   U = ggUnitVector( ggCross(v, W) );
   V = ggCross( W, U );

#ifdef GGSAFE
	if( !this->isONB3() )
		exitBadInputVectors2( "InitFromWV", w, v );
#endif

	return *this;
}


#ifdef GGSAFE
ggBoolean ggONB3::isONB3( )
{
	if(
		ggEqual( u().length(), 1.0, ggTinyEpsilon ) &&
		ggEqual( v().length(), 1.0, ggTinyEpsilon ) &&
		ggEqual( w().length(), 1.0, ggTinyEpsilon ) &&
		isNulVector( w() - ggCross(u(), v()), ggTinyEpsilon ) &&
		isNulVector( u() - ggCross(v(), w()), ggTinyEpsilon ) &&
		isNulVector( v() - ggCross(w(), u()), ggTinyEpsilon )
	)
		return ggTrue;
	else
		return ggFalse;
}
#endif

//
// == operator
//
ggBoolean  operator==( const ggONB3 & o1, const ggONB3 & o2 )
{
	return(
		o1.u() == o2.u() &&
		o1.v() == o2.v() &&
		o1.w() == o2.w()
	);
}

//
// Input operator
//
istream & operator>>( istream & is, ggONB3 & t )
{
	ggVector3 new_u, new_v, new_w;

	is >> new_u >> new_v >> new_w;

	t.InitFromUV( new_u, new_v );

#ifdef GGSAFE
	if(  !isNulVector( t.w() - new_w, ggBigEpsilon )  )
	{
		exitBadInputVectors3( ">>", new_u, new_v, new_w );
			// The three vectors read do not form an orthonormal basis
	}
#endif

	return is;
}

//
// Output operator
//
ostream & operator<<( ostream & os, const ggONB3 & t )
{
	os << t.u() << "\n" << t.v() << "\n" << t.w() << "\n"; 
	return os;
}

