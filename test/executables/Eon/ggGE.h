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



#ifndef GGGE_H
#define GGGE_H

class ggGE
{
public:

    //
    // constructors
    //
    
    ggGE(ggFloat grey); // this is float->ge
    ggGE(double grey); // this is double->ge
    ggGE(int grey); // this is int->ge
    // others?

    //
    // destructors
    //
    ~ggGE(); // use default?

    //
    // access operators/methods
    //
    ggGE operator=(const ggGE& b);
    ggBoolean operator==(const ggGE& b);
    ggGE operator+(const ggGE& b);
    ggGE operator*(const ggGE& b);
    ggGE operator-(const ggGE& b);
    ggGE operator/(const ggGE& b);

    friend ostream &operator<<(ostream os);
    friend istream &operator>>(istream is);
    //
    // miscellaneous methods
    //
    void ggConvertToDouble( double a);

protected:
    // note: 
    // data [0] == grey
    // data [1] == exponent
    
    unsigned char data[2];
};

