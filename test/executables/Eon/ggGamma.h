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
// ggGamma.h -- definition of the ggGamma class
//
// Author:    Greg Ward
//
// Modified: Tom Loos 11/1/93 to fit into iu library
//
//
// Copyright 1993 by Greg Ward (used by permission)
//            and by Tom Loos and Beata Winnicka for IU modifications
//               
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
//
//
#ifndef GG_GAMMA
#define GG_GAMMA
#include <ggRGBE.h>

//
// this is adapted from Ward's colorops.c code and is designed
// for use with his file formats
//
// hmmm,
#ifdef os390
double pow(double,double);
#endif

const int GGMAXGSHIFT=15;
//const double ggDefaultGamma=2.2; // Note: THIS DOESN'T Work?!?

// NOTE: this works
#define ggDefaultGamma 2.2 
class ggGamma
{
public:
    ggGamma();
    ggGamma(double g);
    void SetGamma( double gamma );
    double GetGamma() { return gamma; };
    void RGBEToGamma( ggRGBE *scanline, int count);
    void GammaToRGBE( ggRGBE *scanline, int count);
protected:
    double gamma;
    unsigned char RGBEToGam[GGMAXGSHIFT+1][256];
    unsigned char GamToRGBEMantissa[256];
    unsigned char GamToRGBEExponent[256];
    void SetGammaTables();
};

#endif
