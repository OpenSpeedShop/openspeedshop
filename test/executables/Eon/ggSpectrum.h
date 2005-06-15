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
// ggSpectrum-- declarations for class ggSpectrum
//
// Author:    Pete Shirley
//            10/30/93
// Modified:
//
#ifndef GGSPECTRUM_H
#define GGSPECTRUM_H

#include <iostream.h>
#include <ggMacros.h>


class ggSpectrum {
public:
    ggSpectrum() {Set(-1.0);}
    ggSpectrum(float *d);
    ggSpectrum(float d) {Set(d);}
    void Set(float);
    void SetBlack() {Set(0.0);}
    static int nComponents() {return NLAMBDA;};
    double wavelength(int i) const { return (MINLAMBDA + i * deltaLambda()); }
#ifdef GGSAFE
    float operator[] (int i) const { return data[i]; }
    float& operator[] (int i)      { return data[i]; }
#else
    float operator[] (int i) const { assert (i >= 0 && i < NLAMBDA);
                                     return data[i]; }
    float& operator[] (int i)      { assert (i >= 0 && i < NLAMBDA);
#if DEBUG&0x01
    if( i==7 || i==6 ) {
      cout << "Asking for ggSpectrum["<<i<<"]" << endl; }
#endif
                                     return data[i]; }
#endif
    float amplitude(double wavelength) const;
    double deltaLambda() const {return width;}
    static int minLambda() {return MINLAMBDA;}
    static int maxLambda() {return MAXLAMBDA;}
    double area() const;
    ggBoolean dark() const;
    ggSpectrum& operator=(const ggSpectrum&);
    ggSpectrum& operator-=(const ggSpectrum&);
    ggSpectrum& operator+=(const ggSpectrum&);
    ggSpectrum& operator*=(const ggSpectrum&);
    ggSpectrum& operator*=(float);
    ggSpectrum& operator/=(float);
    ggSpectrum& operator/=(const ggSpectrum&);
// Change back to protected later.  -- Ken
//protected:
public:
    enum { MINLAMBDA = 400 };  // hack-- static constant
    enum { MAXLAMBDA = 700 };
    enum { NLAMBDA = 8 }; 
    float data[NLAMBDA];
    static double width;
#if defined DEBUG
  static int assign_instance;
  static int set_instance;
#endif
};


istream& operator>>(istream&, ggSpectrum&);
ostream& operator<<(ostream&, const ggSpectrum&);
ggBoolean operator==(const ggSpectrum& s1, const ggSpectrum& s2);
ggBoolean operator!=(const ggSpectrum& s1, const ggSpectrum& s2);


ggSpectrum operator-(const ggSpectrum& u);
ggSpectrum operator+(const ggSpectrum& u, const ggSpectrum& v);
ggSpectrum operator-(const ggSpectrum& u, const ggSpectrum& v);
ggSpectrum operator*(const ggSpectrum& u, const ggSpectrum& v);
ggSpectrum operator/(const ggSpectrum& u, const ggSpectrum& v);
ggSpectrum operator/(float f, const ggSpectrum& v);
ggSpectrum operator*(const ggSpectrum& v, float d);
ggSpectrum operator*(float d, const ggSpectrum& v);
ggSpectrum operator/(const ggSpectrum& v, float d);






#endif
