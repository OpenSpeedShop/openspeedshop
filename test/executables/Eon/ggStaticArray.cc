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






template <class T>
ggStaticArray<T>::ggStaticArray(int a) {
   arraySize = a;
   nx = a; ny = nz = 1;
   data = new T[arraySize];
}

template <class T>
ggStaticArray<T>::ggStaticArray(int a, int b) {
   arraySize = a*b;
   nx = a;
   ny = b;
   nz = 1;
   data = new T[arraySize];
}

template <class T>
ggStaticArray<T>::ggStaticArray(int a, int b, int c) {
   arraySize = a*b*c;
   nx = a;
   ny = b;
   nz = c;
   data = new T[arraySize];
}

template <class T>
ggStaticArray<T>::ggStaticArray(const ggStaticArray<T>& t) {
   arraySize = t.arraySize;
   nx = t.nx;
   ny = t.ny;
   nz = t.nz;
   data = new T[arraySize];
   for (int i = 0; i < t.length(); i++)
      data[i] = t(i);
}

template <class T>
ggStaticArray<T>::~ggStaticArray() {
   if (data) delete [] data;
}

template <class T>
ggStaticArray<T>& ggStaticArray<T>::operator=(const ggStaticArray<T>& t) {
   if (&t == this) return *this;
   nx = t.nx;
   ny = t.ny;
   nz = t.nz;
   if (data ) delete [] data;
   arraySize = t.arraySize;
   data = new T[arraySize];
   for (int i = 0; i < t.length(); i++)
      data[i] = t(i);
   return *this;
}

template <class T>
ggBoolean ggStaticArray<T>::Resize(int N) {
   T* t = new T[N];
   nx = N;
   ny = nz = 1;
   if (t == 0) return ggFalse;
   for (int i = 0; i < length() && i < N; i++)
      t[i] = data[i];
   arraySize = N;
   if (data) delete [] data;
   data = t;
   return ggTrue;
}

template <class T>
ggBoolean ggStaticArray<T>::Resize(int a, int b) {
   T* t = new T[a*b];
   nx = a;
   ny = b;
   nz = 1;
   if (t == 0) return ggFalse;
   for (int i = 0; i < length() && i < a*b; i++)
      t[i] = data[i];
   arraySize = a*b;
   if (data) delete [] data;
   data = t;
   return ggTrue;
}

template <class T>
ggBoolean ggStaticArray<T>::Resize(int a, int b, int c) {
   T* t = new T[a*b*c];
   nx = a;
   ny = b;
   nz = c;
   if (t == 0) return ggFalse;
   for (int i = 0; i < length() && i < a*b*c; i++)
      t[i] = data[i];
   arraySize = a*b*c;
   if (data) delete [] data;
   data = t;
   return ggTrue;
}

