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



#include <iostream.h>

template <class T>
ggTrain<T>::ggTrain() {
   nData = 0;
   arraySize = 4;
   data = new T[arraySize];
}

template <class T>
ggTrain<T>::ggTrain(int a) {
   nData = 0;
   arraySize = a;
   data = new T[arraySize];
}

template <class T>
ggTrain<T>::ggTrain(const ggTrain<T>& t) {
   nData = t.length();
   arraySize = t.arraySize;
   data = new T[arraySize];
   for (int i = 0; i < t.length(); i++)
      data[i] = t[i];
}

template <class T>
ggTrain<T>::~ggTrain() {
   nData = 0;
   delete [] data;
}

template <class T>
ggTrain<T>& ggTrain<T>::operator=(const ggTrain<T>& t) {
   if (&t == this) return *this;
   if (data != 0) delete [] data;
   nData = t.length();
   arraySize = t.arraySize;
   data = new T[arraySize];
   for (int i = 0; i < t.length(); i++)
      data[i] = t[i];
   return *this;
}

template <class T>
ggBoolean ggTrain<T>::Append(T item) {
    if (nData == arraySize) {
       arraySize *= 2;
       T *temp = data;
       if (!(data = new T[arraySize])) return ggFalse;
       for (int i = 0; i < nData; i++)
           data[i] = temp[i];
       delete [] temp;
    }
    data[nData++] = item;
    return ggTrue;
}

template <class T>
T ggAverage(const ggTrain<T>& t) {
#ifdef GGSAFE
    assert (t.length() > 0);
#endif
    T sum = t[0];
    for (int i = 1; i < t.length(); i++)
        sum += t[i];
    return (1.0 / double(t.length())) * sum;
}

