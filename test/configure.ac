################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2013 Krell Institute   All Rights Reserved.
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

AC_INIT(OpenSpeedShop, 2.0.2)
AC_COPYRIGHT([[Copyright (c) 2006-2013 Krell Institute. All Rights Reserved.]])
AC_CONFIG_AUX_DIR([libltdl/config])
AM_INIT_AUTOMAKE

################################################################################
# Setup Configuration Header File
################################################################################

AM_CONFIG_HEADER([config.h])

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

AC_DISABLE_STATIC

# Name the subdirectory that contains libltdl sources
LT_CONFIG_LTDL_DIR([libltdl])

AC_CONFIG_MACRO_DIR([libltdl/m4])

# Configure libtool with dlopen support if possible
LT_INIT([dlopen])

# Enable building of the installable libltdl library
LTDL_INIT([installable])

#AC_LIBLTDL_INSTALLABLE
#AC_SUBST(LTDLINCL)
#AC_SUBST(LIBLTDL)
#LT_CONFIG_LTDL_DIR([libltdl])
#AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
#AC_CONFIG_SUBDIRS(libltdl)

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
        executables/Makefile
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

)

AC_OUTPUT
