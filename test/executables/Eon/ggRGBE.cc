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
// ggRGBE.c -- implementation of class ggRGBE
//
// Author:    Tom Loos 
//            Beata Winnicka
//            10/1/93
// Modified:
//
//
// Copyright 1993 by Beata Winnicka and Tom Loos
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
//
//

// #includes
#include <ggRGBE.h>

// utility functions that I didn't template
inline int imax( int a, int b)
{
    if (a > b)
	return a;
    else
	return b;
}

inline double dmax( double a, double b)
{
    if (a > b)
	return a;
    else
	return b;
}
#ifdef FMAX_IS_DOUBLE
inline double fmax( double a, double b)
#else
inline float fmax( double a, double b)
#endif
{
    if (a > b)
	return a;
    else
	return b;
}

//
// constructors
//

//null assign
ggRGBE::ggRGBE()
{
    data[0] = data[1] = data[2] = data[3] = 0;
}

// this is float->rgbe
//inline 
ggRGBE::ggRGBE(float r, float g, float b)
{
    float rgb[3];
    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;

    FloatToRGBE(rgb);
}

//inline 
ggRGBE::ggRGBE(float r[3])
{
    FloatToRGBE(r);
}

//inline 
ggRGBE::ggRGBE(double r,double g,double b)
{
    double rgb[3];
    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;
    DoubleToRGBE(rgb);
}
//inline 
ggRGBE::ggRGBE(double rgb[3])
{
    DoubleToRGBE(rgb);
}
ggRGBE::ggRGBE(int r,int g,int b) 
{
    int rgb[3];
    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;
    IntToRGBE(rgb);
}
ggRGBE::ggRGBE(int rgb[3])
{
    IntToRGBE(rgb);
}

//inline 
void ggRGBE::FloatToRGBE(float rgb[3])
{
    float v;
    int e;
    
    v = fmax(rgb[0], (fmax(rgb[1], rgb[2])));
    if (v < 1e-32)
    {
	data[0] = data[1] = data[2] = data[3] = 0;
    }
    else
    {
	v = frexp(v,&e) * 256.0/v;
	data[0] = (ggRGBEType) (rgb[0] * v);
	data[1] = (ggRGBEType) (rgb[1] * v);
	data[2] = (ggRGBEType) (rgb[2] * v);
	data[3] = (ggRGBEType) (e + ggRGBEBase);
    }
}

//inline 
void ggRGBE::DoubleToRGBE(double rgb[3])
{
    double v;
    int e;
    
    v = dmax(rgb[0], dmax(rgb[1], rgb[2]));
    //printf("v = %f\n",v);
    
    if (v < 1e-32)
    {
	data[0] = data[1] = data[2] = data[3] = 0;
    }
    else
    {
	v = frexp(v,&e) * 256.0/v;
	data[0] = (ggRGBEType) (rgb[0] * v);
	data[1] = (ggRGBEType) (rgb[1] * v);
	data[2] = (ggRGBEType) (rgb[2] * v);
	data[3] = (ggRGBEType) (e + ggRGBEBase);
    }
}

// this loses a substantial amount of precision if
// any value is > 255 (up to 1/128)
//
// worst case: 256 65537 511
// converts to 0 128 0 * 2**9
// converts back to: 0 65536 0
// loses           256     1 511
//

void ggRGBE::AdjustCenter(unsigned char base)
{
    int e = data[3]-base;
    int d;

    if (e > 0)
	if (e >= 8)
	    data[0] = data[1] = data[2] = 255;
	else
	{
	    d = data[0] << e;
	    data[0] = ((d > 255) ? 255 : d); // hmmm
	    d = data[1] << e;
	    data[1] = ((d > 255) ? 255 : d); // hmmm
	    d = data[2] << e;
	    data[2] = ((d > 255) ? 255 : d); // hmmm
	}
    else 
    {
	if (e <= -8)
	    data[0] = data[1] = data[2] = 0;
	else
	{
	    e = -e;
	    data[0] = data[0] >> e;
	    data[1] = data[1] >> e;
	    data[2] = data[2] >> e;
	}
    }
    data[3] = base;
}

	
void ggRGBE::Normalize(int indata[4])
{
    int m;
    int e = 0;
    //print();
    m = imax(indata[0], imax(indata[1], indata[2]));
    if (m <= 0)
    {
	// if negative, set to 0
	data[0] = data[1] = data[2] = 0;
    }
    else if (m > 255)
    {
	while (m > 255)
	{
	    e++;
	    m=m>>1;
	}
    }
    data[0] = indata[0] >> e;
    data[1] = indata[1] >> e;
    data[2] = indata[2] >> e;

    // data 3 calc 
    if ((e + indata[3]) > 255)
    {
	data[3] = 255;
    }
    else
	data[3] = indata[3] + e;
}    

void ggRGBE::IntToRGBE(int rgb[3])
{
    double v;
    int e,m;
    m = imax(rgb[0], imax(rgb[1],rgb[2]));
    v = (double) m;
    if (v <= 1e-32)
    {
	data[0] = data[1] = data[2] = data[3] = 0;
    }
    else
    {
	v = frexp(v,&e) * 256.0/v;
	data[0] = (ggRGBEType) (rgb[0] * v);
	data[1] = (ggRGBEType) (rgb[1] * v);
	data[2] = (ggRGBEType) (rgb[2] * v);
	data[3] = (ggRGBEType) (e + ggRGBEBase);
    }
}

//
// destructors
//
ggRGBE::~ggRGBE()
{
    data[0] = data[1] = data[2] = data[3] = 0;
}

//
// in-class operators
//
ggBoolean ggRGBE::operator==(ggRGBE& b)
{
    if (&b == this)
	return ggTrue;
    return ((data[0] == b[0]) && 
	    (data[1] == b[1]) && 
	    (data[2] == b[2]) && 
	    (data[3] == b[3]));
}

ggRGBE ggRGBE::operator=(const ggRGBE& b)
{
    if (!(&b == this))
    {
	data[0] = b[0];
	data[1] = b[1];
	data[2] = b[2];
	data[3] = b[3];
    }
    return *this;
}

//
// need both []'s for LHS and RHS array accesses
//
ggRGBEType ggRGBE::operator[](int i) const // RHS
{
    return data[i];
}
ggRGBEType& ggRGBE::operator[](int i) // LHS
{
    return data[i];
}


//void ggRGBE::Set(int i, ggRGBEType d){
//#ifdef GGSAFE
//    if ((i <0) || (i > 3))
//    {
//	cerr << "address" << i << " out of range \n";
//	exit(-1);
//    }
//#endif GGSAFE
//    data[i] = d;
//}

//
// misc. stuff
//
 
// debug routine
void ggRGBE::print()
{
      cout << "data[0] = " << data[0]  << ", data[1] = " << data[1] << ", data[2] = " << data[2] << ", data[3] = " << data[3]  << endl;
//    printf("data[0] = %d, data[1] = %d, data[2] = %d, data[3] = %d\n",data[0],
//	   data[1], data[2], data[3]);
}

//
// conversions from RGBE to arrays of (double, float, and int)
//
void ggRGBE::RGBEToDouble( double frgb[3]) const
{
    double v;

    if (data[3] == 0)
    {
	frgb[0] = frgb[1] = frgb[2] = 0.0;
    }
    else
    {
	v = ldexp(1.0, (int) data[3]-(ggRGBEBase+8));
	frgb[0] = v * (data[0] + 0.5);
	frgb[1] = v * (data[1] + 0.5);
	frgb[2] = v * (data[2] + 0.5);
    }

}
void ggRGBE::RGBEToFloat( float frgb[3]) const
{
    float v;

    if (data[3] == 0)
    {
	frgb[0] = frgb[1] = frgb[2] = 0.0;
    }
    else
    {
	v = ldexp(1.0, (int)data[3]-(ggRGBEBase+8));
	
	frgb[0] = v * (data[0] + 0.5);
	frgb[1] = v * (data[1] + 0.5);
	frgb[2] = v * (data[2] + 0.5);
    }

}
void ggRGBE::RGBEToInt( int a[3]) const
{
    //int e = data[3] - ggRGBEBase;
    double v;

    if (data[3] == 0)
    {
	a[0] = a[1] = a[2] = 0;
    }
    else
    {
	v = ldexp(1.0, (int) data[3]-(ggRGBEBase+8));
	a[0] = (int) (v * (data[0] + 0.5));
	a[1] = (int) (v * (data[1] + 0.5));
	a[2] = (int) (v * (data[2] + 0.5));
    }

}

// 
// out-of-class operators
//
ggRGBE operator+(const ggRGBE& a, const ggRGBE& b)
{
    ggRGBE max(0,0,0), min(0,0,0);
    int indata[4];
    int expdiff;

    // figure out max and min
    if (b[3] > a[3])
    {
	max = b;
	min = a;
    }
    else
    {
	max = a;
	min = b;
    }
    
    // calculate differences in exponents
    expdiff = max[3] - min[3];
    //printf("expdiff = %d\n",expdiff);
    // if non-zero difference, downshift smaller RGBE
    if (expdiff != 0)
    {
	//min.Set(0, min[0] >> expdiff);
	min[0] = (min[0] >> expdiff);
	min[1] = (min[1] >> expdiff);
	min[2] = (min[2] >> expdiff);
    }
    
    // do the addition thing
    indata[0] = (min[0] + max[0]);
    indata[1] = (min[1] + max[1]);
    indata[2] = (min[2] + max[2]);
    indata[3] = max[3];
    //printf("op +: indata 0 = %d, indata 1 = %d, indata 2 = %d, indata 3 = %d\n",indata[0], indata[1],indata[2],indata[3]);
    min.Normalize(indata);
    //printf("op + : min after norm:\n");
    return (min);
}


//
// this is, of course, a brutally inefficient way to do this
// but it works and ggRGBE operators are not expected to be
// used much, so...
//
ggRGBE operator*(const ggRGBE& a, const ggRGBE& b)
{
    double da[3], db[3], dc[3];
    a.RGBEToDouble(da);
    b.RGBEToDouble(db);
    dc[0] = da[0] * db[0];
    dc[1] = da[1] * db[1];
    dc[2] = da[2] * db[2];
    return( ggRGBE(dc));
}

//
// see comment for *
//
ggRGBE operator/(const ggRGBE& a, const ggRGBE& b)
{
    double da[3], db[3], dc[3];
    a.RGBEToDouble(da);
    b.RGBEToDouble(db);
    dc[0] = da[0] / db[0];
    dc[1] = da[1] / db[1];
    dc[2] = da[2] / db[2];
    return( ggRGBE(dc));
}

ggRGBE operator-(const ggRGBE& a, const ggRGBE& b)
{
    ggRGBE max, min;
    int indata[4];
    int expdiff;

    // figure out max and min
    if (b[3] > a[3])
    {
	max = b;
	min = a;
    }
    else
    {
	max = a;
	min = b;
    }
    
    // calculate differences in exponents
    expdiff = max[3] - min[3];
    //printf("expdiff = %d\n",expdiff);
    // if non-zero difference, downshift smaller RGBE
    if (expdiff != 0)
    {
	//min.Set(0, min[0] >> expdiff);
	min[0] = (min[0] >> expdiff);
	min[1] = (min[1] >> expdiff);
	min[2] = (min[2] >> expdiff);
    }
    
    // do the subtraction
    indata[0] = (max[0] - min[0]);
    if (indata[0] < 0)
	indata[0] = 0;
    indata[1] = (max[1] - min[1]);
    if (indata[1] < 0)
	indata[1] = 0;
    indata[2] = (max[2] - min[2]);
    if (indata[2] < 0)
	indata[2] = 0;
    indata[3] = max[3];
    // printf("op -: indata 0 = %d, indata 1 = %d, indata 2 = %d, indata 3 = %d\n", indata[0], indata[1],indata[2],indata[3]);
    min.Normalize(indata);
    //printf("op - : min after norm:\n");
    //min.print();
    return (min);
}
 
