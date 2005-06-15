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
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <math.h>
#include <mrCookPixelRenderer.h>


//*************************************************************************************
//*************************************************************************************
// i and j are pixel numbers.  n^2 samples will be taken and returned
// as elements of trian
ggBoolean mrCookPixelRenderer::samplePixel(int i, int j, int n,
                                         ggTrain<ggSpectrum>& train) {
     ggRay3 r;
     double time;
     int samples = n*n;

     // four 2D and one 1D sample patterns with n^2 samples each
     // all are in the unit n-D square ([0,1]^2 or [0,1]
     shadowSamp.SetNSamples(n,n);
     reflectSamp.SetNSamples(n,n);
     lensSamp.SetNSamples(n,n);
     pixelSamp.SetNSamples(n,n);
     timeSamp.SetNSamples(samples);

     // the pixel coordinate system is [0,nx] x [0,ny] so points close to the
     // center of pixel (i,j) than any other pixel are in [i,i+1]x[j,j+1].
     // offset is the distance from the lower lefthand corner of the pixel.
     ggVector2 offset((double)i, (double)j);

     for (int s = 0; s < samples; s++) {
          time = time1 + timeSamp[s] * (time2 - time1);
          r = cameraPtr->getRay(pixelSamp[s] + offset, time, lensSamp[s]);
          r.direction() = ggUnitVector(r.direction());

         // send in a ray and sample points for shadow/reflection rays
         // the ggFalse is CORL-- "count only reflected light".  If we hit
         // a light we want to see it, so it is false here.  If we send
         // a reflection ray later we we set this to true iff a shadow ray is also sent.
         // depth is zero
          train.Append(cookRadiance(r, time, shadowSamp[s], reflectSamp[s], ggFalse, 0)); 
   }
   return ggTrue;

}

//*************************************************************************************
//*************************************************************************************
// what is the radiance seen at point r.origin() looking in
// direction r.direction().  This implements the 1984 distribution
// ray tracing article by Cook et al. in Siggraph 84
ggSpectrum mrCookPixelRenderer::cookRadiance(const ggRay3& r,
                                double time,
                                const ggPoint2& uvShadow,
                                const ggPoint2& uvReflect,
                                ggBoolean CORL,
                                int depth) const {

     // mrViewingHitRecord defined in "mrSurface.h"-- contains surface hit info
      mrViewingHitRecord VHR;

      // ggMaterialRecord defined in "ggMaterial.h"
      ggMaterialRecord MR;

      MR.UV = uvReflect;

      // if there is a hit at time "time" with ray parameter in [ggEpsilon,ggInfinity]...
      if (scenePtr->objects()->viewingHit(r, time, ggEpsilon, ggInfinity, VHR, MR)) {

         ggSpectrum s((float)0.0);

         // if the object is at least semi-diffuse and has a local coord. system...
         if (MR.BRDFPointer && VHR.hasUVW) {
             // in case ambient is not zero-- ambient is taken to
             // mean the constant indirect field radiance.
             s += ggPi * scenePtr->ambient(VHR.p, VHR.UVW.w(), time) * MR.kBRDF  *
                              MR.BRDFPointer->averageValue();

             // MR.kBRDF is an attenuation coefficient for things like texture, etc...
             s += MR.kBRDF * directLight(r, time, VHR.p, VHR.UVW, uvShadow, MR.BRDFPointer);
         }

         // there is at least one reflected ray
         if (MR.hasRay1 && depth < maxDepth)
          if (!MR.CORLRay1)
             s += MR.kRay1 * cookRadiance(MR.ray1, time, uvShadow,
                                           MR.UV, MR.CORLRay1, depth+1);

         // there is a reflected ray in the second slot (e.g. transmission ray)
         if (MR.hasRay2 && depth < maxDepth)
          if (!MR.CORLRay2)
             s += MR.kRay2 * cookRadiance(MR.ray2, time, uvShadow,
                                           MR.UV, MR.CORLRay2, depth+1);

         // if some light is emitted and it was not counted in a previous
         // direct lighting calculation, then count it.
         if (VHR.hasEmit && !CORL)
             s += VHR.kEmit;

          return s;
      }
      else
         return  scenePtr->background(r, time);

}


// calculates direct lighting via Monte Carlo integration.  Here
// we have an integral of f(y) where y is on the luminaire
// surface.  According to MC integration our estimate is f(x)/p(x)
// where p(x) is the probability density function that x is
// associated with.  For information on exactly what f is, see
// the external documentation.
ggSpectrum mrCookPixelRenderer::directLight(const ggRay3& r, double time,
                                           const ggPoint3& x,
                                           const ggONB3& uvw,
                                           const ggPoint2& uv,
                                           ggBRDF *brdfPtr) const {

     
    // if there are no lights return 0
    if (!scenePtr->luminaires()) return ggSpectrum((float)0.0);

    mrSurface* luminairesPtr = scenePtr->luminaires();
    mrSurface* surfacesPtr = scenePtr->objects();

    ggVector3 N = uvw.w();  // we don't care about the u,v tangent vectors-- normal vector = w.
    ggPoint3 xPrime;        // point on light
    double invProb;         // 1 / p(xPrime)
    double cosT;
    double t;

    if (ggDot(N, r.direction()) >= 0.0) N = -N;  // we need an "outward" normal

    if(luminairesPtr->selectVisiblePoint(x, N, uv, time, xPrime, invProb)) {
            ggRay3 toLight(x, xPrime - x);
            ggBoolean emits;
            ggVector3 NPrime;
            ggSpectrum kEmit;

            // since the length of the shadow ray direction vector is the distance
            // to the source (it is xPrime - x), it will hit arount t = 1.0 in the
            // ray parameter space if there is no shadowing
             if ( (cosT = ggDot(N, toLight.direction())) > 0.0 &&
                  surfacesPtr->shadowHit(toLight, time, ggBigEpsilon,
                            1.0 + ggBigEpsilon, t, NPrime, emits, kEmit) ) {
                 if ( t < (1.0 - ggBigEpsilon) || !emits)  // block or light not on at xPrime
                     return ggSpectrum((float)0.0);
                 else {   // evaluate big ugly direct lighting calculation
                    double invDistance2 = 1.0/
                          ggDot(toLight.direction(), toLight.direction());
                    double idist = sqrt(invDistance2);
                    cosT *= idist;
                    double cosTprime = -ggDot(toLight.direction(), NPrime);

                    if (cosTprime < ggEpsilon)
                            return ggSpectrum((float)0.0);
                    else
                          cosTprime *= idist;

                    ggSpectrum R = brdfPtr->value(-toLight.direction(),
                                                    -r.direction(), uvw);
                    return  (invDistance2 * cosT * cosTprime * invProb) * R * kEmit;
                }
            }
            else
                return ggSpectrum((float)0.0);
    }
    else
        return ggSpectrum((float)0.0);
}

