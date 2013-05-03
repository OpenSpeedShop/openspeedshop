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
// ggIO.h -- definition of class ggIO
//
// Author:    Tom Loos 
//            Beata Winnicka
//            11/1/93
// Modified:  11/27 -- changed filetype in myread, etc. to a pointer so it
//                     can be calculated on the fly
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
#ifndef GGIO_H
#define GGIO_H

#include <fcntl.h>
#include <stdlib.h>
#include <ggString.h>
#include <ggRaster.h>
#include <fstream.h>
#include <ctype.h>
//#include <ggIOhdr.h>  FOR TESTING ONLY
#include <ggIOhdr.h>

//
// NOTES:
// any type T that wants to use this class must support
// conversion to and from a ggRGBE plus be able to be stored in
// and retrieved from a ggRaster.
//
template <class T>
class ggIO
{
public:
    
    // constructors
    ggIO(ggString filename, ggFileType intype); //default to input
    ggIO(ggString filename, ggFileType intype, ggIODirection indir);
    
    // destructor
    ~ggIO();
    
    // operators
    //friend 
    ofstream &operator<<(ggRaster<T>& raster);
    ifstream &operator>>(ggRaster<T>& raster);
    // void ggIOWrite(ggRaster<T> raster);
    // void ggIORead(ggRaster<T> raster);
    
 ggString ggRGBEId;
 ggString ggGEId;
 ggString ggRGBECoordFormat;
 ggString ExposureString;


    // misc. operations
    void SetInputName(ggString s);
    void SetOutputName(ggString s);
    void IOInformation(ostream &os);
    
    // inline from here down...
    inline void SetInputType(ggFileType t) {intype = t;};
    inline void SetOutputType(ggFileType t) {outtype = t;};
    inline void SetGrayChannel(ggChannel c) {channel = c;};
    inline void SetInputAscii(ggBoolean a) { inascii = a; };
    inline void SetOutputAscii(ggBoolean a) { outascii = a; };
    inline void SetExposure(double e) {exposure = e; };
    inline void SetUseInputGammaCorrection(ggBoolean ig) { useingamma = ig; };
    inline void SetUseOutputGammaCorrection(ggBoolean og) { useoutgamma = og; };
    inline void SetInputGamma(double ig) { ingamma = ig; };
    inline void SetOutputGamma(double og) { outgamma = og; };
    inline void SetInputProcessingType(ggIOHandling ip) { inproc = ip; };
    inline void SetOutputProcessingType(ggIOHandling op) { outproc = op; };
    inline void SetPPMCookingValue(int s) { shiftval = s; }; 

    inline ggFileType GetInputType() { return intype; };
    inline ggFileType GetOutputType() { return outtype; }
    inline ggChannel GetGrayChannel() { return channel; };
    inline ggString GetInputName() { return inname; };
    inline ggString GetOutputName() { return outname; };
    inline ggBoolean GetInputAscii() { return inascii; };
    inline ggBoolean GetOutputAscii() { return outascii; };
    inline double GetExposure() { return exposure; };
    inline ggBoolean GetUseInputGammaCorrection() { return useingamma; };
    inline ggBoolean GetUseOutputGammaCorrection() { return useoutgamma; };
    inline double GetInputGamma() { return ingamma; };
    inline double GetOutputGamma() { return outgamma; };
    inline ggIOHandling GetInputProcessingType() { return inproc; };
    inline ggIOHandling GetOutputProcessingType() { return outproc; };
    inline int GetPPMCookingValue() { return shiftval; }; 
    inline void CloseInputStream() { instream.close(); };
    inline void CloseOutputStream() { outstream.close(); };
    inline void OpenInputStream() { instream.open( inname, ios::in); };
    inline void OpenOutputStream() { outstream.open(outname, ios::out|ios::trunc); }
    // formatting functions
    ggString FormatBoolean(ggBoolean b);
    ggString FormatType (ggFileType f);
    ggString FormatHandling(ggIOHandling h);
    ggString FormatChannel(ggChannel c);

protected:
    
    void myread(ifstream *is, ggRaster<T> *raster, ggFileType *type);
    ggBoolean readheader(ifstream *is, ggFileType *type, ggBoolean *ascii, 
	       int *width, int *height);
    ggBoolean readline( ifstream *is, char *buf );
    ggBoolean readbody( ifstream *is, ggRaster<T> *raster, int ascii, ggFileType *type);
    ggBoolean ReadOldColors(istream *is, ggRGBE *scanline, int width, int compmax);
    ggBoolean ReadRGBEScanline(istream *is, ggRGBE *scanline, int width, int
compmax, ggGamma& g);
    void mywrite(ofstream *os, ggRaster<T>& raster, ggFileType type);
    ggBoolean writeheader(ofstream *os, ggFileType type, ggBoolean ascii, 
		     int width, int height);
    ggBoolean writeline( ofstream *is, T *buf, int count, ggBoolean ascii, int mask[4] );
    ggBoolean writebody( ofstream *is, ggRaster<T>& raster, int ascii, ggFileType type);
    int getNum(ifstream *is);
    // redundent ? mywrite(ofstream os, ggRaster<T> raster, ggFileType type);
    // data below here
    ggChannel channel;
    ggIOHandling inproc, outproc;
    ggString inname, outname;
    ggFileType intype, outtype;
    ggBoolean inascii, outascii;
    ggBoolean useingamma, useoutgamma;
    double ingamma, outgamma;
    ifstream instream;
    ofstream outstream;
    double exposure; // for RGBE only -- unsupported
    int orientation; // for RGBE only -- unsupported
    int shiftval; // how much to shift PPM for cooking
};

// out of class functions

//#if defined( _WIN32 )
#include "ggIO.cc"
//#endif

#endif
