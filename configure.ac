################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2010 Krell Institute All Rights Reserved.
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

AC_INIT(OpenSpeedShop, 1.9.3.3)
AC_COPYRIGHT([[Copyright (c) 2006-2010 Krell Institute. All Rights Reserved.]])
AC_CONFIG_AUX_DIR([libltdl/config])
AM_INIT_AUTOMAKE

################################################################################
# Setup Configuration Header File
################################################################################

AM_CONFIG_HEADER([config.h])

AH_TOP([
/*******************************************************************************
** Copyright (c) 2006-2010 Krell Institute. All Rights Reserved.
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
#  Is ln available
AC_PROG_LN_S

################################################################################
# Handle PowerPC and X86-64 Special Cases
################################################################################

case "$host" in
    powerpc-*-linux*) 
        abi_libdir="lib"
        alt_abi_libdir="lib64"
        if test x"$libdir" == x'${exec_prefix}/lib64'; then
            libdir='${exec_prefix}/lib'
        fi
        LDFLAGS="-L/usr/lib -DLIB_DIR=lib $LDFLAGS"
        ;;
    powerpc64-*-linux*) 
        abi_libdir="lib"
        alt_abi_libdir="lib64"
        if test x"$libdir" == x'${exec_prefix}/lib64'; then
            libdir='${exec_prefix}/lib'
        fi
        LDFLAGS="-L/usr/lib -DLIB_DIR=lib $LDFLAGS"
        ;;
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
			   [tls (implicit, explicit) @<:@explicit@:>@]),
	    TLS=$withval, TLS="explicit")

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
# Target O/S Specification
################################################################################

target_os=$host
AC_MSG_CHECKING(for Target O/S)
AC_ARG_WITH(target-os,
    AC_HELP_STRING([--with-target-os=TARGET_OS],
                   [target os specification @<:@$host@:>@]),
                    target_os=$withval, target_os="$host")

TARGET_OS=$target_os

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
    AC_DEFINE([HAVE_TARGET_POSIX_THREADS], [1],
        [Enable support on the target architecture for pthreads.])
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
    AC_DEFINE([HAVE_TARGET_SIGNALS], [1],
        [Enable support on target architecture for signals.])
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
fi

AC_SUBST(HAVE_TARGET_FORK)

################################################################################
# Check for Additional Packages
################################################################################

AC_PKG_ARRAYSVCS()
AC_PKG_BINUTILS()
AC_PKG_EPYDOC()
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
AC_PKG_TARGET_BINUTILS()
AC_PKG_TARGET_LIBMONITOR()
AC_PKG_TARGET_LIBUNWIND()
AC_PKG_TARGET_OTF()
AC_PKG_TARGET_PAPI()
AC_PKG_TARGET_VT()
AC_PKG_VT()
AC_PKG_XERCES()
################################################################################
# Check for Instrumentor
################################################################################

AC_ARG_WITH(instrumentor,
	    AC_HELP_STRING([--with-instrumentor=TYPE],
			   [instrumentor (dyninst, mrnet, offline, none) @<:@offline@:>@]),
	    INSTRUMENTOR=$withval, INSTRUMENTOR="offline")

case "$INSTRUMENTOR" in
    none)
	AM_CONDITIONAL(BUILD_DPCL, false)
	AM_CONDITIONAL(BUILD_OFFLINE, false)
	AM_CONDITIONAL(BUILD_MRNET, false)
	AM_CONDITIONAL(HAVE_MRNET, false)
	AM_CONDITIONAL(BUILD_INSTRUMENTOR, false)
	AC_DEFINE(BUILD_INSTRUMENTOR, 0, [Define to 1 if you do not want an instrumentor built.])
	;; 

    dyninst)
	AC_PKG_DYNINST()
	AM_CONDITIONAL(BUILD_DPCL, false)
	AM_CONDITIONAL(BUILD_MRNET, false)
	AM_CONDITIONAL(BUILD_OFFLINE, false)
	AM_CONDITIONAL(HAVE_MRNET, false)
	AM_CONDITIONAL(BUILD_INSTRUMENTOR, true)
	AC_DEFINE(BUILD_INSTRUMENTOR, 1, [Define to 1 if you do not want an instrumentor built.])
	;;

    mrnet)
	AC_PKG_DYNINST()
	AC_PKG_MRNET()
	AM_CONDITIONAL(BUILD_MRNET, true)
	AC_DEFINE(BUILD_MRNET, 1, [Define to 1 if you only want the mrnet instrumentor.])
	AM_CONDITIONAL(BUILD_DPCL, false)
	AM_CONDITIONAL(BUILD_OFFLINE, false)
	AM_CONDITIONAL(BUILD_INSTRUMENTOR, true)
	AC_DEFINE(BUILD_INSTRUMENTOR, 1, [Define to 1 if you do not want an instrumentor built.])
	;;

    offline)
	AM_CONDITIONAL(BUILD_DPCL, false)
	AM_CONDITIONAL(BUILD_MRNET, false)
	AM_CONDITIONAL(HAVE_MRNET, false)
	AM_CONDITIONAL(BUILD_OFFLINE, true)
	AC_DEFINE(BUILD_OFFLINE, 1, [Define to 1 if you only want the offline instrumentor.])
	AM_CONDITIONAL(BUILD_INSTRUMENTOR, true)
	AC_DEFINE(BUILD_INSTRUMENTOR, 1, [Define to 1 if you do not want an instrumentor built.])
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
    AM_CONDITIONAL(BUILD_RUNTIME, true)
    AC_DEFINE(BUILD_RUNTIME, 1, [Define to 1 if you do want only collector runtime support built.])
else
    AM_CONDITIONAL(BUILD_RUNTIME, false)
    AC_DEFINE(BUILD_RUNTIME, 0, [Define to 1 if you do not want only collector runtime support built.])
fi
AC_SUBST(BUILD_RUNTIME)

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
	doc/users_guide/BuildInstallGuide_files/Makefile
	doc/users_guide/BuildTutorial/Makefile
	doc/users_guide/TroubleShootingGuide/Makefile
	doc/users_guide/usersGuide-1.9c_files/Makefile
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
	libopenss-runtime/dpcl/Makefile
	libopenss-runtime/dyninst/Makefile
	libopenss-runtime/mrnet/Makefile
	libopenss-runtime/offline/Makefile
	libopenss-runtime/fileio/Makefile
	libopenss-framework/Makefile
	libopenss-framework/dpcl/Makefile
	libopenss-framework/dyninst/Makefile
	libopenss-framework/mrnet/Makefile
	libopenss-framework/mrnet/common/Makefile
	libopenss-framework/mrnet/openssd/Makefile
	libopenss-framework/mrnet/openssd/watcher/Makefile
	libopenss-framework/offline/Makefile
	libopenss-queries/Makefile
	man/Makefile
	man/man1/Makefile
	man/man3/Makefile
	plugins/Makefile
	plugins/collectors/Makefile
	plugins/collectors/pcsamp/Makefile
	plugins/collectors/usertime/Makefile
	plugins/collectors/hwc/Makefile
	plugins/collectors/hwctime/Makefile
	plugins/collectors/io/Makefile
	plugins/collectors/iot/Makefile
	plugins/collectors/mpi/Makefile
	plugins/collectors/mpit/Makefile
	plugins/collectors/mpiotf/Makefile
	plugins/collectors/fpe/Makefile
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
	plugins/panels/HWCTimePanel/Makefile
	plugins/panels/IOPanel/Makefile
	plugins/panels/IOTPanel/Makefile
	plugins/panels/MPIPanel/Makefile
	plugins/panels/MPITPanel/Makefile
	plugins/panels/MPIOTFPanel/Makefile
	plugins/panels/DebugPanel/Makefile
	plugins/panels/FPE_TracingPanel/Makefile
	plugins/panels/CompareExperimentsPanel/Makefile
	plugins/panels/loadPanel/Makefile
	plugins/views/Makefile
	plugins/views/basic/Makefile
	plugins/views/hwctime/Makefile
	plugins/views/io/Makefile
	plugins/views/iot/Makefile
	plugins/views/mpi/Makefile
	plugins/views/mpit/Makefile
	plugins/views/mpiotf/Makefile
	plugins/views/usertime/Makefile
	plugins/views/fpe/Makefile
	plugins/wizards/Makefile
	plugins/wizards/IntroWizardPanel/Makefile
	plugins/wizards/pcSampleWizardPanel/Makefile
	plugins/wizards/UserTimeWizardPanel/Makefile
	plugins/wizards/FPE_TracingWizardPanel/Makefile
	plugins/wizards/HW_CounterWizardPanel/Makefile
	plugins/wizards/IOWizardPanel/Makefile
	plugins/wizards/MPIWizardPanel/Makefile
	plugins/wizards/CompareWizardPanel/Makefile
	pyscripting/Makefile
	pyscripting/openss/Makefile
	pyscripting/sources/Makefile
	pyscripting/test/Makefile
	openss/Makefile
	libopenss-guibase/Makefile
	libopenss-guiplugin/Makefile
	libopenss-guiexprinfo/Makefile
	libopenss-guiobjects/Makefile
	libopenss-guidialogs/Makefile
	libopenss-guichart/Makefile
	libopenss-cli/Makefile
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
