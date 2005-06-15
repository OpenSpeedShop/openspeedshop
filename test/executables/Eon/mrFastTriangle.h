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



#ifndef MRFASTTRIANGLE_H
#define MRFASTTRIANGLE_H


#include <mrSurface.h>
#include <mrTriangle.h>


class mrFastTriangle : public mrTriangle {
public:

  //  returns true if bounding box is found
  virtual ggBoolean boundingBox(
             double time1, // low end of time range
             double time2, // low end of time range
             ggBox3& bbox)
             const;

 virtual ggBoolean print(ostream& s) const;

 mrFastTriangle(const ggPoint3 pt0, 
	      const ggPoint3 pt1,
              const ggPoint3 pt2) : mrTriangle(pt0, pt1, pt2) {
        bbox = triangle.boundingBox();
  }

  protected:
     mrFastTriangle() { }
     ggBox3 bbox;
};


#endif
