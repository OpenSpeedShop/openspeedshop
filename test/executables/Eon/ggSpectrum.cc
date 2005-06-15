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
// ggSpectrum.C-- declarations for class ggSpectrum
//
// Author:    Peter Shirley
//            10/30/93
// Modified:
//
//
// Copyright 1993 by Peter Shirley  
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
//

#if defined(DEBUG)
#include <stdio.h>
#endif
#include <math.h>
#include <assert.h>
#include <ggSpectrum.h>
#include <ggFineSpectrum.h>

double ggSpectrum::width = (ggSpectrum::maxLambda() - ggSpectrum::minLambda())/
#if 1
                             double(ggSpectrum::nComponents());
#else
                             double(ggSpectrum::nComponents()-1);
#endif

ggBoolean ggSpectrum::dark() const {
     for (int i = 0; i < nComponents(); i++)
         if (data[i] > 0.00001) return ggFalse;
     return ggTrue;
}

ggBoolean operator==(const ggSpectrum& s1, const ggSpectrum& s2) {
     for (int i = 0; i < s1.nComponents(); i++)
         if (ggEqual(s1[i],s2[i],0.0001)) ; else return ggFalse;
     return ggTrue;
}

ggBoolean operator!=(const ggSpectrum& s1, const ggSpectrum& s2) {
    return !(s1 == s2);
}


ggSpectrum::ggSpectrum(float a[]) { 
  //  if( a[6]!=a[7] ) {
  //    cout << "a[6]=" << a[6] << " << a[7]=" << a[7] << endl;
  //  }
   for (int i = 0; i < nComponents(); i++) 
      data[i] = a[i];
}

#if DEBUG & 0x1
int ggSpectrum::set_instance = 0;
int ggSpectrum::assign_instance = 0;
#endif

void ggSpectrum::Set(float d) {
#if DEBUG & 0x1
  set_instance++;
  if( d>=0.0 ) {
    cout << "ggSpectrum::Set(" << set_instance << ") to " << d << endl;
  }
#endif
   for (int i = 0; i < nComponents(); i++) 
      data[i] = d;
}

float ggSpectrum::amplitude(double lambda) const {
   if (lambda <= minLambda()) return data[0];
   else if (lambda >= maxLambda()) return data[nComponents() - 1];
   else
   {
     // intervals and endpoints seem to be confused.
     // we have nComponents()-1 intervals, but nComponents() endpoints.
     // However, this change seems to make no change in the output,
     // so leave the original (Use the endpoint count)
#if 1
       double t = (nComponents()) *
#else
       double t = (nComponents()-1) *
#endif
                (lambda - minLambda()) / (maxLambda() - minLambda());
       int it = int(t);
       assert( (it>=0) && ((it+1)<nComponents()) );
       double dt = t - it;
       return (1 - dt)*data[it] + dt * data[it+1]; 
   }
}

double ggSpectrum::area() const
{
   double a = 0.0;
   for (int i = 0; i < nComponents(); i++) 
      a += data[i];
   return a * deltaLambda(); 
}


ggSpectrum& ggSpectrum::operator=(const ggSpectrum& c) {
#if DEBUG & 0x1
  assign_instance++;
  //  fprintf( stderr, "ggSpectrum=, was %8.3g, %8.3g, ..., %8.3g, %8.3g\n",
  //	  data[0], data[1], data[nComponents()-1], data[nComponents()] );
  cout << "ggSpectrum=(" << assign_instance << ")  is " << c << endl ;
#endif
   for (int i = 0; i < nComponents(); i++) 
      data[i] = c[i];
   return *this;
}

ggSpectrum& ggSpectrum::operator-=(const ggSpectrum& c)
{
   for (int i = 0; i < nComponents(); i++) 
      data[i] -= c.data[i];
   return *this;
}

ggSpectrum& ggSpectrum::operator+=(const ggSpectrum& c)
{
   for (int i = 0; i < nComponents(); i++) 
      data[i] += c.data[i];
   return *this;
}

ggSpectrum& ggSpectrum::operator*=(const ggSpectrum& c) {
   for (int i = 0; i < nComponents(); i++) 
      data[i] *= c.data[i];
   return *this;
}

ggSpectrum& ggSpectrum::operator*=(float c) {
   for (int i = 0; i < nComponents(); i++) 
      data[i] *= c;
   return *this;
}

ggSpectrum& ggSpectrum::operator/=(const ggSpectrum& c) {
   for (int i = 0; i < nComponents(); i++) {
#ifdef GGSAFE
    assert(fabs(c[i]) > 0.0000001);
#endif
      data[i] /= c[i];
   }
   return *this;
}

ggSpectrum& ggSpectrum::operator/=(float  c) {
#ifdef GGSAFE
    assert(fabs(c) > 0.0000001);
#endif
   for (int i = 0; i < nComponents(); i++) 
      data[i] /= c;
   return *this;
}


istream& operator>>(istream& stream, ggSpectrum& spectrum)
{
   ggFineSpectrum fs;
   stream >> fs;
   double k = 1.0 / spectrum.deltaLambda();
   for (int i = 0; i < spectrum.nComponents(); i++)
      spectrum[i] = k*fs.area(spectrum.wavelength(i), spectrum.wavelength(i+1));
   return stream;
}

ostream& operator<<(ostream& stream, const ggSpectrum& spectrum) {
   for (int i = 0; i < spectrum.nComponents(); i++) { 
     stream << "<" <<int(spectrum.wavelength(i)) + 1 << ":" ;
     stream << spectrum[i] << "> ";
   }
   stream << endl;
    return stream;
}


ggSpectrum operator+(const ggSpectrum& u, const ggSpectrum& v) {
    ggSpectrum c;
    for (int i = 0; i < c.nComponents(); i++) 
        c[i] = u[i] + v[i];
    return c;
}


ggSpectrum operator-(const ggSpectrum& u, const ggSpectrum& v)
{
    ggSpectrum c;
    for (int i = 0; i < c.nComponents(); i++) 
        c[i] = u[i] - v[i];
    return c;
}


ggSpectrum operator*(const ggSpectrum& u, const ggSpectrum& v)
{
    ggSpectrum c;
    for (int i = 0; i < c.nComponents(); i++) 
        c[i] = u[i] * v[i];
    return c;
}


ggSpectrum operator/(float f, const ggSpectrum& v)
{
    ggSpectrum c;
    for (int i = 0; i < c.nComponents(); i++)  {
#ifdef GGSAFE
    assert(fabs(v[i]) > 0.0000001);
#endif
        c[i] = f / v[i];
    }
    return c;
}

ggSpectrum operator/(const ggSpectrum& u, const ggSpectrum& v)
{
    ggSpectrum c;
    for (int i = 0; i < c.nComponents(); i++)  {
#ifdef GGSAFE
    assert(fabs(v[i]) > 0.0000001);
#endif
        c[i] = u[i] / v[i];
    }
    return c;
}


ggSpectrum operator*(const ggSpectrum& v, float d)
{
    ggSpectrum c;
    for (int i = 0; i < c.nComponents(); i++) 
        c[i] = d * v[i];
    return c;
}


ggSpectrum operator*(float d, const ggSpectrum& v)
{
    ggSpectrum c;
    for (int i = 0; i < c.nComponents(); i++) 
        c[i] = d * v[i];
    return c;
}


ggSpectrum operator-(const ggSpectrum& v)
{
    ggSpectrum c;
    for (int i = 0; i < c.nComponents(); i++) 
        c[i] = - v[i];
    return c;
}


ggSpectrum operator/(const ggSpectrum& v, float d)
{
#ifdef GGSAFE
    assert(fabs(d) > 0.0000001);
#endif
    ggSpectrum c;
    float f = 1.0 / d;
    for (int i = 0; i < c.nComponents(); i++) 
        c[i] = v[i] * f;
    return c;
}



