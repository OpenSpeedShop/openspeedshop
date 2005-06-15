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



#ifndef GGJRSAMPLE2_H
#define GGJRSAMPLE2_H

#include <ggSample2.h>

class ggJRSample2 : public ggSample2 {
public:
    ggJRSample2() { iData = 0; jData = 0; xData = yData = 0; }
    ggJRSample2(int N);
    virtual void Generate(); 
    virtual void SetN(int);
    virtual ggPoint2 getPoint(int num) const;
protected:
    double *xData;
    double *yData;
    int *iData;
    int *jData;
    int n;
    int nSquared;
};

#endif



