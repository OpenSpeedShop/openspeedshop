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
// ggRaster.h-- declarations for class ggRaster
//
// one-dim version
//
// Author:    Tom Loos, Beata Winnicka
//            October ?? 1993
// Modified:
//
//
// Copyright 1993 by Tom Loos, Beata Winnicka  
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
//
//


#ifndef GGRASTER_H
#define GGRASTER_H
//#define GGSAFE
// uncomment next define if you want to use 2 dim implementation
// (an array of pointers to colums); 2 dim implementation is faster than
// the one dim implementation on SUNs but slower on SGIs
//#define GGRASTER2D    //commented out for SGI

#include <iostream.h>
#include <math.h>
#include <stdlib.h>
#include <ggBoolean.h>
#include <stdio.h>

template <class T>          class ggGrayPixel;
                            class ggPgm;
                            class ggPpm;
                            class ggVector2;
                            class ggVector3;
                            class ggPoint2;
                            class ggPoint3;
                            class ggHPoint3;
template <class T>          class ggRaster;
                            class ggRasterCoreP;
template <class T>          class ggRasterTCoreP;
template <class T>          class ggRGBPixel;
#include <ggPoint2.h>


//  a ggRaster stores an ordered set of objects in such a way
//  that the objects can be entered or retrieved in a similar
//  way as to/from 2-dim array
//  useful for storing images and surface textures
//  in [i][j] and (i,j) i is column (i.e. x) index 
//  and j is row (i.e. y) index

template <class T>
class ggRaster {

public:
    inline ggRaster():w(0),h(0),data(0){} //creates empty raster
    inline ggRaster(const int width, const int height):
               w(0), h(0), data(0) {Resize(width, height);}
                             //creates width by height   
                            // raster, allocates memory by calling Resize
    ~ggRaster() { Delete(); }
    void Delete();

#ifndef GGSAFE
#ifndef GGRASTER2D
    inline T operator()(int x, int y) const {return data[x*h+y];} 
                    //returns object at position x,y no error checking
    inline T &operator()(int x, int y)  {return data[x*h+y];}
                   //does not do error checking

    inline T *  operator[](int x) { return &data[x*h]; }
    inline void SetData(const T newdata, int x, int y)
                {data[x*h+y] = newdata;} //store objext at position x,y
                                     // no error checking!!
#else 
    inline T operator()(int x, int y) const {return data[x][y];}
    inline T &operator()(int x, int y) {return data[x][y];} 

    inline T * operator[](int x) { return data[x]; }
    inline T  const * operator[](int x) const { return data[x]; }
    inline void SetData(const T newdata, int x, int y) 
                 { data[x][y] = newdata; }
#endif 
#endif 
#ifdef GGSAFE
     T operator()(int x, int y) const;
                    //returns object at position x,y does error checking
     T &operator()(int x, int y);
                   //does  error checking

     T *  operator[](int x); 
#ifdef GGRASTER2D
     T const * operator[] (int x) const;
#endif
     void SetData(const T newdata, int x, int y);
                                       //store objext at position x,y
                                     // with error checking!!

#endif 

    void Resize(int width, int height); //delete old raster (if any,
                         // and allocate new memory

    int width() const { return w; }
    int height() const { return h; }

    ggRaster<T>& operator=(const ggRaster<T>& rhs);
    ggBoolean operator==(const ggRaster<T> rhs) const;



protected:

    int w, h;
#ifndef GGRASTER2D
    T *data; 
#else 
    T **data;
#endif  
};

//  functions outside the class

template <class T>
    ggRaster<T> &operator*=(ggRaster<T> &r, double &f);
template <class T>
    ggRaster<T> &operator*=(ggRaster<T> &r, float &f);
template <class T1, class T2>
    ggRaster<T1> &operator*=(ggRaster<T1> &r, ggRaster<T2> &f);
template <class T1, class T2>
    ggRaster<T1> &operator+=(ggRaster<T1> &r, ggRaster<T2> &f);
template <class T>
    ggRaster<T> * ggConvolveWithGaussian(ggRaster<T> input, double coef);
template <class T>
    ggRaster<T> * ggEnlargeByTwo(ggRaster<T> input);
template <class T>
    ggRaster<T> * ggReduceByTwo(ggRaster<T> input);
template <class T>
    T* ggAverage(ggRaster<T> r);
template <class T>
    int ggDump(char *pixel_out, ggRaster<T> const &r);


// draw a gauraud shaded triangle
// p0,p1,p2 in pixel coordinates-- raster is [0,width] by [0,height]
template <class T>
   void ggDrawTriangle(ggRaster<T>& ras,
                       const ggPoint2& p0,
                       const ggPoint2& p1,
                       const ggPoint2& p2,
                       const T& c0,
                       const T& c1,
                       const T& c2);
//#if defined( _WIN32 )
#include "ggRaster.cc"
//#endif
#endif
