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


################################################################################
# Check for MPI (http://www.mpi-forum.org/)
################################################################################

AC_DEFUN([AC_PKG_MPI], [

    AC_ARG_WITH(mpi,
                AC_HELP_STRING([--with-mpi=DIR],
                               [MPI installation @<:@/usr@:>@]),
                mpi_dir=$withval, mpi_dir="/usr")

    AC_ARG_WITH(mpich-driver,
                AC_HELP_STRING([--with-mpich-driver=NAME],
                               [MPICH driver name @<:@ch-p4@:>@]),
                mpich_driver=$withval, mpich_driver="ch-p4")

    AC_MSG_CHECKING([for MPI library and headers])

    mpi_saved_CPPFLAGS=$CPPFLAGS
    mpi_saved_LDFLAGS=$LDFLAGS

    MPI_CPPFLAGS="-I$mpi_dir/include"
    MPI_LDFLAGS="-L$mpi_dir/$mpich_driver/$abi_libdir/shared"
    MPI_LIBS="-lmpich"
    MPI_HEADER="$mpi_dir/include/mpi.h"
    MPI_DIR="$mpi_dir"
    AM_CONDITIONAL(HAVE_MPICH, true)
    AC_DEFINE(HAVE_MPICH, 1, [Define to 1 if you have MPICH.])

    CPPFLAGS="$CPPFLAGS $MPI_CPPFLAGS"
    LDFLAGS="$LDFLAGS $MPI_LDFLAGS $MPI_LIBS"

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <mpi.h>
        ]], [[
        MPI_Initialized((int*)0);
        ]]), , [

            MPI_CPPFLAGS=""
            MPI_LDFLAGS=""
            MPI_LIBS=""
            MPI_HEADER=""
            MPI_DIR=""
            AM_CONDITIONAL(HAVE_MPICH, false)
            AC_DEFINE(HAVE_MPICH, 0, [Define to 1 if you have MPICH.])

        ]
    )

    if test x"$MPI_LIBS" == x""; then

        MPI_CPPFLAGS="-I$mpi_dir/include"
        MPI_LDFLAGS="-L$mpi_dir/$abi_libdir"
        MPI_LIBS="-lmpi"
        MPI_HEADER="$mpi_dir/include/mpi.h"
        MPI_DIR="$mpi_dir"
        AM_CONDITIONAL(HAVE_MPICH, false)
        AC_DEFINE(HAVE_MPICH, 0, [Define to 1 if you have MPICH.])

        CPPFLAGS="$mpi_saved_CPPFLAGS $MPI_CPPFLAGS"
        LDFLAGS="$mpi_saved_LDFLAGS $MPI_LDFLAGS $MPI_LIBS"

        AC_LINK_IFELSE(AC_LANG_PROGRAM([[
            #include <mpi.h>
            ]], [[
            MPI_Initialized((int*)0);
            ]]), , [

                MPI_CPPFLAGS=""
                MPI_LDFLAGS=""
                MPI_LIBS=""
                MPI_HEADER=""
                MPI_DIR=""
                AM_CONDITIONAL(HAVE_MPICH, false)
                AC_DEFINE(HAVE_MPICH, 0, [Define to 1 if you have MPICH.])

            ]
        )

    fi

    CPPFLAGS=$mpi_saved_CPPFLAGS
    LDFLAGS=$mpi_saved_LDFLAGS

    if test x"$MPI_LIBS" == x""; then
        AC_MSG_RESULT(no)
        AM_CONDITIONAL(HAVE_MPI, false)
    else
        AC_MSG_RESULT(yes)
        AM_CONDITIONAL(HAVE_MPI, true)
        AC_DEFINE(HAVE_MPI, 1, [Define to 1 if you have MPI.])
    fi

    AC_SUBST(MPI_CPPFLAGS)
    AC_SUBST(MPI_LDFLAGS)
    AC_SUBST(MPI_LIBS)
    AC_SUBST(MPI_HEADER)
    AC_SUBST(MPI_DIR)

])



