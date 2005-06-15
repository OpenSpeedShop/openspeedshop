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
// ggIOhdr.h -- type definitions and include files for class ggIO
//
// Author:    Tom Loos 
//            11/1/93
// Modified:
//
//
// Copyright 1993 by SOMEBODY(S)  
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
//
//
#ifndef GGIOHDR_H
#define GGIOHDR_H
#include <ggRGBE.h>
#include <ggGamma.h>
#include <stdlib.h>
#include <string.h>

typedef enum {GGIN, GGOUT} ggIODirection;
typedef enum {GGNONE, GGPPM, GGPGM, GGRGBE, GGGE} ggFileType;
typedef enum {GGRED=0, GGGREEN=1, GGBLUE=2} ggChannel;
typedef enum {GGRAW, GGCOOKED} ggIOHandling;
const int ggPGMId=5; //grey scale
const int ggPPMId=6; // color 
const int ggPGMAsciiId=2;
const int ggPPMAsciiId=3;
//const char *ggRGBEId="FORMAT=32-bit_rle_rgbe\n\n";
//const char *ggGEId="FORMAT=32-bit_rle_GE\n\n";
//const ggString ggRGBEId("FORMAT=32-bit_rle_rgbe\n\n");
//const ggString ggGEId("FORMAT=32-bit_rle_GE\n\n");

// this is the same for GE and RGBE, so use just 1 number
const int ggRGBEIdCheckLen = 7; // note:  combined with FormatCheckLen,
                                // this is the length of the string
const int ggGEIdCheckLen = 7;  // see note above

const int ggRGBEFormatCheckLen = 15; 
const int ggGEFormatCheckLen = 13; 

//const char *ggRGBECoordFormat="-Y%4d +X%4d\n";
//const char *ExposureString="EXPOSURE=";
const int ExposureStringLen = 9;
// Ward's orientation values
const int MINRUN = 4;
const int MINELEN = 8;
const int MAXLINESIZE = 32767;
const int ggXDECR = 1;
const int ggYDECR = 2;
const int ggYMAJOR = 4;
const int ggStandardOrientation = ggYMAJOR|ggYDECR;
#endif
