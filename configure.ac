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

AC_PROG_CXX

AC_DISABLE_STATIC
AC_LIBLTDL_INSTALLABLE
AC_SUBST(LTDLINCL)
AC_SUBST(LIBLTDL)
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_CONFIG_SUBDIRS(libltdl)

AC_PROG_INSTALL

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
# Check for OpenMP (http://www.openmp.org)
################################################################################

AC_CHECK_HEADER(omp.h, [
    AC_DEFINE(HAVE_OPENMP, 1, [Define to 1 if you have OpenMP.])
])

################################################################################
# Check for MPI (http://www.mpi-forum.org)
################################################################################

AC_CHECK_HEADER(mpi.h, [ AC_CHECK_LIB(mpi, MPI_Init, [
    AC_DEFINE(HAVE_MPI, 1, [Define to 1 if you have MPI.])
]) ])

################################################################################
# Check for SGI Array Services (proprietary)
################################################################################

AC_CHECK_HEADER(arraysvcs.h, [ AC_CHECK_LIB(array, asgeterror, [
    AC_DEFINE(HAVE_ARRAY_SERVICES, 1, [Define to 1 if you have array services.])

    case "$host" in
        *linux*)
            CPPFLAGS="$CPPFLAGS -DLINUX"
            ;;
    esac

]) ])

################################################################################
# Generate Output Files
################################################################################

AC_CONFIG_FILES(Makefile
		libopenss-framework/Makefile
		libopenss-framework/dpcl/Makefile
		libopenss-framework/dyninst/Makefile
		plugins/Makefile
		plugins/collectors/Makefile
		plugins/collectors/pcsamp/Makefile
		examples/Makefile
		examples/libopenss-framework/Makefile
		examples/libopenss-framework/collector/Makefile
		examples/libopenss-framework/tool/Makefile
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
		Panels/TopPanel/Makefile
		Panels/StatsPanel/Makefile
		Panels/PerformancePanel/Makefile
		Panels/DebugPanel/Makefile
		Panels/HW_CounterPanel/Makefile
		Panels/IOPanel/Makefile
		Panels/MPIPanel/Makefile
		Panels/FPE_TracingPanel/Makefile
		Panels/ConstructNewExperimentPanel/Makefile
		Panels/Wizards/Makefile
		Panels/Wizards/IntroWizardPanel/Makefile
		Panels/Wizards/pcSampleWizardPanel/Makefile
		Panels/Wizards/UserTimeWizardPanel/Makefile
		Panels/Wizards/FPE_TracingWizardPanel/Makefile
		Panels/Wizards/HW_CounterWizardPanel/Makefile
		Panels/Wizards/IOWizardPanel/Makefile
		Panels/Wizards/MPIWizardPanel/Makefile
		tests/fw/database/Makefile
)

AC_OUTPUT
