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
// ggIO.c -- implementation of class ggIO
//
// Author:    Tom Loos
//            Beata Winnicka
//            11/1/93

// Modified:  11/27/93 -- added heuristics for guessing unknown input
//                        type from 1st line of file (tjl) and cleaned
//                        out an unneeded constant (ggGEIdCheckLen)
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
#include <ggIOhdr.h>
#include <stdio.h>
  // for perror(), if stdio.h not included before this
//
// constructors
//
template <class T>
ggIO<T>::ggIO(ggString filename, ggFileType itype) //default to input
{
    channel = GGRED; // assume grey == 1st component
    inname = filename;
    outname = "";
    exposure = 1.0;
    instream.open( inname , ios::in);
    if (instream.fail())
    {
	perror("");
	exit(-1);
    }
    intype = itype;
    inascii = outascii = ggFalse;
// added to avoid multiple define problems
ggRGBEId = "FORMAT=32-bit_rle_rgbe\n\n";
ggGEId = "FORMAT=32-bit_rle_GE\n\n";
ggRGBECoordFormat = "-Y%4d +X%4d\n";
ExposureString = "EXPOSURE=";



    useingamma = useoutgamma = ggTrue;
    inproc = outproc = GGCOOKED; // default to PPMish case
    ingamma = outgamma = ggDefaultGamma;
#ifdef GGSAFE
    if (outascii == ggFalse)
	cerr << "c1: outascii = ggFalse\n";
    else if (outascii == ggTrue)
	cerr << "c1: outascii = ggTrue\n";
    else
	cerr << "c1: outascii = ???\n";
#endif
//GGSAFE

    outtype = GGNONE; // default
    shiftval = 8;
}
template <class T>
ggIO<T>::ggIO(ggString filename, ggFileType ftype, ggIODirection indir)
{
    channel = GGRED; // assume grey == 1st component
    exposure = 1.0;
// added to avoid multiple define problems
ggRGBEId = "FORMAT=32-bit_rle_rgbe\n\n";
ggGEId = "FORMAT=32-bit_rle_GE\n\n";
ggRGBECoordFormat = "-Y%4d +X%4d\n";
ExposureString = "EXPOSURE=";
    useingamma = useoutgamma = ggTrue;
    ingamma = outgamma = ggDefaultGamma;
    inproc = outproc = GGCOOKED; // default to PPMish case
    if (indir ==  GGIN)
    {
	inname = filename;
	outname = "";
	instream.open( inname , ios::in);
	if (instream.fail())
	{
	    perror("");
	    exit(-1);
	}
	intype = ftype;
	inascii = outascii = ggFalse;
	outtype = GGNONE; // default
    }
    else
    {
	outname = filename;
	inname = "";
	outstream.open( filename, ios::out|ios::trunc);
	if (outstream.fail())
	{
	    perror("");
	    exit(-1);
	}
	outtype = ftype;
	intype = GGNONE; // default
	inascii = outascii = ggFalse;
    }
#ifdef GGSAFE
    if (outascii == ggFalse)
	cerr << "c2: outascii = ggFalse\n";
    else if (outascii == ggTrue)
	cerr << "c2: outascii = ggTrue\n";
    else
	cerr << "c2: outascii = ???\n";
#endif
    shiftval = 8;

}
//
// destructor
//
template <class T>
ggIO<T>:: ~ggIO()
{
    instream.close();
    outstream.close();
    intype = GGNONE;
    outtype = GGNONE;
    
}
//
// operators
//
template <class T>
ifstream &ggIO<T>::operator>>(ggRaster<T>& raster)
{
    myread(&instream, &raster, &intype);
    return instream;
}

template <class T>
ofstream &ggIO<T>::operator<<(ggRaster<T>& raster)
{
    mywrite(&outstream, raster,  outtype);
    return outstream;
}

// misc. operations
// 
// first the sets
// inlines:
// void ggIO<T>::ggSetInputType(ggFileType t) 
// void ggIO<T>::ggSetOutputType(ggFileType t)
//void ggIO<T>::ggSetGrayChannel(ggChannel c)
//
// setting input name implies an implicit close/open
//
template <class T>
void ggIO<T>::SetInputName(ggString s)
{ 
    instream.close(); 
    inname = s; 
    //instream.open(inname, ios::in);
    OpenInputStream();
    if (instream.fail())
    {
	perror("");
	exit(-1);
    }
}
template <class T>
void ggIO<T>::SetOutputName(ggString s)
{ 
    outstream.close(); 
    outname = s; 
    //outstream.open(outname, ios::out|ios::trunc);
    OpenOutputStream();
    if (outstream.fail())
    {
	perror(""); 
	exit(-1);
    }
}

//
// then the gets
// done inline in ggIO.h
// ggFileType ggIO<T>::GetInputType()
// ggFileType ggIO<T>::GetOutputType()
// ggChannel  ggIO<T>::GetGrayChannel()
// ggString ggIO<T>::GetInputName() 
// ggString ggIO<T>::GetOutputName()

// explicit close operations
// done inline in ggIO.h
//
// void ggIO<T>::CloseInputStream()
// void ggIO<T>::CloseOutputStream()

// IO and IO helper routines

template <class T>
int ggIO<T>::getNum(ifstream *is) {
	int n = 0;
	char c;

	while (is->get(c)) {
		if (c == '#')
			while (is->get(c) && c != '\n');
		else if (!isspace(c))
			break;
	}
	is->putback(c);
	while (is->get(c) && isdigit(c))
		n = 10*n + c - '0';
	is->putback(c);

	return n;
}
template <class T>
void ggIO<T>::myread(ifstream *is, ggRaster<T> *raster, ggFileType *type) 
{
    ggBoolean myEOF;
    int width, height;
    // hmmm...

    is->unsetf(ios::skipws);

    myEOF = readheader(is, type, &inascii, &width, &height);

#ifdef GGSAFE
    cerr << "from readheader: ascii = " << inascii;
    cerr << ", width = " << width ;
    cerr << ", height = " << height << "\n";
#endif
//GGSAFE

    if (myEOF == ggTrue)
    {
	cerr << "premature EOF found reading header\n";
	exit(-1);
    }
    raster->Resize(width,height); // this seems backwards
    myEOF = readbody(is, raster, inascii, type);
    if (myEOF == ggTrue)
    {
	cerr << "premature EOF found reading raster body\n";
	exit(-1);
    }
}
template <class T>
ggBoolean
ggIO<T>::readline( ifstream *is, char *buf )
{
    unsigned char i;
    int j = 0;
    ggBoolean myEOF = ggTrue;
    *is >> i;
    while (!((*is).eof()))
    {
	if (i == '\n')
	{
	    buf[j] = '\0';
	    myEOF = ggFalse;
	    break;
	}
	buf[j++] = (char) i;
	(*is) >> i;
    }
    return myEOF;
}
template <class T>
ggBoolean
ggIO<T>::readheader(ifstream *is, ggFileType *type, ggBoolean *ascii, int *width,
		 int *height)
{
    char buf[100]; // should be enough
    int b,i;
    char c;
    int xndx, yndx;
    int maxval; // totally unused outta here
    ggBoolean myEOF = ggFalse;

    if ((myEOF = readline(is,buf)) == ggTrue)
    {
	return ggTrue;
    }
    // heuristic for UNKNOWN type
    // for RGBE to work, FORMAT= has to be on the first line
    //
    if (*type == GGNONE)
    {
	// try for PPM/PGM first 
	if (buf[0] == 'P')
	{
	    b = buf[1] - '0';
	    if ((b == ggPGMId) || (b == ggPGMAsciiId))
	    {
		*type = GGPGM;
	    }
	    else if  ((b == ggPPMId) || (b == ggPPMAsciiId))
	    {
		*type = GGPPM;
	    }
	}
	// now try for GGRGBE/GGGE

	else if (!strncmp(buf,ggRGBEId,ggRGBEIdCheckLen))
	{
	    i = ggRGBEIdCheckLen;
	    while (isspace(buf[i]))
		i++;

	    if (!strncmp(&(buf[i]),&(ggGEId[ggRGBEIdCheckLen]),ggGEFormatCheckLen))
	    {
		*type = GGGE;
	    }
	    else if (!strncmp(&(buf[i]),&(ggRGBEId[ggRGBEIdCheckLen]),ggRGBEFormatCheckLen))
	    {
		*type = GGRGBE;
	    }
	}
    }	    

    switch (*type)
    {
    case GGPGM: // grey scale
    case GGPPM: // color
	if (buf[0] != 'P')
	{
	    cerr << "invalid file header found\n";
	    exit(-1);
	}
	b = buf[1] - '0';
	if ((b == ggPGMId) || (b == ggPPMId) ||
	    (b == ggPGMAsciiId) || (b == ggPPMAsciiId))
	{
	    // set ascii
	    if ((b == ggPGMAsciiId) || (b == ggPPMAsciiId))
		*ascii = ggTrue;
	    else
		*ascii = ggFalse;

	    // file consistency check
	    if (((b == ggPGMId) || (b == ggPGMAsciiId)) && (*type == GGPPM))
	    {
		cerr << "readheader:  type is GGPPM for a PGM file\n";
		myEOF=ggTrue;
	    }
	    else if (((b == ggPPMId) || (b == ggPPMAsciiId)) && 
		     (*type == GGPGM))
	    {
		cerr << "readheader:  type is GGPGM for a PPM file\n";
		myEOF=ggTrue;
	    }
	}
	// get the real stuff
	*width = getNum(is);
	*height = getNum(is);
	maxval = getNum(is);
	is->get(c); // get newline
	myEOF=ggFalse;
	break;
    case GGRGBE: // color
    case GGGE: // hacked grey == RGBE for now

	//cerr << "in GGRGBE/GE code: buf = " << buf << "\n";
	//cerr << "type = " << FormatType(*type) << "\n";
	// Note: a \n gets translated to a \0 by readline,
	// So, while not a blank line and not EOF
	while ((buf[0] != '\0') && (myEOF == ggFalse))
	{
	    // check for exposure
	    if (!strncmp(buf,ExposureString,ExposureStringLen))
		exposure *= atof(buf+ExposureStringLen);

	    // check for FORMAT=
	    else if (!strncmp(buf,ggRGBEId,ggRGBEIdCheckLen))
	    {
		if (*type == GGRGBE)
		    i = ggRGBEIdCheckLen;
		while (isspace(buf[i]))
		    i++;
		if (*type == GGRGBE)
		{
		    if (strncmp(&(buf[i]),&(ggRGBEId[ggRGBEIdCheckLen]),ggRGBEFormatCheckLen))
		    {
			cerr << "invalid RGBE header: invalid format type\n";
			exit(-1);
		    }
		}
		else // GE
		{
		    if (strncmp(&(buf[i]),&(ggGEId[ggRGBEIdCheckLen]),ggGEFormatCheckLen))
		    {
			cerr << "invalid GE header: invalid format type\n";
			exit(-1);
		    }
		}
		    
		//views not handled
	    }
	    myEOF = readline(is,buf);
	}
	// get format line
	if (myEOF == ggFalse)
	    myEOF = readline(is,buf);

	if (myEOF == ggFalse)
	{
	    xndx = yndx = -1;
	    for (i = 0; i < strlen(buf); i++)
	    {
		if (buf[i] == 'X')
		    xndx = i;
		else if (buf[i] == 'Y')
		    yndx = i;
	    }
	    if ((xndx == -1) || (yndx == -1))
	    {
		cerr << "invalid RGBE/GE header: no X and/or Y in format\n";
		exit(-1);
	    }
	    orientation = 0;
	    if (xndx > yndx)
		orientation |= ggYMAJOR;
 	    if (buf[xndx-1] == '-')
		orientation |= ggXDECR;
 	    if (buf[yndx-1] == '-')
		orientation |= ggYDECR;
	    if (orientation != ggStandardOrientation)
	    {
		cerr << "valid but unsupported RGBE/GE header: will go on anyway\n";
	    }
	    if ((*width = atoi(&(buf[xndx+1]))) < 0)
	    {
		cerr << "invalid width in RGBE/GE header: width = " << width << endl;
		exit(-1);
	    }
	    if ((*height = atoi(&(buf[yndx+1]))) < 0)
	    {
		cerr << "invalid width in RGBE/GE header: width = " << width << endl;
		exit(-1);
	    }
	}
	else
	{
	    cerr << "invalid RGBE/GE header: premature end of file seen\n";
	    exit(-1);
	}
	break;
    case GGNONE:
    default:
	cerr << "Tried to read a file of unspecified type\n";
	myEOF=ggFalse; //not yet
    }
    return myEOF;
}
template <class T>
ggBoolean ggIO<T>::readbody(ifstream *is, ggRaster<T> *raster, int ascii, ggFileType *type) 
{
    int count;
    int i, row, col;
    int height = raster->height();
    int width = raster->width();
    T foo;
    ggRGBE rgbe;
    ggRGBE *scanline; 
    ggGamma g(ingamma);
    unsigned char c;
    int compmax = 4;
    int curval[4];
    ggBoolean myEOF=ggFalse;

    switch (*type)
    {
    case GGPPM:
	count = 3;
	break;
    case GGPGM:
	count = 1;
	break;
    case GGGE:
	count = 2;
	scanline = new ggRGBE[width];
	break;
    case GGRGBE:
	count = 4;
	scanline = new ggRGBE[width];
	break;
    case GGNONE:
	cerr << "invalid file type passed to readbody\n";
	return ggTrue; //why are we here?
    }

#ifdef GGSAFE
    cerr << "count = " << count << ", width = " << width << " height = " << height << "\n";
#endif
//GGSAFE

    // main loop
    for (row = height - 1; row >= 0 && (myEOF == ggFalse); row--)
    {
#ifdef GGSAFE
	if ((row % 10) == 0)
	    cerr << "on row " << row << "\n";
#endif
//GGSAFE
	if ((*type == GGPPM) || (*type == GGPGM))
	{
	    for (col = 0; col < width; col++)
	    {
		//if (row == (height -1))
		// cerr << "on row " << row << "\n";
		
		// read current pixel
		for (i = 0; i < count; i++)
		{
		    if (ascii)
		    {
			curval[i] = getNum(is);
		    }
		    else
		    {
			*is >> c;
#ifdef GGSAFE
			if (is->eof())
			{
			    cerr << "premature eof reached in reading file\n";
			    cerr << "current pixel: row = " << row << ", col = " << col << "\n";
			    exit(-1);
			}
#endif

			curval[i] = (int) c;
		    }
		    
		}

		switch (count)
		{
		case 1:
		    curval[1] = curval[2] = curval[0];
		    // fall through expected;
		case 3:
		    curval[3] = 128; // default
		    break;
		    
		}
		// convert input to RGBE
		rgbe.IntToRGBE(curval);
		(raster->operator[](col))[row] = foo.ConvertFromRGBE(rgbe);
		
	    } // end of for loop
	} 
	else if ((*type == GGGE) || (*type == GGRGBE))
	{
	    if (*type == GGGE)
		compmax = 2;
	    else // RGBE
		compmax = 4;
	    //printf("calling readscanline for row = %d, width = %d\n",row,width);
	    myEOF=ReadRGBEScanline(is, scanline, width, compmax,g);
	    //printf("done calling readscanline for row = %d\n",row);
	    if (myEOF == ggTrue)
	    {
		cerr << "premature EOF reached in RGBE file\n";
		exit(-1);
	    }
	    for (col = 0; col < width; col++)
	    {
		(*raster)[col][row] = foo.ConvertFromRGBE(scanline[col]);
#ifdef GGSAFE
		// debugging
		if (row == (height-1))
		{
		    cout << "row = " << row << ", col = " << col;
		    scanline[col].print();
		    printf("                   RGB : ");
//		    (*raster)[col][row].print();
		}
#endif
//GGSAFE
	    }
	}
		    
    } // of if row
    return ggFalse;
}

// WRITE ROUTINES
template <class T>
void ggIO<T>::mywrite(ofstream *os, ggRaster<T>& raster, ggFileType type) 
{
    //ggBoolean myEOF;
    // char buf[100];
    //int width, height;
    // hmmm...

    //os->unsetf(ios::skipws);
    if (!writeheader(os, type, outascii, raster.width(), raster.height()))
	exit(-1);

#ifdef GGSAFE
    if (outascii == ggFalse)
	cerr << "mywrite: outascii = ggFalse\n";
    else if (outascii == ggTrue)
	cerr << "mywrite: outascii = ggTrue\n";
    else
	cerr << "mywrite: outascii = ???\n";
#endif
//GGSAFE

    if (!writebody(os, raster, outascii, type))
	exit(-1);

}
template <class T>
ggBoolean
ggIO<T>::writeheader(ofstream *os, ggFileType type, ggBoolean ascii, int width,
		     int height)
{
    char buf[100];
    int id;
    switch (type)
    {
    case GGPGM: // grey scale
    case GGPPM: // color
	if (type == GGPGM)
	    if (ascii)
		id = ggPGMAsciiId;
	    else
		id = ggPGMId;
	else
	    if (ascii)
		id = ggPPMAsciiId;
	    else
		id = ggPPMId;

#if defined(_WIN32)
	//Win32 begins in ASCII mode by default -- CHANGE IT!
	//  The file must not have CR added when outchar==LF.
	(*os) << binary;
#endif
	sprintf(buf,"P%d\n",id);

//	cerr << "header 1 = " << buf;
	(*os) << buf;
	sprintf(buf,"%d %d\n",width,height);
//	cerr << "header 2 = " << buf;
	(*os) << buf;
	// maxval 
	//sprintf(buf,"255");
//	cerr << "header 3 = " << 255 << endl;
	(*os) << 255 << endl;
	break;
    case GGRGBE: // color
    case GGGE: // hacked grey
	if (type == GGRGBE)
	{
	    (*os) << ggRGBEId;
#ifdef GGSAFE
	    cerr << "RGBE header 1 = " << ggRGBEId;
#endif
//GGSAFE
	}
	else // GE
	{
	    (*os) << ggGEId;
#ifdef GGSAFE
	    cerr << "RGBE header 1 = " << ggGEId;
#endif
//GGSAFE
	}
	sprintf(buf,ggRGBECoordFormat,height,width);
#ifdef GGSAFE
	cerr << "RGBE header 2 = " << buf;
#endif
//GGSAFE
	(*os) << buf;
	break;
    case GGNONE:
    default:
	cerr << "Tried to write a file of unspecified type\n";
	return ggFalse;
    }
    return ggTrue;
}
template <class T>
ggBoolean ggIO<T>::writebody(ofstream *os, ggRaster<T>& raster, int ascii, ggFileType type) 
{
    ggRGBE rgbe;

    char buf[62];
    int row, col,j = 0;
    int height = raster.height();
    int width = raster.width();
    ggRGBE *scanline;
    int mask[4], vals[3];
    unsigned char outchar;
    int m;
    int cnt;
    int beg;
    int comp, cur_comp;
    int c2;
    ggGamma g(outgamma);

    mask[0] = mask[1] = mask[2] = mask[3] = -1;
//
// write a raster line 
//
// channel selection is done with mask[]
// mask[i] = the ith channel (r,g,b,e) to be written
// so if we want just r and e to be written out (say for GE type)
// mask[0] = 0;  // red
// mask[1] = 3;  // exponent
// mask[2]  = -1; // stop after two channels
// mask[3]  = -1; // just for completeness
//
    switch (type)
    {
    case GGGE:
	mask[1] = 3;
	// fall through expected
    case GGPGM:
	mask[0] = (int) channel;
	break;

    case GGRGBE:
	mask[3] = 3; //exponent
	// fall through expected
    case GGPPM:
	mask[0] = 0;
	mask[1] = 1;
	mask[2] = 2;
	break;
    default:
	cerr << "invalid file type\n";
	return ggFalse;
    }
#ifdef GGSAFE
    cerr << "writebody: ascii =" << ascii << endl;
    cerr << "         : mask[0] =" << mask[0] << endl;
    cerr << "         : mask[1] =" << mask[1] << endl;
    cerr << "         : mask[2] =" << mask[2] << endl;
    cerr << "         : mask[3] =" << mask[3] << endl;
#endif
//GGSAFE

    if ((type == GGGE) || (type == GGRGBE))
	scanline = new ggRGBE [width];

    for (row = height - 1; row >= 0; row--)
    {
#ifdef GGSAFE
	if ((row % 10) == 0)
	    cerr << "on row " << row << "\n";
#endif
//GGSAFE

	//scanline
	if ((type == GGPPM) || (type == GGPGM))
	{
	    for (col = 0; col < width; col++)
	    {
		// convert current pixel to RGBE
		// cooking a PPM file converts from 0-1 to 0-255 range 
		rgbe = raster[col][row].ConvertToRGBE();
		if (outproc == GGCOOKED) // hall
		{
		    rgbe.ShiftExponent(shiftval);
		    rgbe.AdjustCenter(ggRGBEBase+8);
		}
		rgbe.RGBEToInt(vals);
		
#ifdef GGSAFE
		if (row == (height-1))
		{
		    printf("only for %dth row do we print values: col = %d rgbe: ",row,col);
//		    raster[col][row].print(); //can not require this in general
		    rgbe.print();
		    printf("                  vals : %d %d %d\n",vals[0],
			   vals[1],vals[2]);

		}
#endif
//GGSAFE

		// reset mask counter
		m = 0;
		// printf("row = %d, col = %d, rgbe = %d, %d, %d, %d\n",row,col,rgbe[0],rgbe[1],rgbe[2],rgbe[3]);
		while ((mask[m] != -1) && (m < 4)) // for each channel to be written
		{
		    //    printf("rgbe[mask[m]] = %d, outchar = %d, vals[mask[m]] = %d\n",rgbe[mask[m]],outchar,vals[mask[m]]);
		    outchar = (unsigned char) (vals[mask[m]] & 0xff);

		    m++;
		
		    // if not ascii, simple put
		    if (ascii == ggFalse)
		    {
#if 0
		      if( outchar=='\012' ) { os->put('\013'); } else
#endif
			os->put(outchar);
		    }
		    else // handle ascii 
		    {
			sprintf(&(buf[j]),"%3d ",outchar);
			if (j == 56)
			{
			    buf[60] = '\n';
			    os->write(buf,61);
			    j = 0;
			}
			else
			    j+=4;
		    }
		} // end of while mask loop
	    } // end of scanline loop for GGPPM/PGM
	}
	else // type == GGGE || GGRGBE
	{
	    ascii = ggFalse;
	    // get the scan line for run-length encoding (RLE)
	    // yucck...
	    for (col = 0; col < width; col++)
	    {
		scanline[col] = raster[col][row].ConvertToRGBE();
		// make all RGBEs based on 2**8
		// i.e. normalize them to 0..255
		if (outproc == GGCOOKED) // hall
		    scanline[col].AdjustCenter(ggRGBEBase+8);
#ifdef GGSAFE
		if (row == (height -1))
		{
		    printf("col = %d: pixel = %f\n", col,
			   raster[col][row]);
		    if (type == GGRGBE)
		    {
			printf("rgbe: scanline = %d %d %d %d\n",
			   scanline[col][0],
			   scanline[col][1],
			   scanline[col][2],
			   scanline[col][3]);
		    }
		    else 
		    {
		cout << "ge (mask = " << mask[0] << ", " << mask[1] << ") ";
		cout << "scanline = " << (int) scanline[col][mask[0]] << " " << (int) scanline[col][mask[1]];
		cout << " orig = " << (int) scanline[col][0] << " " << (int) scanline[col][1] << " " << (int) scanline[col][2] << " " << (int) scanline[col][3] << "\n";		

		    }
		}
#endif
//GGSAFE
	    }
	    // convert from gamma corrected to RGBE
	    if ((useoutgamma == ggTrue) && (outproc == GGCOOKED))
		g.GammaToRGBE(scanline,width);
#ifdef GGSAFE
	    if (row == (height -1))
		for (col = 0; col < width; col++)
		{
		printf("rgbe b = %d %d %d %d\n",
		       scanline[col][0],
		       scanline[col][1],
		       scanline[col][2],
		       scanline[col][3]);
		}
#endif
//GGSAFE
	    // if line too short, no RLE 
	    if (width < MINELEN)
	    {
		// write out normal RGBEs
		for (col = 0; col < width; col++)
		{
		    if (type == GGRGBE) 
		    {
			os->put(scanline[col][0]);
			os->put(scanline[col][1]);
			os->put(scanline[col][2]);
			os->put(scanline[col][3]);
		    }
		    else //type == GGGE
		    {
			os->put(scanline[col][mask[0]]);
			os->put(scanline[col][mask[1]]);
		    }
		}
		return ggTrue;
	    }
	
            // RLE loops-- comp is component of pixel	    
            // for each component
            // output magic header first
	    os->put((unsigned char) 2);
	    os->put((unsigned char) 2);
	    os->put((unsigned char) (width>>8)); // high byte
	    os->put((unsigned char) (width&0xff)); // low byte

	    for (cur_comp = 0; ((mask[cur_comp] != -1) && (cur_comp < 4)); cur_comp++)
	    {
		// use current component
		comp = mask[cur_comp];
		// printf("comp = %d\n",comp);
		// update column by cnt positions at a time
		for (col = 0; col < width; col += cnt)
		{
		    //printf("row = %d, col = %d: ",row,col);
		    // find next run
		    for (beg = col; beg < width; beg += cnt)
		    {
			cnt = 1;
			while ((cnt < 127) &&(( beg+cnt) < width)
			       && (scanline[beg+cnt][comp] == scanline[beg][comp]))
			{
			    cnt++;
			}
			// if big enough, process scanline
			if (cnt >= MINRUN)
			    break;
		    }
		    if (beg-col > 1 &&  beg-col < MINRUN)
		    {
			c2 = col+1;
			while (scanline[c2++][comp] == scanline[col][comp])
			    if (c2 == beg)
			    {
				// short run
				//printf("short run: %d %d\n",128+beg-col,scanline[col][comp]);
				os->put((unsigned char) (128+beg-col));
				os->put(scanline[col][comp]);
				col = beg;
				break;
			    }
		    }
		    // non run
		    while (col < beg)
		    {
			if ((c2 = beg-col) > 128)
			    c2 = 128;
			os->put((unsigned char) (c2));
			//printf("non run: c2 = %d: ",c2);
			while (c2--)
			{
			    //printf("%d ",scanline[col][comp]);
			    os->put(scanline[col++][comp]);
			}
			//printf("\n");
		    }
		    // write out run
		    if (cnt >= MINRUN)
		    {
			//printf("run: %d %d\n",128+cnt, scanline[beg][comp]);
			os->put((unsigned char) (128+cnt));
			os->put(scanline[beg][comp]);
		    }
		    else
			cnt = 0;
		} // for each column
	    } // for each component
	} // end of else type == GGGE || GGRGE
    } // end of for all rows
    // final line for ascii 
    if ((ascii == ggTrue) && (j != 0))
    {
	buf[j] = '\n';
	os->write(buf,j+1);
    }
    // cleanup
    if ((type == GGGE) || (type == GGRGBE))
	delete [] scanline;
    return ggTrue;
}

// 
// NOTE: ReadOldColors tells the difference betweeen GE and RGBE by
// looking at compmax (if compmax == 2, it's GE, otherwise it's RGBE (and
// compmax should be 4.))
// 
template <class T>
ggBoolean ggIO<T>::ReadOldColors(istream *is, ggRGBE *scanline, 
				 int width, int compmax)
{
    int rshift = 0; 
    int i;
    char c;
    while (width < 0)
    {
	is->get(c);
	scanline[0][0] = c; // red
	is->get(c);
	scanline[0][1] = c; // green
	if (compmax == 4)
	{
	    is->get(c);
	    scanline[0][2] = c; // blue
	    is->get(c);
	    scanline[0][3] = c; // exponent
	}
	else // compmax = 2 (checked before), so 
	    // scanline[1] = exponent and need to copy scanline[0] to 
	    // scanline[1] and [2]
	{
	    scanline[0][3] = scanline[0][1];
	    scanline[0][1] = scanline[0][2] = scanline[0][0];
	}

	if (is->eof())
	{
	    return(ggTrue);
	}
	// only supported for RGBE
	if ((compmax == 4) &&
	    (scanline[0][0] == 1) &&
	    (scanline[0][1] == 1) &&
	    (scanline[0][2] == 1))
	{
	    // copy
	    for (i = 0; i < (scanline[0][3] << rshift); i++)
	    {
		scanline[0] = scanline[-1];
		scanline++;
		width--;
	    }
	    rshift += 8;
	}
	else 
	{
	    scanline++;
	    width--;
	    rshift = 0;
	}

    }
    return(ggFalse);
}
template <class T>
ggBoolean ggIO<T>::ReadRGBEScanline(istream *is, ggRGBE *scanline, int width, int compmax, ggGamma& g)
{
    char c,code;
    ggBoolean isGE;
    int comp,col;

    if ((compmax != 2) && (compmax != 4))
    {
	cerr << "BUG: ReadRGBEScanline: compmax = " << compmax << "(should be 2 or 4)\n";
	exit(-1);
    }

    if (width < MINELEN)
	return(ReadOldColors(is, scanline, width, compmax));

    // check for magic header for RLE encoding
    //
    // header is: 0x02 0x02 length_hi length_lo
    // where length_hi * 256 + length_lo == width
    //

    is->get(c);
    if (c != 2)
    {
	// no ungetc --- AARRGGHH
	
	scanline[0][0] = c;
	if (compmax == 2)
	{
	    is->get(c);
	    scanline[0][3] = c;
	}
	else
	{
	    is->get(c);
	    scanline[0][1] = c;
	    is->get(c);
	    scanline[0][2] = c;
	    is->get(c);
	    scanline[0][3] = c;
	}

	//printf("c = %d, calling old read colors 1\n",c);
	return(ReadOldColors(is, scanline+1, width-1, compmax));
    }
    // think it's RLE, now check for 2nd 2, and length
    is->get(c);
    scanline[0][1] = c;
    is->get(c);
    scanline[0][2] = c;
    is->get(c);
    scanline[0][3] = c;
    if (is->eof())
    {
	return ggTrue;
    }
    // if not 2nd 2 or length too big, use old version
    if ((scanline[0][1] != 2) || (scanline[0][2] & 128))
    {
	scanline[0][0] = 2;
	if (compmax == 4)
	//printf("sc[1] = %d, sc[2] = %d, calling old read colors 2\n",scanline[0][1],scanline[0][2]);
	    return(ReadOldColors(is, scanline+1, width-1, compmax));
	else // read in 2 GEs, so fix it up
	{
	    // 0, 3 is the 2nd exponent
	    scanline[1][3] = scanline[0][3];
	    // 0, 2 is the 2nd grey scale value
	    scanline[1][0] = scanline[1][1] = scanline[0][2];
	    // 0, 1 is the 1st exponent
	    scanline[0][1] = scanline[0][3];
	    // 0, 0 is the 1st value
	    scanline[0][1] = scanline[0][2] = scanline[0][0];
	}
    }
    
    if ((scanline[0][2] << 8 | scanline[0][3]) != width)
    {
	cerr << "header scanline length != scanline length\n";
	return(-1);
    }

    // hack for GE (part 1)
    if (compmax == 2)
    {
	isGE = ggTrue;
	compmax = 4;
    }
    else
	isGE = ggFalse;

    // cerr << "have a good scanline header\n";
    for (comp = 0; comp < compmax; comp++)
    {
	// second part of hack for GE
	if ((isGE == ggTrue) && (comp == 1))
	{
	    comp = compmax - 1; 
	}

	for (col = 0; col < width; ) // col is updated below!!
	{
	    is->get(code);
	    //printf("comp = %d, col = %d, code = %d\n",comp,col,code);
	    
	    if (is->eof())
		return ggTrue;
	    if (code > 128) // we have a run!
	    {
		// get common character
		is->get(c);
		scanline[col++][comp] = c;

		//printf("common char = %x\n",scanline[col-1][comp]);

		// yes, col++ below
		for (code &= 127; --code; col++)
		{
		    scanline[col][comp] = scanline[col-1][comp];
		}
	    }
	    else
	    {
		while (code--)
		{
		    is->get(c);
		    scanline[col++][comp] = c;
		    // printf("got char %x\n",scanline[col-1][comp]);
		}
	    }
	}

	// hack for GE (part 3)
	if (isGE == ggTrue)
	{
	    for (col = 0; col < width; col++)
	    {
		scanline[col][1] = scanline[col][2] = scanline[col][0];
	    }
	}

#ifdef GGSAFE
	if (col > width)
	    printf("col = %d, width = %d, comp = %d\n",col,width,comp);
#endif
//GGSAFE
    }

    // do gamma correction (i.e. cook the input data) if desired
    if ((inproc == GGCOOKED) && (useingamma == ggTrue)) //hall
    {
	g.RGBEToGamma(scanline,width);
    }
    if (is->eof())
	return ggTrue;
    return ggFalse;
}

template <class T>
ggString ggIO<T>:: FormatBoolean(ggBoolean b)
{
    if (b == ggTrue)
	return "ggTrue";
    
    if (b == ggFalse)
	return "ggFalse";
    return "unknown";
}

template <class T>
ggString ggIO<T>::FormatType (ggFileType f)
{
    if (f == GGNONE)
	return "GGNONE -- No file type specified";
    if (f == GGGE)
	return "GGGE   -- grayscale Radiance format";
    if (f == GGRGBE)
	return "GGRGBE -- Radiance format";
    if (f == GGPPM)
	return "GGPPM  -- PPM (portable pixmap (RGB color)) format";
    if (f == GGPGM)
	return "GGPGM  -- PGM (generic greyscale) format";
    return "unknown";
}

template <class T>
ggString ggIO<T>::FormatHandling(ggIOHandling h)
{
    if (h == GGRAW)
	return "GGRAW";
    if (h == GGCOOKED)
	return "GGCOOKED";
    return "unknown";
}

template <class T>
ggString ggIO<T>::FormatChannel(ggChannel c)
{
    if (c == GGRED)
	return "GGRED";
    if (c == GGGREEN)
	return "GGGREEN";
    if (c == GGBLUE)
	return "GGBLUE";
    return "unknown";
}

template <class T>
void ggIO<T>::IOInformation(ostream &os)
{
    (os) << "Input  file name:              " << GetInputName() << "\n";
    (os) << "       file type:              " << FormatType(GetInputType()) << "\n";
    (os) << "       file processing:        " << FormatHandling(GetInputProcessingType()) << "\n";
    (os) << "       ASCII used:             " << FormatBoolean(GetInputAscii()) << "\n";
    (os) << "       gamma correction Used:  " << FormatBoolean(GetUseInputGammaCorrection()) << "\n";
    (os) << "       gamma correction value: " << GetInputGamma() << "\n";
    (os) << "\n";
    (os) << "Output file name:              " << GetOutputName() << "\n";
    (os) << "       file type:              " << FormatType(GetOutputType()) << "\n";
    (os) << "       file processing:        " << FormatHandling(GetOutputProcessingType()) << "\n";
    (os) << "       ASCII used:             " << FormatBoolean(GetOutputAscii()) << "\n";
    (os) << "       gamma correction Used:  " << FormatBoolean(GetUseOutputGammaCorrection()) << "\n";
    (os) << "       gamma correction value: " << GetOutputGamma() << "\n";
    (os) << "\n";
    (os) << "Gray channel used: " << FormatChannel(GetGrayChannel()) << "\n";
    (os) << "PPM cooking value: " << GetPPMCookingValue() << "\n";
    (os) << "Radiance Exposure value (not supported): " << GetExposure() << "\n";
    os << "\n";
}
