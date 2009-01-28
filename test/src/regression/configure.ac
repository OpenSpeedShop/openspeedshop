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

AC_PROG_CC()
AC_PROG_CXX()
AC_PROG_F77
AC_PROG_FC

if test "$ac_cv_f77_compiler_gnu" = no; then
        AM_CONDITIONAL(HAVE_F77, false)
        AC_DEFINE(HAVE_F77, 0, [Define to 0 if you do not have F77.])
else
        AM_CONDITIONAL(HAVE_F77, true)
        AC_DEFINE(HAVE_F77, 1, [Define to 1 if you have F77.])
fi

AC_PROG_LIBTOOL


################################################################################
# Handle X86-64 Special Case
################################################################################

case "$host" in
    x86_64-*-linux*)
        if test x"$libdir" == x'${topdir_execprefix}/lib'; then
            libdir='${topdir_execprefix}/lib64'
        fi
        abi_libdir="lib64"
        LDFLAGS="-L/usr/lib64 $LDFLAGS"
        ;;
    *)
        abi_libdir="lib"
        ;;
esac


################################################################################
# Check for Additional Packages
################################################################################

AC_PKG_LIBDWARF()
AC_PKG_DYNINST()

################################################################################
# Check for Instrumentor
################################################################################

AC_ARG_WITH(instrumentor,
            AC_HELP_STRING([--with-instrumentor=TYPE],
                           [instrumentor (dpcl, dyninst, mrnet, offline) @<:@dpcl@:>@]),
            INSTRUMENTOR=$withval, INSTRUMENTOR="dpcl")

case "$INSTRUMENTOR" in
    dpcl)
        AC_PKG_DYNINST()
        ;;

    dyninst)
        AC_PKG_DYNINST()
	;;

    mrnet)
        AC_PKG_DYNINST()
        ;;

    offline)
        ;;

    *)
        AC_MSG_ERROR([invalid instrumentor "$INSTRUMENTOR".])
        ;;
esac

AC_SUBST(INSTRUMENTOR)


################################################################################
# Generate Output Files
################################################################################

AC_CONFIG_FILES([
	Makefile	
	bb/Makefile
	bb/close/Makefile
	bb/close/close1/Makefile
	bb/close/close2/Makefile
	bb/close/close3/Makefile
	bb/close/close4/Makefile
	bb/close/close5/Makefile
	bb/detach/Makefile
	bb/detach/detach1/Makefile
	bb/detach/detach2/Makefile
	bb/detach/detach3/Makefile
	bb/detach/detach4/Makefile
	bb/detach/detach5/Makefile
	bb/disable/Makefile
	bb/disable/disable1/Makefile
	bb/disable/disable2/Makefile
	bb/disable/disable3/Makefile
	bb/disable/disable4/Makefile
	bb/disable/disable5/Makefile
	bb/enable/Makefile
	bb/enable/enable1/Makefile
	bb/enable/enable2/Makefile
	bb/enable/enable3/Makefile
	bb/enable/enable4/Makefile
	bb/enable/enable5/Makefile
	bb/focus/Makefile
	bb/focus/focus1/Makefile
	bb/focus/focus2/Makefile
	bb/focus/focus3/Makefile
	bb/focus/focus4/Makefile
	bb/focus/focus5/Makefile
	bb/pause/Makefile
	bb/pause/pause1/Makefile
	bb/pause/pause2/Makefile
	bb/pause/pause3/Makefile
	bb/pause/pause4/Makefile
	bb/pause/pause5/Makefile
	bb/restore/Makefile
	bb/restore/restore1/Makefile
	bb/restore/restore2/Makefile
	bb/restore/restore3/Makefile
	bb/restore/restore4/Makefile
	bb/restore/restore5/Makefile
	fpe/Makefile
	fpe/fpe1/Makefile
	inf/Makefile
	inf/hosts/Makefile
	inf/hosts/hosts1/Makefile
	inf/hosts/hosts2/Makefile
	inf/hosts/hosts3/Makefile
	inf/hosts/hosts4/Makefile
	inf/listexp/Makefile
	inf/listexp/listexp1/Makefile
	inf/listexp/listexp2/Makefile
	inf/listexp/listexp3/Makefile
	inf/listexp/listexp4/Makefile
	inf/views/Makefile
	inf/views/views1/Makefile
	inf/views/views2/Makefile
	inf/views/views3/Makefile
	inf/views/views4/Makefile
	inf/types/Makefile
	inf/types/types1/Makefile
	inf/types/types2/Makefile
	inf/types/types3/Makefile
	inf/types/types4/Makefile
	inf/metrics/Makefile
	inf/metrics/metrics1/Makefile
	inf/metrics/metrics2/Makefile
	inf/metrics/metrics3/Makefile
	inf/metrics/metrics4/Makefile
	inf/params/Makefile
	inf/params/params1/Makefile
	inf/params/params2/Makefile
	inf/params/params3/Makefile
	inf/params/params4/Makefile
	inf/pids/Makefile
	inf/pids/pids1/Makefile
	inf/pids/pids2/Makefile
	inf/pids/pids3/Makefile
	inf/pids/pids4/Makefile
	inf/status/Makefile
	inf/status/status1/Makefile
	inf/status/status2/Makefile
	inf/status/status3/Makefile
	inf/status/status4/Makefile
	io/Makefile
	io/io1/Makefile
	pcsamp/Makefile
	pcsamp/pcsamp1/Makefile
	pcsamp/pcsamp2/Makefile
	pcsamp/pcsamp3/Makefile
	pvs/Makefile
	pvs/939887/Makefile
	pvs/939885/Makefile	
	hwc/Makefile
	hwc/hwc1/Makefile
	hwc/hwc2/Makefile
	hwc/hwc3/Makefile
	hwctime/Makefile
	hwctime/hwctime1/Makefile
	hwctime/hwctime2/Makefile
	hwctime/hwctime3/Makefile
	usertime/Makefile
	usertime/usertime1/Makefile
	usertime/usertime2/Makefile
	usertime/usertime3/Makefile
	usertime/usertime4/Makefile
	usertime/usertime5/Makefile
	usertime/usertime6/Makefile
	mpi/Makefile
	mpi/mpi1/Makefile
	mpi/smg2000/Makefile
	mpi/smg2000/smg1/Makefile
	views/Makefile
	views/io/Makefile
	views/io/calltrees/Makefile
	views/io/calltrees/call1/Makefile
	views/io/calltrees/call2/Makefile
	views/io/calltrees/call3/Makefile
	views/io/functions/Makefile
	views/io/functions/functions1/Makefile
	views/io/functions/functions2/Makefile
	views/io/trace/Makefile
	views/io/trace/trace1/Makefile
	views/io/tracebacks/Makefile
	views/io/tracebacks/tb1/Makefile
	views/io/tracebacks/tb2/Makefile
	views/io/tracebacks/tb3/Makefile
	views/pcsamp/Makefile
	views/pcsamp/functions/Makefile
	views/pcsamp/functions/functions1/Makefile
	views/pcsamp/linked_objects/Makefile
	views/pcsamp/linked_objects/lo1/Makefile
	views/pcsamp/statements/Makefile
	views/pcsamp/statements/state1/Makefile

])

AC_OUTPUT
