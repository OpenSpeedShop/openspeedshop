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



#include <iostream.h>
#include <ggRange.h>
#include <ggRGBE.h>
#include <ggRGBFPixel.h>




/******************************************************************************
* ggRGBFPixel
*/

float ggRGBFPixel::tolerance = 1E-4;

ggRGBFPixel ggRGBFPixel::ConvertFromRGBE(ggRGBE& a)
{
    double data[3];

    a.RGBEToDouble(data);
    r() =  data[0];
    g() =  data[1];
    b() =  data[2];

    return *this;
}

ggRGBE ggRGBFPixel::ConvertToRGBE()
{
    double data[3];
    data[0] = (double) e[0];
    data[1] = (double) e[1];
    data[2] = (double) e[2];
    return ggRGBE(data[0], data[1], data[2]);
}



/*
 * Conversions
 */


/*
 * Operators
 */

ggBoolean operator==(const ggRGBFPixel &t1, const ggRGBFPixel &t2) {
return ggEqual(t1.r() ,  t2.r() , t1.tol())
     && ggEqual(t1.g() ,  t2.g() , t1.tol())
     && ggEqual(t1.b(),  t2.b(), t1.tol()); }

ggBoolean operator!=(const ggRGBFPixel &t1, const ggRGBFPixel &t2)
    { return !(t1 == t2); }

ostream &operator<<(ostream &os, const ggRGBFPixel &t)
  { os << t.r() << " " << t.g() << " " << t.b();
  cerr << t.r() << t.g() << t.b() << "\n";
  return os; }

istream &operator>>(istream &is, ggRGBFPixel &t)
  { is >> t.r() >> t.g() >> t.b();  return is; }

