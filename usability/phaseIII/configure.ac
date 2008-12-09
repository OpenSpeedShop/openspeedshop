################################################################################
# Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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

if test "$ac_cv_f77_compiler_gnu" = no; then
        AM_CONDITIONAL(HAVE_F77, false)
else
        AM_CONDITIONAL(HAVE_F77, true)
        AC_DEFINE(HAVE_F77, 1, [Define to 1 if you have F77.])
fi

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


################################################################################
# Check for Additional Packages
################################################################################

AC_PKG_MPI()

################################################################################
# Generate Output Files
################################################################################

AC_CONFIG_FILES([
        Makefile
        smg2000A/Makefile
        smg2000A/krylov/Makefile
        smg2000A/struct_ls/Makefile
        smg2000A/struct_mv/Makefile
        smg2000A/utilities/Makefile
        smg2000A/test/Makefile
        smg2000B/Makefile
        smg2000B/krylov/Makefile
        smg2000B/struct_ls/Makefile
        smg2000B/struct_mv/Makefile
        smg2000B/utilities/Makefile
        smg2000B/test/Makefile
])

AC_OUTPUT
