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



// This class generates sample points that are distributed over a unit cube in
// such a way that no two samples are closer than a fixed distance, specified
// during initialization. The strategy degenerates into Random sampling
// after a certain number of failures.
//
// Author:     Peter Shirley
// Modified:   Rajesh Kamath, 1993
//
// Copyright 1993 by Peter Shirley, Rajesh Kamath
//
// Permission to use, copy, and distribute for non-commercial purposes,
// is hereby granted without fee, providing that the above copyright
// notice appears in all copies.
//
// The software may be modified for your own purposes, but modified versions
// may not be distributed.
//

#ifndef GGPOISSONSPHERESAMPLE3_H
#define GGPOISSONSPHERESAMPLE3_H

#include <ggSample3.h>
#include <ggConstants.h>


class ggPoissonSphereSample3 : public ggSample3{
public:
				// Constructors
  ggPoissonSphereSample3();
  ggPoissonSphereSample3(const int& );
  ggPoissonSphereSample3(const int& , const double& );
  ~ggPoissonSphereSample3(){
    if (dataIndex != 0) delete [] dataIndex;
  }

  void SetN(const int&, const double&);
  void SetSeparation(const double&);
  virtual int  Generate();

protected:
  inline ggBoolean
  isProximal(const ggPoint3 & p1,
			     const ggPoint3 & p2,
			     const double & tol)
  {return((
	  (p1.x()-p2.x())*(p1.x()-p2.x()) +
	  (p1.y()-p2.y())*(p1.y()-p2.y()) +
	  (p1.z()-p2.z())*(p1.z()-p2.z()) 
	   ) < tol ? ggTrue: ggFalse);}
  double  sqMinDist;
};
  
#endif
