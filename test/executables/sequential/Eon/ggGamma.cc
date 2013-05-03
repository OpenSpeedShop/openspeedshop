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
// ggGamma.c -- implementation of the ggGamma class
//
// Author:    Greg Ward
//
// Modified:  Tom Loos and Beata Winnicka for use in the IU library 
//            11/1/93
//
// Copyright 1993 by Greg Ward (used by permission)
//                   Tom Loos and Beata Winnicka for IU modifications
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
//
//
#include <ggGamma.h>

ggGamma::ggGamma()
{
    gamma = ggDefaultGamma;
    SetGammaTables();
}
ggGamma::ggGamma(double g)
{
    gamma = g;
    SetGammaTables();
}

void ggGamma::SetGamma(double g)
{
    gamma = g;
    SetGammaTables();
}

void ggGamma::RGBEToGamma( ggRGBE *scanline, int count)
{
    int i, expo;
    while (count-- > 0)
    {
	expo = scanline[0][3] - 128;
	if (expo < -GGMAXGSHIFT) //if really small
	{
	    // if too small, well, it's 0
	    if (expo < -GGMAXGSHIFT-8 )
	    {
		scanline[0][0] = scanline[0][1] = scanline[0][2] = 0;
	    }
	    else // try to get from really small to too small
	    {
		i = (-GGMAXGSHIFT-1) - expo;
		scanline[0][0] = 
		    RGBEToGam[GGMAXGSHIFT][((scanline[0][0]>>i)+1)>>1];
		scanline[0][1] = 
		    RGBEToGam[GGMAXGSHIFT][((scanline[0][1]>>i)+1)>>1];
		scanline[0][2] = 
		    RGBEToGam[GGMAXGSHIFT][((scanline[0][2]>>i)+1)>>1];
	    }
	}
	else if (expo > 0) // else positive i.e. too big for gamma
	{
	    if (expo > 8) // way too big
	    {
		scanline[0][0] = scanline[0][1] = scanline[0][2] = 255;
	    }
	    else // just big
	    {
		// red = (red * 2 + 1) << exponent - 1
		i = (scanline[0][0] << 1 | 1) << (expo - 1);
		// if i is too big, clamp on top, otherwise gamma correct
		scanline[0][0] = (i > 255) ? 255 : RGBEToGam[0][i];

		i = (scanline[0][1] << 1 | 1) << (expo - 1);
		scanline[0][1] = (i > 255) ? 255 : RGBEToGam[0][i];

		i = (scanline[0][2] << 1 | 1) << (expo - 1);
		scanline[0][2] = (i > 255) ? 255 : RGBEToGam[0][i];
	    }
	}
	else // normal (gamma corrected case)
	{
	    scanline[0][0] = RGBEToGam[-expo][scanline[0][0]];
	    scanline[0][1] = RGBEToGam[-expo][scanline[0][1]];
	    scanline[0][2] = RGBEToGam[-expo][scanline[0][2]];
	}
	// all have the same exponent 2**0
	scanline[0][3] = 128;
	//printf("outscan: %d, %d, %d: expo = %d\n",scanline[0][0],scanline[0][1],scanline[0][2],expo);
	scanline++;
    }
}

void ggGamma::GammaToRGBE( ggRGBE *scanline, int count)
{
    int minexpo;
    int expo_red, expo_green, expo_blue;

    while (count-- > 0)
    {
	expo_red = GamToRGBEExponent[scanline[0][0]];
	expo_green = GamToRGBEExponent[scanline[0][1]];
	expo_blue = GamToRGBEExponent[scanline[0][2]];
	minexpo = expo_red;
	if (minexpo > expo_green)
	    minexpo = expo_green;
	if (minexpo > expo_blue)
	    minexpo = expo_blue;
	
	if (minexpo < expo_red)
	    scanline[0][0] = GamToRGBEMantissa[scanline[0][0]] >> (expo_red - minexpo);
	else
	    scanline[0][0] = GamToRGBEMantissa[scanline[0][0]];

	if (minexpo < expo_green)
	    scanline[0][1] = GamToRGBEMantissa[scanline[0][1]] >> (expo_green - minexpo);
	else
	    scanline[0][1] = GamToRGBEMantissa[scanline[0][1]];

	if (minexpo < expo_blue)
	    scanline[0][2] = GamToRGBEMantissa[scanline[0][2]] >> (expo_blue - minexpo);
	else
	    scanline[0][2] = GamToRGBEMantissa[scanline[0][2]];
	
	scanline[0][3] = 128 - minexpo;
	scanline++;
    }
}

void ggGamma::SetGammaTables()
{
    double multiplier; 
    int i,j;

    // do RGBE to Gamma
    for (i = 0; i <= GGMAXGSHIFT; i++)
    {
	multiplier = pow(0.5, (double) (i+8));
	for (j = 0; j < 256; j++)
	{
	    RGBEToGam[i][j] = (unsigned char) (256.0 * pow((j+0.5)*multiplier, 1.0/gamma));
//	    if (i == 0)
//		printf("j = %d, RTG = %d, gamma = %f, multiplier = %f,raw = %f\n",j,RGBEToGam[i][j],gamma,multiplier, pow((j+0.5)*multiplier,1.0/gamma));
	}

    }

    // do Gamma to RGBE
    i = 0;
    multiplier = 256.0;
    for (j = 255; j > 0; j--)
    {
	// get the mantissa and exponent straight
	while ((GamToRGBEMantissa[j] = (int) (multiplier * pow(j/256.0, gamma))) < 128)
	{
	    i++;
	    multiplier *= 2.0;
	}
	// store right exponent
	GamToRGBEExponent[j] = i;
    }
    // do the right thing for 0
    GamToRGBEMantissa[0] = 0;
    GamToRGBEExponent[0] = 128;
}

