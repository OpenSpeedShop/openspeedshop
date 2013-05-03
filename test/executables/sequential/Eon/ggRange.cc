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



/* $Id$ */



#include <iostream.h>
#include <ggBoolean.h>



/******************************************************************************
* ggRange
*/



/*
 * Constructors
 */



/*
 * Operators
 */



/*
 * Methods
 */



/*
 * Non-friends
 */

template <class T1, class T2, class T3>
T3
ggScaleIntegralToIntegral(const T1 x, const ggRange<T2> &domain,
 const ggRange<T3> &range) {

    T3 y = (T3) (((2*x - 2*domain.x0() + 1)*(range.x1() - range.x0() + 1))
     /(2*(domain.x1() - domain.x0() + 1)) + range.x0());

    if (y < range.x0())
        y = range.x0();
    else if (y > range.x1())
        y = range.x1();

    return y;
}

template <class T1, class T2, class T3>
T3
ggScaleIntegralToReal(const T1 x, const ggRange<T2> &domain,
 const ggRange<T3> &range) {

    T3 y = (T3) ((x - domain.x0() + 0.5)*((range.x1() - range.x0())
     /(domain.x1() - domain.x0() + 1)) + range.x0());

    if (y < range.x0())
        y = range.x0();
    else if (y > range.x1())
        y = range.x1();

    return y;
}

template <class T1, class T2, class T3>
T3
ggScaleRealToIntegral(const T1 x, const ggRange<T2> &domain,
 const ggRange<T3> &range) {

    T3 y = (T3) ((x - domain.x0())*((range.x1() - range.x0() + 1)
     /(domain.x1() - domain.x0())) + range.x0());

    if (y < range.x0())
        y = range.x0();
    else if (y > range.x1())
        y = range.x1();

    return y;
}

template <class T1, class T2, class T3>
T3
ggScaleRealToReal(const T1 x, const ggRange<T2> &domain,
 const ggRange<T3> &range) {

    T3 y = (T3) ((x - domain.x0())*((range.x1() - range.x0())
     /(domain.x1() - domain.x0())) + range.x0());

    if (y < range.x0())
        y = range.x0();
    else if (y > range.x1())
        y = range.x1();

    return y;
}

template <class T>
ostream &
operator<<(ostream &os, const ggRange<T> &r) {
    return os << "(" << r.x0() << ", " << r.x1() << ")";
}
