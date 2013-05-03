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
// ggFineSpectrum.h-- declarations for class ggFineSpectrum
//
// Author:    Peter Shirley
//            10/20/93
// Modified:
//
//
// Copyright 1993 by Peter Shirley
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
//

#ifndef GGFINESPECTRUMH
#define GGFINESPECTRUMH

#include <iostream.h>
#include <assert.h>
#include <ggMacros.h>



class ggFineSpectrum {

public:
    ggFineSpectrum() {};
    int nComponents() const { return NLAMBDA; }
    float wavelength(int i) const { return float (MINLAMBDA + i) ; }
    int minLambda() const {return MINLAMBDA;}
    int maxLambda() const {return MAXLAMBDA;}
#ifdef GGSAFE
    float operator[] (int i) const { return data[i]; }
    float& operator[] (int i)      { return data[i]; }
#else
    float operator[] (int i) const { assert (i >= 0 && i < NLAMBDA);
                                     return data[i]; }
    float& operator[] (int i)      { assert (i >= 0 && i < NLAMBDA);
                                     return data[i]; }
#endif
    float amplitude(int lambda) const { return (*this)[index(lambda)]; }
    int index(int lambda) const { return lambda-MINLAMBDA; }
    void SetRange(int lambda1, float amp1, int lambda2, float amp2);
    double area(double l1, double l2) const;

private:
    double sum(int index1, int index2) const;
    enum { MINLAMBDA = 400 };  // hack-- static constant
    enum { MAXLAMBDA = 700 };
    enum { NLAMBDA   = 301 };  // MAXLAMBDA + 1 - MINLAMBDA
    float data[NLAMBDA];
};

istream& operator>>(istream&, ggFineSpectrum&);
ostream& operator<<(ostream&, const ggFineSpectrum&);
ggBoolean operator==(const ggFineSpectrum& s1, const ggFineSpectrum& s2);
ggBoolean operator!=(const ggFineSpectrum& s1, const ggFineSpectrum& s2);

#endif
