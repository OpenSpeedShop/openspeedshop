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

AC_INIT(OpenSpeedShop, 0.875)
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
AC_CONFIG_SUBDIRS(test/executables/mpi)
AC_CONFIG_SUBDIRS(test/src/dyninst)

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
AC_PYTHON_DEVEL([>= '2.3'])
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
	doc/Makefile
	doc/users_guide/Makefile
	doc/users_guide/TroubleShootingGuide/Makefile
	doc/cli_doc/Makefile
	doc/plugin_guide/Makefile
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
	plugins/collectors/iot/Makefile
	plugins/collectors/mpi/Makefile
	plugins/collectors/mpit/Makefile
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
	plugins/panels/DebugPanel/Makefile
	plugins/panels/FPE_TracingPanel/Makefile
	plugins/panels/CompareExperimentsPanel/Makefile
	plugins/views/Makefile
	plugins/views/basic/Makefile
	plugins/views/hwctime/Makefile
	plugins/views/io/Makefile
	plugins/views/iot/Makefile
	plugins/views/mpi/Makefile
	plugins/views/mpit/Makefile
	plugins/views/usertime/Makefile
	plugins/wizards/Makefile
	plugins/wizards/IntroWizardPanel/Makefile
	plugins/wizards/pcSampleWizardPanel/Makefile
	plugins/wizards/UserTimeWizardPanel/Makefile
	plugins/wizards/FPE_TracingWizardPanel/Makefile
	plugins/wizards/HW_CounterWizardPanel/Makefile
	plugins/wizards/IOWizardPanel/Makefile
	plugins/wizards/MPIWizardPanel/Makefile
	plugins/wizards/CompareWizardPanel/Makefile
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
	test/src/Makefile
	test/src/regression/Makefile	
	test/src/regression/bb/Makefile
	test/src/regression/bb/close/Makefile
	test/src/regression/bb/close/close1/Makefile
	test/src/regression/bb/close/close2/Makefile
	test/src/regression/bb/close/close3/Makefile
	test/src/regression/bb/close/close4/Makefile
	test/src/regression/bb/close/close5/Makefile
	test/src/regression/bb/detach/Makefile
	test/src/regression/bb/detach/detach1/Makefile
	test/src/regression/bb/detach/detach2/Makefile
	test/src/regression/bb/detach/detach3/Makefile
	test/src/regression/bb/detach/detach4/Makefile
	test/src/regression/bb/detach/detach5/Makefile
	test/src/regression/bb/disable/Makefile
	test/src/regression/bb/disable/disable1/Makefile
	test/src/regression/bb/disable/disable2/Makefile
	test/src/regression/bb/disable/disable3/Makefile
	test/src/regression/bb/disable/disable4/Makefile
	test/src/regression/bb/disable/disable5/Makefile
	test/src/regression/bb/enable/Makefile
	test/src/regression/bb/enable/enable1/Makefile
	test/src/regression/bb/enable/enable2/Makefile
	test/src/regression/bb/enable/enable3/Makefile
	test/src/regression/bb/enable/enable4/Makefile
	test/src/regression/bb/enable/enable5/Makefile
	test/src/regression/bb/focus/Makefile
	test/src/regression/bb/focus/focus1/Makefile
	test/src/regression/bb/focus/focus2/Makefile
	test/src/regression/bb/focus/focus3/Makefile
	test/src/regression/bb/focus/focus4/Makefile
	test/src/regression/bb/focus/focus5/Makefile
	test/src/regression/bb/pause/Makefile
	test/src/regression/bb/pause/pause1/Makefile
	test/src/regression/bb/pause/pause2/Makefile
	test/src/regression/bb/pause/pause3/Makefile
	test/src/regression/bb/pause/pause4/Makefile
	test/src/regression/bb/pause/pause5/Makefile
	test/src/regression/bb/restore/Makefile
	test/src/regression/bb/restore/restore1/Makefile
	test/src/regression/bb/restore/restore2/Makefile
	test/src/regression/bb/restore/restore3/Makefile
	test/src/regression/bb/restore/restore4/Makefile
	test/src/regression/bb/restore/restore5/Makefile
	test/src/regression/inf/Makefile
	test/src/regression/inf/hosts/Makefile
	test/src/regression/inf/hosts/hosts1/Makefile
	test/src/regression/inf/hosts/hosts2/Makefile
	test/src/regression/inf/hosts/hosts3/Makefile
	test/src/regression/inf/hosts/hosts4/Makefile
	test/src/regression/inf/listexp/Makefile
	test/src/regression/inf/listexp/listexp1/Makefile
	test/src/regression/inf/listexp/listexp2/Makefile
	test/src/regression/inf/listexp/listexp3/Makefile
	test/src/regression/inf/listexp/listexp4/Makefile
	test/src/regression/inf/views/Makefile
	test/src/regression/inf/views/views1/Makefile
	test/src/regression/inf/views/views2/Makefile
	test/src/regression/inf/views/views3/Makefile
	test/src/regression/inf/views/views4/Makefile
	test/src/regression/inf/types/Makefile
	test/src/regression/inf/types/types1/Makefile
	test/src/regression/inf/types/types2/Makefile
	test/src/regression/inf/types/types3/Makefile
	test/src/regression/inf/types/types4/Makefile
	test/src/regression/inf/metrics/Makefile
	test/src/regression/inf/metrics/metrics1/Makefile
	test/src/regression/inf/metrics/metrics2/Makefile
	test/src/regression/inf/metrics/metrics3/Makefile
	test/src/regression/inf/metrics/metrics4/Makefile
	test/src/regression/inf/params/Makefile
	test/src/regression/inf/params/params1/Makefile
	test/src/regression/inf/params/params2/Makefile
	test/src/regression/inf/params/params3/Makefile
	test/src/regression/inf/params/params4/Makefile
	test/src/regression/inf/pids/Makefile
	test/src/regression/inf/pids/pids1/Makefile
	test/src/regression/inf/pids/pids2/Makefile
	test/src/regression/inf/pids/pids3/Makefile
	test/src/regression/inf/pids/pids4/Makefile
	test/src/regression/inf/status/Makefile
	test/src/regression/inf/status/status1/Makefile
	test/src/regression/inf/status/status2/Makefile
	test/src/regression/inf/status/status3/Makefile
	test/src/regression/inf/status/status4/Makefile
	test/src/regression/io/Makefile
	test/src/regression/io/io1/Makefile
	test/src/regression/pcsamp/Makefile
	test/src/regression/pcsamp/pcsamp1/Makefile
	test/src/regression/pcsamp/pcsamp2/Makefile
	test/src/regression/pcsamp/pcsamp3/Makefile
	test/src/regression/pvs/Makefile
	test/src/regression/pvs/939887/Makefile
	test/src/regression/pvs/939885/Makefile	
	test/src/regression/hwc/Makefile
	test/src/regression/hwc/hwc1/Makefile
	test/src/regression/hwc/hwc2/Makefile
	test/src/regression/hwc/hwc3/Makefile
	test/src/regression/hwctime/Makefile
	test/src/regression/hwctime/hwctime1/Makefile
	test/src/regression/hwctime/hwctime2/Makefile
	test/src/regression/hwctime/hwctime3/Makefile
	test/src/regression/usertime/Makefile
	test/src/regression/usertime/usertime1/Makefile
	test/src/regression/usertime/usertime2/Makefile
	test/src/regression/usertime/usertime3/Makefile
	test/src/regression/usertime/usertime4/Makefile
	test/src/regression/usertime/usertime5/Makefile
	test/src/regression/usertime/usertime6/Makefile
	test/src/regression/mpi/Makefile
	test/src/regression/mpi/mpi1/Makefile
	test/src/regression/mpi/smg2000/Makefile
	test/src/regression/mpi/smg2000/smg1/Makefile
	test/src/regression/views/Makefile
	test/src/regression/views/io/Makefile
	test/src/regression/views/io/calltrees/Makefile
	test/src/regression/views/io/calltrees/call1/Makefile
	test/src/regression/views/io/calltrees/call2/Makefile
	test/src/regression/views/io/calltrees/call3/Makefile
	test/src/regression/views/io/functions/Makefile
	test/src/regression/views/io/functions/functions1/Makefile
	test/src/regression/views/io/functions/functions2/Makefile
	test/src/regression/views/io/trace/Makefile
	test/src/regression/views/io/trace/trace1/Makefile
	test/src/regression/views/io/tracebacks/Makefile
	test/src/regression/views/io/tracebacks/tb1/Makefile
	test/src/regression/views/io/tracebacks/tb2/Makefile
	test/src/regression/views/io/tracebacks/tb3/Makefile
	test/src/regression/views/pcsamp/Makefile
	test/src/regression/views/pcsamp/functions/Makefile
	test/src/regression/views/pcsamp/functions/functions1/Makefile
	test/src/regression/views/pcsamp/linked_objects/Makefile
	test/src/regression/views/pcsamp/linked_objects/lo1/Makefile
	test/src/regression/views/pcsamp/statements/Makefile
	test/src/regression/views/pcsamp/statements/state1/Makefile
	test/src/dpcl/Makefile
	test/src/dpcl/one_shot/Makefile
	test/src/dpcl/probe_module/Makefile
	test/src/dpcl/point_probe/Makefile
	test/src/dpcl/start_terminate/Makefile
	test/src/dpcl/module_addr/Makefile
	test/src/dpcl/stmt_info/Makefile
	test/src/dpcl/daemonRT_locking/Makefile
	test/src/dpcl/phase/Makefile
	test/src/dpcl/data_type/Makefile
	usability/Makefile
	usability/phaseII/Makefile
	usability/phaseIII_scripting/Makefile
)

AC_OUTPUT
