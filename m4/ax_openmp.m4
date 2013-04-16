################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2007 William Hachfeld. All Rights Reserved.
# Copyright (c) 2006-2013 Krell Institute. All Rights Reserved.
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
# Check for OpenMP (http://www.openmp.org)
################################################################################

AC_DEFUN([AX_OPENMP], [

    OPENMP_CPPFLAGS="-fopenmp"
    OPENMP_LDFLAGS=""
    OPENMP_LIBS="-lgomp"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    openmp_saved_CPPFLAGS=$CPPFLAGS
    openmp_saved_LDFLAGS=$LDFLAGS
    openmp_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $OPENMP_CPPFLAGS"
    LDFLAGS="$LDFLAGS $OPENMP_LDFLAGS"
    LIBS="$OPENMP_LIBS"

    AC_MSG_CHECKING([for OpenMP support])

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
	#include <omp.h>
	#include <stdio.h>
        ]], [[
	#pragma omp parallel for
	for(int i = 0; i < 100; ++i)
	    printf("[%d] i = %d\n", omp_get_thread_num(), i);
        ]])], [ AC_MSG_RESULT(yes)

            AM_CONDITIONAL(HAVE_OPENMP, true)
	    AC_DEFINE(HAVE_OPENMP, 1, [Define to 1 if you have OpenMP.])

        ], [ AC_MSG_RESULT(no)

            AM_CONDITIONAL(HAVE_OPENMP, false)
	    OPENMP_CPPFLAGS=""
            OPENMP_LDFLAGS=""
            OPENMP_LIBS=""

        ]
    )

    CPPFLAGS=$openmp_saved_CPPFLAGS
    LDFLAGS=$openmp_saved_LDFLAGS
    LIBS=$openmp_saved_LIBS

    AC_LANG_POP(C++)

    AC_SUBST(OPENMP_CPPFLAGS)
    AC_SUBST(OPENMP_LDFLAGS)
    AC_SUBST(OPENMP_LIBS)

])


################################################################################
# Check for Target OpenMP (http://www.openmp.org)
################################################################################

AC_DEFUN([AX_TARGET_OPENMP], [

    AC_ARG_WITH(target-openmp,
                AC_HELP_STRING([--with-target-openmp=DIR],
                               [Targetted OpenMP  library installation @<:@/usr@:>@]),
                target_openmp_dir=$withval, target_openmp_dir="/usr")

    TARGET_OPENMP_DIR="$target_openmp_dir"
    TARGET_OPENMP_CPPFLAGS="-fopenmp"
    TARGET_OPENMP_LIBS="-lgomp"


    #OPENMP_CPPFLAGS="-fopenmp"
    #OPENMP_LDFLAGS=""
    #OPENMP_LIBS="-lgomp"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    openmp_saved_CPPFLAGS=$CPPFLAGS
    openmp_saved_LDFLAGS=$LDFLAGS


    AC_MSG_CHECKING([for Targetted OpenMP support])

    found_target_openmp=0
    if test -f $target_openmp_dir/$abi_libdir/libgomp.so; then
       TARGET_OPENMP_LIBSDIR="$target_openmp_dir/$abi_libdir"
       TARGET_OPENMP_LDFLAGS="-L$target_openmp_dir/$abi_libdir"
       AC_MSG_CHECKING([found Targetted OTF library])
       found_target_openmp=1
    elif test -f $target_openmp_dir/$alt_abi_libdir/libgomp.so; then
       TARGET_OPENMP_LIBSDIR="$target_openmp_dir/$alt_abi_libdir"
       TARGET_OPENMP_LDFLAGS="-L$target_openmp_dir/$alt_abi_libdir"
       AC_MSG_CHECKING([found Targetted OTF library])
       found_target_openmp=1
    fi

    CPPFLAGS="$CPPFLAGS $OPENMP_CPPFLAGS"
    LDFLAGS="$LDFLAGS $OPENMP_LDFLAGS $OPENMP_LIBS"


    CPPFLAGS=$openmp_saved_CPPFLAGS
    LDFLAGS=$openmp_saved_LDFLAGS

    AC_LANG_POP(C++)

    if test $found_target_openmp == 1; then
      AC_MSG_CHECKING([found all Targetted OPENMP library])
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_TARGET_OPENMP, true)
      AC_DEFINE(HAVE_TARGET_OPENMP, 1, [Define to 1 if you have OPENMP.])
    else
      AC_MSG_CHECKING([FAILED to find all Targetted OPENMP library])
      AC_MSG_RESULT(no)
      AM_CONDITIONAL(HAVE_TARGET_OPENMP, false)
      TARGET_OPENMP_DIR=""
      TARGET_OPENMP_LIBSDIR=""
      TARGET_OPENMP_CPPFLAGS=""
      TARGET_OPENMP_LDFLAGS=""
      TARGET_OPENMP_LIBS=""
    fi

    AC_SUBST(TARGET_OPENMP_DIR)
    AC_SUBST(TARGET_OPENMP_LIBSDIR)
    AC_SUBST(TARGET_OPENMP_CPPFLAGS)
    AC_SUBST(TARGET_OPENMP_LDFLAGS)
    AC_SUBST(TARGET_OPENMP_LIBS)

])
