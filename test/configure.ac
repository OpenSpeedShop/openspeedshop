################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2014 Krell Institute All Rights Reserved.
# Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
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
################################################################################

AC_INIT([OpenSpeedShop],[2.1_u2])
AC_COPYRIGHT([[Copyright (c) 2006-2013 Krell Institute. All Rights Reserved.]])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([libltdl/config])
AM_INIT_AUTOMAKE

################################################################################
# Setup Configuration Header File
################################################################################

AC_CONFIG_HEADERS([config.h])

AH_TOP([
/*******************************************************************************
** Copyright (c) 2006-2013 Krell Institute. All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/
])

################################################################################
# Check for Standard Build Tools
################################################################################

AC_USE_SYSTEM_EXTENSIONS
AC_GNU_SOURCE
AC_PROG_CC
AC_PROG_CXX
AC_PROG_FC
AC_PROG_F77

if test "$ac_cv_f77_compiler_gnu" = no; then
        AM_CONDITIONAL(HAVE_F77, false)
        AC_DEFINE(HAVE_F77, 0, [Define to 0 if you do not have F77.])
else
        AM_CONDITIONAL(HAVE_F77, true)
        AC_DEFINE(HAVE_F77, 1, [Define to 1 if you have F77.])
fi

AC_SUBST([build])
AC_SUBST([build_cpu])
AC_SUBST([build_os])
AC_SUBST([host])
AC_SUBST([host_cpu])
AC_SUBST([host_os])
AC_SUBST([is_cross_compile])
AC_SUBST([prefix])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AC_DISABLE_STATIC

# Name the subdirectory that contains libltdl sources
LT_CONFIG_LTDL_DIR([libltdl])
     
AC_CONFIG_MACRO_DIR([libltdl/m4])

# Configure libtool with dlopen support if possible
LT_INIT([dlopen])
     
# Enable building of the installable libltdl library
LTDL_INIT([installable])

LT_INIT

AC_PATH_PROG([DOXYGEN], doxygen)
AC_PROG_INSTALL
AC_PROG_LEX
AC_PATH_PROG([RPCGEN], rpcgen)
AC_PROG_YACC
#  Is ln available
AC_PROG_LN_S

################################################################################
# Handle PowerPC and X86-64 Special Cases
################################################################################
AC_ARG_WITH(ppc64_bitmode,
	    AS_HELP_STRING([--with-ppc64-bitmode=<32,64>],[Specify ppc64 library bit mode @<:@32@:>@]),
	    ppc64_bitmode=$withval, ppc64_bitmode="32")

AC_MSG_CHECKING([Configuring for host = $host  with ppc64_bitmode = $ppc64_bitmode])

case "$host" in
    powerpc64-*-*) 

	AC_MSG_CHECKING([Configuring for powerpc64-star-linux with ppc64_bitmode = $ppc64_bitmode])
	if test "$ppc64_bitmode" = "64" ; then
		AC_MSG_CHECKING([Configuring for 64 bit powerpc64-star-linux host])
		abi_libdir="lib64"
		alt_abi_libdir="lib"
		if test x"$libdir" == x'${exec_prefix}/lib'; then
		    libdir='${exec_prefix}/lib64'
		fi
		LDFLAGS="-DLIB_DIR=lib64 $LDFLAGS"
		CFLAGS="-m64 $CFLAGS"
		CXXFLAGS="-m64 $CXXFLAGS"
		CPPFLAGS="-m64 $CPPFLAGS"
	elif test "$ppc64_bitmode" = "32" ; then
		AC_MSG_CHECKING([Configuring for 32 bit powerpc64-star-linux host])
		abi_libdir="lib"
		alt_abi_libdir="lib64"
		if test x"$libdir" == x'${exec_prefix}/lib64'; then
		    libdir='${exec_prefix}/lib'
		fi
		LDFLAGS="-DLIB_DIR=lib $LDFLAGS"
	fi
	;;

    powerpc-*-linux*) 
        AC_MSG_CHECKING([Configuring for powerpc-star-linux])

        abi_libdir="lib"
        alt_abi_libdir="lib64"
        if test x"$libdir" == x'${exec_prefix}/lib64'; then
            libdir='${exec_prefix}/lib'
        fi
        LDFLAGS="-DLIB_DIR=lib $LDFLAGS"
        ;;

    x86_64-*-linux*)
        AC_MSG_CHECKING([Configuring for x86_64--star-linux host])
        if test -d /usr/lib/x86_64-linux-gnu ; then
          if test x"$libdir" == x'${exec_prefix}/lib64'; then
            libdir='${exec_prefix}/lib'
          fi
          abi_libdir="lib"
          alt_abi_libdir="lib64"
          LDFLAGS="-DLIB_DIR=lib $LDFLAGS"
        else
          if test x"$libdir" == x'${exec_prefix}/lib'; then
            libdir='${exec_prefix}/lib64'
          fi
          abi_libdir="lib64"
          alt_abi_libdir="lib"
          LDFLAGS="-DLIB_DIR=lib64 $LDFLAGS"
        fi
        ;;

    *)
        AC_MSG_CHECKING([Configuring for default host])
        abi_libdir="lib"
        alt_abi_libdir="lib64"
        LDFLAGS="-DLIB_DIR=lib $LDFLAGS"
        ;;
esac
topdir_execprefix=${exec_prefix}
AC_SUBST(topdir_execprefix)

abi_libdir_name=${abi_libdir}
AC_SUBST(abi_libdir_name)
alt_abi_libdir_name=${alt_abi_libdir}
AC_SUBST(alt_abi_libdir_name)

oss_platform=`uname -i`
oss_hardware_platform=$oss_platform
AC_MSG_CHECKING([Checking for oss_hardware_platform=$oss_hardware_platform])
AC_SUBST(oss_hardware_platform)

AC_MSG_CHECKING([Checking whether to enable debug compilation])
AC_ARG_ENABLE(debug, [  --enable-debug Turn on debug compilation], [enable_debug=yes], [enable_debug=no])
if test x$enable_debug = xyes; then
  CPPFLAGS="${CPPFLAGS} -g"
  CFLAGS="${CFLAGS} -g"
  CXXFLAGS="${CXXFLAGS} -g"
fi
AC_MSG_RESULT([$enable_debug])


################################################################################
# Check for POSIX Threads (ISO/IEC 9945-1:1996)
################################################################################

AC_CHECK_HEADER(pthread.h, [ AC_CHECK_LIB(pthread, pthread_create, [
    AC_DEFINE(HAVE_POSIX_THREADS, 1, [Define to 1 if you have POSIX threads.])
], [
    AC_MSG_FAILURE([cannot locate POSIX thread library and/or headers.])
]) ])

################################################################################
# Check for symbol resolution option, default to bfd
################################################################################

AC_ARG_ENABLE(resolve-symbols,
[  --enable-resolve-symbols=mechanism - Control the mechanism used to resolve symbols in Open|SpeedShop
       bfd       - Use the binutils bfd interface
       symtabapi - Use the Dyninst symtabAPI interface (default)],,enable_resolve_symbols=symtabapi)

AC_MSG_CHECKING([Checking for enable_resolve_symbols=$enable_resolve_symbols])

case "$enable_resolve_symbols" in
    bfd)
    AM_CONDITIONAL(OPENSS_USE_SYMTABAPI, false)
    ;;

    symtabapi)
    AM_CONDITIONAL(OPENSS_USE_SYMTABAPI, true)
    AC_DEFINE(OPENSS_USE_SYMTABAPI, 1, [Define to 1 if you want to use bfd to resolve symbols.])
    ;;

    *)
    AC_MSG_ERROR([invalid symbolresolution (resolve-symbols) specification "$RESSYM".])
    ;;
esac 

################################################################################
# Check for TLS option, default to implicit
################################################################################

AC_ARG_WITH(tls,
	    AS_HELP_STRING([--with-tls=TYPE],[tls (implicit, explicit) @<:@implicit@:>@]),
	    TLS=$withval, TLS="implicit")

case "$TLS" in
    implicit)
	AM_CONDITIONAL(USE_EXPLICIT_TLS, false)
	;; 

    explicit)
	AM_CONDITIONAL(USE_EXPLICIT_TLS, true)
	AC_DEFINE(USE_EXPLICIT_TLS, 1, [Define to 1 if you want to use an explicit TLS implementation instead of the __thread keyword.])
	;;

    *)
	AC_MSG_ERROR([invalid tls specification "$TLS".])
	;;
esac

################################################################################
# Check for a --with-runtime-dir option
################################################################################

AC_DEFUN([AX_RUNTIME_DIR], [

    AC_ARG_WITH(runtime-dir,
                AC_HELP_STRING([--with-runtime-dir=DIR],
                               [Compute node runtime environment installation directory path@<:@/nodefault@:>@]),
                runtime_dir=$withval, runtime_dir="/nodefault")

    AC_MSG_CHECKING([for compute OpenSpeedShop runtime environment library])
    RUNTIME_DIR=""
    RUNTIME_DIR_LDFLAGS=""

    if (test -e $runtime_dir/$abi_libdir) ; then
       RUNTIME_DIR="$runtime_dir"
       AC_DEFINE(HAVE_RUNTIME_DIR, 1, [Define to 1 if you to indicate a runtime directory exists  .])
       AM_CONDITIONAL(HAVE_RUNTIME_DIR, true)
       RUNTIME_DIR_LDFLAGS="-L$runtime_dir/$abi_libdir"
       AC_MSG_RESULT(yes)
    elif (test -e $runtime_dir/$alt_abi_libdir) ; then
       RUNTIME_DIR="$runtime_dir"
       AC_DEFINE(HAVE_RUNTIME_DIR, 1, [Define to 1 if you to indicate a runtime directory exists  .])
       AM_CONDITIONAL(HAVE_RUNTIME_DIR, true)
       RUNTIME_DIR_LDFLAGS="-L$runtime_dir/$alt_abi_libdir"
       AC_MSG_RESULT(yes)
    else
       AM_CONDITIONAL(HAVE_RUNTIME_DIR, false)
       AC_MSG_RESULT(no)
    fi

    AC_SUBST(RUNTIME_DIR)
    AC_SUBST(RUNTIME_DIR_LDFLAGS)

])

################################################################################
# Target O/S Specification
################################################################################

autoconf_for_target=0

target_os=$host
AC_MSG_CHECKING(for Target O/S)
AC_ARG_WITH(target-os,
    AS_HELP_STRING([--with-target-os=TARGET_OS],[target os specification @<:@$host@:>@]),
                    TARGET_OS=$withval, TARGET_OS="$host")

AM_CONDITIONAL(BUILD_TARGETED_BGL, false)
AM_CONDITIONAL(BUILD_TARGETED_BGP, false)
AM_CONDITIONAL(BUILD_TARGETED_BGQ, false)
AM_CONDITIONAL(BUILD_TARGETED, false)
AM_CONDITIONAL(FE_SETUP_BUILD_TARGETED, false)

case "$TARGET_OS" in
    cray-xk)
        AM_CONDITIONAL(FE_SETUP_BUILD_TARGETED, false)
	AM_CONDITIONAL(BUILD_TARGETED, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGL, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGP, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGQ, false)
	AC_DEFINE(BUILD_TARGETED, 0, [Define to 0 if not building a targeted runtime.])
	AC_DEFINE(TARGET_OS_CRAYXK, 1, [Do not define if not building for targeted cray-xk.])
	AC_DEFINE_UNQUOTED(TARGET_OS,cray-xk,
			   [Name targeted OS])
        target_os=cray-xk
	autoconf_for_target=0
	;; 
    cray-xe)
        AM_CONDITIONAL(FE_SETUP_BUILD_TARGETED, false)
	AM_CONDITIONAL(BUILD_TARGETED, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGL, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGP, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGQ, false)
	AC_DEFINE(BUILD_TARGETED, 0, [Do not Define if not building a targeted runtime.])
	AC_DEFINE(TARGET_OS_CRAYXE, 1, [Do not define if not building for targeted cray-xe.])
	AC_DEFINE_UNQUOTED(TARGET_OS,cray-xe,
			   [Name targeted OS])
        target_os=cray-xe
	autoconf_for_target=0
	;; 
    cray-xt5)
        AM_CONDITIONAL(FE_SETUP_BUILD_TARGETED, false)
	AM_CONDITIONAL(BUILD_TARGETED, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGL, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGP, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGQ, false)
	AC_DEFINE(BUILD_TARGETED, 0, [Do not Define if not building a targeted runtime.])
	AC_DEFINE(TARGET_OS_CRAYXT5, 1, [Do not define if not building for targeted cray-xt5.])
	AC_DEFINE_UNQUOTED(TARGET_OS,cray-xt5,
			   [Name targeted OS])
        target_os=cray-xt5
	autoconf_for_target=0
	;; 
    cray-xt4)
        AM_CONDITIONAL(FE_SETUP_BUILD_TARGETED, false)
	AM_CONDITIONAL(BUILD_TARGETED, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGL, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGP, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGQ, false)
	AC_DEFINE(BUILD_TARGETED, 0, [Do not Define if not building a targeted runtime.])
	AC_DEFINE(TARGET_OS_CRAYXT4, 1, [Do not define if not building for targeted cray-xt4.])
	AC_DEFINE_UNQUOTED(TARGET_OS,cray-xt4,
			   [Name targeted OS])
        target_os=cray-xt4
	autoconf_for_target=0
	;; 
    bgl)
        AM_CONDITIONAL(FE_SETUP_BUILD_TARGETED, false)
	AM_CONDITIONAL(BUILD_TARGETED, true)
	AM_CONDITIONAL(BUILD_TARGETED_BGL, true)
	AM_CONDITIONAL(BUILD_TARGETED_BGP, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGQ, false)
	AC_DEFINE(BUILD_TARGETED, 1, [Define to 0 if not building a targeted runtime.])
	AC_DEFINE(BUILD_TARGETED_BGL, 1, [Define to 0 if not building a targeted runtime.])
	AC_DEFINE(TARGET_OS_BGL, 1, [Do not define if not building for targeted bgl.])
	AC_DEFINE_UNQUOTED(TARGET_OS,bgl,
			   [Name targeted OS])
        target_os=bgl
	autoconf_for_target=1
	;; 
    bgp)
        AM_CONDITIONAL(FE_SETUP_BUILD_TARGETED, false)
	AM_CONDITIONAL(BUILD_TARGETED, true)
	AM_CONDITIONAL(BUILD_TARGETED_BGL, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGP, true)
	AM_CONDITIONAL(BUILD_TARGETED_BGQ, false)
	AC_DEFINE(BUILD_TARGETED, 1, [Define to 0 if not building a targeted runtime.])
	AC_DEFINE(BUILD_TARGETED_BGP, 1, [Define to 0 if not building a targeted runtime.])
	AC_DEFINE(TARGET_OS_BGP, 1, [Do not define if not building for targeted bgp.])
	AC_DEFINE_UNQUOTED(TARGET_OS,bgp,
			   [Name targeted OS])
        target_os=bgp
	autoconf_for_target=1
	;; 
    bgqfe)
        AM_CONDITIONAL(FE_SETUP_BUILD_TARGETED, true)
        AM_CONDITIONAL(BUILD_TARGETED, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGL, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGP, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGQ, false)
        AC_DEFINE(FE_SETUP_BUILD_TARGETED, 1, [Define to 1 if setting up the front end for targeted runtime.])
        AC_DEFINE_UNQUOTED(TARGET_OS,bgq,
                           [Name targeted OS])
        target_os=bgq
	TARGET_OS=bgq
	autoconf_for_target=0
        ;;

    bgq)
        AM_CONDITIONAL(FE_SETUP_BUILD_TARGETED, false)
        AM_CONDITIONAL(BUILD_TARGETED, true)
	AM_CONDITIONAL(BUILD_TARGETED_BGL, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGP, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGQ, true)
        AC_DEFINE(BUILD_TARGETED, 1, [Define to 0 if not building a targeted runtime.])
	AC_DEFINE(BUILD_TARGETED_BGQ, 1, [Define to 0 if not building a targeted runtime.])
        AC_DEFINE(TARGET_OS_BGQ, 1, [Do not define if not building for targeted bgq.])
        AC_DEFINE_UNQUOTED(TARGET_OS,bgq,
                           [Name targeted OS])
        target_os=bgq
	autoconf_for_target=1
        ;;
    *)
        AM_CONDITIONAL(FE_SETUP_BUILD_TARGETED, false)
	AM_CONDITIONAL(BUILD_TARGETED, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGL, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGP, false)
	AM_CONDITIONAL(BUILD_TARGETED_BGQ, false)
	AC_DEFINE(BUILD_TARGETED, 0, [Define to 1 if building a targeted runtime.])
	AC_DEFINE_UNQUOTED(TARGET_OS,$host,
			   [Name targeted OS])
	autoconf_for_target=0
	;; 
esac

AC_SUBST(FE_SETUP_BUILD_TARGETED)
AC_SUBST(BUILD_TARGETED_BGL)
AC_SUBST(BUILD_TARGETED_BGP)
AC_SUBST(BUILD_TARGETED_BGQ)
AC_SUBST(BUILD_TARGETED)
AC_SUBST(TARGET_OS)

################################################################################
# Target PTHREADS Availability Specification
################################################################################
#
# Option: --enable-target-pthreads=yes
# Requires libpthread.
#
AC_MSG_CHECKING(for target pthread support specification)
AC_ARG_ENABLE([target-pthreads],
    [AS_HELP_STRING([--enable-target-pthreads],
        [Enable support on target architecture for pthreads [default=yes]])],
    [], [enable_target_pthreads=yes])

AM_CONDITIONAL([HAVE_TARGET_TEST_POSIX_THREADS],
    [test x$enable_target_pthreads = xyes])

if test "x$enable_target_pthreads" = xyes ; then
    AC_MSG_RESULT(yes)
    AC_DEFINE([HAVE_TARGET_POSIX_THREADS], [1],
        [Enable support on the target architecture for pthreads.])
else
    AC_MSG_RESULT(no)
fi

AC_SUBST(HAVE_TARGET_POSIX_THREADS)

################################################################################
# Target DLOPEN Availability Specification
################################################################################
#
# Option: --enable-target_dlopen=yes
#
AC_MSG_CHECKING(for target dlopen support specification)
AC_ARG_ENABLE([target_dlopen],
    [AS_HELP_STRING([--enable-target_dlopen],
        [include support on the target architecture for dlopen [default=yes]])],
    [], [enable_target_dlopen=yes])

if test "x$enable_target_dlopen" = xyes ; then
    AC_DEFINE([HAVE_TARGET_DLOPEN], [1],
        [Enable support on the target architecture for dlopen.])
    AC_MSG_RESULT(yes)
else
    AC_MSG_RESULT(no)
fi

AC_SUBST(HAVE_TARGET_DLOPEN)


################################################################################
# Target SIGNALS Availability Specification
################################################################################
#
# Option: --enable-target-signals=yes
#
AC_MSG_CHECKING(for target signal support specification)
AC_ARG_ENABLE([target-signals],
    [AS_HELP_STRING([--enable-target-signals],
        [Enable support on target architecture for signals [default=yes]])],
    [],
    [enable_target_signals=yes])

if test "x$enable_target_signals" = xyes ; then
    AC_MSG_RESULT(yes)
    AC_DEFINE([HAVE_TARGET_SIGNALS], [1],
        [Enable support on target architecture for signals.])
else
    AC_MSG_RESULT(no)
fi

AC_SUBST(HAVE_TARGET_SIGNALS)

################################################################################
# Target FORK Availability Specification
################################################################################
#
# Option: --enable-target-fork=yes
# Requires fork and exec families.
#
AC_MSG_CHECKING(for target fork support specification)
AC_ARG_ENABLE([target-fork],
    [AS_HELP_STRING([--enable-target-fork],
        [Enable support on target architecture for fork and exec families [default=yes]])],
    [], [enable_target_fork=yes])

if test "x$enable_target_fork" = xyes ; then
    AC_DEFINE([HAVE_TARGET_FORK], [1],
        [Enable support on target architecture for fork and exec families.])
    AC_MSG_RESULT(yes)
else
    AC_MSG_RESULT(no)
fi

AC_SUBST(HAVE_TARGET_FORK)

################################################################################
# Target SHARED LIBRAY BUILD Specification
################################################################################
#
# Option: --enable-target-shared=yes
#
AC_MSG_CHECKING(for target shared libraries build specification)
AC_ARG_ENABLE([target-shared],
    [AS_HELP_STRING([--enable-target-shared=@<:@yes/no@:>@],
        [Enable support on target architecture for building shared libraries @<:@default=no@:>@])],
    [],
    [enable_target_shared=no])

if test "x$enable_target_shared" = xyes ; then
    AC_MSG_RESULT(yes)
    AM_CONDITIONAL(HAVE_TARGET_SHARED, true)
    AC_DEFINE([HAVE_TARGET_SHARED], [1],
        [Enable support on target architecture for shared libraries.])
else
    AC_MSG_RESULT(no)
    AM_CONDITIONAL(HAVE_TARGET_SHARED, false)
fi

AC_SUBST(HAVE_TARGET_SHARED)

################################################################################
# Check for Additional Packages
################################################################################


AX_EPYDOC()
AX_ARRAYSVCS()
AX_BINUTILS()
AX_LIBELF()
AX_LIBDWARF()
AX_LIBMONITOR()
AX_LIBUNWIND()
AX_PKG_MPI()
AX_OPENMP()
AX_OTF()
AX_PAPI()
AX_PYTHON_DEVEL([>= '2.3'])
AX_QTLIB()
AX_SQLITE()
AX_VT()

AX_BOOST_BASE([1.35.0])
AX_BOOST_DATE_TIME()
AX_BOOST_FILESYSTEM()
AX_BOOST_PROGRAM_OPTIONS()
AX_BOOST_SYSTEM()
AX_BOOST_THREAD()

if test "$autoconf_for_target" = 1 ; then

AX_TARGET_OPENMP()
AX_TARGET_STDC_PLUSPLUS()
AX_TARGET_BINUTILS()
AX_TARGET_PERSONALITY()
AX_TARGET_LIBDWARF()
AX_TARGET_LIBMONITOR()
AC_PKG_TARGET_LIBPTHREAD()
AC_PKG_TARGET_LIBRT()
AX_TARGET_LIBUNWIND()
AX_TARGET_OTF()
AX_TARGET_PAPI()
AX_TARGET_SQLITE()
AX_TARGET_SYMTABAPI()
AX_TARGET_VT()

else

AM_CONDITIONAL(HAVE_TARGET_OPENMP, false)
AM_CONDITIONAL(HAVE_TARGET_BINUTILS, false)
AM_CONDITIONAL(HAVE_TARGET_PERSONALITY, false)
AM_CONDITIONAL(HAVE_TARGET_LIBDWARF, false)
AM_CONDITIONAL(HAVE_TARGET_LIBMONITOR, false)
AM_CONDITIONAL(HAVE_TARGET_LIBPTHREAD, false)
AM_CONDITIONAL(HAVE_TARGET_LIBRT, false)
AM_CONDITIONAL(HAVE_TARGET_LIBUNWIND, false)
AM_CONDITIONAL(HAVE_TARGET_OTF, false)
AM_CONDITIONAL(HAVE_TARGET_PAPI, false)
AM_CONDITIONAL(HAVE_TARGET_PAPI_SHARED, false)
AM_CONDITIONAL(HAVE_TARGET_SQLITE, false)
AM_CONDITIONAL(HAVE_TARGET_SYMTABAPI, false)
AM_CONDITIONAL(HAVE_TARGET_VT, false)
AM_CONDITIONAL(HAVE_TARGET_STDC_PLUSPLUS, false)

fi


################################################################################
# Check for Instrumentor
################################################################################

AC_ARG_WITH(instrumentor,
	    AS_HELP_STRING([--with-instrumentor=TYPE],[instrumentor (cbtf, dyninst, mrnet, offline, none) @<:@offline@:>@]),
	    INSTRUMENTOR=$withval, INSTRUMENTOR="offline")

case "$INSTRUMENTOR" in
    none)
	AM_CONDITIONAL(HAVE_XERCES, false)
	AM_CONDITIONAL(HAVE_SYMTABAPI, false)
	AM_CONDITIONAL(BUILD_CBTF, false)
	AM_CONDITIONAL(BUILD_DPCL, false)
	AM_CONDITIONAL(BUILD_OFFLINE, false)
	AM_CONDITIONAL(BUILD_MRNET, false)
	AM_CONDITIONAL(HAVE_MRNET, false)
	AM_CONDITIONAL(HAVE_DYNINST, false)
	AM_CONDITIONAL(BUILD_INSTRUMENTOR, false)
	AC_DEFINE(BUILD_INSTRUMENTOR, 0, [Define to 1 if you do not want an instrumentor built.])
        AM_CONDITIONAL(HAVE_RUNTIME_DIR, false)
	;; 

    dyninst)
	AM_CONDITIONAL(HAVE_XERCES, false)
	AM_CONDITIONAL(HAVE_SYMTABAPI, false)
	AX_DYNINST()
	AM_CONDITIONAL(BUILD_CBTF, false)
	AM_CONDITIONAL(BUILD_DPCL, false)
	AM_CONDITIONAL(BUILD_MRNET, false)
	AM_CONDITIONAL(BUILD_OFFLINE, false)
	AM_CONDITIONAL(HAVE_MRNET, false)
	AM_CONDITIONAL(BUILD_INSTRUMENTOR, true)
	AC_DEFINE(BUILD_INSTRUMENTOR, 1, [Define to 1 if you do not want an instrumentor built.])
        AM_CONDITIONAL(HAVE_RUNTIME_DIR, false)
	;;

    mrnet)
	AX_DYNINST()
	AX_SYMTABAPI()
	AX_MRNET()
        #AX_XERCESC()
        # This helps with targeted builds when there is no xercesc
        #if test "$LIBXERCES_C" = "-lxerces-c" ; then
        #  AX_XERCESC_VERSION()
        #fi
	AM_CONDITIONAL(HAVE_XERCES, false)
	AM_CONDITIONAL(HAVE_DYNINST, true)
	AM_CONDITIONAL(BUILD_CBTF, false)
	AM_CONDITIONAL(BUILD_MRNET, true)
	AC_DEFINE(BUILD_MRNET, 1, [Define to 1 if you only want the mrnet instrumentor.])
	AM_CONDITIONAL(BUILD_DPCL, false)
	AM_CONDITIONAL(BUILD_OFFLINE, false)
	AM_CONDITIONAL(BUILD_INSTRUMENTOR, true)
	AC_DEFINE(BUILD_INSTRUMENTOR, 1, [Define to 1 if you do not want an instrumentor built.])
        AM_CONDITIONAL(HAVE_RUNTIME_DIR, false)
	;;

    offline)
	AX_SYMTABAPI()
        #AX_XERCESC()
        ## This helps with targeted builds when there is no xercesc
        #if test "$LIBXERCES_C" = "-lxerces-c" ; then
        #  AX_XERCESC_VERSION()
        #fi
	AM_CONDITIONAL(HAVE_XERCES, false)
	AM_CONDITIONAL(BUILD_CBTF, false)
	AM_CONDITIONAL(BUILD_DPCL, false)
	AM_CONDITIONAL(HAVE_DYNINST, false)
	AM_CONDITIONAL(BUILD_MRNET, false)
	AM_CONDITIONAL(HAVE_MRNET, false)
	AM_CONDITIONAL(BUILD_OFFLINE, true)
	AC_DEFINE(BUILD_OFFLINE, 1, [Define to 1 if you only want the offline instrumentor.])
	AM_CONDITIONAL(BUILD_INSTRUMENTOR, true)
	AC_DEFINE(BUILD_INSTRUMENTOR, 1, [Define to 1 if you do not want an instrumentor built.])
	AX_RUNTIME_DIR()
	;;

    cbtf)
	AX_DYNINST()
	AX_SYMTABAPI()
	AX_XERCESC()
	# This helps with targeted builds when there is no xercesc
	if test "$LIBXERCES_C" = "-lxerces-c" ; then
	   AX_XERCESC_VERSION()
	fi
	AX_CBTF()
	AX_CBTF_XML()
        AX_MRNET()
	AX_CBTF_MRNET()
	AX_CBTF_MESSAGES()
	AX_CBTF_SERVICES()

	AM_CONDITIONAL(BUILD_DPCL, false)
	AM_CONDITIONAL(HAVE_DYNINST, true)
	AM_CONDITIONAL(BUILD_MRNET, false)
	AM_CONDITIONAL(BUILD_OFFLINE, false)
	AM_CONDITIONAL(BUILD_CBTF, true)
	AC_DEFINE(BUILD_CBTF, 1, [Define to 1 if you only want the cbtf instrumentor.])
	AM_CONDITIONAL(BUILD_INSTRUMENTOR, true)
	AC_DEFINE(BUILD_INSTRUMENTOR, 1, [Define to 1 if you do not want an instrumentor built.])
        AM_CONDITIONAL(HAVE_RUNTIME_DIR, false)
	;;

    *)
	AC_MSG_ERROR([invalid instrumentor "$INSTRUMENTOR".])
	;;
esac

AC_SUBST(INSTRUMENTOR)
AC_SUBST(BUILD_INSTRUMENTOR)

################################################################################
# build runtime collector software only
################################################################################
#
# Option: 
#
AC_MSG_CHECKING(for building runtime collectors only)
AC_ARG_ENABLE([runtime-only],
    [AS_HELP_STRING([--enable-runtime-only],
        [Enable support building just the runtime collectors with no client [default=no]])],
    [], [enable_runtime_only=no])

if test "x$enable_runtime_only" = xyes ; then
    AM_CONDITIONAL(BUILD_RUNTIME_ONLY, true)
    AC_DEFINE(BUILD_RUNTIME_ONLY, 1, [Define to 1 if you do want only collector runtime support built.])
else
    AM_CONDITIONAL(BUILD_RUNTIME_ONLY, false)
    AC_DEFINE(BUILD_RUNTIME_ONLY, 0, [Define to 1 if you do not want only collector runtime support built.])
fi
AC_SUBST(BUILD_RUNTIME_ONLY)

################################################################################
# build gui/cli with no collectors but supports running experiments with
# targeted collectors (e.g. dynamic collectors on BGP BE nodes).
################################################################################
#
# Option: 
#
AC_MSG_CHECKING(for gui,cli build only)
AC_ARG_ENABLE([gui-only],
    [AS_HELP_STRING([--enable-gui-only],
        [Enable support building just the gui with no instrumentor [default=no]])],
    [], [enable_gui_only=no])

#AM_CONDITIONAL([BUILD_INSTRUMENTOR],
#    [test x$enable_gui_only = xyes])

if test "x$enable_gui_only" = xyes ; then
    AM_CONDITIONAL(BUILD_GUI_ONLY, true)
    AC_DEFINE(BUILD_GUI_ONLY, 1, [Define to 1 if you do want only gui/cli support built.])
else
    AM_CONDITIONAL(BUILD_GUI_ONLY, false)
    AC_DEFINE(BUILD_GUI_ONLY, 0, [Define to 1 if you do want only gui/cli support built.])
fi

AC_SUBST(BUILD_GUI_ONLY)



################################################################################
# Generate Output Files
################################################################################

AC_CONFIG_FILES(
	Makefile
        executables/Makefile
        executables/hybrid/Makefile
        executables/hybrid/lulesh/Makefile
        executables/hybrid/smg2000/Makefile
        executables/hybrid/smg2000/krylov/Makefile
        executables/hybrid/smg2000/struct_ls/Makefile
        executables/hybrid/smg2000/struct_mv/Makefile
        executables/hybrid/smg2000/utilities/Makefile
        executables/hybrid/smg2000/test/Makefile
        executables/sequential/Makefile
        executables/sequential/forever/Makefile
        executables/sequential/forever_cxx/Makefile
        executables/sequential/fpetest/Makefile
        executables/sequential/fpetest_attach/Makefile
        executables/sequential/fpe_print/Makefile
        executables/sequential/fredO2/Makefile
        executables/sequential/fred/Makefile
        executables/sequential/mutatee/Makefile
        executables/sequential/mutatee/c_version/Makefile
        executables/sequential/mutatee/cplus_version/Makefile
        executables/sequential/mutatee/f77_version/Makefile
        executables/sequential/mutatee/io_c_version/Makefile
        executables/sequential/Eon/Makefile
        executables/sequential/dlopen/Makefile
        executables/sequential/deep_callpath/Makefile
        executables/sequential/lulesh/Makefile
        executables/sequential/threads/Makefile
        executables/sequential/smg2000/Makefile
        executables/sequential/smg2000/krylov/Makefile
        executables/sequential/smg2000/struct_ls/Makefile
        executables/sequential/smg2000/struct_mv/Makefile
        executables/sequential/smg2000/utilities/Makefile
        executables/sequential/smg2000/test/Makefile
        executables/mpi/Makefile
	executables/mpi/helloworld/Makefile
	executables/mpi/lulesh/Makefile
        executables/mpi/smg2000/Makefile
        executables/mpi/smg2000/krylov/Makefile
        executables/mpi/smg2000/struct_ls/Makefile
        executables/mpi/smg2000/struct_mv/Makefile
        executables/mpi/smg2000/utilities/Makefile
        executables/mpi/smg2000/test/Makefile
        executables/mpi/nbody/Makefile
        executables/multi/Makefile
        executables/multi/stress_omp/Makefile
        executables/multi/smg2000/Makefile
        executables/multi/smg2000/krylov/Makefile
        executables/multi/smg2000/struct_ls/Makefile
        executables/multi/smg2000/struct_mv/Makefile
        executables/multi/smg2000/utilities/Makefile
        executables/multi/smg2000/test/Makefile
        executables/openmp/Makefile
        executables/openmp/lulesh/Makefile
	src/Makefile
	src/dpcl/Makefile
	src/dpcl/one_shot/Makefile
	src/dpcl/probe_module/Makefile
	src/dpcl/point_probe/Makefile
	src/dpcl/start_terminate/Makefile
	src/dpcl/module_addr/Makefile
	src/dpcl/stmt_info/Makefile
	src/dpcl/daemonRT_locking/Makefile
	src/dpcl/phase/Makefile
	src/dpcl/data_type/Makefile
	src/dyninst/Makefile
	src/dyninst/create/Makefile
	src/dyninst/createf77/Makefile
	src/dyninst/findPoint/Makefile
	src/dyninst/fpe/Makefile
	src/dyninst/exitpt/Makefile
        src/regression/Makefile
        src/regression/bb/Makefile
        src/regression/bb/close/Makefile
        src/regression/bb/close/close1/Makefile
        src/regression/bb/close/close2/Makefile
        src/regression/bb/close/close3/Makefile
        src/regression/bb/close/close4/Makefile
        src/regression/bb/close/close5/Makefile
        src/regression/bb/detach/Makefile
        src/regression/bb/detach/detach1/Makefile
        src/regression/bb/detach/detach2/Makefile
        src/regression/bb/detach/detach3/Makefile
        src/regression/bb/detach/detach4/Makefile
        src/regression/bb/detach/detach5/Makefile
        src/regression/bb/disable/Makefile
        src/regression/bb/disable/disable1/Makefile
        src/regression/bb/disable/disable2/Makefile
        src/regression/bb/disable/disable3/Makefile
        src/regression/bb/disable/disable4/Makefile
        src/regression/bb/disable/disable5/Makefile
        src/regression/bb/enable/Makefile
        src/regression/bb/enable/enable1/Makefile
        src/regression/bb/enable/enable2/Makefile
        src/regression/bb/enable/enable3/Makefile
        src/regression/bb/enable/enable4/Makefile
        src/regression/bb/enable/enable5/Makefile
        src/regression/bb/focus/Makefile
        src/regression/bb/focus/focus1/Makefile
        src/regression/bb/focus/focus2/Makefile
        src/regression/bb/focus/focus3/Makefile
        src/regression/bb/focus/focus4/Makefile
        src/regression/bb/focus/focus5/Makefile
        src/regression/bb/pause/Makefile
        src/regression/bb/pause/pause1/Makefile
        src/regression/bb/pause/pause2/Makefile
        src/regression/bb/pause/pause3/Makefile
        src/regression/bb/pause/pause4/Makefile
        src/regression/bb/pause/pause5/Makefile
        src/regression/bb/restore/Makefile
        src/regression/bb/restore/restore1/Makefile
        src/regression/bb/restore/restore2/Makefile
        src/regression/bb/restore/restore3/Makefile
        src/regression/bb/restore/restore4/Makefile
        src/regression/bb/restore/restore5/Makefile
        src/regression/fpe/Makefile
        src/regression/fpe/fpe1/Makefile
        src/regression/inf/Makefile
        src/regression/inf/hosts/Makefile
        src/regression/inf/hosts/hosts1/Makefile
        src/regression/inf/hosts/hosts2/Makefile
        src/regression/inf/hosts/hosts3/Makefile
        src/regression/inf/hosts/hosts4/Makefile
        src/regression/inf/listexp/Makefile
        src/regression/inf/listexp/listexp1/Makefile
        src/regression/inf/listexp/listexp2/Makefile
        src/regression/inf/listexp/listexp3/Makefile
        src/regression/inf/listexp/listexp4/Makefile
        src/regression/inf/views/Makefile
        src/regression/inf/views/views1/Makefile
        src/regression/inf/views/views2/Makefile
        src/regression/inf/views/views3/Makefile
        src/regression/inf/views/views4/Makefile
        src/regression/inf/types/Makefile
        src/regression/inf/types/types1/Makefile
        src/regression/inf/types/types2/Makefile
        src/regression/inf/types/types3/Makefile
        src/regression/inf/types/types4/Makefile
        src/regression/inf/metrics/Makefile
        src/regression/inf/metrics/metrics1/Makefile
        src/regression/inf/metrics/metrics2/Makefile
        src/regression/inf/metrics/metrics3/Makefile
        src/regression/inf/metrics/metrics4/Makefile
        src/regression/inf/params/Makefile
        src/regression/inf/params/params1/Makefile
        src/regression/inf/params/params2/Makefile
        src/regression/inf/params/params3/Makefile
        src/regression/inf/params/params4/Makefile
        src/regression/inf/pids/Makefile
        src/regression/inf/pids/pids1/Makefile
        src/regression/inf/pids/pids2/Makefile
        src/regression/inf/pids/pids3/Makefile
        src/regression/inf/pids/pids4/Makefile
        src/regression/inf/status/Makefile
        src/regression/inf/status/status1/Makefile
        src/regression/inf/status/status2/Makefile
        src/regression/inf/status/status3/Makefile
        src/regression/inf/status/status4/Makefile
        src/regression/io/Makefile
        src/regression/io/io1/Makefile
        src/regression/pcsamp/Makefile
        src/regression/pcsamp/pcsamp1/Makefile
        src/regression/pcsamp/pcsamp2/Makefile
        src/regression/pcsamp/pcsamp3/Makefile
        src/regression/pvs/Makefile
        src/regression/pvs/939887/Makefile
        src/regression/pvs/939885/Makefile
        src/regression/hwc/Makefile
        src/regression/hwc/hwc1/Makefile
        src/regression/hwc/hwc2/Makefile
        src/regression/hwc/hwc3/Makefile
        src/regression/hwctime/Makefile
        src/regression/hwctime/hwctime1/Makefile
        src/regression/hwctime/hwctime2/Makefile
        src/regression/hwctime/hwctime3/Makefile
        src/regression/usertime/Makefile
        src/regression/usertime/usertime1/Makefile
        src/regression/usertime/usertime2/Makefile
        src/regression/usertime/usertime3/Makefile
        src/regression/usertime/usertime4/Makefile
        src/regression/usertime/usertime5/Makefile
        src/regression/usertime/usertime6/Makefile
        src/regression/mpi/Makefile
        src/regression/mpi/mpi1/Makefile
        src/regression/mpi/smg2000/Makefile
        src/regression/mpi/smg2000/smg1/Makefile
        src/regression/views/Makefile
        src/regression/views/io/Makefile
        src/regression/views/io/calltrees/Makefile
        src/regression/views/io/calltrees/call1/Makefile
        src/regression/views/io/calltrees/call2/Makefile
        src/regression/views/io/calltrees/call3/Makefile
        src/regression/views/io/functions/Makefile
        src/regression/views/io/functions/functions1/Makefile
        src/regression/views/io/functions/functions2/Makefile
        src/regression/views/io/trace/Makefile
        src/regression/views/io/trace/trace1/Makefile
        src/regression/views/io/tracebacks/Makefile
        src/regression/views/io/tracebacks/tb1/Makefile
        src/regression/views/io/tracebacks/tb2/Makefile
        src/regression/views/io/tracebacks/tb3/Makefile
        src/regression/views/pcsamp/Makefile
        src/regression/views/pcsamp/functions/Makefile
        src/regression/views/pcsamp/functions/functions1/Makefile
        src/regression/views/pcsamp/linked_objects/Makefile
        src/regression/views/pcsamp/linked_objects/lo1/Makefile
        src/regression/views/pcsamp/statements/Makefile
        src/regression/views/pcsamp/statements/state1/Makefile
	test_scripts/Makefile
    ossdumpsym/Makefile
)

AC_OUTPUT
