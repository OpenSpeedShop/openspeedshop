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
#include <ggRange.h>
#include <ggRGBE.h>



/******************************************************************************
* ggRGBPixel
*/

template <class T>
ggRGBPixel<T> ggRGBPixel<T>::ConvertFromRGBE(ggRGBE& a)
{
    double data[3];

    a.RGBEToDouble(data);
    r() = (T) data[0];
    g() = (T) data[1];
    b() = (T) data[2];

    return *this;
}

template <class T>
ggRGBE ggRGBPixel<T>::ConvertToRGBE()
{
    double data[3];
    data[0] = (double) e[0];
    data[1] = (double) e[1];
    data[2] = (double) e[2];
    return ggRGBE(data);
}



/*
 * Conversions
 */

template <class T>
ggRGBPixel<T>::ggRGBPixel(const ggRGBPixel<char> &p) {
    r() = (T) p.r();
    g() = (T) p.g();
    b() = (T) p.b();
}

template <class T>
ggRGBPixel<T>::ggRGBPixel(const ggRGBPixel<unsigned char> &p) {
    r() = (T) p.r();
    g() = (T) p.g();
    b() = (T) p.b();
}

template <class T>
ggRGBPixel<T>::ggRGBPixel(const ggRGBPixel<short> &p) {
    r() = (T) p.r();
    g() = (T) p.g();
    b() = (T) p.b();
}

template <class T>
ggRGBPixel<T>::ggRGBPixel(const ggRGBPixel<unsigned short> &p) {
    r() = (T) p.r();
    g() = (T) p.g();
    b() = (T) p.b();
}

template <class T>
ggRGBPixel<T>::ggRGBPixel(const ggRGBPixel<int> &p) {
    r() = (T) p.r();
    g() = (T) p.g();
    b() = (T) p.b();
}

template <class T>
ggRGBPixel<T>::ggRGBPixel(const ggRGBPixel<unsigned int> &p) {
    r() = (T) p.r();
    g() = (T) p.g();
    b() = (T) p.b();
}

template <class T>
ggRGBPixel<T>::ggRGBPixel(const ggRGBPixel<long> &p) {
    r() = (T) p.r();
    g() = (T) p.g();
    b() = (T) p.b();
}

template <class T>
ggRGBPixel<T>::ggRGBPixel(const ggRGBPixel<float> &p) {
    r() = (T) p.r();
    g() = (T) p.g();
    b() = (T) p.b();
}

template <class T>
ggRGBPixel<T>::ggRGBPixel(const ggRGBPixel<double> &p) {
    r() = (T) p.r();
    g() = (T) p.g();
    b() = (T) p.b();
}



/*
 * Operators
 */

template <class T1, class T2>
ggBoolean operator==(const ggRGBPixel<T1> &t1, const ggRGBPixel<T2> &t2) {
return ggEqual(t1.r() , (T1) t2.r() , t1.tol())
     && ggEqual(t1.g() , (T1) t2.g() , t1.tol())
     && ggEqual(t1.b(), (T1) t2.b(), t1.tol()); }

template <class T1, class T2>
ggBoolean operator!=(const ggRGBPixel<T1> &t1, const ggRGBPixel<T2> &t2)
    { return !(t1 == t2); }

template <class T>
ostream &operator<<(ostream &os, const ggRGBPixel<T> &t)
  { os << t.r() << " " << t.g() << " " << t.b();  return os; }

template <class T>
istream &operator>>(istream &is, ggRGBPixel<T> &t)
  { is >> t.r() >> t.g() >> t.b();  return is; }


//template <class T>
//unsigned char ggRGBPixel<T>::tolerance;

/*
 * Methods
 */

