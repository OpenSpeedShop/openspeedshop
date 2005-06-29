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

AC_INIT(OpenSpeedShop, 1.0)
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
AC_PROG_CXX

AC_DISABLE_STATIC
AC_LIBLTDL_INSTALLABLE
AC_SUBST(LTDLINCL)
AC_SUBST(LIBLTDL)
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_CONFIG_SUBDIRS(libltdl)

AC_PATH_PROG([DOXYGEN], doxygen)
AC_PROG_INSTALL
AC_PROG_LEX
AC_PATH_PROG([RPCGEN], rpcgen)
AC_PROG_YACC

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
# Check for SQLite (http://www.sqlite.org/)
################################################################################

AC_CHECK_HEADER(sqlite3.h, [ AC_CHECK_LIB(sqlite3, sqlite3_open, [
    AC_DEFINE(HAVE_SQLITE, 1, [Define to 1 if you have SQLite.])
], [
    AC_MSG_FAILURE([cannot locate SQLite library and/or headers.])
]) ])

################################################################################
# Check for POSIX Threads (ISO/IEC 9945-1:1996)
################################################################################

AC_CHECK_HEADER(pthread.h, [ AC_CHECK_LIB(pthread, pthread_create, [
    AC_DEFINE(HAVE_POSIX_THREADS, 1, [Define to 1 if you have POSIX threads.])
], [
    AC_MSG_FAILURE([cannot locate POSIX thread library and/or headers.])
]) ])

################################################################################
# Check for Python (http://www.python.org)
################################################################################

AC_PKG_PYTHON()
AC_PKG_QTLIB()

################################################################################
# Generate Output Files
################################################################################

AC_CONFIG_FILES(Makefile
    	    	message/Makefile
		libopenss-framework/Makefile
		libopenss-framework/dpcl/Makefile
		libopenss-framework/dyninst/Makefile
		libopenss-framework/runtime/Makefile
		libopenss-framework/runtime/dpcl/Makefile
		libopenss-framework/runtime/dyninst/Makefile
		plugins/Makefile
		plugins/collectors/Makefile
		plugins/collectors/pcsamp/Makefile
		plugins/collectors/usertime/Makefile
		examples/Makefile
		examples/libopenss-framework/Makefile
		examples/libopenss-framework/collector/Makefile
		examples/libopenss-framework/tool/Makefile
		openss/Makefile
		base/Makefile
		plugin/Makefile
		expr_info/Makefile
		objects/Makefile
		dialogs/Makefile
		chart/Makefile
		pcli/Makefile
		gui/Makefile
		Panels/Makefile
		Panels/pcSamplePanel/Makefile
		Panels/UserTimePanel/Makefile
		Panels/SourcePanel/Makefile
		Panels/CmdPanel/Makefile
		Panels/StatsPanel/Makefile
		Panels/ManageProcessesPanel/Makefile
		Panels/HW_CounterPanel/Makefile
		Panels/IOPanel/Makefile
		Panels/MPIPanel/Makefile
		Panels/DebugPanel/Makefile
		Panels/FPE_TracingPanel/Makefile
		Panels/CustomExperimentPanel/Makefile
		Panels/Wizards/Makefile
		Panels/Wizards/IntroWizardPanel/Makefile
		Panels/Wizards/pcSampleWizardPanel/Makefile
		Panels/Wizards/UserTimeWizardPanel/Makefile
		Panels/Wizards/FPE_TracingWizardPanel/Makefile
		Panels/Wizards/HW_CounterWizardPanel/Makefile
		Panels/Wizards/IOWizardPanel/Makefile
		Panels/Wizards/MPIWizardPanel/Makefile
		test/Makefile
		test/executables/Makefile
		test/executables/mutatee/Makefile
		test/executables/mutatee/c_version/Makefile
		test/executables/mutatee/cplus_version/Makefile
		test/executables/mutatee/f77_version/Makefile
		test/executables/Eon/Makefile
		test/test_source/Makefile
		test/test_source/unit/Makefile
		test/test_source/unit/fw/Makefile
		test/test_source/unit/fw/database/Makefile
		test/test_source/unit/fw/interval/Makefile
		test/test_source/unit/fw/locking/Makefile
		test/test_source/unit/pcli/Makefile
		test/test_source/unit/pcli/parse_result/Makefile
		test/test_source/unit/pcli/parse_target/Makefile
		test/test_source/dpcl/Makefile
		test/test_source/dpcl/one_shot/Makefile
		test/test_source/dpcl/probe_module/Makefile
		test/test_source/dpcl/point_probe/Makefile
		test/test_source/dpcl/start_terminate/Makefile
		test/test_source/dpcl/module_addr/Makefile
		test/test_source/dpcl/stmt_info/Makefile
		test/test_source/dpcl/phase/Makefile
		usability/Makefile
		usability/phaseII/Makefile
)

AC_OUTPUT
