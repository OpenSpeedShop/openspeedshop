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




// p0,p1,p2 in pixel coordinates-- raster is [0,width] by [0,height]
template <class T>
   void ggDrawTriangle(ggRaster<T>& ras,
                       const ggPoint2& p0,
                       const ggPoint2& p1,
                       const ggPoint2& p2,
                       const T& c0,
                       const T& c1,
                       const T& c2) {


    double x0, y0;
    double x1, y1;
    double x2, y2;
    double u, v;
    T c_init, c;
    double u_init, v_init;
    double xmin, xmax, ymin, ymax;
    int imin, imax, jmin, jmax;
    double u00, u01, u10;
    double v00, v01, v10;
    double dux, duy, dvx, dvy;
    T dcx, dcy;
    T c00, c10, c01;
    
    int i, j;

    x0 = p0.x();
    x1 = p1.x();
    x2 = p2.x();

    y0 = p0.y();
    y1 = p1.y();
    y2 = p2.y();
   
    xmin = ggMin(x0, x1, x2);
    xmax = ggMax(x0, x1, x2);
    ymin = ggMin(y0, y1, y2);
    ymax = ggMax(y0, y1, y2);
  
  // find bbox of triangles in discrete pixel coords
    imin = int(xmin + 0.5);
    imax = int(xmax);
    jmin = int(ymin + 0.5);
    jmax = int(ymax);
  // clip range to legal screen values
    if (imin < 0) imin = 0;
    if (imax >= ras.width()) imax = ras.width() - 1; 
    if (jmin < 0) jmin = 0;
    if (jmax >= ras.height()) jmax = ras.height() - 1; 

    double denom_inv = (x1-x0) * (y2-y0) - (x2-x0) * (y1-y0);
    denom_inv = 1 / denom_inv;

    u00 = (double(imin) - x0) * (y2-y0) - (x2-x0)*(double(jmin) - y0);
    u00 *= denom_inv;
    v00 = (x1-x0)*(double(jmin) - y0)  - (double(imin) - x0) * (y1-y0);
    v00 *= denom_inv;

    u10 = (double(imax) - x0) * (y2-y0) - (x2-x0)*(double(jmin) - y0);
    u10 *= denom_inv;
    v10 = (x1-x0)*(double(jmin) - y0)  - (double(imax) - x0) * (y1-y0);
    v10 *= denom_inv;

    u01 = (double(imin) - x0) * (y2-y0) - (x2-x0)*(double(jmax) - y0);
    u01 *= denom_inv;
    v01 = (x1-x0)*(double(jmax) - y0)  - (double(imin) - x0) * (y1-y0);
    v01 *= denom_inv;

    double inv_di, inv_dj;
    inv_di = 1.0 /  double(imax - imin);
    inv_dj = 1.0 /  double(jmax - jmin);
    dux = (u10 -u00) * inv_di;
    duy = (u01 -u00) * inv_dj;
    dvx = (v10 -v00) * inv_di;
    dvy = (v01 -v00) * inv_dj;
    u_init  = u00;
    v_init = v00;

    c00 = c0 + u00 * (c1 - c0) + v00 * (c2 - c0);
    c10 = c0 + u10 * (c1 - c0) + v10 * (c2 - c0);
    c01 = c0 + u01 * (c1 - c0) + v01 * (c2 - c0);
    dcx = (c10 - c00) * inv_di;
    dcy = (c01 - c00) * inv_dj;
    c_init = c00;

    for (j = jmin; j <= jmax; j++)
    {
       c = c_init;
       u = u_init;
       v = v_init;
       for (i = imin; i <= imax; i++) {
           if (u > 0 && v > 0 && (u + v) < 1)
              ras[i][j] = c;
           u += dux;
           v += dvx;
           c += dcx;
       }
       c_init += dcy;
       v_init += dvy;
       u_init += duy;
    }

}

template <class T>
void ggRaster<T>::Delete() 
{
    if (data != 0) {
#ifdef GGRASTER2D
      for (int i=0; i <w; i++)
          delete [] data[i];
#endif 
        delete [] data;
        }
    w = h = 0;
    data = 0;
}

#ifdef GGSAFE

template <class T>
T ggRaster<T>::operator()(int x, int y) const {
    if (x < 0 || x > w - 1) {
        cerr << "x out of range" << endl;
        exit(-1);
    }
    if (y < 0 || y > h - 1) {
        cerr << "y out of range" << endl;
        exit(-1);
    }
#ifndef GGRASTER2D
    return data[x*h+y];
#else  
    return data[x][y];
#endif
}



template <class T>
T &ggRaster<T>::operator()(int x, int y) {
    if (x < 0 || x > w - 1) {
        cerr << "x out of range" << endl;
        exit(-1);
    }
    if (y < 0 || y > h - 1) {
        cerr << "y out of range" << endl;
        exit(-1);
    }
#ifndef GGRASTER2D
    return data[x*h+y];
#else 
    return data[x][y];
#endif
}

template <class T>
     T *ggRaster<T>::operator[](int x){
    if (x < 0 || x > w - 1) {
        cerr << "ggRaster, operator[], x out of range" << endl;
        exit(-1);
    }
#ifndef GGRASTER2D
    return &data[x*h];
#else 
    return data[x];
#endif
} 

#ifdef GGRASTER2D
template <class T>
     T const *ggRaster<T>::operator[](int x) const{
    if (x < 0 || x > w - 1) {
        cerr << "ggRaster, operator[], x out of range" << x << endl;
        exit(-1);
    }
    return data[x];
} 
#endif

template <class T>
     void ggRaster<T>::SetData(const T newdata, int x, int y){
    if (x < 0 || x > w - 1) {
        cerr << "x out of range" << endl;
        exit(-1);
    }
    if (y < 0 || y > h - 1) {
        cerr << "y out of range" << endl;
        exit(-1);
    }
#ifndef GGRASTER2D
    data[x*h+y] = newdata;
#else 
   data[x][y] = newdata;
#endif
}

#endif

template <class T>
void ggRaster<T>::Resize(int width, int height) 
{
#ifdef GGSAFE
    if ((width < 1) || (height < 1))
    {
        cerr << "width and height must be positive\n";
        exit(-1);
    }
#endif 

    if (data != 0)
        Delete();
    w = width;
    h = height;
#ifndef GGRASTER2D
    data = new T [w*h];
#else 
    data = new T *[w];
    for (int i = 0; i < w; i++)
        data[i] = new T [h];
#endif
}

  template <class T>
ggRaster<T>& ggRaster<T>::operator=(const ggRaster<T>& rhs)
{
  int i;
#ifdef GGRASTER2D
  int j;
#endif
  if (this == &rhs) return *this;   
  w=rhs.w;
  h=rhs.h;
  Resize(w,h);
#ifndef GGRASTER2D
    for(i=0;i<w*h; i++) data[i]=rhs.data[i];
#else 
    for (i = 0; i < w; i++)
    {
        for (j = 0; j < h; j++)
        {
            data[i][j] = rhs[i][j];
        }
    }
#endif
  return *this;
}

  template <class T>
ggBoolean ggRaster<T>::operator==(const ggRaster<T> rhs) const
{
    int i;
#ifdef GGRASTER2D
    int j;
#endif
    // simple test
    if (&rhs == this)
    {
        return ggTrue;
    }

  if(w == rhs.w && h == rhs.h) {
#ifndef GGRASTER2D
          for(i=0;i<w*h; i++) 
             if( (data[i] == rhs.data[i]) != ggTrue) return ggFalse;
#else 
          for(i=0; i<w; i++){
            for(j=0; j<h; j++){
             if( (data[i][j] == rhs.data[i][j]) != ggTrue) return ggFalse;
            }
          }
#endif  
           return ggTrue;
  }
  return ggFalse;
}

//
// non-class functions
//

template <class T>
ggRaster<T> &operator *=(ggRaster<T> &r, double &f)
{
    int i,j;
    int width = r.width();
    int height = r.height();

    for (i = 0; i < width; i++)
    {
        for (j = 0; j < height; j++)
        {
            r[i][j] *= f;
        }
    }
    return r;
}



template <class T>
ggRaster<T> &operator *=(ggRaster<T> &r, float &f)
{
    int i, j;
    int width = r.width();
    int height = r.height();
    for (i = 0; i < width; i++)
    {
        for (j = 0; j < height; j++)
        {
            r[i][j] *= f;
        }
    }
    return r;
}

template <class T1, class T2>
ggRaster<T1> &operator*=(ggRaster<T1> &t1, ggRaster<T2> &t2)
{
    int width = t1.width();
    int height = t1.height();
    int i,j;

#ifdef GGSAFE

    int t2width = t2.width();
    int t2height = t2.height();
    
    if ((width != t2width) || (height != t2height))
    {
        cerr << "rasters must be of same size for *=\n";
        exit(-1);
    }
        
#endif 

    for (i = 0; i < width; i++)
    {
        for (j = 0; j < height; j++)
        {
            t1[i][j] *= t2[i][j];
        }
    }
    return t1;
}


template <class T1, class T2>
ggRaster<T1> &operator+=(ggRaster<T1> &t1, ggRaster<T2> &t2)
{
    int width = t1.width();
    int height = t1.height();
    int i, j;
#ifdef GGSAFE
    int t2width = t2.width();
    int t2height = t2.height();
    
    if ((width != t2width) || (height != t2height))
    {
        cerr << "rasters must be of same size for +\n";
        exit(-1);
    }
        
#endif 

    for (i = 0; i < width; i++)
    {
        for (j = 0; j < height; j++)
        {
            t1[i][j] += t2[i][j];
        }
    }
    return t1;
}

#define tabs(x) (((x)<0)?-(x):(x))

template <class T>
ggRaster<T> * ggConvolveWithGaussian(ggRaster<T> input, double coef)
{
   int i, j, p, q, imax, ilower, iupper, jlower, jupper;
   int itemp;
   T num;
   double lden, f12, totarea, invtotarea, tmp123;
   int w=input.width();
   int h=input.height();
   int size = ((w >= h) ? w : h) ;  //take max of w and h
   ggRaster<T> * ans = new ggRaster<T>;
   ans->Resize(w,h);

   const double epsilon = 1.0e-10; //smallest values of filter that will be
                                   //treated as non-zero
   double maxcoef = -log(epsilon);
   if(coef > maxcoef){
      cerr<<" ggConvolveWithGaussian: \n";
      cerr<<" width of Gaussian smaller than one pixel \n";
      cerr<<" otput image identical to input image \n";
      (*ans) = input;
      return ans;
      }
   double * filter = new double [size+1];
   
   filter[0]=1.;
   for(imax=1; imax<=size; imax++)
      {
       if((filter[imax] = exp(-coef*imax*imax)) < epsilon) break;
      }
// memset filter from imax+1 to size+1 to all zeros

   memset((char*)(&filter[imax+1]),0,sizeof(double)*(size-imax));

   // imax is the index of the largest non-zero filter value
   // if 2*imax is less than height and less than width, precalculate
   //  total area under the Gaussian and its inverse 
   if(2*imax < h && 2*imax < w){
     totarea = 0.0;
          for(i = 0; i < (2*imax+1); i++){
             for(j = 0; j < (2*imax+1); j++){
                f12 = filter[tabs((long)(i-imax))] * filter[tabs((long)(j-imax))];
                totarea = totarea +f12;
             }
           }
     invtotarea=1./totarea;
    }

// precalculate denominator for one quadrant

    int denw = (w+1)/2;
    int denh = (h+1)/2;
    ggRaster<double> den(denw, denh);
    for(p = 0; p < denw; p++){
       for(q = 0; q < denh; q++){    //gaussian fits inside the image
          if(p >= imax && q>= imax) { 
             den[p][q] = totarea;}
          else{
           itemp = p-imax;
           ilower = (itemp>0) ? itemp : 0;
           itemp =  p+imax+1;
           iupper = (itemp>w) ? w : itemp;

           itemp = q-imax;
           jlower = (itemp>0) ? itemp : 0;
           itemp =  q+imax+1;
           jupper = (itemp>h) ? h : itemp;

            if(q == 0){  //will use incremental method as q
                         //changes to q+1, need starting value
               lden = 0.0;
               for(i = ilower; i < iupper; i++){
                  for(j = jlower; j < jupper; j++){
                       f12 = filter[tabs((long)(i-p))] * filter[tabs((long)(j-q))];
                       lden = lden + f12;
                  }
                }
                den[p][q] =lden;
            }
            else if(jlower > 0) { //no first add needed
               if(jupper < h){ //no last subtract needed
                  den[p][q]=den[p][q-1];
               }
               else{ //need to do last subtract
                  lden = 0.0;
                  j = jupper - 1;
                  for(i = ilower; i < iupper; i++){
                     f12 = filter[tabs((long)(i-p))] * filter[tabs((long)(j-q+1))];
                     lden = lden + f12;
                  }
                  den[p][q] = den[p][q-1] - lden;
              }
            }
            else { //need first add
               if(jupper < h){ //no last subtract needed
                  lden = 0.0;
                  j=jlower;
                  for(i = ilower; i < iupper; i++){
                     f12 = filter[tabs((long)(i-p))] * filter[tabs((long)(j-q))];
                     lden = lden + f12;
                  }
                  den[p][q]=den[p][q-1] + lden;
               }
               else{ //need to do last subtract
                  lden = 0.0;
                  j = jupper - 1;
                  for(i = ilower; i < iupper; i++){
                     f12 = filter[tabs((long)(i-p))] * filter[tabs((long)(j-q+1))];
                     lden = lden - f12;
                  }
                  j=jlower;  //first add
                  for(i = ilower; i < iupper; i++){
                     f12 = filter[tabs((long)(i-p))] * filter[tabs((long)(j-q))];
                     lden = lden + f12;
                  }
                  den[p][q]=den[p][q-1] + lden;
              }
          }
       }
    }
   }
// find inverses
    for(p = 0; p < denw; p++){
       for(q = 0; q < denh; q++){
          if(p >= imax && q>= imax) { 
             den[p][q] = invtotarea;}
          else{
              den[p][q] = 1./den[p][q];
          }
       }
     }
 
//calculate numerator and multiply by den 
   for(p = 0; p < w; p++){
      for(q = 0; q < h; q++){

            tmp123  = 0.0;
            num = input[0][0] * tmp123;

           itemp = p-imax;
           ilower = (itemp>0) ? itemp : 0;
           itemp =  p+imax+1;
           iupper = (itemp>w) ? w : itemp;

           itemp = q-imax;
           jlower = (itemp>0) ? itemp : 0;
           itemp =  q+imax+1;
           jupper = (itemp>h) ? h : itemp;
   
          for(i = ilower; i < iupper; i++){
             for(j = jlower; j < jupper; j++){
                f12 = filter[tabs((long)(i-p))] * filter[tabs((long)(j-q))];
                num = num + input[i][j]*f12;
             }
           }
           (*ans)[p][q] = num * 
                 den[((p < denw) ? p : (w-1-p))][((q < denh) ? q : (h-1-q))];
       }
    }
    delete [] filter;
    den.Delete();
    return ans;
}             

template <class T>
    ggRaster<T> * ggEnlargeByTwo(ggRaster<T> input)
{
    int p, q;
    int w = 2*(input.width());
    int h = 2*(input.height());
    ggRaster<T> * ans = new ggRaster<T>;
    ans->Resize(w,h);

   for(p = 0; p < w; p++){
      for(q = 0; q < h; q++){
         (*ans)[p][q] = input[p/2][q/2];
      }
   }
   return ans;
}
template <class T>
    ggRaster<T> * ggReduceByTwo(ggRaster<T> input)
{
    int p, q, p2, p21, q2, q21;
    int oldwidth = input.width();
    int oldheight = input.height();
    int w = (oldwidth+1)/2;
    int h = (oldheight+1)/2;
    ggRaster<T> * ans = new ggRaster<T>;
    ans->Resize(w,h);

   for(p = 0; p < w; p++){
      for(q = 0; q < h; q++){
         p2 = 2*p;
         p21 = 2*p+1;
         p21 = (p21==oldwidth) ? p21 - 1 : p21;
         q2 = 2*q;
         q21 = 2*q+1;
         q21 = (q21==oldheight) ? q21 - 1 : q21;

         (*ans)[p][q]= (input[p2][q2]+input[p21][q21]
                              +input[p2][q21]+input[p21][q2]) * 0.25;
      }
    }
    return ans;
}

template <class T>
    T* ggAverage(ggRaster<T> r)
{
    int i,j;
    int width = r.width();
    int height = r.height();
    double invden = 1./double(width*height);
    T temp;
    double tmp123;

    T* ans = new T;
    *ans = 0.;
    tmp123 = 0.;

    for (i = 0; i < width; i++)
    {
        temp = r[0][0]*tmp123;
        for (j = 0; j < height; j++)
        {
           temp += r[i][j];
        }
    *ans += temp * invden;
    }
    return ans;
}

template <class T>
    int ggDump(char *pixel_out, ggRaster<T> const &r)
{
    int i,j,jj;
    int width = r.width();
    int height = r.height();
    T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
    float r1, r2, r3, r4, r5, r6, r7, r8;
    FILE *fp;

    fp = fopen(pixel_out,"w");
    if (fp == NULL ) {
      fprintf(stderr, "Could not open file %s.  Aborting...\n", pixel_out);
      exit(0);
    }
    for (i = 0; i < width; i++)
    {
        for (j = 0; j < height-7; j+=8)
        {
           temp1 = r(i,j);
           temp2 = r(i,j+1);
           temp3 = r(i,j+2);
           temp4 = r(i,j+3);
           temp5 = r(i,j+4);
           temp6 = r(i,j+5);
           temp7 = r(i,j+6);
           temp8 = r(i,j+7);
           r1 = temp1.r();
           r2 = temp2.r();
           r3 = temp3.r();
           r4 = temp4.r();
           r5 = temp5.r();
           r6 = temp6.r();
           r7 = temp7.r();
           r8 = temp8.r();
           fprintf(fp, "%d  %d  %d  %f  %f  %f  %f  %f  %f  %f  %f\n", i, j, j+7, r1, r2, r3, r4, r5, r6, r7, r8);
//           cerr << i << "  " << j << "  ";
//           cerr << temp.r() << "  " << temp.g() << "  " << temp.b() << "\n";
        }
        if ( j < height ) fprintf(stderr, "%d  %d  %d", i, j, height-1);
        for (jj = j; jj < height; jj++) {
           temp1 = r(i,jj);
           r1 = temp1.r();
           fprintf(fp, "  %f", r1 );
        }
        if ( j < height ) fprintf(stderr, "\n");
    }
    fclose(fp);
    return 1;
}
