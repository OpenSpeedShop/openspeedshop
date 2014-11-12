#################################################################################
# Copyright (c) 2013-2014 Krell Institute. All Rights Reserved.
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
#################################################################################

################################################################################
# Check for libiomp 
################################################################################

AC_DEFUN([AX_LIBIOMP], [

    AC_ARG_WITH(libiomp,
                AC_HELP_STRING([--with-libiomp=DIR],
                               [libiomp installation @<:@/usr@:>@]),
                libiomp_dir=$withval, libiomp_dir="/usr")

    found_libiomp=0

    if test "x${libiomp_dir}" == "x" || test "x${libiomp_dir}" == "x/usr" ; then
      LIBIOMP_DIR=""
      LIBIOMP_CPPFLAGS=""
      LIBIOMP_LDFLAGS=""
    else
      LIBIOMP_DIR="$libiomp_dir"
      LIBIOMP_CPPFLAGS="-I$libiomp_dir/include"
      LIBIOMP_LDFLAGS="-L$libiomp_dir/$abi_libdir"
    fi

    LIBIOMP_LIBS="-liomp5"

    libiomp_saved_CPPFLAGS=$CPPFLAGS
    libiomp_saved_LDFLAGS=$LDFLAGS
    libiomp_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $LIBIOMP_CPPFLAGS"
    LDFLAGS="$LDFLAGS $LIBIOMP_LDFLAGS"
    LIBS="$LIBIOMP_LIBS"

    AC_MSG_CHECKING([for ompt headers])

    if test -f $libiomp_dir/include/ompt.h; then
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_OMPT, true)
      AC_DEFINE(HAVE_OMPT, 1, [Define to 1 if you have OMPT.])
    else
      AC_MSG_RESULT(no)
      AM_CONDITIONAL(HAVE_OMPT, false)
    fi

    AC_MSG_CHECKING([for libiomp library])

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
        #include <ompt.h>
        ]], [[ ompt_function_lookup_t ompt_fn_lookup;
               const char *runtime_version;
               int ompt_version;
               ompt_initialize(ompt_fn_lookup, runtime_version, ompt_version);
        ]])], [ found_libiomp=1 ], [ found_libiomp=0 ])

    if test $found_libiomp -eq 1; then
        AC_MSG_RESULT(yes)
        AM_CONDITIONAL(HAVE_LIBIOMP, true)
        AC_DEFINE(HAVE_LIBIOMP, 1, [Define to 1 if you have LIBIOMP.])
    else
# FIXME.  if we expect libraries in spme specific directory then
# create an option to set it rather than override the not found above
# due to lib lib64 inconsistencies.
# Try again with the traditional library path (lib???) instead
         found_libiomp=0
         if test "x${libiomp_dir}" == "x" || test "x${libiomp_dir}" == "x/usr" ; then
           LIBIOMP_DIR=""
           LIBIOMP_CPPFLAGS=""
           LIBIOMP_LDFLAGS=""
         else
           LIBIOMP_DIR="$libiomp_dir"
           LIBIOMP_CPPFLAGS="-I$libiomp_dir/include"
           LIBIOMP_LDFLAGS="-L$libiomp_dir/$alt_abi_libdir"
         fi 

         CPPFLAGS=$libiomp_saved_CPPFLAGS 
         LDFLAGS=$libiomp_saved_LDFLAGS
         LIBS=$libiomp_saved_LIBS

         CPPFLAGS="$CPPFLAGS $LIBIOMP_CPPFLAGS"
         LDFLAGS="$LDFLAGS $LIBIOMP_LDFLAGS"
         LIBS="$LIBS $LIBIOMP_LIBS"


         AC_MSG_CHECKING([for libiomp library and headers])

         AC_LINK_IFELSE([AC_LANG_PROGRAM([[
             #include <ompt.h>
             ]], [[ ompt_function_lookup_t ompt_fn_lookup;
                    const char *runtime_version;
                    int ompt_version;
                    ompt_initialize(ompt_fn_lookup, runtime_version, ompt_version);
             ]])], [ found_libiomp=1 ], [ found_libiomp=0 ])

         if test $found_libiomp -eq 1; then
             AC_MSG_RESULT(yes)
             AM_CONDITIONAL(HAVE_LIBIOMP, true)
             AC_DEFINE(HAVE_LIBIOMP, 1, [Define to 1 if you have LIBIOMP.])
         else
             AC_MSG_RESULT(no)
             AM_CONDITIONAL(HAVE_LIBIOMP, false)
             AC_DEFINE(HAVE_LIBIOMP, 0, [Define to 0 if you do not have LIBIOMP.])
             LIBIOMP_CPPFLAGS=""
             LIBIOMP_LDFLAGS=""
             LIBIOMP_LIBS=""
             LIBIOMP_DIR=""
         fi
    fi

    CPPFLAGS=$libiomp_saved_CPPFLAGS 
    LDFLAGS=$libiomp_saved_LDFLAGS
    LIBS=$libiomp_saved_LIBS

    AC_SUBST(LIBIOMP_DIR)
    AC_SUBST(LIBIOMP_CPPFLAGS)
    AC_SUBST(LIBIOMP_LDFLAGS)
    AC_SUBST(LIBIOMP_LIBS)

])

