################################################################################
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
# Check for Libunwind (http://www.hpl.hp.com/research/linux/libunwind)
################################################################################

AC_DEFUN([AX_LIBUNWIND], [

    AC_ARG_WITH(libunwind,
                AC_HELP_STRING([--with-libunwind=DIR],
                               [libunwind installation @<:@/usr@:>@]),
                libunwind_dir=$withval, libunwind_dir="/usr")

    LIBUNWIND_CPPFLAGS="-I$libunwind_dir/include -DUNW_LOCAL_ONLY"
    LIBUNWIND_LDFLAGS="-L$libunwind_dir/$abi_libdir"
    LIBUNWIND_LIBS="-lunwind"
    LIBUNWIND_DIR="$libunwind_dir"

    libunwind_saved_CPPFLAGS=$CPPFLAGS
    libunwind_saved_LDFLAGS=$LDFLAGS
    libunwind_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $LIBUNWIND_CPPFLAGS"
    LDFLAGS="$LDFLAGS $LIBUNWIND_LDFLAGS"
    LIBS="$LIBUNWIND_LIBS"

    AC_MSG_CHECKING([for libunwind library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <libunwind.h>
        ]], [[
        unw_init_local((void*)0, (void*)0);
        ]]), [ AC_MSG_RESULT(yes)

            AM_CONDITIONAL(HAVE_LIBUNWIND, true)
            AC_DEFINE(HAVE_LIBUNWIND, 1, [Define to 1 if you have libunwind.])

        ], [ AC_MSG_RESULT(no)

            AM_CONDITIONAL(HAVE_LIBUNWIND, false)
            LIBUNWIND_CPPFLAGS=""
            LIBUNWIND_LDFLAGS=""
            LIBUNWIND_LIBS=""
            LIBUNWIND_DIR=""

        ]
    )

    CPPFLAGS=$libunwind_saved_CPPFLAGS
    LDFLAGS=$libunwind_saved_LDFLAGS
    LIBS=$libunwind_saved_LIBS

    AC_SUBST(LIBUNWIND_CPPFLAGS)
    AC_SUBST(LIBUNWIND_LDFLAGS)
    AC_SUBST(LIBUNWIND_LIBS)
    AC_SUBST(LIBUNWIND_DIR)

])

#############################################################################################
# Check for Libunwind for Target Architecture (http://www.hpl.hp.com/research/linux/libunwind)
#############################################################################################

AC_DEFUN([AX_TARGET_LIBUNWIND], [

    AC_ARG_WITH(target-libunwind,
                AC_HELP_STRING([--with-target-libunwind=DIR],
                               [libunwind target architecture installation @<:@/opt@:>@]),
                target_libunwind_dir=$withval, target_libunwind_dir="/zzz")

    AC_MSG_CHECKING([for Targetted libunwind support])

    found_target_libunwind=0
    if test -f  $target_libunwind_dir/$abi_libdir/libunwind.so -o -f $target_libunwind_dir/$abi_libdir/libunwind.a; then
       found_target_libunwind=1
       TARGET_LIBUNWIND_LDFLAGS="-L$target_libunwind_dir/$abi_libdir"
    elif test -f  $target_libunwind_dir/$alt_abi_libdir/libunwind.so -o -f $target_libunwind_dir/$alt_abi_libdir/libunwind.a; then
       found_target_libunwind=1
       TARGET_LIBUNWIND_LDFLAGS="-L$target_libunwind_dir/$alt_abi_libdir"
    fi

    if test $found_target_libunwind == 0 && test "$target_libunwind_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_LIBUNWIND, false)
      TARGET_LIBUNWIND_CPPFLAGS=""
      TARGET_LIBUNWIND_LDFLAGS=""
      TARGET_LIBUNWIND_LIBS=""
      TARGET_LIBUNWIND_DIR=""
      AC_MSG_RESULT(no)
    elif test $found_target_libunwind == 1 ; then
      AM_CONDITIONAL(HAVE_TARGET_LIBUNWIND, true)
      AC_DEFINE(HAVE_TARGET_LIBUNWIND, 1, [Define to 1 if you have a target version of LIBUNWIND.])
      TARGET_LIBUNWIND_CPPFLAGS="-I$target_libunwind_dir/include -DUNW_LOCAL_ONLY"
      TARGET_LIBUNWIND_LDFLAGS="-L$target_libunwind_dir/$abi_libdir"
      TARGET_LIBUNWIND_LIBS="-lunwind"
      TARGET_LIBUNWIND_DIR="$target_libunwind_dir"
      AC_MSG_RESULT(yes)
    else
      AM_CONDITIONAL(HAVE_TARGET_LIBUNWIND, false)
      TARGET_LIBUNWIND_CPPFLAGS=""
      TARGET_LIBUNWIND_LDFLAGS=""
      TARGET_LIBUNWIND_LIBS=""
      TARGET_LIBUNWIND_DIR=""
      AC_MSG_RESULT(no)
    fi

    AC_SUBST(TARGET_LIBUNWIND_CPPFLAGS)
    AC_SUBST(TARGET_LIBUNWIND_LDFLAGS)
    AC_SUBST(TARGET_LIBUNWIND_LIBS)
    AC_SUBST(TARGET_LIBUNWIND_DIR)
])
