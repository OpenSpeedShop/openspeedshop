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
// ggRGBE.h -- definition of class ggRGBE
//
// Author:    Tom Loos 
//            Beata Winnicka
//            10/1/93
// Modified:
//
//
// Copyright 1993 by Tom Loos and Beata Winnicka
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
//
//
#ifndef GGRGBE_H
#define GGRGBE_H
#include <ggBoolean.h>    
#include <iostream.h>
#include <math.h>
typedef unsigned char ggRGBEType;
const unsigned char ggRGBEBase = 128;

class ggRGBE
{
public:

    //
    // constructors
    //

    ggRGBE(); // this will do a null assign
    ggRGBE(float r, float g,float b); // this is float->rgbe
    ggRGBE(float rgb[3]);

    ggRGBE(double r,double g,double b); // this is double->rgbe
    ggRGBE(double rgb[3]);

    ggRGBE(int r,int g,int b); // this is int->rgbe
    ggRGBE(int rgb[3]);

    //
    // destructors
    //
    ~ggRGBE(); // use default?

    //
    // access operators/methods
    //

    ggRGBEType operator[](int i) const;
    ggRGBEType& operator[](int i);
    ggRGBE operator=(const ggRGBE& b);
    ggBoolean operator==(ggRGBE& b);



//    inline friend ostream &operator<<(ostream os)
//    {
//	os << ((int) data[0]) << (int) data[1] << (int) data[2] << (int) data[3];
//	return os;
//    }

//    inline friend istream &operator>>(istream is)
//{
//    is >> data[0] >> data[1] >> data[2] >> data[3];
//    return is;
//}
    void print();

    //
    // miscellaneous methods
    //
    void Normalize(int rgb[4]);
    void AdjustCenter(ggRGBEType base);
    void ShiftExponent(int p) {
	if (((data[3]+p) > 0) && ((data[3]+ p) < 255))
		data[3] += p;
	else {
		data[3] = (data[3]+p <= 0) ? 0 : 255;
	}
    }
      
    void FloatToRGBE( float rgb[3]);
    void DoubleToRGBE( double rgb[3]);
    void IntToRGBE( int rgb[3]);
    void RGBEToDouble( double a[3]) const;
    void RGBEToFloat( float a[3]) const;
    void RGBEToInt( int a[3]) const;
    ggRGBE ConvertToRGBE() { return *this; };
    ggRGBE ConvertFromRGBE(ggRGBE& a) { *this = a; return *this; };
protected:
// KAI 12/13/94
// defined above already. sunproc CC does not like it redefined here
//    typedef unsigned char ggRGBEType;
    
    // note: 
    // data [0] == red
    // data [1] == green
    // data [2] == blue
    // data [3] == exponent
    
    ggRGBEType data[4];
};

// out classed methods
ggRGBE operator+(const ggRGBE&a, const ggRGBE& b);
ggRGBE operator*(const ggRGBE&a, const ggRGBE& b);
// subtraction will clamp to 0
ggRGBE operator-(const ggRGBE&a, const ggRGBE& b);
ggRGBE operator/(const ggRGBE&a, const ggRGBE& b);
//

#endif

