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



#include <eonImageCalculator.h>
#include <gg.h>
#include <fstream.h>
#include <stdlib.h>
#if defined( _WIN32 )
#else
#include <unistd.h>
#include <sys/times.h>
#endif
#include <stdio.h>

#undef assert                                                                 
#define assert(EX) (EX)                                                       

int Flag;
int DumpX, DumpY;
void kenfoo();

int main (int argc, char *argv[])
{
    // WDH: FIX FOR DEMO
    {
	std::string cwd = argv[0];
	std::string::size_type pos = cwd.rfind('/');
	cwd = std::string(cwd, 0, pos);
	chdir(cwd.c_str());	
    }
    // WDH: END FIX FOR DEMO
    
    eonImageCalculator renderer;
    int count;
    char *pixel_out;   

// start timing
#if defined( _WIN32 )
#else
    struct tms time_usage;
    float clk_tck = (float) sysconf(_SC_CLK_TCK);
    times(&time_usage);
    float ustart_time = time_usage.tms_utime/clk_tck;
    float sstart_time = time_usage.tms_stime/clk_tck;
#endif
    char Version[10] = "1.1";



// check for usage
//

    cout << "Eon, Version " << Version << endl;

    // WDH: FIX FOR DEMO
/*    if (argc != 6) { */
//    if (argc != 7) {
//        cerr << "Bad Arg List.  Usage: " << argv[0] <<
//        "controlFile cameraFile surfacesFile\n";
//        exit(-1);
//    }
    argv = new char*[7];
    argv[1] = strdup("chair.control.cook");
    argv[2] = strdup("chair.camera");
    argv[3] = strdup("chair.surfaces");
    argv[4] = strdup("chair.cook.ppm");
    argv[5] = strdup("ppm");
    argv[6] = strdup("pixels_out.cook");
    // WDH: END FIX FOR DEMO

// get pixel output file name
//
    if ( strlen(argv[6]) <= 0 || strlen(argv[6]) >= 255 ) {
#ifdef SPEC_CPU2000_P64
      cerr << "pixel output filename length is " << (int)strlen(argv[6]) << ".  This value must be between 1 and 255.  Aborting.";
#else
      cerr << "pixel output filename length is " << strlen(argv[6]) << ".  This value must be between 1 and 255.  Aborting.";
#endif /* SPEC_CPU2000_P64 */
      exit(-1);
    }
    pixel_out = (char *) malloc( (strlen(argv[6])+1)*sizeof(char) );
    if ( pixel_out == NULL ) {
      cerr<< "unable to allocate space for pixel_out filename.  Aborting.";
      exit(-1);
    }
    strcpy(pixel_out, argv[6]);
    cerr << "getting pixel output filename " << argv[6] << "\n";

// open control file
//
    ifstream control(argv[1]);
    cerr << "opening control file " << argv[1] << "\n";
    if (!control) {
      cerr << "cannot open control file " << argv[1] << "\n";
      exit(-1);
    }

// open cameraFile file
//
    ifstream camera(argv[2]);
    cerr << "opening camera file " << argv[2] << "\n";
    if (!camera) {
      cerr << "cannot open camera file " << argv[2] << "\n";
      exit(-1);
    }

// open surfacesFile file
//
    ifstream surfaces(argv[3]);
    cerr << "opening surfaces file " << argv[3] << "\n";
    if (!surfaces) {
      cerr << "cannot open surfaces file " << argv[3] << "\n";
      exit(-1);
    }

// read data
//
    int width, height;
    cerr << "reading data\n";
    assert(renderer.readData(control, camera, surfaces, width, height));
    control.close();
    camera.close();
    surfaces.close();

// allocate raster
//
     ggRaster< ggRGBFPixel >
        color(width, height);
     ggRaster< ggRGBFPixel >
        sl(2, 2);


    cerr << "Writing to " << argv[4] << "\n";
// calculate all pixels
//
    int ns = renderer.nSamples[renderer.nLevels()-1];
    cerr << "calculating " << width << " by " << height <<
            " image with " << ns*ns << " samples\n";
	if (strcmp(argv[5], "xyz") == 0) {
		sl.Resize(width,height);
		count = 0;
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				ggSpectrum sp = renderer.pixelRadiance(i, j, renderer.nLevels() - 1);
				color[i][j] = renderer.tristimulusValue(sp);
				float f = renderer.scotopicLuminance(sp);
				sl[i][j] = ggRGBFPixel (f, f, f);
				if (++count % 10 == 0) {
					cerr << ".";
				}
			}
		}
	} else {
		count = 0;
		for (int i = 0; i < width; i++) {
#if defined(DEBUG)
			for (int j = 0; j <= 0; j++) {
#else
			for (int j = 0; j < height; j++) {
#endif
				if (i == DumpX && j == DumpY) {
					Flag = 1;
				} else {
					Flag = 0;
				}
				ggSpectrum sp = renderer.pixelRadiance(i, j, renderer.nLevels() - 1);
				color[i][j] = renderer.metamer(sp);
			}
			cerr << "col " << i << ". . ." << endl;
		}
	}

// output raster to file with name argv[4]
//
    cerr << "Writing to " << argv[4] << "\n";

    if (strcmp(argv[5], "ppm") == 0) {
       ggIO<ggRGBFPixel > outputStream(argv[4], GGPPM, GGOUT);
       outputStream.SetOutputProcessingType(GGCOOKED);
       ggDump( pixel_out, color );
       outputStream << color;
    }
    else if (strcmp(argv[5], "rgbe") == 0) {
       ggIO<ggRGBFPixel > outputStream(argv[4], GGRGBE, GGOUT);
       outputStream.SetOutputProcessingType(GGRAW);
       outputStream << color;
    }
    else if (strcmp(argv[5], "xyz") == 0) {
       char tempString[100];
       strcpy(tempString, argv[4]);
       ggIO<ggRGBFPixel > outputStream(strcat(tempString, ".xyz"),
                                              GGRGBE, GGOUT);
       outputStream.SetOutputProcessingType(GGRAW);
       outputStream << color;
       outputStream.SetOutputName(strcat(argv[4], ".sl"));
cerr << sl.width() << " " << sl.height() << "\n";
   //    outputStream.SetOutputType(GGGE);
   //    outputStream.SetGrayChannel(GGRED);
       outputStream << sl;
    }
    else {
        cerr << "Unknown file type " << argv[4] << "\n";
        cerr << "writing to ppm file\n";
       ggIO<ggRGBFPixel > outputStream(argv[4], GGPPM, GGOUT);
       outputStream << color;
    }

// end timing
#if defined( _WIN32 )
    printf("00-style eon Time not available on NT\n" );
#else
    times(&time_usage);
    float uend_time = time_usage.tms_utime/clk_tck;
    float send_time = time_usage.tms_stime/clk_tck;

    float total_time = (uend_time - ustart_time) + (send_time - sstart_time);
    printf("OO-style eon Time= %f\n", total_time);
#endif
    return 0;
}
