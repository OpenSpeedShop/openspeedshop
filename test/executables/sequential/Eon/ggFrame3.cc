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
// ggFrame3.C -- Frames defined by a point and an orthonormal basis
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

#include <iostream.h>
#include <math.h>
#include <ggFrame3.h>



// == Operator
ggBoolean operator==(const ggFrame3 &f1, const ggFrame3 &f2) {
    return (f1.origin() == f2.origin() && f1.basis() == f2.basis());
}

#if defined(SPEC_STDCPP)
# define SPEC_SETSTATE(x,y) (x).setstate((y));
#else
# define SPEC_SETSTATE(x,y)
#endif
//
// Input operator
//
istream & operator>>( istream & is, ggFrame3 & t )
{
	ggPoint3 input_p;
	ggONB3   input_onb3;
        char c;

	is >> c;
        if (c != '(') {
          SPEC_SETSTATE(is,ios::failbit)
          return is;
        }
        is >> input_p;
        is >> c;
        if (c != ',') {
          SPEC_SETSTATE(is,ios::failbit)
          return is;
        }
        is >> input_onb3;
        is >> c;
        if (c != ')') {
          SPEC_SETSTATE(is,ios::failbit)
          return is;
        }

	t.Set( input_p, input_onb3 );

   return is;
}

// Output Operator
ostream &operator<<(ostream &os, const ggFrame3 &t)
{
    os << "( " << t.origin() << ",\n" << t.basis() << ") ";
    return os;
}
