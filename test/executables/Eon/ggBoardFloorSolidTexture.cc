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



#include <math.h>
#include <ggBoardFloorSolidTexture.h>



ggBoolean ggBoardFloorSolidTexture::getTexture(const ggPoint3& argp,
                                     ggSpectrum& tex,
                                     double& cov) const
{
     int i, j;
     double boardLength;
     ggPoint3 p;

     if (axis == 0)
        p.Set(argp.y() + 10000, argp.z() + 10000,  argp.x() + 10000); 
     else if (axis == 1)
        p.Set( argp.x() + 10000,  argp.z() + 10000,  argp.y() + 10000);
     else
        p.Set( argp.x() + 10000, argp.y() + 10000, argp.z() + 10000); 
     i = int(p.x()/ boardWidth); 

     ggPoint3 offset; 

     offset.x() = p.x() - i*boardWidth;
     offset.z() = 0.0;

     int ni = noise.intGamma(i, 7);

     if (i % 3 == 0) {
          j = int(p.y()  / boardLength0);
          offset.y() = p.y() - j * boardLength0;
          boardLength = boardLength0;
     }
     else if (i % 3 == 1) {
          j = int(((p.y() + ni*boardLength0)  / boardLength1));
          offset.y() = p.y() + ni*boardLength0 - j * boardLength1;
          boardLength = boardLength1;
     }
     else {
          j = int(((p.y() + ni*boardLength1)  / boardLength2));
          offset.y() = p.y() + ni*boardLength1 - j * boardLength2;
          boardLength = boardLength2;
     }

     int ran = noise.intGamma(i,j);

     int xran = ran % 16;
     ran /= 16;
     int yran = ran % 2;
     ran /= 2;
     int zran = ran % 8;

     if (offset.x() < 0.02*boardWidth || offset.y() < 0.04*boardWidth) {
         cov = 0.0;
         tex.Set(0.0);
         return ggTrue;
     }

     double mult = 0.75 + 0.05 * (ran % 6);
     ggPoint3 start(boardWidth, -boardWidth, 0.0);
     ggVector3 offsetTree(xran*boardDepth ,
                                  yran*boardWidth + offset.x(),
                                  zran*boardDepth + offset.y());
     ggPoint3 inTree = start + offsetTree;

     ggBoolean temp = tPtr->getTexture(inTree, tex, cov);
     tex *= mult;
     return temp;
      
}
