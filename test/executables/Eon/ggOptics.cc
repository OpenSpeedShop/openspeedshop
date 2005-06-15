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
#include <ggOptics.h>
#include <ggMacros.h>
#include <assert.h>






ggVector3 ggReflection(const ggVector3& v,
                       const ggVector3& n)
{
    double l = 2.0*ggDot(n, v);
    return ggVector3(v.x() - l * n.x(),
                     v.y() - l * n.y(),
                     v.z() - l * n.z());
}

//  Note-- both v and n must be unit vectors!
ggBoolean ggVacuumDielectricRefract(const ggVector3& n, double Nt,
                                    ggVector3& v) {
    double k;
    ggVector3 s;

    #ifdef GGSAFE
	assert(ggEqual(ggDot(v,v),1.0,1E-10));
        assert(ggEqual(ggDot(n,n),1.0,1E-10));
	assert(Nt >= 1.0);
    #endif 

    k = ggDot(v, n);

    s = (1.0/Nt) * (v - k*n);

    k=1.0 - ggDot(s,s);

    if (k <  ggEpsilon)
        return ggFalse; // no refraction possible
    else {
       v = s - sqrt(k)*n;
       return ggTrue; 
    }

}


ggBoolean ggDielectricVacuumRefract(const ggVector3& n, double Ni,
                                    ggVector3& v) {
    double k;
    ggVector3 s;

    #ifdef GGSAFE
	assert(ggEqual(ggDot(v,v),1.0,1E-10));
        assert(ggEqual(ggDot(n,n),1.0,1E-10));
        assert(Ni >= 1.0);
    #endif 

    k = ggDot(v, n);
    s = Ni * (v - k*n);
    k=1.0 - ggDot(s,s);

    if (k <  ggEpsilon)
        return ggFalse; // no refraction possible
    else
    {
       v = s - sqrt(k)*n;
       return ggTrue; 
    }

}


//  Note-- both v and n must be unit vectors!
ggBoolean ggRefract(const ggVector3& n, double Ni, double Nt,
                    ggVector3& v)
{
    double  k;
    ggVector3 s;

    #ifdef GGSAFE
	assert(ggEqual(ggDot(v,v),1.0,1E-10));
        assert(ggEqual(ggDot(n,n),1.0,1E-10));
        assert(Ni >= 1.0);
	assert(Nt >= 1.0);
    #endif 

    k = ggDot(v, n);

    s = (Ni/Nt) * (v - k*n);

    k=1.0 - ggDot(s,s);

    if (k <  ggEpsilon)
    {
        return ggFalse; // no refraction possible
    }
    else
    {
       v = s - sqrt(k)*n;
       return ggTrue; 
    }

}



double ggMaterialReflectance(const double Re)
{
  double r;

  if (Re > 1.0)
  {
    #ifdef GGSAFE
       cerr << "Reflectivity  should be <= 1.0 \n";
    #endif
    r=1.0;
  }
  else r=Re;
  return r;

}



double ggVacuumConductorReflectance(const ggVector3& v,
                              const ggVector3& n,
                              double Nt, double kt) {
   double Rs, Rp;
   double a, a2, b, b2;
   double cos, cos2, sin, sin2, tan, tan2;
   double Nt2,  kt2;
   double Ns,Nk,Sq,ab2,astan,stan,dcos,dNi;


    #ifdef GGSAFE
	assert(ggEqual(ggDot(v,v),1.0,1E-10));
        assert(ggEqual(ggDot(n,n),1.0,1E-10));
	assert(Nt >= 1.0);
    #endif 


   cos = fabs(ggDot(v, n));
   if (cos < ggEpsilon) cos = ggEpsilon;
   cos2 = cos*cos;
   sin2 = 1 - cos2;
   if (sin2 < 0.0) sin2 = 0.0;
   sin = sqrt(sin2);
   tan = sin / cos;
   tan2 = tan*tan;

	
   Nt2 = Nt*Nt;
   kt2 = kt*kt;
   Ns  = sin2;
   Nk  = 4*Nt2*kt2;
   a2 = Nt2 - kt2 - Ns;
   Sq = sqrt (a2*a2 + Nk);
   b2 = Sq - Nt2 + kt2 + Ns;
   a2 = Sq + Nt2 - kt2 - Ns;
   dNi = (0.5);
   a2 *= dNi;
   b2 *= dNi;
   if (a2 < 0.0) a2 = 0.0;
   if (b2 < 0.0) b2 = 0.0;
   a = sqrt(a2);
   b = sqrt(b2);
   ab2 = a2 +b2;
   dcos = 2*a*cos;
   astan = 2*a*sin*tan;
   stan =  sin2*tan2;
   Rs = (ab2 - dcos  + cos2) / (ab2 + dcos + cos2);
   Rp = Rs * (ab2 - astan + stan) / (ab2 + astan + stan);
   return 0.5*(Rp + Rs);
}


double ggConductorReflectance(const ggVector3& v,
                            const ggVector3& n,
                            double Ni, double Nt, double kt)
{
   double Rs, Rp;
   double a, a2, b, b2;
   double cos, cos2, sin, sin2, tan, tan2;
   double Nt2, Ni2, kt2;
   double Ns,Nk,Sq,ab2,astan,stan,dcos,dNi;


    #ifdef GGSAFE
	assert(ggEqual(ggDot(v,v),1.0,1E-10));
        assert(ggEqual(ggDot(n,n),1.0,1E-10));
        assert(Ni >= 1.0);
	assert(Nt >= 1.0);
    #endif 


   cos = fabs(ggDot(v, n));
   if (cos < ggEpsilon) cos = ggEpsilon;
   cos2 = cos*cos;
   sin2 = 1 - cos2;
   if (sin2 < 0.0) sin2 = 0.0;
   sin = sqrt(sin2);
   tan = sin / cos;
   tan2 = tan*tan;

   if (Ni == 1.0) Ni2 = 1.0;
   else Ni2 = Ni*Ni;
	
   Nt2 = Nt*Nt;
   kt2 = kt*kt;
   Ns  = Ni2*sin2;
   Nk  = 4*Nt2*kt2;
   a2 = Nt2 - kt2 - Ns;
   Sq = sqrt (a2*a2 + Nk);
   b2 = Sq - Nt2 + kt2 + Ns;
   a2 = Sq + Nt2 - kt2 - Ns;
   dNi = (0.5 / Ni2);
   a2 *= dNi;
   b2 *= dNi;
   if (a2 < 0.0) a2 = 0.0;
   if (b2 < 0.0) b2 = 0.0;
   a = sqrt(a2);
   b = sqrt(b2);
   ab2 = a2 +b2;
   dcos = 2*a*cos;
   astan = 2*a*sin*tan;
   stan =  sin2*tan2;
   Rs = (ab2 - dcos  + cos2) / (ab2 + dcos + cos2);
   Rp = Rs * (ab2 - astan + stan) / (ab2 + astan + stan);
   return 0.5*(Rp + Rs);
}


double ggVacuumDielectricReflectance(const ggVector3& v,
                               const ggVector3& tv, 
                               const ggVector3& n,
                               double Nt) {
    double nn, cc, Nt2, ci2, ct2, cosAi, cosAt;
    double answer, num, denom,Nit,cit;

    #ifdef GGSAFE
	assert(ggEqual(ggDot(v,v),1.0,1E-10));
        assert(ggEqual(ggDot(n,n),1.0,1E-10));
        assert(ggEqual(ggDot(tv,tv),1.0,1E-10));
	assert(Nt >= 1.0);
    #endif 


    cosAi = fabs(ggDot(v, n));
    cosAt = fabs(ggDot(tv, n));
    nn = Nt;
    cc = cosAt * cosAi;
    Nt2 = Nt * Nt;
    ci2 = cosAi * cosAi;
    ct2 = cosAt * cosAt;
    Nit = 1.0 - Nt2;
    cit = ci2 - ct2;
    num = Nit*Nit*cc*cc +  cit*cit*nn*nn;
    denom = cc*(1.0 + Nt2) + nn*(ci2 + ct2);
    denom = denom*denom;
    if ((denom < 0.000001) && (num < 0.000001))
        answer = 1.0;
    else
    answer = num/denom;

    if (answer < 1.0)
         return answer;
    else{
         #ifdef GGSAFE
           cerr << "Reflectivity  should be <= 1.0 \n";
         #endif
         return 1.00;
    }
}



double ggDielectricVacuumReflectance(const ggVector3& v,
                               const ggVector3& tv, 
                               const ggVector3& n,
                               double Ni) {
    double nn, cc, Ni2, ci2, ct2, cosAi, cosAt;
    double answer, num, denom,Nit,cit;

    #ifdef GGSAFE
	assert(ggEqual(ggDot(v,v),1.0,1E-10));
        assert(ggEqual(ggDot(n,n),1.0,1E-10));
        assert(ggEqual(ggDot(tv,tv),1.0,1E-10));
        assert(Ni >= 1.0);
    #endif 


    cosAi = fabs(ggDot(v, n));
    cosAt = fabs(ggDot(tv, n));
    nn = Ni ;
    cc = cosAt * cosAi;
    Ni2 = Ni * Ni;
    ci2 = cosAi * cosAi;
    ct2 = cosAt * cosAt;
    Nit = Ni2 - 1.0;
    cit = ci2 - ct2;
    num = Nit*Nit*cc*cc +  cit*cit*nn*nn;
    denom = cc*(Ni2 + 1.0) + nn*(ci2 + ct2);
    denom = denom*denom;
    if ((denom < 0.000001) && (num < 0.000001))
        answer = 1.0;
    else
    answer = num/denom;

    if (answer < 1.0)
         return answer;
    else{
         #ifdef GGSAFE
           cerr << "Reflectivity  should be <= 1.0 \n";
         #endif
         return 1.00;
    }
}

double ggDielectricReflectance(const ggVector3& v,
                               const ggVector3& tv, 
                               const ggVector3& n,
                                double Ni, double Nt)
{
    double nn, cc, Ni2, Nt2, ci2, ct2, cosAi, cosAt;
    double answer, num, denom,Nit,cit;

    #ifdef GGSAFE
	assert(ggEqual(ggDot(v,v),1.0,1E-10));
        assert(ggEqual(ggDot(n,n),1.0,1E-10));
        assert(ggEqual(ggDot(tv,tv),1.0,1E-10));
        assert(Ni >= 1.0);
	assert(Nt >= 1.0);
    #endif 


    cosAi = fabs(ggDot(v, n));
    cosAt = fabs(ggDot(tv, n));
    nn = Ni * Nt;
    cc = cosAt * cosAi;
    Ni2 = Ni * Ni;
    Nt2 = Nt * Nt;
    ci2 = cosAi * cosAi;
    ct2 = cosAt * cosAt;
    Nit = Ni2 - Nt2;
    cit = ci2 - ct2;
    num = Nit*Nit*cc*cc +  cit*cit*nn*nn;
    denom = cc*(Ni2 + Nt2) + nn*(ci2 + ct2);
    denom = denom*denom;
    if ((denom < 0.000001) && (num < 0.000001))
        answer = 1.0;
    else
    answer = num/denom;

    if (answer < 1.0)
         return answer;
    else{
         #ifdef GGSAFE
           cerr << "Reflectivity  should be <= 1.0 \n";
         #endif
         return 1.00;
    }
}
