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



#include <mrFastTriangle.h>
#include <ggMacros.h>
#include <ggGeometry.h>
#include <math.h>


ggBoolean mrFastTriangle::print(ostream& s) const {
    s << "mrFastTriangle" << "\n";
    s << triangle << "\n";
    return ggTrue;
}



ggBoolean mrFastTriangle::boundingBox(
             double, // low end of time range
             double, // low end of time range
             ggBox3& box) const
{ 
   box = bbox;
   return ggTrue;
}

