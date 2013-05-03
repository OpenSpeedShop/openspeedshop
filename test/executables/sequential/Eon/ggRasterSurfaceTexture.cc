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
// ggRasterSurfaceTexture.C-- definitions of the member functions 
//                            for class ggRasterSurfaceTexture
//
// Author: Peter Shirley
//         Aug. 30, 1993
//         
// Modified: Byung Sung Cho
//           Nov. 1, 1993
// Modified: Peter Shirley
//           Feb 15, 1994 remove spectrum, inheritance
//

#include <assert.h>
#include <ggConstants.h>
#include <string.h>
#include <ggMacros.h>
#include <ggRasterSurfaceTexture.h>

ggRasterSurfaceTexture::ggRasterSurfaceTexture( istream& in) {
     char header[255];
   in >> header;
   assert(strcmp(header, "P6") == 0);
   int nx, ny, dummy;
   in >> nx >> ny >> dummy;

   imagePtr = new ggRaster< ggRGBPixel<unsigned char> >(nx, ny);

   char red, green, blue;
   in.get(red); // eat a byte
   int y, x;
   for ( y = ny-1; y >= 0; y--)
      for (x = 0; x < nx; x++)
      {
          in.get(red);
          in.get(green);
          in.get(blue);
          (*imagePtr)[x][y] = ggRGBPixel<char> (red, green, blue);
      }

}

ggRGBFPixel ggRasterSurfaceTexture::bilinearColor(const ggPoint2& p) const
{
     double di, dj;
     int i, j;
     float u, v;

    
     di = p.x() * (imagePtr->width()-1);
     dj = p.y() * (imagePtr->height()-1);

     if (di < 0) di = 0;
     if (dj < 0) dj = 0;
     if (di >= imagePtr->width() - 1) di =  imagePtr->width() - 1.0001;
     if (dj >= imagePtr->height() - 1) dj =  imagePtr->height() - 1.0001;

     i = int(di);
     j = int(dj);

     u = di - i;
     v = dj - j;


     ggRGBFPixel c00((float) (*imagePtr)[i  ][j  ].r(), 
                     (float) (*imagePtr)[i  ][j  ].g(), 
                     (float) (*imagePtr)[i  ][j  ].b()); 
     ggRGBFPixel c01((float) (*imagePtr)[i  ][j+1].r(), 
                     (float) (*imagePtr)[i  ][j+1].g(), 
                     (float) (*imagePtr)[i  ][j+1].b()); 
     ggRGBFPixel c10((float) (*imagePtr)[i+1][j  ].r(), 
                     (float) (*imagePtr)[i+1][j  ].g(), 
                     (float) (*imagePtr)[i+1][j  ].b()); 
     ggRGBFPixel c11((float) (*imagePtr)[i+1][j+1].r(), 
                     (float) (*imagePtr)[i+1][j+1].g(), 
                     (float) (*imagePtr)[i+1][j+1].b()); 

// ggColorRatio is 1.0/255.0
     ggRGBFPixel c = (1 - u)*(1 - v)*c00 +
                     (    u)*(1 - v)*c10 +
                     (1 - u)*(    v)*c01 +
                     (    u)*(    v)*c11;
     
     return ggColorRatio * c;
}

ggRGBFPixel ggRasterSurfaceTexture::bicubicColor(const ggPoint2& p) const
{
     double di, dj;
     int i, j;
     float u, v ;

    
     di = p.x() * (imagePtr->width()-1);
     dj = p.y() * (imagePtr->height()-1);

     if (di < 0) di = 0;
     if (dj < 0) dj = 0;
     if (di >= imagePtr->width() - 1) di =  imagePtr->width() - 1.0001;
     if (dj >= imagePtr->height() - 1) dj =  imagePtr->height() - 1.0001;

     i = int(di);
     j = int(dj);

     u = di - i;
     v = dj - j;

     ggRGBFPixel c00((float) (*imagePtr)[i  ][j  ].r(), 
                     (float) (*imagePtr)[i  ][j  ].g(), 
                     (float) (*imagePtr)[i  ][j  ].b()); 
     ggRGBFPixel c01((float) (*imagePtr)[i  ][j+1].r(), 
                     (float) (*imagePtr)[i  ][j+1].g(), 
                     (float) (*imagePtr)[i  ][j+1].b()); 
     ggRGBFPixel c10((float) (*imagePtr)[i+1][j  ].r(), 
                     (float) (*imagePtr)[i+1][j  ].g(), 
                     (float) (*imagePtr)[i+1][j  ].b()); 
     ggRGBFPixel c11((float) (*imagePtr)[i+1][j+1].r(), 
                     (float) (*imagePtr)[i+1][j+1].g(), 
                     (float) (*imagePtr)[i+1][j+1].b()); 

     u = 3*u*u - 2*u*u*u;
     v = 3*v*v - 2*v*v*v;
// ggColorRatio is 1.0/255.0
     ggRGBFPixel c = (1 - u)*(1 - v)*c00 +
                     (    u)*(1 - v)*c10 +
                     (1 - u)*(    v)*c01 +
                     (    u)*(    v)*c11;
     
     return ggColorRatio * c;
}

ggRGBFPixel ggRasterSurfaceTexture::pointColor(const ggPoint2& p) const
{
     double di, dj;
     int i, j;

    
     di = p.x() * (imagePtr->width());
     dj = p.y() * (imagePtr->height());

     if (di < 0) di = 0;
     if (dj < 0) dj = 0;
     if (di >= imagePtr->width()) di =  imagePtr->width() - 0.0001;
     if (dj >= imagePtr->height()) dj =  imagePtr->height() - 0.0001;

     i = int(di);
     j = int(dj);


     ggRGBFPixel c((float)(*imagePtr)[i  ][j  ].r(), 
                   (float)(*imagePtr)[i  ][j  ].g(), 
                   (float)(*imagePtr)[i  ][j  ].b()); 

// ggColorRatio is 1.0/255.0
     return ggColorRatio * c;
}
