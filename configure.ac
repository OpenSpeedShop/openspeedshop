################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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

AC_INIT(OpenSpeedShop, 0.5)
AC_COPYRIGHT([[Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.]])
AM_INIT_AUTOMAKE

################################################################################
# Setup Configuration Header File
################################################################################

AM_CONFIG_HEADER([config.h])

AH_TOP([
/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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

AC_DISABLE_STATIC
AC_LIBLTDL_INSTALLABLE
AC_SUBST(LTDLINCL)
AC_SUBST(LIBLTDL)
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_CONFIG_SUBDIRS(libltdl)
AC_CONFIG_SUBDIRS(test/executables)
AC_CONFIG_SUBDIRS(test/test_source/dyninst)

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
        LDFLAGS="-L/usr/lib64 $LDFLAGS"
        ;;
    *)
        abi_libdir="lib"
        ;;
esac
topdir_execprefix=${exec_prefix}
AC_SUBST(topdir_execprefix)

topdir_execprefix=${exec_prefix}
AC_SUBST(topdir_execprefix)

################################################################################
# Check for POSIX Threads (ISO/IEC 9945-1:1996)
################################################################################

AC_CHECK_HEADER(pthread.h, [ AC_CHECK_LIB(pthread, pthread_create, [
    AC_DEFINE(HAVE_POSIX_THREADS, 1, [Define to 1 if you have POSIX threads.])
], [
    AC_MSG_FAILURE([cannot locate POSIX thread library and/or headers.])
]) ])

################################################################################
# Check for Additional Packages
################################################################################

AC_PKG_ARRAYSVCS()
AC_PKG_LIBUNWIND()
AC_PKG_MPI()
AC_PKG_PAPI()
AM_CHECK_PYTHON_HEADERS()
AC_PKG_PYTHON()
AC_PKG_SQLITE()
AC_PKG_QTLIB()

################################################################################
# Check for Instrumentor
################################################################################

AC_ARG_WITH(instrumentor,
	    AC_HELP_STRING([--with-instrumentor=TYPE],
			   [instrumentor (dpcl, dyninst) @<:@dpcl@:>@]),
	    INSTRUMENTOR=$withval, INSTRUMENTOR="dpcl")

case "$INSTRUMENTOR" in
    dpcl)
	AC_PKG_DPCL()
	;;

    dyninst)
	AC_PKG_DYNINST()
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
	libopenss-message/Makefile
	libopenss-runtime/Makefile
	libopenss-runtime/dpcl/Makefile
	libopenss-runtime/dyninst/Makefile
	libopenss-framework/Makefile
	libopenss-framework/dpcl/Makefile
	libopenss-framework/dyninst/Makefile
	libopenss-queries/Makefile
	plugins/Makefile
	plugins/collectors/Makefile
	plugins/collectors/pcsamp/Makefile
	plugins/collectors/usertime/Makefile
	plugins/collectors/hwc/Makefile
	plugins/collectors/hwctime/Makefile
	plugins/collectors/io/Makefile
	plugins/collectors/mpi/Makefile
	plugins/collectors/mpit/Makefile
	plugins/panels/Makefile
	plugins/panels/pcSamplePanel/Makefile
	plugins/panels/UserTimePanel/Makefile
	plugins/panels/SourcePanel/Makefile
	plugins/panels/CmdPanel/Makefile	
	plugins/panels/StatsPanel/Makefile
	plugins/panels/ManageProcessesPanel/Makefile
	plugins/panels/ComparePanel/Makefile
	plugins/panels/HW_CounterPanel/Makefile
	plugins/panels/IOPanel/Makefile
	plugins/panels/MPIPanel/Makefile
	plugins/panels/DebugPanel/Makefile
	plugins/panels/FPE_TracingPanel/Makefile
	plugins/panels/CustomExperimentPanel/Makefile
	plugins/views/Makefile
	plugins/views/basic/Makefile
	plugins/views/mpi/Makefile
	plugins/views/io/Makefile
	plugins/wizards/Makefile
	plugins/wizards/IntroWizardPanel/Makefile
	plugins/wizards/pcSampleWizardPanel/Makefile
	plugins/wizards/UserTimeWizardPanel/Makefile
	plugins/wizards/FPE_TracingWizardPanel/Makefile
	plugins/wizards/HW_CounterWizardPanel/Makefile
	plugins/wizards/IOWizardPanel/Makefile
	plugins/wizards/MPIWizardPanel/Makefile
	examples/Makefile
	examples/libopenss-framework/Makefile
	examples/libopenss-framework/collector/Makefile
	examples/libopenss-framework/tool/Makefile
	openss/Makefile
	libopenss-guibase/Makefile
	libopenss-guiplugin/Makefile
	libopenss-guiexprinfo/Makefile
	libopenss-guiobjects/Makefile
	libopenss-guidialogs/Makefile
	libopenss-guichart/Makefile
	libopenss-cli/Makefile
	libopenss-gui/Makefile
	test/Makefile
	test/test_scripts/Makefile
	test/test_source/Makefile
	test/test_source/regression/Makefile	
	test/test_source/regression/building_block/Makefile
	test/test_source/regression/building_block/close/Makefile
	test/test_source/regression/building_block/close/close1/Makefile
	test/test_source/regression/building_block/close/close2/Makefile
	test/test_source/regression/building_block/close/close3/Makefile
	test/test_source/regression/building_block/close/close4/Makefile
	test/test_source/regression/building_block/detach/Makefile
	test/test_source/regression/building_block/detach/detach1/Makefile
	test/test_source/regression/building_block/detach/detach2/Makefile
	test/test_source/regression/building_block/detach/detach3/Makefile
	test/test_source/regression/building_block/detach/detach4/Makefile
	test/test_source/regression/building_block/disable/Makefile
	test/test_source/regression/building_block/disable/disable1/Makefile
	test/test_source/regression/building_block/disable/disable2/Makefile
	test/test_source/regression/building_block/disable/disable3/Makefile
	test/test_source/regression/building_block/disable/disable4/Makefile
	test/test_source/regression/building_block/enable/Makefile
	test/test_source/regression/building_block/enable/enable1/Makefile
	test/test_source/regression/building_block/enable/enable2/Makefile
	test/test_source/regression/building_block/enable/enable3/Makefile
	test/test_source/regression/building_block/enable/enable4/Makefile
	test/test_source/regression/building_block/focus/Makefile
	test/test_source/regression/building_block/focus/focus1/Makefile
	test/test_source/regression/building_block/focus/focus2/Makefile
	test/test_source/regression/building_block/focus/focus3/Makefile
	test/test_source/regression/building_block/focus/focus4/Makefile
	test/test_source/regression/building_block/pause/Makefile
	test/test_source/regression/building_block/pause/pause1/Makefile
	test/test_source/regression/building_block/pause/pause2/Makefile
	test/test_source/regression/building_block/pause/pause3/Makefile
	test/test_source/regression/building_block/pause/pause4/Makefile
	test/test_source/regression/building_block/restore/Makefile
	test/test_source/regression/building_block/restore/restore1/Makefile
	test/test_source/regression/building_block/restore/restore2/Makefile
	test/test_source/regression/building_block/restore/restore3/Makefile
	test/test_source/regression/building_block/restore/restore4/Makefile
	test/test_source/regression/info/Makefile
	test/test_source/regression/info/hosts/Makefile
	test/test_source/regression/info/hosts/hosts1/Makefile
	test/test_source/regression/info/hosts/hosts2/Makefile
	test/test_source/regression/info/hosts/hosts3/Makefile
	test/test_source/regression/info/listexp/Makefile
	test/test_source/regression/info/listexp/listexp1/Makefile
	test/test_source/regression/info/listexp/listexp2/Makefile
	test/test_source/regression/info/listexp/listexp3/Makefile
	test/test_source/regression/info/views/Makefile
	test/test_source/regression/info/views/views1/Makefile
	test/test_source/regression/info/views/views2/Makefile
	test/test_source/regression/info/views/views3/Makefile
	test/test_source/regression/info/types/Makefile
	test/test_source/regression/info/types/types1/Makefile
	test/test_source/regression/info/types/types2/Makefile
	test/test_source/regression/info/types/types3/Makefile
	test/test_source/regression/info/metrics/Makefile
	test/test_source/regression/info/metrics/metrics1/Makefile
	test/test_source/regression/info/metrics/metrics2/Makefile
	test/test_source/regression/info/metrics/metrics3/Makefile
	test/test_source/regression/info/params/Makefile
	test/test_source/regression/info/params/params1/Makefile
	test/test_source/regression/info/params/params2/Makefile
	test/test_source/regression/info/params/params3/Makefile
	test/test_source/regression/info/pids/Makefile
	test/test_source/regression/info/pids/pids1/Makefile
	test/test_source/regression/info/pids/pids2/Makefile
	test/test_source/regression/info/pids/pids3/Makefile
	test/test_source/regression/info/status/Makefile
	test/test_source/regression/info/status/status1/Makefile
	test/test_source/regression/info/status/status2/Makefile
	test/test_source/regression/info/status/status3/Makefile
	test/test_source/regression/pcsamp/Makefile
	test/test_source/regression/pcsamp/pcsamp1/Makefile
	test/test_source/regression/pcsamp/pcsamp2/Makefile
	test/test_source/regression/pcsamp/pcsamp3/Makefile
	test/test_source/regression/pvs/Makefile
	test/test_source/regression/pvs/939887/Makefile
	test/test_source/regression/pvs/939885/Makefile	
	test/test_source/regression/hwc/Makefile
	test/test_source/regression/hwc/hwc1/Makefile
	test/test_source/regression/hwc/hwc2/Makefile
	test/test_source/regression/hwc/hwc3/Makefile
	test/test_source/regression/hwctime/Makefile
	test/test_source/regression/hwctime/hwctime1/Makefile
	test/test_source/regression/hwctime/hwctime2/Makefile
	test/test_source/regression/hwctime/hwctime3/Makefile
	test/test_source/regression/usertime/Makefile
	test/test_source/regression/usertime/usertime1/Makefile
	test/test_source/regression/usertime/usertime2/Makefile
	test/test_source/regression/usertime/usertime3/Makefile
	test/test_source/regression/usertime/usertime4/Makefile
	test/test_source/regression/usertime/usertime5/Makefile
	test/test_source/regression/usertime/usertime6/Makefile
	test/test_source/regression/mpi/Makefile
	test/test_source/regression/mpi/mpi1/Makefile
	test/test_source/regression/mpi/smg2000/Makefile
	test/test_source/regression/mpi/smg2000/smg1/Makefile
	test/test_source/dpcl/Makefile
	test/test_source/dpcl/one_shot/Makefile
	test/test_source/dpcl/probe_module/Makefile
	test/test_source/dpcl/point_probe/Makefile
	test/test_source/dpcl/start_terminate/Makefile
	test/test_source/dpcl/module_addr/Makefile
	test/test_source/dpcl/stmt_info/Makefile
	test/test_source/dpcl/daemonRT_locking/Makefile
	test/test_source/dpcl/phase/Makefile
	test/test_source/dpcl/data_type/Makefile
	usability/Makefile
	usability/phaseII/Makefile
	usability/phaseIII/Makefile
	usability/phaseIII_scripting/Makefile
)

AC_OUTPUT
