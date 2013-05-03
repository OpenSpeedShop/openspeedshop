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
// ggFPoint3.h-- declarations for class ggFPoint3
//
// Author:    Pete Shirley
//            10/30/93
//
// Simple points used only for storage-- has conversion op to ggFPoint3


#ifndef GGFPOINT3_H
#define GGFPOINT3_H



#include <stdlib.h>
#include <ggPoint3.h>
#include <ggMacros.h>



//  Class ggFPoint3


class ggFPoint3  {
    
public:
    
    // Constructors/Destructors

    ggFPoint3(double e0, double e1, double e2) {
                e[0] = e0; e[1] = e1; e[2] = e2; }
    ggFPoint3() { e[0] = e[1] = e[2] = 0.0; }


    
   inline void Set(double a, double b, double c) {e[0] = a; e[1] = b; e[2] = c;}
    inline float x() const { return e[0]; }
    inline float y() const { return e[1]; }
    inline float z() const { return e[2]; }
    inline float &x() { return e[0]; }
    inline float &y() { return e[1]; }
    inline float &z() { return e[2]; }


    ggFPoint3(const ggFPoint3 &p) {x() = p.x(); y() = p.y(); z() = p.z();}
    ggFPoint3(const ggPoint3 &p) {x() = p.x(); y() = p.y(); z() = p.z();}
    operator ggPoint3() const { return ggPoint3(e[0], e[1], e[2]); } 

    inline void SetX(double a) { x() = a; }
    inline void SetY(double a) { y() = a; }
    inline void SetZ(double a) { z() = a; }
    
    // Operators

    inline float operator[](int i) const;
    inline float& operator[](int i) ;

     
private:
     float e[3];
};



// [] operator

inline float& ggFPoint3::operator[](int i) { 

#ifdef GGSAFE
    if ( i < 0 || i > 2 ) 
    {
	fprintf(stderr, "ggFPoint3 : out of range :%d\n", i);
	exit(1);
    }
#endif

    return e[i]; 
}
inline float ggFPoint3::operator[](int i) const { 

#ifdef GGSAFE
    if ( i < 0 || i > 2 ) 
    {
	fprintf(stderr, "ggFPoint3 : out of range :%d\n", i);
	exit(1);
    }
#endif

    return e[i]; 
}


#endif

