
      subroutine inner_auto (it, jt, kt, nm, isct, mm,mmo,mmi, mk,
     & dx, dy, dz, epsi, err, iprint, do_dsa,
     & it_g, jt_g, istart,jstart,
     & myid, numtasks, north, south, east, west,
     & its,t0,t1,et0,et1,
     & it_dsa,jt_dsa,kt_dsa, ifixups,nfixups,
     & ibc,jbc,kbc,
     & jt_ibc,kt_ibc,mm_ibc,
     & it_jbc,kt_jbc,mm_jbc,
     & it_kbc,jt_kbc,mm_kbc)

      implicit none

      integer it,jt,kt,nm,isct,mm,mmo,mmi,mk
      double precision dx, dy, dz, epsi, err
      integer iprint
      logical do_dsa
      integer it_g,jt_g,istart,jstart
      integer myid, numtasks
      integer north, south, east, west
      integer its
      double precision t0,t1,et0,et1
      integer it_dsa,jt_dsa,kt_dsa
      integer ifixups,nfixups
      integer ibc,jbc,kbc
      integer jt_ibc,kt_ibc,mm_ibc
      integer it_jbc,kt_jbc,mm_jbc
      integer it_kbc,jt_kbc,mm_kbc

c...AUTOMATIC ARRAYS
      double precision  Sigt(it,jt,kt)
      double precision Pflux(it,jt,kt)
      double precision  Srcx(it,jt,kt)
      double precision  Sigs(it,jt,kt,isct+1)
      double precision  Flux(it,jt,kt,nm)
      double precision   Src(it,jt,kt,nm)
      double precision hi(it), hj(jt), hk(kt)
      double precision di(it), dj(jt), dk(kt)
      double precision w(mm), mu(mm), eta(mm), tsi(mm)
      double precision wmu(mm), weta(mm), wtsi(mm)
      double precision pn(mm,nm,8)
      double precision Phi(it)
      double precision Phii(0:it+1)
      double precision Phiib(jt,mk,mmi)
      double precision Phijb(it,mk,mmi)
      double precision Phikb(it,jt,mmi)
      integer mdiag(mmi)
      double precision Face(it_dsa,jt_dsa,kt_dsa,3)
      double precision Phiibc(jt_ibc,kt_ibc,mm_ibc,0:1,0:1)
      double precision Phijbc(it_jbc,kt_jbc,mm_jbc,0:1)
      double precision Phikbc(it_kbc,jt_kbc,mm_kbc)

      call inner (it, jt, kt, nm, isct, mm,mmo,mmi, mk,
     & dx, dy, dz, epsi, err, iprint,
     & it_g, jt_g, istart, jstart,
     & myid, numtasks, north, south, east, west,
     & its,t0,t1,et0,et1,
     & Sigt,Pflux,Srcx,Sigs,Flux,Src,
     & hi,hj,hk, di,dj,dk, w,mu,eta,tsi, wmu,weta,wtsi, pn,
     & Phi,Phii,Phiib,Phijb,Phikb,mdiag,
     & do_dsa,Face,it_dsa,jt_dsa,kt_dsa, ifixups,nfixups,
     & ibc,jbc,kbc,
     & Phiibc,jt_ibc,kt_ibc,mm_ibc,
     & Phijbc,it_jbc,kt_jbc,mm_jbc,
     & Phikbc,it_kbc,jt_kbc,mm_kbc)

      return
      end

