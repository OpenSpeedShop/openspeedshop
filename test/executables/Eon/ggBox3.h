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
// ggBox3.h-- declarations for class ggBox3
//
// Author: Peter Shirley
//         Aug. 30, 1993
//        
// Modified: Byung Sung Cho
//           Nov. 1, 1993
//

#ifndef GGBOX3_H
#define GGBOX3_H



#include <iostream.h>
#include <math.h>
#include <ggPoint3.h>
#include <ggRay3.h>

//
// ggBox3
//

class ggBox3 {
    friend ggBoolean ggOverlapBox3(const ggBox3& b1, const ggBox3& b2);
public:
    ggPoint3 min() const { return pmin;}
    ggPoint3 max() const { return pmax;}

    ggPoint3& min() { return pmin;}
    ggPoint3& max() { return pmax;}


    ggBox3() { }
    ggBox3(const ggPoint3& a, const ggPoint3& b) {pmin = a; pmax = b; }
    void Set(const ggPoint3& a, const ggPoint3& b) {pmin = a; pmax = b; }
    ggBox3(const ggBox3&  b) {pmin = b.min(); pmax = b.max();}

    void SetMin(const ggPoint3&  u) { pmin = u;}
    void SetMax(const ggPoint3&  u) { pmax = u;}

    ggBoolean hitByRay(const ggRay3& r, double& t1, double& t2) const;

    ggBoolean surroundsPoint(const ggPoint3& p) const
      {return p.x() >= pmin.x() && p.y() >= pmin.y() && p.z() >= pmin.z() &&
              p.x() <= pmax.x() && p.y() <= pmax.y() && p.z() <= pmax.z(); }

    ggPoint3 corner(int i, int j, int k) const {
        return ggPoint3((i)? pmax.x() : pmin.x(),
                        (j)? pmax.y() : pmin.y(),
                        (k)? pmax.z() : pmin.z());
    }


protected:
    ggPoint3 pmin;
    ggPoint3 pmax;
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


inline ggBoolean ggOverlapBox3(const ggBox3& b1, const ggBox3& b2) {
     return (b1.pmin.e[0]<= b2.pmax.e[0]&& b1.pmax.e[0]>= b2.pmin.e[0]&&
             b1.pmin.e[1]<= b2.pmax.e[1]&& b1.pmax.e[1]>= b2.pmin.e[1]&&
             b1.pmin.e[2]<= b2.pmax.e[2]&& b1.pmax.e[2]>= b2.pmin.e[2]);
}


// == operator
ggBoolean  operator==(const ggBox3& b1, const ggBox3 &b2);

// Output Operator
ostream &operator<<(ostream &os, const ggBox3 &t);

// Intersection of two boxes
ggBoolean ggCommonIntersectBox3(const ggBox3& b1,
				const ggBox3& b2, 
				ggBox3& bi);


#endif

