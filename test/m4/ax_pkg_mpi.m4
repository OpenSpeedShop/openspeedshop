#################################################################################
# Copyright (c) 2010-2012 Krell Institute. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 59 Temple
# Place, Suite 330, Boston, MA  02111-1307  USA
#################################################################################

################################################################################
# Check for LAM/MPI (http://www.lam-mpi.org)
################################################################################

AC_DEFUN([AX_LAM], [

    AC_ARG_WITH(lam,
		AC_HELP_STRING([--with-lam=DIR],
			       [LAM installation @<:@/usr/local@:>@]),
		lam_dir=$withval, lam_dir="/usr/local")

    AC_MSG_CHECKING([for LAM/MPI library and headers])

    found_lam=0

    LAM_CC="$lam_dir/bin/mpicc"
    LAM_CPPFLAGS="-I$lam_dir/include"
    LAM_LDFLAGS="-L$lam_dir/$abi_libdir"
    LAM_LIBS="-lmpi -llam -llamf77mpi -lutil"
    LAM_HEADER="$lam_dir/include/mpi.h"
    LAM_DIR="$lam_dir"
         
# LAM has a level of indirection.  The mpi collector
# searches the header file for MPI functions
    if test -f $lam_dir/include/mpi/mpi.h; then
      LAM_HEADER="$lam_dir/include/mpi/mpi.h"
    fi 

    lam_saved_CC=$CC
    lam_saved_CPPFLAGS=$CPPFLAGS
    lam_saved_LDFLAGS=$LDFLAGS
    lam_saved_LIBS=$LIBS

    CC="$LAM_CC"
    CPPFLAGS="$CPPFLAGS $LAM_CPPFLAGS"
    LDFLAGS="$LDFLAGS $LAM_LDFLAGS"
    LIBS="$LAM_LIBS"

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
	#include <mpi.h>
	]], [[
	MPI_Initialized((int*)0);
	]])],

	if objdump -T $lam_dir/$abi_libdir/libmpi.so \
		| cut -d' ' -f3 | grep "^lam_init" >/dev/null; then
	    found_lam=1
	fi
 
	, )

    CC=$lam_saved_CC
    CPPFLAGS=$lam_saved_CPPFLAGS
    LDFLAGS=$lam_saved_LDFLAGS
    LIBS=$lam_saved_LIBS

    if test $found_lam -eq 1; then
	AC_MSG_RESULT(yes)
	AM_CONDITIONAL(HAVE_LAM, true)
	AC_DEFINE(HAVE_LAM, 1, [Define to 1 if you have LAM.])	
    else
# Try again with $alt_abi_libdir instead
         found_lam=0

         LAM_CC="$lam_dir/bin/mpicc"
         LAM_CPPFLAGS="-I$lam_dir/include"
         LAM_LDFLAGS="-L$lam_dir/$alt_abi_libdir"
         LAM_LIBS="-lmpi"
         LAM_HEADER="$lam_dir/include/mpi.h"
         LAM_DIR="$lam_dir"
         
# If LAM has a level of indirection find the mpi.h file.  
# The mpi collector searches the header file for MPI functions
# when building the mpi collector.
         if test -f $lam_dir/include/mpi/mpi.h; then
             LAM_HEADER="$lam_dir/include/mpi/mpi.h"
         fi 

         lam_saved_CC=$CC
         lam_saved_CPPFLAGS=$CPPFLAGS
         lam_saved_LDFLAGS=$LDFLAGS

         CC="$LAM_CC"
         CPPFLAGS="$CPPFLAGS $LAM_CPPFLAGS"
         LDFLAGS="$LDFLAGS $LAM_LDFLAGS $LAM_LIBS"

         AC_LINK_IFELSE([AC_LANG_PROGRAM([[
             #include <mpi.h>
             ]], [[
             MPI_Initialized((int*)0);
             ]])],

             if objdump -T $lam_dir/$alt_abi_libdir/libmpi.so \
                | cut -d' ' -f3 | grep "^lam_init" >/dev/null; then
                 found_lam=1
             fi


             , )

         CC=$lam_saved_CC
         CPPFLAGS=$lam_saved_CPPFLAGS
         LDFLAGS=$lam_saved_LDFLAGS

         if test $found_lam -eq 1; then
             AC_MSG_RESULT(yes)
             AM_CONDITIONAL(HAVE_LAM, true)
             AC_DEFINE(HAVE_LAM, 1, [Define to 1 if you have LAM.])
         else
          AC_MSG_RESULT(no)
          AM_CONDITIONAL(HAVE_LAM, false)
          LAM_CC=""
          LAM_CPPFLAGS=""
          LAM_LDFLAGS=""
          LAM_LIBS=""
          LAM_HEADER=""
          LAM_DIR=""
        fi
    fi

    AC_SUBST(LAM_CC)
    AC_SUBST(LAM_CPPFLAGS)
    AC_SUBST(LAM_LDFLAGS)
    AC_SUBST(LAM_LIBS)
    AC_SUBST(LAM_HEADER)
    AC_SUBST(LAM_DIR)

])



################################################################################
# Check for LAMPI (http://public.lanl.gov/lampi)
################################################################################

AC_DEFUN([AX_LAMPI], [

    AC_ARG_WITH(lampi,
		AC_HELP_STRING([--with-lampi=DIR],
			       [LAMPI installation @<:@/usr/local@:>@]),
		lampi_dir=$withval, lampi_dir="/usr/local")

    AC_MSG_CHECKING([for LAMPI library and headers])

    found_lampi=0

    LAMPI_CC="$lampi_dir/bin/mpicc"
    LAMPI_CPPFLAGS="-I$lampi_dir/include"
    LAMPI_LDFLAGS="-L$lampi_dir/$abi_libdir"
    LAMPI_LIBS="-lmpi"
    LAMPI_HEADER="$lampi_dir/include/mpi.h"
    LAMPI_DIR="$lampi_dir"
         
# If LAMPI has a level of indirection find the mpi.h file.  
# The mpi collector searches the header file for MPI functions
    if test -f $lampi_dir/include/mpi/mpi.h; then
      LAMPI_HEADER="$lampi_dir/include/mpi/mpi.h"
    fi 

    lampi_saved_CC=$CC
    lampi_saved_CPPFLAGS=$CPPFLAGS
    lampi_saved_LDFLAGS=$LDFLAGS
    lampi_saved_LIBS=$LIBS

    CC="$LAMPI_CC"
    CPPFLAGS="$CPPFLAGS $LAMPI_CPPFLAGS"
    LDFLAGS="$LDFLAGS $LAMPI_LDFLAGS"
    LIBS="$LAMPI_LIBS"

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
	#include <mpi.h>
	]], [[
	MPI_Initialized((int*)0);
	]])],

	if objdump -T $lampi_dir/$abi_libdir/libmpi.so \
		| cut -d' ' -f3 | grep "^lampi_init" >/dev/null; then
	    found_lampi=1
	fi
 
	, )

    CC=$lampi_saved_CC
    CPPFLAGS=$lampi_saved_CPPFLAGS
    LDFLAGS=$lampi_saved_LDFLAGS
    LIBS=$lampi_saved_LIBS

    if test $found_lampi -eq 1; then
	AC_MSG_RESULT(yes)
	AM_CONDITIONAL(HAVE_LAMPI, true)
	AC_DEFINE(HAVE_LAMPI, 1, [Define to 1 if you have LAMPI.])	
    else
# Try again with $alt_abi_libdir instead
         found_lampi=0

         AC_MSG_CHECKING([for LAMPI library in alternative lib location and headers])
         LAMPI_CC="$lampi_dir/bin/mpicc"
         LAMPI_CPPFLAGS="-I$lampi_dir/include"
         LAMPI_LDFLAGS="-L$lampi_dir/$alt_abi_libdir"
         LAMPI_LIBS="-lmpi"
         LAMPI_HEADER="$lampi_dir/include/mpi.h"
         LAMPI_DIR="$lampi_dir"
         
# LAMPI has a level of indirection.  The mpi collector
# searches the header file for MPI functions
         if test -f $lampi_dir/include/mpi/mpi.h; then
             LAMPI_HEADER="$lampi_dir/include/mpi/mpi.h"
         fi 

         lampi_saved_CC=$CC
         lampi_saved_CPPFLAGS=$CPPFLAGS
         lampi_saved_LDFLAGS=$LDFLAGS
         lampi_saved_LIBS=$LIBS

         CC="$LAMPI_CC"
         CPPFLAGS="$CPPFLAGS $LAMPI_CPPFLAGS"
         LDFLAGS="$LDFLAGS $LAMPI_LDFLAGS"
         LIBS="$LAMPI_LIBS"

         AC_LINK_IFELSE([AC_LANG_PROGRAM([[
             #include <mpi.h>
             ]], [[
             MPI_Initialized((int*)0);
             ]])],

             if objdump -T $lampi_dir/$alt_abi_libdir/libmpi.so \
                | cut -d' ' -f3 | grep "^lampi_init" >/dev/null; then
                 found_lampi=1
             fi


             , )

         CC=$lampi_saved_CC
         CPPFLAGS=$lampi_saved_CPPFLAGS
         LDFLAGS=$lampi_saved_LDFLAGS
         LIBS=$lampi_saved_LIBS

         if test $found_lampi -eq 1; then
             AC_MSG_RESULT(yes)
             AM_CONDITIONAL(HAVE_LAMPI, true)
             AC_DEFINE(HAVE_LAMPI, 1, [Define to 1 if you have LAMPI.])
         else
          AC_MSG_RESULT(no)
          AM_CONDITIONAL(HAVE_LAMPI, false)
          LAMPI_CC=""
          LAMPI_CPPFLAGS=""
          LAMPI_LDFLAGS=""
          LAMPI_LIBS=""
          LAMPI_HEADER=""
          LAMPI_DIR=""
        fi
    fi

    AC_SUBST(LAMPI_CC)
    AC_SUBST(LAMPI_CPPFLAGS)
    AC_SUBST(LAMPI_LDFLAGS)
    AC_SUBST(LAMPI_LIBS)
    AC_SUBST(LAMPI_HEADER)
    AC_SUBST(LAMPI_DIR)

])


################################################################################
# Check for MPICH (http://www-unix.mcs.anl.gov/mpi/mpich1)
################################################################################

AC_DEFUN([AX_MPICH], [

    AC_ARG_WITH(mpich,
		AC_HELP_STRING([--with-mpich=DIR],
			       [MPICH installation @<:@/opt/mpich@:>@]),
		mpich_dir=$withval, mpich_dir="/opt/mpich")

    AC_ARG_WITH(mpich-driver,
		AC_HELP_STRING([--with-mpich-driver=NAME],
			       [MPICH driver name @<:@ch-p4@:>@]),
		mpich_driver=$withval, mpich_driver="ch-p4")

    AC_MSG_CHECKING([for MPICH library and headers])

    found_mpich=0

    # Put -shlib into MPICH_CC, since it is needed when building the
    # tests, where $MPICH_CC is used, and is not needed when building
    # the MPI-related plugins, where $MPICH_CC is not used.
    MPICH_CC="$mpich_dir/$mpich_driver/bin/mpicc -shlib"
    MPICH_CPPFLAGS="-I$mpich_dir/include"
    MPICH_LDFLAGS="-L$mpich_dir/$mpich_driver/$abi_libdir/shared"
    MPICH_LIBS="-lmpich"
    MPICH_HEADER="$mpich_dir/include/mpi.h"
    MPICH_DIR="$mpich_dir"

    # On the systems "mcr" and "thunder" at LLNL they have an MPICH variant
    # that has things moved around a bit. Handle this by allowing a "llnl"
    # pseudo-driver that makes the necessary configuration changes.
    if test x"$mpich_driver" == x"llnl"; then
	MPICH_CC="$mpich_dir/bin/mpicc -shlib"
	# even on the Opterons/Peloton systems (x86_64) all libraries are in lib
        # MPICH_LDFLAGS="-L$mpich_dir/$abi_libdir"
        MPICH_LDFLAGS="-L$mpich_dir/lib"
    fi

    if test x"$mpich_driver" == x"llnlib"; then
	MPICH_CC="$mpich_dir/bin/mpicc -shlib"
	# even on the Opterons/Peloton systems (x86_64) all libraries are in lib
        # MPICH_LDFLAGS="-L$mpich_dir/$abi_libdir"
        MPICH_LDFLAGS="-L$mpich_dir/lib"
        MPICH_LIBS="-lmpich -libverbs"
    fi

    # On the systems at LANL they have an MPICH variant
    # that has things moved around a bit. Handle this by allowing a "lanl"
    # pseudo-driver that makes the necessary configuration changes.
    if test x"$mpich_driver" == x"lanl"; then
	MPICH_CC="$mpich_dir/bin/mpicc"
        MPICH_LDFLAGS="-L$mpich_dir/$alt_abi_libdir/shared"
        MPICH_LIBS="-lfmpich"
    fi

    # On the systems at Sandia they have an MPICH variant
    # that has things moved around a bit. Handle this by allowing a "sandia"
    # pseudo-driver that makes the necessary configuration changes.
    if test x"$mpich_driver" == x"sandia"; then
	MPICH_CC="$mpich_dir/bin/mpicc"
        MPICH_LDFLAGS="-L$mpich_dir/$alt_abi_libdir"
        MPICH_LIBS="-lmpich"
    fi


    # On the systems at Sandia they have an MPICH variant
    # that has things moved around a bit. Handle this by allowing a "shared"
    # pseudo-driver that makes the necessary configuration changes.
    if test x"$mpich_driver" == x"shared"; then
	MPICH_CC="$mpich_dir/bin/mpicc"
        MPICH_LDFLAGS="-L$mpich_dir/$abi_libdir/shared"
        MPICH_LIBS="-lmpich"
    fi


    mpich_saved_CC=$CC
    mpich_saved_CPPFLAGS=$CPPFLAGS
    mpich_saved_LDFLAGS=$LDFLAGS
    mpich_saved_LIBS=$LIBS

    CC="$MPICH_CC"
    CPPFLAGS="$CPPFLAGS $MPICH_CPPFLAGS"
    LDFLAGS="$LDFLAGS $MPICH_LDFLAGS"
    LIBS="$MPICH_LIBS"

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
	#include <mpi.h>
	]], [[
	MPI_Initialized((int*)0);
	]])],

        if ( (test -f $mpich_dir/$abi_libdir/shared/libmpich.so) ) ; then
	    found_mpich=1
        fi 

	, )


    if test $found_mpich -eq 1; then
	AC_MSG_RESULT(yes)
	AM_CONDITIONAL(HAVE_MPICH, true)
	AC_DEFINE(HAVE_MPICH, 1, [Define to 1 if you have MPICH.])	
    else
        AC_MSG_RESULT([Temporary output: found_mpich=$found_mpich])
        # Try again looking in the alternative lib dir with no driver
        AC_MSG_CHECKING([for MPICH alternative library and headers with no mpich_driver specified])
        MPICH_LDFLAGS="-L$mpich_dir/$alt_abi_libdir"
#        MPICH_LDFLAGS="-L$mpich_dir/$alt_abi_libdir -L$mpich_dir/$alt_abi_libdir/libmpich.a"
        LDFLAGS="$mpich_saved_LDFLAGS $MPICH_LDFLAGS $MPICH_LIBS"

        AC_LINK_IFELSE([AC_LANG_PROGRAM([[
    	    #include <mpi.h>
	    ]], [[
	    MPI_Initialized((int*)0);
	    ]])],

            if ( (test -f $mpich_dir/$alt_abi_libdir/libmpich.so) ) ; then
    	        found_mpich=1
            fi 

	    , )

        if test $found_mpich -eq 1; then
	  AC_MSG_RESULT(yes)
	  AM_CONDITIONAL(HAVE_MPICH, true)
	  AC_DEFINE(HAVE_MPICH, 1, [Define to 1 if you have MPICH.])	

        else
             AC_MSG_RESULT([Temporary output: found_mpich=$found_mpich])

             # Try again looking in the normal lib dir with no driver
             AC_MSG_CHECKING([for MPICH normal lib library and headers with no mpich_driver specified])
             MPICH_LDFLAGS="-L$mpich_dir/$abi_libdir"
#             MPICH_LDFLAGS="-L$mpich_dir/$abi_libdir -L$mpich_dir/$abi_libdir/libmpich.a"
             LDFLAGS="$mpich_saved_LDFLAGS $MPICH_LDFLAGS $MPICH_LIBS"

             AC_LINK_IFELSE([AC_LANG_PROGRAM([[
                 #include <mpi.h>
                 ]], [[
                 MPI_Initialized((int*)0);
                 ]])],

                 if ( (test -f $mpich_dir/$abi_libdir/libmpich.so) ) ; then
	           found_mpich=1
                 fi 

                 , )

              if test $found_mpich -eq 1; then
                AC_MSG_RESULT(yes)
                AM_CONDITIONAL(HAVE_MPICH, true)
                AC_DEFINE(HAVE_MPICH, 1, [Define to 1 if you have MPICH.])
               else
  	         AC_MSG_RESULT(no)
	         AM_CONDITIONAL(HAVE_MPICH, false)
	         MPICH_CC=""
	         MPICH_CPPFLAGS=""
	         MPICH_LDFLAGS=""
	         MPICH_LIBS=""
	         MPICH_HEADER=""
	         MPICH_DIR=""
               fi
        fi


    fi

    CC=$mpich_saved_CC
    CPPFLAGS=$mpich_saved_CPPFLAGS
    LDFLAGS=$mpich_saved_LDFLAGS
    LIBS=$mpich_saved_LIBS

    AC_SUBST(MPICH_CC)
    AC_SUBST(MPICH_CPPFLAGS)
    AC_SUBST(MPICH_LDFLAGS)
    AC_SUBST(MPICH_LIBS)
    AC_SUBST(MPICH_HEADER)
    AC_SUBST(MPICH_DIR)

])


################################################################################
# Check for MPICH2 (http://www-unix.mcs.anl.gov/mpi/mpich2)
################################################################################

AC_DEFUN([AX_MPICH2], [

    AC_ARG_WITH(mpich2,
		AC_HELP_STRING([--with-mpich2=DIR],
			       [MPICH2 installation @<:@/opt/mpich2@:>@]),
		mpich2_dir=$withval, mpich2_dir="/opt/mpich2")

    AC_ARG_WITH(mpich2-driver,
		AC_HELP_STRING([--with-mpich2-driver=NAME],
			       [MPICH2 driver name @<:@ch-p4@:>@]),
		mpich2_driver=$withval, mpich2_driver="")

    AC_MSG_CHECKING([for MPICH2 library and headers])

    found_mpich2=0

    # Put -shlib into MPICH2_CC, since it is needed when building the
    # tests, where $MPICH2_CC is used, and is not needed when building
    # the MPI-related plugins, where $MPICH2_CC is not used.
    MPICH2_CC="$mpich2_dir/bin/mpicc"
    MPICH2_LDFLAGS="-L$mpich2_dir/$abi_libdir"
    MPICH2_LIBS="-lmpich"
    if (test "$abi_libdir" == "lib64" && test -d $mpich2_dir/include64 && test -f $mpich2_dir/include64/mpi.h) ; then
         MPICH2_HEADER="$mpich2_dir/include64/mpi.h"
         MPICH2_CPPFLAGS="-I$mpich2_dir/include64"
    elif (test -e $mpich2/include/mpi.h) ; then
         MPICH2_CPPFLAGS="-I$mpich2/include"
         MPICH2_HEADER="$mpich2/include/mpi.h"
    elif (test -e /usr/include/mpich2-$oss_hardware_platform/mpi.h) ; then
         MPICH2_CPPFLAGS="-I/usr/include/mpich2-$oss_hardware_platform"
         MPICH2_HEADER="-I/usr/include/mpich2-$oss_hardware_platform/mpi.h"
    else
         MPICH2_HEADER="$mpich2_dir/include/mpi.h"
         MPICH2_CPPFLAGS="-I$mpich2_dir/include"
    fi
    MPICH2_DIR="$mpich2_dir"

    # On the systems "mcr" and "thunder" at LLNL they have an MPICH variant
    # that has things moved around a bit. Handle this by allowing a "llnl"
    # pseudo-driver that makes the necessary configuration changes.
    
    if test x"$mpich2_driver" == x"llnl"; then
	MPICH2_CC="$mpich2_dir/bin/mpicc -shlib"
        MPICH2_LDFLAGS="-L$mpich2_dir/$abi_libdir"
    fi

    if test x"$mpich2_driver" == x"bluegeneq"; then
	found_mpich2=1
	MPICH2_CC="cc"
        MPICH2_LIBS="-lmpich"
        MPICH2_LDFLAGS="-L$mpich2_dir/lib"
    fi

    if test x"$mpich2_driver" == x"bluegene"; then
	found_mpich2=1
	MPICH2_CC="cc"
        MPICH2_LIBS="-lmpich.cnk"
        MPICH2_LDFLAGS="-L$mpich2_dir/lib"
    fi


    if test x"$mpich2_driver" == x"cray"; then
	found_mpich2=1
	MPICH2_CC="cc"
        MPICH2_LDFLAGS="-L$mpich2_dir/lib"
    fi

    mpich2_saved_CC=$CC
    mpich2_saved_CPPFLAGS=$CPPFLAGS
    mpich2_saved_LDFLAGS=$LDFLAGS
    mpich2_saved_LIBS=$LIBS

    CC="$MPICH2_CC"
    CPPFLAGS="$CPPFLAGS $MPICH2_CPPFLAGS"
    LDFLAGS="$LDFLAGS $MPICH2_LDFLAGS"
    LIBS="$MPICH2_LIBS"

    if test $found_mpich2 -eq 0; then

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
	#include <mpi.h>
	]], [[
	MPI_Initialized((int*)0);
	]])],

        if (test -f $mpich2_dir/$abi_libdir/libmpich.so) ; then
	    found_mpich2=1
            MPICH2_LDFLAGS="-L$mpich2_dir/$abi_libdir"
        fi

        if (test -f $mpich2_dir/$abi_libdir/shared/libmpich.so); then
	    found_mpich2=1
            MPICH2_LDFLAGS="-L$mpich2_dir/$abi_libdir/shared -L$mpich2_dir/$abi_libdir"
        fi 

	, )

    fi


    if test $found_mpich2 -eq 0; then

       AC_MSG_CHECKING([for MPICH2 library (alt locations) and headers])

       # Put -shlib into MPICH2_CC, since it is needed when building the
       # tests, where $MPICH2_CC is used, and is not needed when building
       # the MPI-related plugins, where $MPICH2_CC is not used.
       MPICH2_CC="$mpich2_dir/bin/mpicc"
#       MPICH2_CC="$mpich2_dir/bin/mpicc -shlib"
       MPICH2_CPPFLAGS="-I$mpich2_dir/include"
       MPICH2_LDFLAGS="-L$mpich2_dir/$alt_abi_libdir"
#       MPICH2_LDFLAGS="-L$mpich2_dir/$alt_abi_libdir -L$mpich2_dir/$alt_abi_libdir/libmpich.a"
       MPICH2_LIBS="-lmpich"
#       MPICH2_LIBS=""
       MPICH2_HEADER="$mpich2_dir/include/mpi.h"
       MPICH2_DIR="$mpich2_dir"
       if (test -e $mpich2/include/mpi.h) ; then
         MPICH2_CPPFLAGS="-I$mpich2/include"
         MPICH2_HEADER="$mpich2/include/mpi.h"
       elif (test -e /usr/include/mpich2-$oss_hardware_platform/mpi.h) ; then
         MPICH2_CPPFLAGS="-I/usr/include/mpich2-$oss_hardware_platform"
         MPICH2_HEADER="-I/usr/include/mpich2-$oss_hardware_platform/mpi.h"
       fi

       CC="$MPICH2_CC"
       CPPFLAGS="$CPPFLAGS $MPICH2_CPPFLAGS"
       LDFLAGS="$LDFLAGS $MPICH2_LDFLAGS"
       LIBS="$MPICH2_LIBS"

       AC_LINK_IFELSE([AC_LANG_PROGRAM([[
   	  #include <mpi.h>
	  ]], [[
	  MPI_Initialized((int*)0);
	  ]])],

         if (test -f $mpich2_dir/$alt_abi_libdir/libmpich.so) ; then
            MPICH2_LDFLAGS="-L$mpich2_dir/$alt_abi_libdir"
	    found_mpich2=1
            AC_MSG_CHECKING([found MPICH2 library alt locations found alt_abi_libdir and headers])
         fi

         if (test -f $mpich2_dir/$alt_abi_libdir/shared/libmpich.so) ; then
            MPICH2_LDFLAGS="-L$mpich2_dir/$alt_abi_libdir/shared -L$mpich2_dir/$alt_abi_libdir"
	    found_mpich2=1
            AC_MSG_CHECKING([found MPICH2 library alt locations found shared/alt_abi_libdir and headers])
         fi 

	, )

    fi

#
# Check now for Intel MPI version of installation for their MPICH2 based MPI Implementation
#
    if test $found_mpich2 -eq 0 && test "$abi_libdir" == "lib64" ; then

       AC_MSG_CHECKING([for Intel MPICH2 64 bit library and headers using mpigcc])

       MPICH2_CC="$mpich2_dir/bin64/mpgicc"
       MPICH2_CPPFLAGS="-I$mpich2_dir/include"
       if (test -d $mpich2_dir/$abi_libdir) ; then
          MPICH2_LDFLAGS="-L$mpich2_dir/$abi_libdir"
       elif (test -d $mpich2_dir/$alt_abi_libdir) ; then
          MPICH2_LDFLAGS="-L$mpich2_dir/$alt_abi_libdir"
       fi
       MPICH2_LIBS="-lmpich"
       MPICH2_HEADER="$mpich2_dir/include/mpi.h"
       MPICH2_DIR="$mpich2_dir"

       CC="$MPICH2_CC"
       CPPFLAGS="$CPPFLAGS $MPICH2_CPPFLAGS"
       LDFLAGS="$LDFLAGS $MPICH2_LDFLAGS"
       LIBS="$MPICH2_LIBS"

       AC_LINK_IFELSE([AC_LANG_PROGRAM([[
   	  #include <mpi.h>
	  ]], [[
	  MPI_Initialized((int*)0);
	  ]])],

         if (test -f $mpich2_dir/$abi_libdir/libmpich.so && test -f $mpich2_dir/include/mpi.h) ; then
            MPICH2_LDFLAGS="-L$mpich2_dir/$abi_libdir"
	    found_mpich2=1
            AC_MSG_CHECKING([found Intel MPICH2 64 bit library and headers using mpigcc])
         elif (test -f $mpich2_dir/$alt_abi_libdir/libmpich.so && test -f $mpich2_dir/include/mpi.h) ; then
            MPICH2_LDFLAGS="-L$mpich2_dir/$alt_abi_libdir"
	    found_mpich2=1
            AC_MSG_CHECKING([found Intel MPICH2 64 bit library and headers using mpigcc])
         fi

	, )

    fi

#
# Check now for Intel MPI version of installation for their MPICH2 based MPI Implementation with libmpi not libmpich
#
    if test $found_mpich2 -eq 0 && test "$abi_libdir" == "lib" ; then

       AC_MSG_CHECKING([for Intel 32 bit MPICH2 library and headers using mpicc])

       MPICH2_CC="$mpich2_dir/bin/mpicc"
       MPICH2_CPPFLAGS="-I$mpich2_dir/include"
       MPICH2_LDFLAGS="-L$mpich2_dir/$abi_libdir"
       MPICH2_LIBS="-lmpi"
       MPICH2_HEADER="$mpich2_dir/include/mpi.h"
       MPICH2_DIR="$mpich2_dir"

       CC="$MPICH2_CC"
       CPPFLAGS="$CPPFLAGS $MPICH2_CPPFLAGS"
       LDFLAGS="$LDFLAGS $MPICH2_LDFLAGS"
       LIBS="$MPICH2_LIBS"

       AC_LINK_IFELSE([AC_LANG_PROGRAM([[
   	  #include <mpi.h>
	  ]], [[
	  MPI_Initialized((int*)0);
	  ]])],

         if (test -f $mpich2_dir/$abi_libdir/libmpi.so && test -f $mpich2_dir/include/mpi.h) ; then
            MPICH2_LDFLAGS="-L$mpich2_dir/$abi_libdir"
	    found_mpich2=1
            AC_MSG_CHECKING([found Intel 32 bit MPICH2 library and headers using mpicc])
         fi

	, )

    fi


#
# Check now for Intel MPI version of installation for their MPICH2 based MPI Implementation
#
    if test $found_mpich2 -eq 0 && test "$abi_libdir" == "lib" ; then

       AC_MSG_CHECKING([for Intel 32 bit MPICH2 library and headers using mpigcc])

       MPICH2_CC="$mpich2_dir/bin/mpigcc"
       MPICH2_CPPFLAGS="-I$mpich2_dir/include"
       MPICH2_LDFLAGS="-L$mpich2_dir/$abi_libdir"
       MPICH2_LIBS="-lmpich"
       MPICH2_HEADER="$mpich2_dir/include/mpi.h"
       MPICH2_DIR="$mpich2_dir"

       CC="$MPICH2_CC"
       CPPFLAGS="$CPPFLAGS $MPICH2_CPPFLAGS"
       LDFLAGS="$LDFLAGS $MPICH2_LDFLAGS"
       LIBS="$MPICH2_LIBS"

       AC_LINK_IFELSE([AC_LANG_PROGRAM([[
   	  #include <mpi.h>
	  ]], [[
	  MPI_Initialized((int*)0);
	  ]])],

         if (test -f $mpich2_dir/$abi_libdir/libmpich.so && test -f $mpich2_dir/include/mpi.h) ; then
            MPICH2_LDFLAGS="-L$mpich2_dir/$abi_libdir"
	    found_mpich2=1
            AC_MSG_CHECKING([found Intel 32 bit MPICH2 library and headers using mpigcc])
         fi

	, )

    fi
#
# Check now for Intel MPI version of installation for their MPICH2 based MPI Implementation
#
    if test $found_mpich2 -eq 0 && test "$abi_libdir" == "lib64" ; then

       AC_MSG_CHECKING([for Intel MPICH2 64 bit library and headers using mpicc])

       if ( test -d $mpich2_dir/bin64 && test -f $mpich2_dir/bin64/mpicc) ; then
          MPICH2_CC="$mpich2_dir/bin64/mpicc"
       elif (test -d $mpich2_dir/bin && test -f $mpich2_dir/bin/mpicc) ; then
          MPICH2_CC="$mpich2_dir/bin/mpicc"
       elif ( ! test -d $mpich2_dir/bin ) ; then
          MPICH2_CC="cc"
       fi

       if (test -d $mpich2_dir/$abi_libdir) ; then
          MPICH2_LDFLAGS="-L$mpich2_dir/$abi_libdir"
       elif (test -d $mpich2_dir/$alt_abi_libdir) ; then
          MPICH2_LDFLAGS="-L$mpich2_dir/$alt_abi_libdir"
       fi
       MPICH2_LIBS="-lmpich"
       if (test "$abi_libdir" == "lib64" && test -d $mpich2_dir/include64 && test -f $mpich2_dir/include64/mpi.h) ; then
         MPICH2_HEADER="$mpich2_dir/include64/mpi.h"
         MPICH2_CPPFLAGS="-I$mpich2_dir/include64"
       else
         MPICH2_HEADER="$mpich2_dir/include/mpi.h"
         MPICH2_CPPFLAGS="-I$mpich2_dir/include"
       fi
       MPICH2_DIR="$mpich2_dir"

       CC="$MPICH2_CC"
       CPPFLAGS="$CPPFLAGS $MPICH2_CPPFLAGS"
       LDFLAGS="$LDFLAGS $MPICH2_LDFLAGS"
       LIBS="$MPICH2_LIBS"

       AC_LINK_IFELSE([AC_LANG_PROGRAM([[
   	  #include <mpi.h>
	  ]], [[
	  MPI_Initialized((int*)0);
	  ]])],

         if (test -f $mpich2_dir/$abi_libdir/libmpich.so) ; then
            MPICH2_LDFLAGS="-L$mpich2_dir/$abi_libdir"
	    found_mpich2=1
            AC_MSG_CHECKING([found Intel MPICH2 64 bit library and headers using mpicc])
         elif (test -f $mpich2_dir/$alt_abi_libdir/libmpich.so) ; then
            MPICH2_LDFLAGS="-L$mpich2_dir/$alt_abi_libdir"
	    found_mpich2=1
            AC_MSG_CHECKING([found Intel MPICH2 64 bit library and headers using mpicc])
         fi

	, )

    fi

#
# Check now for Intel MPI version of installation for their MPICH2 based MPI Implementation
#
    if test $found_mpich2 -eq 0 && test "$abi_libdir" == "lib" ; then

       AC_MSG_CHECKING([for Intel 32 bit MPICH2 library and headers using mpicc])

       MPICH2_CC="$mpich2_dir/bin/mpicc"
       MPICH2_CPPFLAGS="-I$mpich2_dir/include"
       MPICH2_LDFLAGS="-L$mpich2_dir/$abi_libdir"
       MPICH2_LIBS="-lmpich"
       MPICH2_HEADER="$mpich2_dir/include/mpi.h"
       MPICH2_DIR="$mpich2_dir"

       CC="$MPICH2_CC"
       CPPFLAGS="$CPPFLAGS $MPICH2_CPPFLAGS"
       LDFLAGS="$LDFLAGS $MPICH2_LDFLAGS"
       LIBS="$MPICH2_LIBS"

       AC_LINK_IFELSE([AC_LANG_PROGRAM([[
   	  #include <mpi.h>
	  ]], [[
	  MPI_Initialized((int*)0);
	  ]])],

         if (test -f $mpich2_dir/$abi_libdir/libmpich.so) ; then
            MPICH2_LDFLAGS="-L$mpich2_dir/$abi_libdir"
	    found_mpich2=1
            AC_MSG_CHECKING([found Intel 32 bit MPICH2 library and headers using mpicc])
         fi

	, )

    fi

    CC=$mpich2_saved_CC
    CPPFLAGS=$mpich2_saved_CPPFLAGS
    LDFLAGS=$mpich2_saved_LDFLAGS
    LIBS=$mpich2_saved_LIBS

    if test $found_mpich2 -eq 1; then
	AC_MSG_RESULT(yes)
	AM_CONDITIONAL(HAVE_MPICH2, true)
	AC_DEFINE(HAVE_MPICH2, 1, [Define to 1 if you have MPICH.])	
    else
	AC_MSG_RESULT(no)
	AM_CONDITIONAL(HAVE_MPICH2, false)
	MPICH2_CC=""
	MPICH2_CPPFLAGS=""
	MPICH2_LDFLAGS=""
	MPICH2_LIBS=""
	MPICH2_HEADER=""
	MPICH2_DIR=""
    fi

    AC_SUBST(MPICH2_CC)
    AC_SUBST(MPICH2_CPPFLAGS)
    AC_SUBST(MPICH2_LDFLAGS)
    AC_SUBST(MPICH2_LIBS)
    AC_SUBST(MPICH2_HEADER)
    AC_SUBST(MPICH2_DIR)

])

################################################################################
# Check for TARGET MPICH2 (http://www-unix.mcs.anl.gov/mpi/mpich2)
################################################################################

AC_DEFUN([AX_TARGET_MPICH2], [

    AC_ARG_WITH(target-mpich2,
		AC_HELP_STRING([--with-target-mpich2=DIR],
			       [MPICH2 target installation @<:@/zzz@:>@]),
		target_mpich2_dir=$withval, target_mpich2_dir="/zzz")

    AC_MSG_CHECKING([for TARGET MPICH2 library and headers])

    found_mpich2=0

    TARGET_MPICH2_CC=""
    TARGET_MPICH2_LIBS="-lmpich"
    TARGET_MPICH2_CPPFLAGS="-I$target_mpich2_dir/include"
    TARGET_MPICH2_HEADER="$target_mpich2_dir/include/mpi.h"
    TARGET_MPICH2_DIR="$target_mpich2_dir"

    if (test -f $target_mpich2_dir/include/mpi.h) ; then
      if (test -f $target_mpich2_dir/$abi_libdir/libmpich.a) ; then
        found_mpich2=1
        TARGET_MPICH2_LDFLAGS="-L$target_mpich2_dir/$abi_libdir"
      elif (test -f $target_mpich2_dir/$alt_abi_libdir/libmpich.a); then
        found_mpich2=1
        TARGET_MPICH2_LDFLAGS="-L$target_mpich2_dir/$alt_abi_libdir"
      elif (test -f $target_mpich2_dir/$alt_abi_libdir/libmpich.cnk.a); then
        found_mpich2=1
        TARGET_MPICH2_LDFLAGS="-L$target_mpich2_dir/$alt_abi_libdir"
        TARGET_MPICH2_LIBS="-lmpich.cnk"
      elif (test -f $target_mpich2_dir/$abi_libdir/libmpich.cnk.a); then
        found_mpich2=1
        TARGET_MPICH2_LDFLAGS="-L$target_mpich2_dir/$abi_libdir"
        TARGET_MPICH2_LIBS="-lmpich.cnk"
      elif (test -f $target_mpich2_dir/$alt_abi_libdir/libmpich-gcc.a) ; then
        found_mpich2=1
        TARGET_MPICH2_LIBS="-lmpich-gcc"
        TARGET_MPICH2_LDFLAGS="-L$target_mpich2_dir/$alt_abi_libdir"
      elif (test -f $target_mpich2_dir/$abi_libdir/libmpich-gcc.a) ; then
        found_mpich2=1
        TARGET_MPICH2_LIBS="-lmpich-gcc"
        TARGET_MPICH2_LDFLAGS="-L$target_mpich2_dir/$abi_libdir"
      fi 
    fi 

    if test $found_mpich2 -eq 1; then
	AC_MSG_RESULT(yes)
	AM_CONDITIONAL(HAVE_TARGET_MPICH2, true)
	AC_DEFINE(HAVE_TARGET_MPICH2, 1, [Define to 1 if you have MPICH.])	
    else
	AC_MSG_RESULT(no)
	AM_CONDITIONAL(HAVE_TARGET_MPICH2, false)
	TARGET_MPICH2_CC=""
	TARGET_MPICH2_CPPFLAGS=""
	TARGET_MPICH2_LDFLAGS=""
	TARGET_MPICH2_LIBS=""
	TARGET_MPICH2_HEADER=""
	TARGET_MPICH2_DIR=""
    fi

    AC_SUBST(TARGET_MPICH2_CC)
    AC_SUBST(TARGET_MPICH2_CPPFLAGS)
    AC_SUBST(TARGET_MPICH2_LDFLAGS)
    AC_SUBST(TARGET_MPICH2_LIBS)
    AC_SUBST(TARGET_MPICH2_HEADER)
    AC_SUBST(TARGET_MPICH2_DIR)

])

################################################################################
# Check for MPT (http://www.sgi.com/products/software/mpt)
################################################################################

AC_DEFUN([AX_MPT], [

    AC_ARG_WITH(mpt,
		AC_HELP_STRING([--with-mpt=DIR],
			       [MPT installation @<:@/usr@:>@]),
		mpt_dir=$withval, mpt_dir="/usr")

    AC_MSG_CHECKING([for MPT library and headers])

    found_mpt=0

    if objdump -T $mpt_dir/$abi_libdir/libmpi.so | grep MPI_debug_rank >/dev/null; then
       found_mpt=1
       MPT_LDFLAGS="-L$mpt_dir/$abi_libdir"
    elif objdump -T $mpt_dir/lib/libmpi.so | grep MPI_debug_rank >/dev/null; then
       found_mpt=1
       MPT_LDFLAGS="-L$mpt_dir/lib"
    fi


    MPT_CC="$CC"
    MPT_CPPFLAGS="-I$mpt_dir/include"
#    MPT_LDFLAGS="-L$mpt_dir/$abi_libdir"
    MPT_LIBS="-lmpi"
    MPT_HEADER="$mpt_dir/include/mpi.h"
    MPT_DIR="$mpt_dir"

    mpt_saved_CC=$CC
    mpt_saved_CPPFLAGS=$CPPFLAGS
    mpt_saved_LDFLAGS=$LDFLAGS
    mpt_saved_LIBS=$LIBS

    CC="$MPT_CC"
    CPPFLAGS="$CPPFLAGS $MPT_CPPFLAGS"
    LDFLAGS="$LDFLAGS $MPT_LDFLAGS"
    LIBS="$MPT_LIBS"

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
	#include <mpi.h>
	]], [[
	MPI_Initialized((int*)0);
	]])],
	, )

    CC=$mpt_saved_CC
    CPPFLAGS=$mpt_saved_CPPFLAGS
    LDFLAGS=$mpt_saved_LDFLAGS
    LIBS=$mpt_saved_LIBS

    if test $found_mpt -eq 1; then
	AC_MSG_RESULT(yes)
	AM_CONDITIONAL(HAVE_MPT, true)
	AC_DEFINE(HAVE_MPT, 1, [Define to 1 if you have MPT.])	
    else
	AC_MSG_RESULT(no)
	AM_CONDITIONAL(HAVE_MPT, false)
	MPT_CC=""
	MPT_CPPFLAGS=""
	MPT_LDFLAGS=""
	MPT_LIBS=""
	MPT_HEADER=""
	MPT_DIR=""
    fi

    AC_SUBST(MPT_CC)
    AC_SUBST(MPT_CPPFLAGS)
    AC_SUBST(MPT_LDFLAGS)
    AC_SUBST(MPT_LIBS)
    AC_SUBST(MPT_HEADER)
    AC_SUBST(MPT_DIR)

])


################################################################################
# Check for OpenMPI (http://www.open-mpi.org)
################################################################################

AC_DEFUN([AX_OPENMPI], [

    AC_ARG_WITH(openmpi,
		AC_HELP_STRING([--with-openmpi=DIR],
			       [OpenMPI installation @<:@/usr@:>@]),
		openmpi_dir=$withval, openmpi_dir="/usr")

    AC_MSG_CHECKING([for OpenMPI library and headers])

    found_openmpi=0

    OPENMPI_CC="$openmpi_dir/bin/mpicc"
    if (test -e $openmpi_dir/include/mpi.h) ; then
      OPENMPI_CPPFLAGS="-I$openmpi_dir/include"
      OPENMPI_HEADER="$openmpi_dir/include/mpi.h"
    elif (test -e /usr/include/mpi/mpi.h) ; then
      OPENMPI_CPPFLAGS="-I/usr/include/mpi"
      OPENMPI_HEADER="-I/usr/include/mpi/mpi.h"
    elif (test -e /usr/include/openmpi-$oss_hardware_platform/mpi.h) ; then
      OPENMPI_CPPFLAGS="-I/usr/include/openmpi-$oss_hardware_platform"
      OPENMPI_HEADER="-I/usr/include/openmpi-$oss_hardware_platform/mpi.h"
    fi
    if (test -e $openmpi_dir/$abi_libdir/libmpi.so) ; then
      OPENMPI_LDFLAGS="-L$openmpi_dir/$abi_libdir"
    elif (test -e $openmpi_dir/$alt_abi_libdir/libmpi.so) ; then
      OPENMPI_LDFLAGS="-L$openmpi_dir/$alt_abi_libdir"
    fi
    OPENMPI_LIBS="-lmpi"
    OPENMPI_DIR="$openmpi_dir"

    openmpi_saved_CC=$CC
    openmpi_saved_CPPFLAGS=$CPPFLAGS
    openmpi_saved_LDFLAGS=$LDFLAGS
    openmpi_saved_LIBS=$LIBS

    CC="$OPENMPI_CC"
    CPPFLAGS="-O0 -g $CPPFLAGS $OPENMPI_CPPFLAGS"
    LDFLAGS="$LDFLAGS $OPENMPI_LDFLAGS"
    LIBS="$OPENMPI_LIBS"

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
	#include <mpi.h>
	]], [[
	MPI_Initialized((int*)0);
	]])],

	if (objdump -T $openmpi_dir/$abi_libdir/libmpi.so | grep "ompi_mpi" >/dev/null) ||
	   (objdump -T $openmpi_dir/$alt_abi_libdir/libmpi.so | grep "ompi_mpi" >/dev/null) ||
           (objdump -T /usr/$abi_libdir/openmpi/libmpi.so | grep "ompi_mpi" >/dev/null) ||
           (objdump -T /usr/$abi_libdir/openmpi/libmpi_f90.a | grep "ompi_mpi" >/dev/null) ||
           (objdump -T $openmpi_dir/$abi_libdir/libmpi.a | grep "^ompi_mpi" >/dev/null) ||
           (objdump -T $openmpi_dir/$alt_abi_libdir/libmpi.a | grep "^ompi_mpi" >/dev/null) ; then
	    found_openmpi=1
	fi

	, )
#
# Try again with different libraries - this case the /usr/lib64/openmpi and /usr/include/openmpi case
#
   if test $found_openmpi -eq 0; then
     OPENMPI_CPPFLAGS="-I$openmpi_dir/include/openmpi"
     OPENMPI_LDFLAGS="-L$openmpi_dir/$abi_libdir/openmpi"
     OPENMPI_HEADER="$openmpi_dir/include/openmpi/mpi.h"
     LDFLAGS="$LDFLAGS $OPENMPI_LDFLAGS"
     LIBS="$OPENMPI_LIBS"
     CPPFLAGS="-O0 -g $CPPFLAGS $OPENMPI_CPPFLAGS"

     AC_LINK_IFELSE([AC_LANG_PROGRAM([[
   	#include <mpi.h>
   	]], [[
   	MPI_Initialized((int*)0);
   	]])],
   
   	if (objdump -T $openmpi_dir/$abi_libdir/openmpi/libmpi.so | grep "ompi_mpi" >/dev/null) ||
           (objdump -T /usr/$abi_libdir/openmpi/libmpi.so | grep "ompi_mpi" >/dev/null) ||
           (objdump -T /usr/$abi_libdir/openmpi/libmpi_f90.a  grep "ompi_mpi" >/dev/null) ||
           (objdump -T $openmpi_dir/$abi_libdir/openmpi/libmpi.a | grep "ompi_mpi" >/dev/null) ; then
	   found_openmpi=1
   	fi
   
   	, )
    fi

#
# Try again with different libraries
#
   if test $found_openmpi -eq 0; then
     OPENMPI_CPPFLAGS="-I$openmpi_dir/include"
     OPENMPI_HEADER="$openmpi_dir/include/mpi.h"
     OPENMPI_LDFLAGS="-L$openmpi_dir/$abi_libdir"
     LDFLAGS="$LDFLAGS $OPENMPI_LDFLAGS"
     LIBS="$OPENMPI_LIBS"
     CPPFLAGS="-O0 -g $CPPFLAGS $OPENMPI_CPPFLAGS"

     AC_LINK_IFELSE([AC_LANG_PROGRAM([[
   	#include <mpi.h>
   	]], [[
   	MPI_Initialized((int*)0);
   	]])],
   
   	if (objdump -T $openmpi_dir/$abi_libdir/libmpi.so | grep "ompi_mpi" >/dev/null) ||
           (objdump -T /usr/$abi_libdir/openmpi/libmpi.so | grep "ompi_mpi" >/dev/null) ||
           (objdump -T /usr/$abi_libdir/openmpi/libmpi_f90.a | grep "ompi_mpi" >/dev/null) ||
           (objdump -T $openmpi_dir/$abi_libdir/libmpi.a | grep "ompi_mpi" >/dev/null) ; then
	   found_openmpi=1
   	fi
   
   	, )
    fi

#
# Try again with different libraries
#
   if test $found_openmpi -eq 0; then
     OPENMPI_LDFLAGS="-L$openmpi_dir/$alt_abi_libdir"
     LDFLAGS="$LDFLAGS $OPENMPI_LDFLAGS $OPENMPI_LIBS"

     AC_LINK_IFELSE([AC_LANG_PROGRAM([[
   	#include <mpi.h>
   	]], [[
   	MPI_Initialized((int*)0);
   	]])],
   
   	if (objdump -T $openmpi_dir/$alt_abi_libdir/libmpi.so | grep "ompi_mpi" >/dev/null) ||
           (objdump -T /usr/$alt_abi_libdir/openmpi/libmpi.so | grep "ompi_mpi" >/dev/null) ||
           (objdump -T /usr/$alt_abi_libdir/openmpi/libmpi_f90.a | grep "ompi_mpi" >/dev/null) ||
           (objdump -T $openmpi_dir/$alt_abi_libdir/libmpi.a |  grep "ompi_mpi" >/dev/null) ; then
	   found_openmpi=1
   	fi
   
   	, )
    fi

#
# Try again with different mpicc - for older versions of openmpi try om-mpicxx
#
   if test $found_openmpi -eq 0 && test -a $openmpi_dir/bin/om-mpicx; then
     OPENMPI_LDFLAGS="-L$openmpi_dir/$abi_libdir/openmpi"
     LDFLAGS="$LDFLAGS $OPENMPI_LDFLAGS"
     LIBS="$OPENMPI_LIBS"
     OPENMPI_CC="$openmpi_dir/bin/om-mpicxx"
     CC="$OPENMPI_CC"

     AC_LINK_IFELSE([AC_LANG_PROGRAM([[
   	#include <mpi.h>
   	]], [[
   	MPI_Initialized((int*)0);
   	]])],
   
   	if (objdump -T $openmpi_dir/$abi_libdir/libmpi.so | grep "ompi_mpi" >/dev/null) ||
           (objdump -T /usr/$abi_libdir/openmpi/libmpi.so | grep "ompi_mpi" >/dev/null) ||
           (objdump -T /usr/$abi_libdir/openmpi/libmpi_f90.a | grep "ompi_mpi" >/dev/null) ||
           (objdump -T $openmpi_dir/$abi_libdir/libmpi.a | grep "ompi_mpi" >/dev/null) ; then
	   found_openmpi=1
   	fi
   
   	, )
    fi

    CC=$openmpi_saved_CC
    CPPFLAGS=$openmpi_saved_CPPFLAGS
    LDFLAGS=$openmpi_saved_LDFLAGS
    LIBS=$openmpi_saved_LIBS

    if test $found_openmpi -eq 1; then
	AC_MSG_RESULT(yes)
	AM_CONDITIONAL(HAVE_OPENMPI, true)
	AC_DEFINE(HAVE_OPENMPI, 1, [Define to 1 if you have OpenMPI.])	
    else
	AC_MSG_RESULT(no)
	AM_CONDITIONAL(HAVE_OPENMPI, false)
	OPENMPI_CC=""
	OPENMPI_CPPFLAGS=""
	OPENMPI_LDFLAGS=""
	OPENMPI_LIBS=""
	OPENMPI_HEADER=""
	OPENMPI_DIR=""
    fi

    AC_SUBST(OPENMPI_CC)
    AC_SUBST(OPENMPI_CPPFLAGS)
    AC_SUBST(OPENMPI_LDFLAGS)
    AC_SUBST(OPENMPI_LIBS)
    AC_SUBST(OPENMPI_HEADER)
    AC_SUBST(OPENMPI_DIR)

])


################################################################################
# Check for MPI (http://www.mpi-forum.org/)
################################################################################

AC_DEFUN([AX_PKG_MPI], [

    AX_LAM()
    AX_LAMPI()
    AX_MPICH()
    AX_MPICH2()
    AX_MVAPICH()
    AX_MVAPICH2()
    AX_MPT()
    AX_OPENMPI()
    AX_TARGET_MPICH2()

    default_mpi=""
    all_mpi_names=""

    default_target_mpi=""
    all_target_mpi_names=""

    # Test for MPI implementations in reverse order of priority for
    # the default choice (ordering compatible with historical
    # AX_MPI behavior), so we can simultaneously gather all of
    # them and easily set $default_mpi* correctly.

    if test x"$OPENMPI_LIBS" != x""; then
	default_mpi=OPENMPI;  default_mpi_name=openmpi
	all_mpi_names=" openmpi $all_mpi_names"
    fi
    if test x"$LAM_LIBS" != x""; then
	default_mpi=LAM;    default_mpi_name=lam
	all_mpi_names=" lam $all_mpi_names"
    fi
    if test x"$LAMPI_LIBS" != x""; then
	default_mpi=LAMPI;    default_mpi_name=lampi
	all_mpi_names=" lampi $all_mpi_names"
    fi
    if test x"$MPICH2_LDFLAGS" != x""; then
	default_mpi=MPICH2;    default_mpi_name=mpich2
	all_mpi_names=" mpich2 $all_mpi_names"
    fi
    if test x"$MPICH_LIBS" != x""; then
	default_mpi=MPICH;    default_mpi_name=mpich
	all_mpi_names=" mpich $all_mpi_names"
    fi
    if test x"$MPT_LIBS" != x""; then
	default_mpi=MPT;      default_mpi_name=mpt
	all_mpi_names=" mpt $all_mpi_names"
    fi
    if test x"$MVAPICH_LDFLAGS" != x""; then
	default_mpi=MVAPICH;    default_mpi_name=mvapich
	all_mpi_names=" mvapich $all_mpi_names"
    fi
    if test x"$MVAPICH2_LDFLAGS" != x""; then
	default_mpi=MVAPICH2;    default_mpi_name=mvapich2
	all_mpi_names=" mvapich2 $all_mpi_names"
    fi

    if test x"$TARGET_MPICH2_LDFLAGS" != x""; then
#	default_mpi=MPICH2;    default_mpi_name=mpich2
	default_target_mpi=MPICH2;    default_target_mpi_name=mpich2
#	all_mpi_names=" mpich2 $all_mpi_names"
	all_target_mpi_names=" mpich2 $all_target_mpi_names"
    fi

    if test x"$default_mpi" != x""; then
	AM_CONDITIONAL(HAVE_MPI, true)
	AC_DEFINE(HAVE_MPI, 1, [Define to 1 if you have MPI.])	
	AC_SUBST(DEFAULT_MPI_IMPL, $default_mpi)
	AC_DEFINE_UNQUOTED(ALL_MPI_IMPL_NAMES, "$all_mpi_names",
			   [Names of all MPI Implementations])
	AC_DEFINE_UNQUOTED(DEFAULT_MPI_IMPL_NAME, "$default_mpi_name",
			   [Name of default MPI Implementation])
    else
	AM_CONDITIONAL(HAVE_MPI, false)
    fi

    if test x"$default_target_mpi" != x""; then
	AM_CONDITIONAL(HAVE_TARGET_MPI, true)
	AC_DEFINE(HAVE_TARGET_MPI, 1, [Define to 1 if you have MPI.])	
	AC_SUBST(DEFAULT_TARGET_MPI_IMPL, $default_target_mpi)
	AC_DEFINE_UNQUOTED(ALL_TARGET_MPI_IMPL_NAMES, "$all_target_mpi_names",
			   [Names of all TARGET_MPI Implementations])
	AC_DEFINE_UNQUOTED(DEFAULT_TARGET_MPI_IMPL_NAME, "$default_target_mpi_name",
			   [Name of default TARGET_MPI Implementation])
    else
	AM_CONDITIONAL(HAVE_TARGET_MPI, false)
    fi

])



################################################################################
# Check for MVAPICH (http://mvapich.cse.ohio-state.edu/overview/mvapich/)
################################################################################

AC_DEFUN([AX_MVAPICH], [

    AC_ARG_WITH(mvapich,
		AC_HELP_STRING([--with-mvapich=DIR],
			       [MVAPICH installation @<:@/opt/mvapich@:>@]),
		mvapich_dir=$withval, mvapich_dir="/opt/mvapich")

#    AC_ARG_WITH(mvapich-driver,
#		AC_HELP_STRING([--with-mvapich-driver=NAME],
#			       [MVAPICH driver name @<:@ch-p4@:>@]),
#		mvapich_driver=$withval, mvapich_driver="")

    AC_MSG_CHECKING([for MVAPICH library and headers])

    found_mvapich=0

    # Put -shlib into MVAPICH_CC, since it is needed when building the
    # tests, where $MVAPICH_CC is used, and is not needed when building
    # the MPI-related plugins, where $MVAPICH_CC is not used.
    MVAPICH_CC="$mvapich_dir/bin/mpicc -shlib"
    MVAPICH_CPPFLAGS="-I$mvapich_dir/include"
    MVAPICH_LDFLAGS="-L$mvapich_dir/$abi_libdir"
    MVAPICH_LIBS="-lmpich -libverbs -libumad -libcommon"
    MVAPICH_HEADER="$mvapich_dir/include/mpi.h"
    MVAPICH_DIR="$mvapich_dir"

    # On the systems "mcr" and "thunder" at LLNL they have an MPICH variant
    # that has things moved around a bit. Handle this by allowing a "llnl"
    # pseudo-driver that makes the necessary configuration changes.
#    if test x"$mvapich_driver" == x"llnl"; then
#	MVAPICH_CC="$mvapich_dir/bin/mpicc -shlib"
#        MVAPICH_LDFLAGS="-L$mvapich_dir/$abi_libdir"
#    fi

    mvapich_saved_CC=$CC
    mvapich_saved_CPPFLAGS=$CPPFLAGS
    mvapich_saved_LDFLAGS=$LDFLAGS
    mvapich_saved_LIBS=$LIBS

    CC="$MVAPICH_CC"
    CPPFLAGS="$CPPFLAGS $MVAPICH_CPPFLAGS"
    LDFLAGS="$LDFLAGS $MVAPICH_LDFLAGS"
    LIBS="$MVAPICH_LIBS"

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
	#include <mpi.h>
	]], [[
	MPI_Initialized((int*)0);
	]])],

        if (test -f $mvapich_dir/$abi_libdir/libmpich.so); then
            MVAPICH_LDFLAGS="-L$mvapich_dir/$abi_libdir"
            found_mvapich=1
        fi

        if (test -f $mvapich_dir/$abi_libdir/shared/libmpich.so); then
            MVAPICH_LDFLAGS="-L$mvapich_dir/$abi_libdir/shared -L$mvapich_dir/$abi_libdir"
            found_mvapich=1
        fi 

	, )

    if test $found_mvapich -eq 0; then
       AC_MSG_CHECKING([for MVAPICH library (alt locations) and headers])

       # Put -shlib into MVAPICH_CC, since it is needed when building the
       # tests, where $MVAPICH_CC is used, and is not needed when building
       # the MPI-related plugins, where $MVAPICH_CC is not used.
       MVAPICH_CC="$mvapich_dir/bin/mpicc -shlib"
       MVAPICH_CPPFLAGS="-I$mvapich_dir/include"
       MVAPICH_LDFLAGS="-L$mvapich_dir/$alt_abi_libdir"
       MVAPICH_LIBS="-lmpich -libverbs -libumad -libcommon"
       MVAPICH_HEADER="$mvapich_dir/include/mpi.h"
       MVAPICH_DIR="$mvapich_dir"

       CC="$MVAPICH_CC"
       CPPFLAGS="$CPPFLAGS $MVAPICH_CPPFLAGS"
       LDFLAGS="$LDFLAGS $MVAPICH_LDFLAGS"
       LIBS="$MVAPICH_LIBS"

       AC_LINK_IFELSE([AC_LANG_PROGRAM([[
   	  #include <mpi.h>
	  ]], [[
	  MPI_Initialized((int*)0);
	  ]])],

         if (test -f $mvapich_dir/$alt_abi_libdir/libmpich.so); then
             MVAPICH_LDFLAGS="-L$mvapich_dir/$alt_abi_libdir"
             found_mvapich=1
         fi

         if (test -f $mvapich_dir/$alt_abi_libdir/shared/libmpich.so); then
             MVAPICH_LDFLAGS="-L$mvapich_dir/$alt_abi_libdir/shared -L$mvapich_dir/$alt_abi_libdir"
             found_mvapich=1
         fi 

	, )

    fi

    CC=$mvapich_saved_CC
    CPPFLAGS=$mvapich_saved_CPPFLAGS
    LDFLAGS=$mvapich_saved_LDFLAGS
    LIBS=$mvapich_saved_LIBS

    if test $found_mvapich -eq 1; then
	AC_MSG_RESULT(yes)
	AM_CONDITIONAL(HAVE_MVAPICH, true)
	AC_DEFINE(HAVE_MVAPICH, 1, [Define to 1 if you have MPICH.])	
    else
	AC_MSG_RESULT(no)
	AM_CONDITIONAL(HAVE_MVAPICH, false)
	MVAPICH_CC=""
	MVAPICH_CPPFLAGS=""
	MVAPICH_LDFLAGS=""
	MVAPICH_LIBS=""
	MVAPICH_HEADER=""
	MVAPICH_DIR=""
    fi

    AC_SUBST(MVAPICH_CC)
    AC_SUBST(MVAPICH_CPPFLAGS)
    AC_SUBST(MVAPICH_LDFLAGS)
    AC_SUBST(MVAPICH_LIBS)
    AC_SUBST(MVAPICH_HEADER)
    AC_SUBST(MVAPICH_DIR)

])




################################################################################
# Check for MVAPICH2 (http://mvapich.cse.ohio-state.edu/overview/mvapich2/)
################################################################################

AC_DEFUN([AX_MVAPICH2], [

    AC_ARG_WITH(mvapich2,
		AC_HELP_STRING([--with-mvapich2=DIR],
			       [MVAPICH2 installation @<:@/opt/mvapich2@:>@]),
		mvapich2_dir=$withval, mvapich2_dir="/opt/mvapich2")

   AC_ARG_WITH(mvapich2-ofed,
	AC_HELP_STRING([--with-mvapich2-ofed=DIR],
		       [MVAPICH2 open fabrics installation dir @<:@/usr@:>@]),
		mvapich2_ofed_dir=$withval, mvapich2_ofed_dir="/usr")

    AC_MSG_CHECKING([for MVAPICH2 library and headers])

    found_mvapich2=0

    # Put -shlib into MVAPICH2_CC, since it is needed when building the
    # tests, where $MVAPICH2_CC is used, and is not needed when building
    # the MPI-related plugins, where $MVAPICH2_CC is not used.
    MVAPICH2_CC="$mvapich2_dir/bin/mpicc -shlib"
    MVAPICH2_CPPFLAGS="-I$mvapich2_dir/include"
    MVAPICH2_LDFLAGS="-L$mvapich2_dir/$abi_libdir -L$mvapich2_ofed_dir/$abi_libdir"
    MVAPICH2_LIBS="-lmpich -libverbs -libcommon"
    MVAPICH2_HEADER="$mvapich2_dir/include/mpi.h"
    MVAPICH2_DIR="$mvapich2_dir"
    MVAPICH2_NULL=""

    # On the systems "mcr" and "thunder" at LLNL they have an MPICH variant
    # that has things moved around a bit. Handle this by allowing a "llnl"
    # pseudo-driver that makes the necessary configuration changes.
#    if test x"$mvapich2_driver" == x"llnl"; then
#	MVAPICH2_CC="$mvapich2_dir/bin/mpicc -shlib"
#        MVAPICH2_LDFLAGS="-L$mvapich2_dir/$abi_libdir"
#    fi

    mvapich2_saved_CC=$CC
    mvapich2_saved_CPPFLAGS=$CPPFLAGS
    mvapich2_saved_LDFLAGS=$LDFLAGS
    mvapich2_saved_LIBS=$LIBS

    CC="$MVAPICH2_CC"
    CPPFLAGS="$CPPFLAGS $MVAPICH2_CPPFLAGS"
    LDFLAGS="$LDFLAGS $MVAPICH2_LDFLAGS"
    LIBS="$MVAPICH2_LIBS"

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
	#include <mpi.h>
	]], [[
	MPI_Initialized((int*)0);
	]])],

         if (test -f $mvapich2_dir/$abi_libdir/libmpich.so); then
             found_mvapich2=1
             MVAPICH2_LDFLAGS="-L$mvapich2_dir/$abi_libdir -L$mvapich2_ofed_dir/$abi_libdir"
         fi
        
         if (test -f $mvapich2_dir/$abi_libdir/shared/libmpich.so); then
	    found_mvapich2=1
            MVAPICH2_LDFLAGS="-L$mvapich2_dir/$abi_libdir/shared -L$mvapich2_ofed_dir/$abi_libdir"
        fi 

	, )

    if test $found_mvapich2 -eq 0; then

       AC_MSG_CHECKING([for MVAPICH2 library (alt locations) and headers])

       # Put -shlib into MVAPICH2_CC, since it is needed when building the
       # tests, where $MVAPICH2_CC is used, and is not needed when building
       # the MPI-related plugins, where $MVAPICH2_CC is not used.
       MVAPICH2_CC="$mvapich2_dir/bin/mpicc -shlib"
       MVAPICH2_CPPFLAGS="-I$mvapich2_dir/include"
       MVAPICH2_LDFLAGS="-L$mvapich2_dir/$alt_abi_libdir -L$mvapich2_ofed_dir/$alt_abi_libdir"
       MVAPICH2_LIBS="-lmpich -libverbs -libcommon"
       MVAPICH2_HEADER="$mvapich2_dir/include/mpi.h"
       MVAPICH2_DIR="$mvapich2_dir"
       MVAPICH2_NULL=""

       CC="$MVAPICH2_CC"
       CPPFLAGS="$CPPFLAGS $MVAPICH2_CPPFLAGS"
       LDFLAGS="$LDFLAGS $MVAPICH2_LDFLAGS"
       LIBS="$MVAPICH2_LIBS"

       AC_LINK_IFELSE([AC_LANG_PROGRAM([[
   	  #include <mpi.h>
	  ]], [[
	  MPI_Initialized((int*)0);
	  ]])],

          if (test -f $mvapich2_dir/$alt_abi_libdir/libmpich.so); then
	    found_mvapich2=1
            MVAPICH2_LDFLAGS="-L$mvapich2_dir/$alt_abi_libdir -L$mvapich2_ofed_dir/$alt_abi_libdir"
          fi

          if (test -f $mvapich2_dir/$alt_abi_libdir/shared/libmpich.so); then
	    found_mvapich2=1
            MVAPICH2_LDFLAGS="-L$mvapich2_dir/$alt_abi_libdir/shared -L$mvapich2_ofed_dir/$alt_abi_libdir"
          fi 

	, )

    fi

    CC=$mvapich2_saved_CC
    CPPFLAGS=$mvapich2_saved_CPPFLAGS
    LDFLAGS=$mvapich2_saved_LDFLAGS
    LIBS=$mvapich2_saved_LIBS

    if test $found_mvapich2 -eq 0; then

      AC_MSG_CHECKING([for MVAPICH2 library and headers])

      # Put -shlib into MVAPICH2_CC, since it is needed when building the
      # tests, where $MVAPICH2_CC is used, and is not needed when building
      # the MPI-related plugins, where $MVAPICH2_CC is not used.
      MVAPICH2_CC="$mvapich2_dir/bin/mpicc -shlib"
      MVAPICH2_CPPFLAGS="-I$mvapich2_dir/include"
      MVAPICH2_LDFLAGS="-L$mvapich2_dir/$abi_libdir -L$mvapich2_ofed_dir/$abi_libdir"
      MVAPICH2_LIBS="-lmpich -libverbs"
      MVAPICH2_HEADER="$mvapich2_dir/include/mpi.h"
      MVAPICH2_DIR="$mvapich2_dir"
      MVAPICH2_NULL=""

      # On the systems "mcr" and "thunder" at LLNL they have an MPICH variant
      # that has things moved around a bit. Handle this by allowing a "llnl"
      # pseudo-driver that makes the necessary configuration changes.
#      if test x"$mvapich2_driver" == x"llnl"; then
#  	   MVAPICH2_CC="$mvapich2_dir/bin/mpicc -shlib"
#          MVAPICH2_LDFLAGS="-L$mvapich2_dir/$abi_libdir"
#      fi

      mvapich2_saved_CC=$CC
      mvapich2_saved_CPPFLAGS=$CPPFLAGS
      mvapich2_saved_LDFLAGS=$LDFLAGS

      CC="$MVAPICH2_CC"
      CPPFLAGS="$CPPFLAGS $MVAPICH2_CPPFLAGS"
      LDFLAGS="$LDFLAGS $MVAPICH2_LDFLAGS"
      LIBS="$MVAPICH2_LIBS"

      AC_LINK_IFELSE([AC_LANG_PROGRAM([[
	#include <mpi.h>
	]], [[
	MPI_Initialized((int*)0);
	]])],

         if (test -f $mvapich2_dir/$abi_libdir/libmpich.so); then
             found_mvapich2=1
             MVAPICH2_LDFLAGS="-L$mvapich2_dir/$abi_libdir -L$mvapich2_ofed_dir/$abi_libdir"
         fi
        
         if (test -f $mvapich2_dir/$abi_libdir/shared/libmpich.so); then
	    found_mvapich2=1
            MVAPICH2_LDFLAGS="-L$mvapich2_dir/$abi_libdir/shared -L$mvapich2_ofed_dir/$abi_libdir"
        fi 

	, )
    fi

    if test $found_mvapich2 -eq 0; then

       AC_MSG_CHECKING([for MVAPICH2 library (alt locations), no libcommon  and headers])

       # Put -shlib into MVAPICH2_CC, since it is needed when building the
       # tests, where $MVAPICH2_CC is used, and is not needed when building
       # the MPI-related plugins, where $MVAPICH2_CC is not used.
       MVAPICH2_CC="$mvapich2_dir/bin/mpicc -shlib"
       MVAPICH2_CPPFLAGS="-I$mvapich2_dir/include"
       MVAPICH2_LDFLAGS="-L$mvapich2_dir/$alt_abi_libdir -L$mvapich2_ofed_dir/$alt_abi_libdir"
       MVAPICH2_LIBS="-lmpich -libverbs" 
       MVAPICH2_HEADER="$mvapich2_dir/include/mpi.h"
       MVAPICH2_DIR="$mvapich2_dir"
       MVAPICH2_NULL=""

       CC="$MVAPICH2_CC"
       CPPFLAGS="$CPPFLAGS $MVAPICH2_CPPFLAGS"
       LDFLAGS="$LDFLAGS $MVAPICH2_LDFLAGS"
       LIBS="$MVAPICH2_LIBS"

       AC_LINK_IFELSE([AC_LANG_PROGRAM([[
   	  #include <mpi.h>
	  ]], [[
	  MPI_Initialized((int*)0);
	  ]])],

          if (test -f $mvapich2_dir/$alt_abi_libdir/libmpich.so); then
	    found_mvapich2=1
            MVAPICH2_LDFLAGS="-L$mvapich2_dir/$alt_abi_libdir -L$mvapich2_ofed_dir/$alt_abi_libdir"
          fi

          if (test -f $mvapich2_dir/$alt_abi_libdir/shared/libmpich.so); then
	    found_mvapich2=1
            MVAPICH2_LDFLAGS="-L$mvapich2_dir/$alt_abi_libdir/shared -L$mvapich2_ofed_dir/$alt_abi_libdir"
          fi 

	, )

    fi

    CC=$mvapich2_saved_CC
    CPPFLAGS=$mvapich2_saved_CPPFLAGS
    LDFLAGS=$mvapich2_saved_LDFLAGS
    LIBS=$mvapich2_saved_LIBS

    if test $found_mvapich2 -eq 1; then
	AC_MSG_RESULT(yes)
	AM_CONDITIONAL(HAVE_MVAPICH2, true)
	AC_DEFINE(HAVE_MVAPICH2, 1, [Define to 1 if you have MVAPICH2.])	
    else
	AC_MSG_RESULT(no)
	AM_CONDITIONAL(HAVE_MVAPICH2, false)
	MVAPICH2_CC=""
	MVAPICH2_CPPFLAGS=""
	MVAPICH2_LDFLAGS=""
	MVAPICH2_LIBS=""
	MVAPICH2_HEADER=""
	MVAPICH2_DIR=""
        MVAPICH2_NULL=""
    fi

    AC_SUBST(MVAPICH2_CC)
    AC_SUBST(MVAPICH2_CPPFLAGS)
    AC_SUBST(MVAPICH2_LDFLAGS)
    AC_SUBST(MVAPICH2_LIBS)
    AC_SUBST(MVAPICH2_HEADER)
    AC_SUBST(MVAPICH2_DIR)
    AC_SUBST(MVAPICH2_NULL)

])


