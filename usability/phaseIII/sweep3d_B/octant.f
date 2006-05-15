      subroutine octant (i0,i1,i2,i3,j0,j1,j2,j3,k0,k1,k2,k3,iq,
     & it,jt,kt)
c
c Set sweep loop limits for each octant
c
      implicit none

      integer i0,i1,i2,i3,j0,j1,j2,j3,k0,k1,k2,k3
      integer it,jt,kt,iq

c
c octant order must be such that minus(-) directions are before
c plus(+) directions so that reflective boundary conditions can
c be met
c
         if (iq .eq. 1) then
            i0 = it
            i1 = 1
            i2 = -1
            j0 = jt
            j1 = 1
            j2 = -1
            k0 = kt
            k1 = 1
            k2 = -1
         else if (iq .eq. 2) then
            i0 = it
            i1 = 1
            i2 = -1
            j0 = jt
            j1 = 1
            j2 = -1
            k0 = 1
            k1 = kt
            k2 = +1
         else if (iq .eq. 3) then
            i0 = it
            i1 = 1
            i2 = -1
            j0 = 1
            j1 = jt
            j2 = +1
            k0 = kt
            k1 = 1
            k2 = -1
         else if (iq .eq. 4) then
            i0 = it
            i1 = 1
            i2 = -1
            j0 = 1
            j1 = jt
            j2 = +1
            k0 = 1
            k1 = kt
            k2 = +1
         else if (iq .eq. 5) then
            i0 = 1
            i1 = it
            i2 = +1
            j0 = jt
            j1 = 1
            j2 = -1
            k0 = kt
            k1 = 1
            k2 = -1
         else if (iq .eq. 6) then
            i0 = 1
            i1 = it
            i2 = +1
            j0 = jt
            j1 = 1
            j2 = -1
            k0 = 1
            k1 = kt
            k2 = +1
         else if (iq .eq. 7) then
            i0 = 1
            i1 = it
            i2 = +1
            j0 = 1
            j1 = jt
            j2 = +1
            k0 = kt
            k1 = 1
            k2 = -1
         else
            i0 = 1
            i1 = it
            i2 = +1
            j0 = 1
            j1 = jt
            j2 = +1
            k0 = 1
            k1 = kt
            k2 = +1
         end if

         if (i2.gt.0) then
           i3 = 1
         else
           i3 = 0
         endif
         if (j2.gt.0) then
           j3 = 1
         else
           j3 = 0
         endif
         if (k2.gt.0) then
           k3 = 1
         else
           k3 = 0
         endif

         return
         end

