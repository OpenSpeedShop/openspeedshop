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
// ggSphere.C-- functions for class ggSphere
//
// Author:    Jai-hoon Kim
//            Nov. 1, 1993
//
//            Peter Shirley	
//
#include <ggSphere.h>



ggBoolean ggSphereBoxOverlap
    (const ggSphere& sphere, const ggBox3& b) {
    ggPoint3 c = sphere.center();
    double r = sphere.radius();

    if ((c.x() < b.min().x() - r || c.x() > b.max().x() + r)
      || (c.y() < b.min().y() - r || c.y() > b.max().y() + r)
      || (c.z() < b.min().z() - r || c.z() > b.max().z() + r))
	return ggFalse;

    if (c.x() < b.min().x()) {
        if (c.y() < b.min().y()) {
            if (c.z() < b.min().z()) {
		if ((b.min().x()-c.x())*(b.min().x()-c.x())
		  + (b.min().y()-c.y())*(b.min().y()-c.y())
		  + (b.min().z()-c.z())*(b.min().z()-c.z())
		  <= r*r )
		    return ggTrue;
		else return ggFalse;
	    }
            else if ( c.z() > b.max().z()) {
		if ((b.min().x()-c.x())*(b.min().x()-c.x())
		  + (b.min().y()-c.y())*(b.min().y()-c.y())
		  + (b.max().z()-c.z())*(b.max().z()-c.z())
		  <= r*r )
		    return ggTrue;
		else return ggFalse;
	    }
            else {
		if ((b.min().x()-c.x())*(b.min().x()-c.x())
		  + (b.min().y()-c.y())*(b.min().y()-c.y())
		  <= r*r )
		    return ggTrue;
		else return ggFalse;
	    }
	}
        else if (c.y() > b.max().y()) {
            if (c.z() < b.min().z()) {
		if ((b.min().x()-c.x())*(b.min().x()-c.x())
		  + (b.max().y()-c.y())*(b.max().y()-c.y())
		  + (b.min().z()-c.z())*(b.min().z()-c.z())
		  <= r*r )
		    return ggTrue;
		else return ggFalse;
	    }
            else if ( c.z() > b.max().z()) {
		if ((b.min().x()-c.x())*(b.min().x()-c.x())
		  + (b.max().y()-c.y())*(b.max().y()-c.y())
		  + (b.max().z()-c.z())*(b.max().z()-c.z())
		  <= r*r )
		    return ggTrue;
		else return ggFalse;
	    }
            else {
		if ((b.min().x()-c.x())*(b.min().x()-c.x())
		  + (b.max().y()-c.y())*(b.max().y()-c.y())
		  <= r*r )
		    return ggTrue;
		else return ggFalse;
	    }
	}
        else {
            if (c.z() < b.min().z()) {
		if ((b.min().x()-c.x())*(b.min().x()-c.x())
		  + (b.min().z()-c.z())*(b.min().z()-c.z())
		  <= r*r )
		    return ggTrue;
		else return ggFalse;
	    }
            else if ( c.z() > b.max().z()) {
		if ((b.min().x()-c.x())*(b.min().x()-c.x())
		  + (b.max().z()-c.z())*(b.max().z()-c.z())
		  <= r*r )
		    return ggTrue;
		else return ggFalse;
	    }
            else
		return ggTrue;
	}
    }
    else if (c.x() > b.max().x()) {
        if (c.y() < b.min().y()) {
            if (c.z() < b.min().z()) {
		if ((b.max().x()-c.x())*(b.max().x()-c.x())
		  + (b.min().y()-c.y())*(b.min().y()-c.y())
		  + (b.min().z()-c.z())*(b.min().z()-c.z())
		  <= r*r )
		    return ggTrue;
		else return ggFalse;
	    }
            else if (c.z() > b.max().z()) {
		if ((b.max().x()-c.x())*(b.max().x()-c.x())
		  + (b.min().y()-c.y())*(b.min().y()-c.y())
		  + (b.max().z()-c.z())*(b.max().z()-c.z())
		  <= r*r )
		    return ggTrue;
		else return ggFalse;
	    }
            else {
		if ((b.max().x()-c.x())*(b.max().x()-c.x())
		  + (b.min().y()-c.y())*(b.min().y()-c.y())
		  <= r*r )
		    return ggTrue;
		else return ggFalse;
	    }
	}
        else if (c.y() > b.max().y()) {
            if (c.z() < b.min().z()) {
		if ((b.max().x()-c.x())*(b.max().x()-c.x())
		  + (b.max().y()-c.y())*(b.max().y()-c.y())
		  + (b.min().z()-c.z())*(b.min().z()-c.z())
		  <= r*r )
		    return ggTrue;
		else return ggFalse;
	    }
            else if ( c.z() > b.max().z()) {
		if ((b.max().x()-c.x())*(b.max().x()-c.x())
		  + (b.max().y()-c.y())*(b.max().y()-c.y())
		  + (b.max().z()-c.z())*(b.max().z()-c.z())
		  <= r*r )
		    return ggTrue;
		else return ggFalse;
	    }
            else {
		if ((b.max().x()-c.x())*(b.max().x()-c.x())
		  + (b.max().y()-c.y())*(b.max().y()-c.y())
		  <= r*r )
		    return ggTrue;
		else return ggFalse;
	   }
	}
        else {
            if (c.z() < b.min().z()) {
		if ((b.max().x()-c.x())*(b.max().x()-c.x())
		  + (b.min().z()-c.z())*(b.min().z()-c.z())
		  <= r*r )
		    return ggTrue;
		else return ggFalse;
	    }
            else if (c.z() > b.max().z()) {
		if ((b.max().x()-c.x())*(b.max().x()-c.x())
		  + (b.max().z()-c.z())*(b.max().z()-c.z())
		  <= r*r )
		    return ggTrue;
		else return ggFalse;
	    }
            else
		return ggTrue;
	}
    }
    else {
        if (c.y() < b.min().y()) {
            if (c.z() < b.min().z()) {
		if ((b.min().y()-c.y())*(b.min().y()-c.y())
		  + (b.min().z()-c.z())*(b.min().z()-c.z())
		  <= r*r )
		    return ggTrue;
		else return ggFalse;
	    }
            else if ( c.z() > b.max().z()) {
		if ((b.min().y()-c.y())*(b.min().y()-c.y())
		  + (b.max().z()-c.z())*(b.max().z()-c.z())
		  <= r*r )
		    return ggTrue;
		else return ggFalse;
	    }
            else {
		  if ((b.min().y()-c.y())*(b.min().y()-c.y())
		  <= r*r )
		    return ggTrue;
		else return ggFalse;
	    }
	}
        else if (c.y() > b.max().y()) {
            if (c.z() < b.min().z()) {
		if ((b.max().y()-c.y())*(b.max().y()-c.y())
		  + (b.min().z()-c.z())*(b.min().z()-c.z())
		  <= r*r )
		    return ggTrue;
		else return ggFalse;
	    }
            else if ( c.z() > b.max().z()) {
		if ((b.max().y()-c.y())*(b.max().y()-c.y())
		  + (b.max().z()-c.z())*(b.max().z()-c.z())
		  <= r*r )
		    return ggTrue;
		else return ggFalse;
	    }
            else 
	        return ggTrue;
	}
        else
	    return ggTrue;
    }
}

ggPoint2
ggSphere::getUV(const ggPoint3& p) const {

    double cosPhi, sinPhi, cosTheta, sinTheta, theta, phi;

    ggVector3 pc = p - c;
    cosTheta = 0.9999999999*pc.z() / r;
    theta = acos(cosTheta);
    sinTheta = sin(theta);
    cosPhi = pc.x() / (1.0000000001*r * sinTheta);
    phi = acos(cosPhi);
    if (pc.y() < 0.0) phi = 2*ggPi - phi;
    sinPhi = sin(phi);

    return( ggPoint2 ((0.5*ggInversePi) * phi, 1.0 - theta*ggInversePi));

}
