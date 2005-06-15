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



#include <assert.h>
#include <ggMacros.h>
#include <mrSurfaceList.h>
#include <kai.h>

mrSurfaceList::mrSurfaceList() {
}


ggBoolean mrSurfaceList::print(ostream& s) const {
    mrSurface* obj_ptr;
    int i;

    s << "mrSurfaceList " << PTR_TO_INTEGRAL(this) << " of length " << length() << "\n";
    for (i = 0; i < length(); i++) {
       obj_ptr = (*this)[i];
       s <<  "element " << i++ << " of mrSurfaceList " << PTR_TO_INTEGRAL(this) << " ";
       obj_ptr->print(s);
    }
    return ggTrue;
}


ggBoolean mrSurfaceList::shadowHit(
             const ggRay3& r,    // ray being sent
             double time,     // time ray is sent
             double tmin,
             double tmax,
             double& t,
             ggVector3& N,
             ggBoolean& emits,
             ggSpectrum& E
             ) const
{
     double a_t;
     double a_tmax = tmax;
     ggVector3 a_N;
     ggBoolean a_emits;
     ggSpectrum a_E;
     ggBox3 rayBox, objectBox;
    
     ggPoint3 p0 = r.pointAtParameter(tmin);
     ggPoint3 p1 = r.pointAtParameter(tmax);

     if (r.direction().x() > 0) {
        rayBox.min().x() = r.origin().x() + tmin * r.direction().x();
        rayBox.max().x() = r.origin().x() + tmax * r.direction().x();
     }
     else {
        rayBox.min().x() = r.origin().x() + tmax * r.direction().x();
        rayBox.max().x() = r.origin().x() + tmin * r.direction().x();
     }
 
     if (r.direction().y() > 0) {
        rayBox.min().y() = r.origin().y() + tmin * r.direction().y();
        rayBox.max().y() = r.origin().y() + tmax * r.direction().y();
     }
     else {
        rayBox.min().y() = r.origin().y() + tmax * r.direction().y();
        rayBox.max().y() = r.origin().y() + tmin * r.direction().y();
     }
 
     if (r.direction().z() > 0) {
        rayBox.min().z() = r.origin().z() + tmin * r.direction().z();
        rayBox.max().z() = r.origin().z() + tmax * r.direction().z();
     }
     else {
        rayBox.min().z() = r.origin().z() + tmax * r.direction().z();
        rayBox.max().z() = r.origin().z() + tmin * r.direction().z();
     }
 

     ggBoolean hit_one;

     hit_one = ggFalse;
     t = ggInfinity;
     
     for (int i = 0; i < length(); i++) {
        mrSurface *sPtr = surfaces[i]; 
        //if (  ((!sPtr->boundingBox(time, time, objectBox)) ||
         //             ggOverlapBox3(rayBox,objectBox)
         //     ) && sPtr->shadowHit(
        if ( sPtr->shadowHit(
                                r,
                                time,
                                tmin,
                                a_tmax,
                                a_t,
                                a_N,
                                a_emits,
                                a_E)
           )

           if (a_t < t) {
              hit_one = ggTrue;
              t = a_t;
              a_tmax = t;
              N = a_N;
              emits = a_emits;
              E = a_E;
           }
     }
     return (hit_one);
}

ggBoolean mrSurfaceList::viewingHit(        
             const ggRay3& r,    // ray being sent
             double time,     // time ray is sent
             double tmin,
             double tmax,
             mrViewingHitRecord& VHR,
             ggMaterialRecord& MR
             ) const
{
     double a_tmax = tmax;

     ggBox3 rayBox, objectBox;
   
     ggPoint3 p0 = r.pointAtParameter(tmin);
     ggPoint3 p1 = r.pointAtParameter(tmax);

	if (p0.x() > p1.x()) {
		rayBox.max().x() = p0.x();
		rayBox.min().x() = p1.x();
	} else {
		rayBox.max().x() = p1.x();
		rayBox.min().x() = p0.x();
	}

	if (p0.y() > p1.y()) {
		rayBox.max().y() = p0.y();
		rayBox.min().y() = p1.y();
	} else {
		rayBox.max().y() = p1.y();
		rayBox.min().y() = p0.y();
	}

	if (p0.z() > p1.z()) {
		rayBox.max().z() = p0.z();
		rayBox.min().z() = p1.z();
	} else {
		rayBox.max().z() = p1.z();
		rayBox.min().z() = p0.z();
	}

	 /*
     rayBox.min().x() = ggMin(p0.x(), p1.x());
     rayBox.min().y() = ggMin(p0.y(), p1.y());
     rayBox.min().z() = ggMin(p0.z(), p1.z());

     rayBox.max().x() = ggMax(p0.x(), p1.x());
     rayBox.max().y() = ggMax(p0.y(), p1.y());
     rayBox.max().z() = ggMax(p0.z(), p1.z());
	 */


     ggBoolean hit_one;

     hit_one = ggFalse;
     VHR.t = ggInfinity;

     ggPoint2 uvTemp = MR.UV;
     
     for (int i = 0; i < length(); i++) {
        mrSurface *sPtr = surfaces[i]; 
        ggMaterialRecord a_MR;
        mrViewingHitRecord a_VHR;
        a_MR.UV = uvTemp;

        if (  ((!sPtr->boundingBox(time, time, objectBox)) ||
                ggOverlapBox3(rayBox,objectBox)
              ) && sPtr->viewingHit( r, time, tmin, a_tmax, a_VHR, a_MR)
           )


           if (a_VHR.t < VHR.t) {
              hit_one = ggTrue;
              VHR = a_VHR;
              a_tmax = VHR.t;
              MR = a_MR;
           }
     }
     return (hit_one);
}


//  returns true if bounding box is found
ggBoolean mrSurfaceList::boundingBox(
             double time1, // low end of time range
             double time2, // low end of time range
             ggBox3& box)   // highest xyz values
             const
{
    mrSurface* obj_ptr;
    ggPoint3 obj_min, obj_max, min, max;
    double big = 1.0e10;
    ggBox3 obox;

    min.Set(big, big, big);
    max.Set(-big, -big, -big);
    for (int i = 0; i < length(); i++) {
       obj_ptr = surfaces[i];
       if (obj_ptr->boundingBox(time1, time2, obox))
       {
         obj_min = obox.min();
         obj_max = obox.max();
         if (obj_min.x() < min.x()) min.x() = (obj_min.x()) ;
         if (obj_min.y() < min.y()) min.y() =( obj_min.y()) ;
         if (obj_min.z() < min.z()) min.z() =(obj_min.z()) ;

         if (obj_max.x() > max.x()) max.x() = (obj_max.x()) ;
         if (obj_max.y() > max.y()) max.y() = (obj_max.y()) ;
         if (obj_max.z() > max.z()) max.z() =(obj_max.z()) ;
       }

    }
    box.SetMin(min);
    box.SetMax(max);
    return ggTrue;

}


ggBoolean mrSurfaceList::selectVisiblePoint(
             const ggPoint3& x,  // viewpoint
             const ggVector3& Nx,  // normal at x
             const ggPoint2& uv,  // input coordinate for warping
             const double time,
             ggPoint3& onLight,  // point corresponding to uv
             double& prob ) const  // probability if uv is random in [0,1]^2
{
     int i;
     int N = length();
     ggSpectrum L;
     if (N < 1) return ggFalse;
     double *alpha = new double[N];
     double sum = 0.0;
     for (i = 0; i < N; i++)
     {
          if (surfaces[i]->approximateDirectRadiance(x, Nx, time, L))
          {
              alpha[i] = L[3];
              sum += alpha[i];
          }
          else
              alpha[i] = 0.0;
     }
     
     double randLight = uv.u();
     double scale = 1.0 / sum;
     double sofar = scale* alpha[0];
     i = 0;
     while (randLight > sofar && i < N-1) {
         i++;
         sofar += alpha[i]*scale;
     }
     double p = alpha[i] * scale;

     mrSurface *lPtr = surfaces[i];
     randLight = randLight - sofar + alpha[i]*scale;
     randLight = randLight / (alpha[i] * scale);
     ggPoint2 uvtemp(randLight, uv.v());

     delete [] alpha;

     if (lPtr->selectVisiblePoint(x, Nx, uvtemp, time, onLight, prob)) {
         prob /= p;
         return ggTrue;
     }
     else
          return ggFalse;

}


