################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
# Copyright (c) 2006-2015 Krell Institute All Rights Reserved.
# Copyright (c) 2014 Argo Navis Technologies. All Rights Reserved.
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

AC_INIT([OpenSpeedShop],[2.1_u8])
AC_CONFIG_AUX_DIR([libltdl/config])
AC_CONFIG_MACRO_DIR([m4])
AC_COPYRIGHT([[Copyright (c) 2006-2015 Krell Institute. All Rights Reserved.]])
AM_INIT_AUTOMAKE()

################################################################################
# Setup Configuration Header File
################################################################################

AC_CONFIG_HEADERS([config.h])

AH_TOP([
/*******************************************************************************
** Copyright (c) 2006-2015 Krell Institute. All Rights Reserved.
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
AC_PROG_CC
AC_PROG_CXX
AC_PROG_FC
AC_PROG_F77

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
AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug],
        [Enable build with debug enabled [default=no]])],
    [], [enable_debug=yes])

AM_CONDITIONAL([BUILD_DEBUG_ENDABLED], [test x$enable_debug = xyes])

if test x$enable_debug = xyes; then
  CPPFLAGS="${CPPFLAGS} -g -O2"
  CFLAGS="${CFLAGS} -g -O2"
  CXXFLAGS="${CXXFLAGS} -g -O2"
else
  AC_DEFINE([NDEBUG], [1],
            [Disable any NDEBUG debug code in OpenSpeedShop.]) 
fi
AC_MSG_RESULT([$enable_debug])

################################################################################
# Build CLI Timing Code Specification
################################################################################
#
# Option: --enable-cli-timing=no
#
AC_MSG_CHECKING(for enable build of CLI timing support specification)
AC_ARG_ENABLE([cli-timing],
    [AS_HELP_STRING([--enable-cli-timing],
        [Enable build of CLI timing support [default=no]])],
    [], [enable_cli_timing=no])

AM_CONDITIONAL([BUILD_CLI_TIMING], [test x$enable_cli_timing = xyes])

if test "x$enable_cli_timing" = xyes ; then
    AC_MSG_RESULT(yes)
    AC_DEFINE([BUILD_CLI_TIMING], [1],
        [Enable build of CLI timing support.])
else
    AC_MSG_RESULT(no)
fi

AC_SUBST(BUILD_CLI_TIMING)

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
# Check for ptgf option, default to none
################################################################################

AC_ARG_WITH(ptgf,
	    AS_HELP_STRING([--with-ptgf=TYPE],[ptgf (none, direct, all) @<:@none@:>@]),
	    PTGF=$withval, PTGF="none")

case "$PTGF" in
    direct)
	AM_CONDITIONAL(BUILD_PTGF_SERVER, false)
	AM_CONDITIONAL(BUILD_PTGF_DIRECT, true)
	AC_DEFINE(BUILD_PTGF_DIRECT, 1, [Define to 1 if you want to use an build only the direct portion of PTGF gui support.])
	;; 

    all)
	AM_CONDITIONAL(BUILD_PTGF_DIRECT, true)
	AC_DEFINE(BUILD_PTGF_DIRECT, 1, [Define to 1 if you want to use an build only the direct portion of PTGF gui support.])
	AM_CONDITIONAL(BUILD_PTGF_SERVER, true)
	AC_DEFINE(BUILD_PTGF_SERVER, 1, [Define to 1 if you want to use an build both the server portion of PTGF gui support.])
	;;

    none)
	AM_CONDITIONAL(BUILD_PTGF_DIRECT, false)
	AM_CONDITIONAL(BUILD_PTGF_SERVER, false)
	;;

    *)
	AC_MSG_WARN([Invalid ptgf specification: value received for with-ptgf="$PTGF". Setting value to none and continuing])
	AM_CONDITIONAL(BUILD_PTGF_DIRECT, false)
	AM_CONDITIONAL(BUILD_PTGF_SERVER, false)
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

    if test "x${runtime_dir}" == "x" ; then
      RUNTIME_DIR=""
      RUNTIME_DIR_LDFLAGS=""
      AM_CONDITIONAL(HAVE_RUNTIME_DIR, false)
      AC_MSG_RESULT(no)
    elif (test -e $runtime_dir/$abi_libdir) ; then
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

AX_ARRAYSVCS()
AX_BINUTILS()
AX_EPYDOC()
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

if test "$autoconf_for_target" = 1 ; then

AX_TARGET_OPENMP()
AX_TARGET_STDC_PLUSPLUS()
AX_TARGET_BINUTILS()
AX_TARGET_PERSONALITY()
AX_TARGET_LIBDWARF()
AX_TARGET_LIBMONITOR()
AX_PKG_TARGET_LIBPTHREAD()
AX_PKG_TARGET_LIBRT()
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

AM_CONDITIONAL(HAVE_DYNINST, false)

case "$INSTRUMENTOR" in
    none)
	AM_CONDITIONAL(HAVE_XERCES, false)
	#AM_CONDITIONAL(HAVE_SYMTABAPI, false)
	AX_SYMTABAPI()
	AM_CONDITIONAL(BUILD_CBTF, false)
	AM_CONDITIONAL(BUILD_DPCL, false)
	AM_CONDITIONAL(BUILD_OFFLINE, false)
	AM_CONDITIONAL(BUILD_MRNET, false)
	AM_CONDITIONAL(HAVE_MRNET, false)
	AM_CONDITIONAL(BUILD_INSTRUMENTOR, false)
	AC_DEFINE(BUILD_INSTRUMENTOR, 0, [Define to 1 if you do not want an instrumentor built.])
        #AM_CONDITIONAL(HAVE_RUNTIME_DIR, false)
        INSTRUMENTOR=offline
        AM_CONDITIONAL(HAVE_CBTF_MESSAGES_CUDA, false)
	AX_RUNTIME_DIR()
        AM_CONDITIONAL(HAVE_OMPT, false)
        AM_CONDITIONAL(HAVE_LIBIOMP, false)
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
        AM_CONDITIONAL(HAVE_CBTF_MESSAGES_CUDA, false)
        AM_CONDITIONAL(HAVE_OMPT, false)
        AM_CONDITIONAL(HAVE_LIBIOMP, false)
	;;

    mrnet)
	AX_BOOST_BASE([1.35.0])
	AX_DYNINST()
	AX_SYMTABAPI()
	AX_MRNET()
        #AX_XERCESC()
        # This helps with targeted builds when there is no xercesc
        #if test "$LIBXERCES_C" = "-lxerces-c" ; then
        #  AX_XERCESC_VERSION()
        #fi
	AM_CONDITIONAL(HAVE_XERCES, false)
	AM_CONDITIONAL(BUILD_CBTF, false)
	AM_CONDITIONAL(BUILD_MRNET, true)
	AC_DEFINE(BUILD_MRNET, 1, [Define to 1 if you only want the mrnet instrumentor.])
	AM_CONDITIONAL(BUILD_DPCL, false)
	AM_CONDITIONAL(BUILD_OFFLINE, false)
	AM_CONDITIONAL(BUILD_INSTRUMENTOR, true)
	AC_DEFINE(BUILD_INSTRUMENTOR, 1, [Define to 1 if you do not want an instrumentor built.])
        AM_CONDITIONAL(HAVE_RUNTIME_DIR, false)
        AM_CONDITIONAL(HAVE_CBTF_MESSAGES_CUDA, false)
        AM_CONDITIONAL(HAVE_OMPT, false)
        AM_CONDITIONAL(HAVE_LIBIOMP, false)
	;;

    offline)
	AX_BOOST_BASE([1.35.0])
	AX_DYNINST()
	AX_SYMTABAPI()
        #AX_XERCESC()
        ## This helps with targeted builds when there is no xercesc
        #if test "$LIBXERCES_C" = "-lxerces-c" ; then
        #  AX_XERCESC_VERSION()
        #fi
	AM_CONDITIONAL(HAVE_XERCES, false)
	AM_CONDITIONAL(BUILD_CBTF, false)
	AM_CONDITIONAL(BUILD_DPCL, false)
	AM_CONDITIONAL(BUILD_MRNET, false)
	AM_CONDITIONAL(HAVE_MRNET, false)
	AM_CONDITIONAL(BUILD_OFFLINE, true)
	AC_DEFINE(BUILD_OFFLINE, 1, [Define to 1 if you only want the offline instrumentor.])
	AM_CONDITIONAL(BUILD_INSTRUMENTOR, true)
	AC_DEFINE(BUILD_INSTRUMENTOR, 1, [Define to 1 if you do not want an instrumentor built.])
	AX_RUNTIME_DIR()
        AM_CONDITIONAL(HAVE_CBTF_MESSAGES_CUDA, false)
        AM_CONDITIONAL(HAVE_OMPT, false)
        AM_CONDITIONAL(HAVE_LIBIOMP, false)
        AM_CONDITIONAL(HAVE_CBTF_KRELL_CORE, false)
        AM_CONDITIONAL(HAVE_CBTF_KRELL_CORE_SYMTABAPI, false)
        AM_CONDITIONAL(HAVE_CBTF_KRELL_CORE_MRNET, false)
        AM_CONDITIONAL(HAVE_CBTF_KRELL_MESSAGES, false)
        AM_CONDITIONAL(HAVE_CBTF_KRELL_SERVICES, false)
	;;

    cbtf)
	AX_BOOST_BASE([1.35.0])
	AX_BOOST_DATE_TIME()
	AX_BOOST_SYSTEM()
#        #if BOOST_VERSION == 105000
#	   AX_BOOST_CHRONO()
#        #endif
	AX_BOOST_THREAD()
	AX_BOOST_FILESYSTEM()
	AX_BOOST_PROGRAM_OPTIONS()
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
	AX_CBTF_KRELL()
	#AX_CBTF_MESSAGES()
	AX_CBTF_MESSAGES_CUDA()
	#AX_CBTF_SERVICES()
	AX_LIBIOMP()

	AM_CONDITIONAL(BUILD_DPCL, false)
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
	doc/Makefile
	doc/users_guide/Makefile
	doc/users_guide/OpenSpeedShop_BuildInstallGuide_files/Makefile
	doc/users_guide/OpenSpeedShop_User_Manual_files/Makefile
	doc/cli_doc/Makefile
	doc/plugin_guide/Makefile
	doc/design_doc/Makefile
	doc/design_doc/Collectors/Makefile
	doc/design_doc/CLI/Makefile
	doc/design_doc/Framework/Makefile
	doc/design_doc/GUI/Makefile
	doc/design_doc/Message/Makefile
	doc/design_doc/Queries/Makefile
	doc/design_doc/Runtime/Makefile
	libopenss-message/Makefile
	libopenss-runtime/Makefile
	libopenss-runtime/cbtf/Makefile
	libopenss-runtime/dpcl/Makefile
	libopenss-runtime/dyninst/Makefile
	libopenss-runtime/mrnet/Makefile
	libopenss-runtime/offline/Makefile
	libopenss-runtime/fileio/Makefile
	libopenss-framework/Makefile
	libopenss-framework/cbtf/Makefile
	libopenss-framework/dpcl/Makefile
	libopenss-framework/dyninst/Makefile
	libopenss-framework/mrnet/Makefile
	libopenss-framework/mrnet/common/Makefile
	libopenss-framework/mrnet/openssd/Makefile
	libopenss-framework/mrnet/openssd/watcher/Makefile
	libopenss-framework/offline/Makefile
	libopenss-queries/Makefile
	libopenss-queries-cuda/Makefile
	man/Makefile
	man/man1/Makefile
	man/man3/Makefile
	plugins/Makefile
	plugins/collectors/Makefile
	plugins/collectors/pcsamp/Makefile
	plugins/collectors/usertime/Makefile
	plugins/collectors/hwc/Makefile
	plugins/collectors/hwcsamp/Makefile
	plugins/collectors/hwctime/Makefile
	plugins/collectors/io/Makefile
	plugins/collectors/iop/Makefile
	plugins/collectors/iot/Makefile
	plugins/collectors/mpi/Makefile
	plugins/collectors/mpit/Makefile
	plugins/collectors/mpip/Makefile
	plugins/collectors/mpiotf/Makefile
	plugins/collectors/fpe/Makefile
	plugins/collectors/mem/Makefile
	plugins/collectors/pthreads/Makefile
	plugins/collectors/cuda/Makefile
	plugins/panels/Makefile
	plugins/panels/CustomExperimentPanel/Makefile
	plugins/panels/pcSamplePanel/Makefile
	plugins/panels/UserTimePanel/Makefile
	plugins/panels/SourcePanel/Makefile
	plugins/panels/CmdPanel/Makefile	
	plugins/panels/StatsPanel/Makefile
	plugins/panels/ManageProcessesPanel/Makefile
	plugins/panels/CustomizeStatsPanel/Makefile
	plugins/panels/HW_CounterPanel/Makefile
	plugins/panels/HWCSampPanel/Makefile
	plugins/panels/HWCTimePanel/Makefile
	plugins/panels/IOPanel/Makefile
	plugins/panels/IOPPanel/Makefile
	plugins/panels/IOTPanel/Makefile
	plugins/panels/MPIPanel/Makefile
	plugins/panels/MPIPPanel/Makefile
	plugins/panels/MPITPanel/Makefile
	plugins/panels/MPIOTFPanel/Makefile
	plugins/panels/DebugPanel/Makefile
	plugins/panels/FPE_TracingPanel/Makefile
	plugins/panels/MEMPanel/Makefile
	plugins/panels/PTHREADSPanel/Makefile
	plugins/panels/CompareExperimentsPanel/Makefile
	plugins/panels/loadPanel/Makefile
	plugins/views/Makefile
	plugins/views/basic/Makefile
	plugins/views/cuda/Makefile
	plugins/views/hwc/Makefile
	plugins/views/pcsamp/Makefile
	plugins/views/hwcsamp/Makefile
	plugins/views/hwctime/Makefile
	plugins/views/io/Makefile
	plugins/views/iop/Makefile
	plugins/views/iot/Makefile
	plugins/views/mpi/Makefile
	plugins/views/mpip/Makefile
	plugins/views/mpit/Makefile
	plugins/views/mpiotf/Makefile
	plugins/views/usertime/Makefile
	plugins/views/fpe/Makefile
	plugins/views/mem/Makefile
	plugins/views/pthreads/Makefile
	plugins/wizards/Makefile
	plugins/wizards/IntroWizardPanel/Makefile
	plugins/wizards/pcSampleWizardPanel/Makefile
	plugins/wizards/UserTimeWizardPanel/Makefile
	plugins/wizards/FPE_TracingWizardPanel/Makefile
	plugins/wizards/HW_CounterWizardPanel/Makefile
	plugins/wizards/HW_CounterSampWizardPanel/Makefile
	plugins/wizards/IOWizardPanel/Makefile
	plugins/wizards/MPIWizardPanel/Makefile
	plugins/wizards/CompareWizardPanel/Makefile
	pyscripting/Makefile
	pyscripting/openss/Makefile
	pyscripting/sources/Makefile
	pyscripting/test/Makefile
	openss/Makefile
	ossdumpcuda/Makefile
	ossdumpsym/Makefile
	libopenss-guibase/Makefile
	libopenss-guiplugin/Makefile
	libopenss-guiexprinfo/Makefile
	libopenss-guiobjects/Makefile
	libopenss-guidialogs/Makefile
	libopenss-guichart/Makefile
	libopenss-cli/Makefile
	libopenss-cli/rapidxml-1.13/Makefile
	libopenss-gui/Makefile
	sanity/Makefile
	sanity/check_dpcl/Makefile
	sanity/check_dpcl/one_shot/Makefile
	sanity/check_dyninst/Makefile
	sanity/check_dyninst/create/Makefile
	sanity/check_openss/Makefile
	sanity/check_openss/simple/Makefile
	sanity/check_setup/Makefile
	usability/Makefile
	usability/phaseII/Makefile
	usability/phaseIII_scripting/Makefile
)

AC_OUTPUT
