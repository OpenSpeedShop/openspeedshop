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
// ggBox2.h-- declarations for class ggBox2
//
// Author: Byung Sung Cho
//         Nov. 23, 1993
//        
// Modified: 
//
// Copyright 1993 by Byung Sung Cho
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
//

#ifndef GGBOX2_H
#define GGBOX2_H



#include <iostream.h>
#include <math.h>
#include <ggPoint2.h>
#include <ggRay2.h>

//
// ggBox2
//

class ggBox2 {

public:

    ggBox2() {
      pmin = ggPoint2(0.0, 0.0);  // Default initializer
      pmax = ggPoint2(0.0, 0.0);
    }
    ggBox2(const ggPoint2& a, const ggPoint2& b) {pmin = a; pmax = b; }
    void Set(const ggPoint2& a, const ggPoint2& b) {pmin = a; pmax = b; }
    ggBox2(const ggBox2&  b) {pmin = b.min(); pmax = b.max();}

    void SetMin(const ggPoint2&  u) { pmin = u;}
    void SetMax(const ggPoint2&  u) { pmax = u;}

    ggPoint2 min() const { return pmin;}
    ggPoint2 max() const { return pmax;}

    ggPoint2& min() { return pmin;}
    ggPoint2& max() { return pmax;}
    
    ggBoolean hitByRay(const ggRay2& r, double& t1, double& t2) const;



    ggBoolean surroundsPoint(const ggPoint2& p) const
      {return p.x() >= min().x() && p.y() >= min().y() && 
              p.x() <= max().x() && p.y() <= max().y(); }


protected:
    ggPoint2 pmin;
    ggPoint2 pmax;
};



//
// Declarations
//

//
// Friends
//

//
// Non-friends
//

ggBoolean ggOverlapBox2(const ggBox2& b1, const ggBox2 &b2);

// == operator
ggBoolean  operator==(const ggBox2& b1, const ggBox2 &b2);

// Output Operator
ostream &operator<<(ostream &os, const ggBox2 &t);

// Intersection of two boxes
ggBoolean ggCommonIntersectBox2(const ggBox2& b1,
				const ggBox2& b2, 
				ggBox2& bi);


#endif

