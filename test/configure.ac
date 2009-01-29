################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2009 Krell Institute   All Rights Reserved.
# Copyright (c) 2007-2008 William Hachfeld. All Rights Reserved.
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

AC_INIT(OpenSpeedShop, 1.9.2)
AC_COPYRIGHT([[Copyright (c) 2009 Krell Institute. All Rights Reserved.]])
AM_INIT_AUTOMAKE

################################################################################
# Setup Configuration Header File
################################################################################

AM_CONFIG_HEADER([config.h])

AH_TOP([
/*******************************************************************************
** Copyright (c) 2008 Krell Institute. All Rights Reserved.
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

AC_GNU_SOURCE
AC_PROG_CC
AC_PROG_CXX
AC_PROG_FC
AC_PROG_F77

AC_DISABLE_STATIC
AC_LIBLTDL_INSTALLABLE
AC_SUBST(LTDLINCL)
AC_SUBST(LIBLTDL)
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_CONFIG_SUBDIRS(libltdl)
AC_CONFIG_SUBDIRS(src/dyninst)
AC_CONFIG_SUBDIRS(executables)
AC_CONFIG_SUBDIRS(executables/mpi)
AC_CONFIG_SUBDIRS(src/regression)

AC_PATH_PROG([DOXYGEN], doxygen)
AC_PROG_INSTALL
AC_PROG_LEX
AC_PATH_PROG([RPCGEN], rpcgen)
AC_PROG_YACC

################################################################################
# Handle X86-64 Special Case
################################################################################

case "$host" in
    x86_64-*-linux*)
        if test x"$libdir" == x'${exec_prefix}/lib'; then
            libdir='${exec_prefix}/lib64'
        fi
        abi_libdir="lib64"
        alt_abi_libdir="lib"
        LDFLAGS="-L/usr/lib64 $LDFLAGS"
        ;;
    *)
        abi_libdir="lib"
        alt_abi_libdir="lib64"
        LDFLAGS="-L/usr/lib -DLIB_DIR=lib $LDFLAGS"
        ;;
esac
topdir_execprefix=${exec_prefix}
AC_SUBST(topdir_execprefix)

abi_libdir_name=${abi_libdir}
AC_SUBST(abi_libdir_name)

################################################################################
# Check for POSIX Threads (ISO/IEC 9945-1:1996)
################################################################################

AC_CHECK_HEADER(pthread.h, [ AC_CHECK_LIB(pthread, pthread_create, [
    AC_DEFINE(HAVE_POSIX_THREADS, 1, [Define to 1 if you have POSIX threads.])
], [
    AC_MSG_FAILURE([cannot locate POSIX thread library and/or headers.])
]) ])

################################################################################
# Check for TLS option, default to implicit
################################################################################

AC_ARG_WITH(tls,
	    AC_HELP_STRING([--with-tls=TYPE],
			   [tls (implicit, explicit) @<:@implicit@:>@]),
	    TLS=$withval, TLS="implicit")

case "$TLS" in
    implicit)
	AM_CONDITIONAL(USE_EXPLICIT_TLS, false)
	AC_DEFINE(USE_EXPLICIT_TLS, 0, [Define to 0 if you want to use implicit tls, i.e. the __thread keyword.])
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
# Check for Additional Packages
################################################################################

AC_PKG_ARRAYSVCS()
AC_PKG_BINUTILS()
AC_PKG_LIBDWARF()
AC_PKG_LIBMONITOR()
AC_PKG_LIBUNWIND()
AC_PKG_MPI()
AC_PKG_OPENMP()
AC_PKG_OTF()
AC_PKG_PAPI()
AC_PYTHON_DEVEL([>= '2.3'])
AC_PKG_QTLIB()
AC_PKG_SQLITE()
AC_PKG_VT()
AC_PKG_XERCES()

################################################################################
# Check for Instrumentor
################################################################################

AC_ARG_WITH(instrumentor,
	    AC_HELP_STRING([--with-instrumentor=TYPE],
			   [instrumentor (dpcl, dyninst, mrnet, offline) @<:@offline@:>@]),
	    INSTRUMENTOR=$withval, INSTRUMENTOR="offline")

case "$INSTRUMENTOR" in
    dpcl)
	AC_PKG_DYNINST()
	AC_PKG_DPCL()
	AM_CONDITIONAL(BUILD_DPCL, true)
	AC_DEFINE(BUILD_DPCL, 1, [Define to 1 if you only want the dpcl instrumentor.])
	AM_CONDITIONAL(BUILD_OFFLINE, false)
	AM_CONDITIONAL(BUILD_MRNET, false)
	AM_CONDITIONAL(HAVE_MRNET, false)
	;; 

    dyninst)
	AC_PKG_DYNINST()
	AM_CONDITIONAL(BUILD_DPCL, false)
	AM_CONDITIONAL(BUILD_MRNET, false)
	AM_CONDITIONAL(BUILD_OFFLINE, false)
	AM_CONDITIONAL(HAVE_MRNET, false)
	;;

    mrnet)
	AC_PKG_DYNINST()
	AC_PKG_MRNET()
	AM_CONDITIONAL(BUILD_MRNET, true)
	AC_DEFINE(BUILD_MRNET, 1, [Define to 1 if you only want the mrnet instrumentor.])
	AM_CONDITIONAL(BUILD_DPCL, false)
	AM_CONDITIONAL(BUILD_OFFLINE, false)
	;;

    offline)
	AM_CONDITIONAL(BUILD_DPCL, false)
	AM_CONDITIONAL(BUILD_MRNET, false)
	AM_CONDITIONAL(HAVE_MRNET, false)
	AM_CONDITIONAL(BUILD_OFFLINE, true)
	AC_DEFINE(BUILD_OFFLINE, 1, [Define to 1 if you only want the offline instrumentor.])
	;;

    *)
	AC_MSG_ERROR([invalid instrumentor "$INSTRUMENTOR".])
	;;
esac

AC_SUBST(INSTRUMENTOR)

################################################################################
# Generate Output Files
################################################################################

AC_CONFIG_FILES(
	Makefile
	test_scripts/Makefile
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
)

AC_OUTPUT
