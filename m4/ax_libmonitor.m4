################################################################################
# Copyright (c) 2006-2014 Krell Institute. All Rights Reserved.
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
# Check for Monitor (http://www.cs.utk.edu/~mucci)
################################################################################

AC_DEFUN([AX_LIBMONITOR], [

    AC_ARG_WITH(libmonitor,
                AC_HELP_STRING([--with-libmonitor=DIR],
                               [libmonitor installation @<:@/usr@:>@]),
                libmonitor_dir=$withval, libmonitor_dir="/usr")

    LIBMONITOR_CPPFLAGS="-I$libmonitor_dir/include"
    LIBMONITOR_DIR="$libmonitor_dir"
    LIBMONITOR_LIBS="-lmonitor"

    if test -f $libmonitor_dir/$abi_libdir/libmonitor.so -o -f $libmonitor_dir/$abi_libdir/libmonitor_wrap.a ; then
       LIBMONITOR_LIBDIR="$libmonitor_dir/$abi_libdir"
       LIBMONITOR_LDFLAGS="-L$libmonitor_dir/$abi_libdir"
    elif test -f  $libmonitor_dir/$alt_abi_libdir/libmonitor.so -o -f $libmonitor_dir/$alt_abi_libdir/libmonitor_wrap.a ; then
       LIBMONITOR_LIBDIR="$libmonitor_dir/$alt_abi_libdir"
       LIBMONITOR_LDFLAGS="-L$libmonitor_dir/$alt_abi_libdir"
    fi

    libmonitor_saved_CPPFLAGS=$CPPFLAGS
    libmonitor_saved_LDFLAGS=$LDFLAGS
    libmonitor_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $LIBMONITOR_CPPFLAGS"
    LDFLAGS="$LDFLAGS $LIBMONITOR_LDFLAGS"
    if test -f /usr/$abi_libdir/x86_64-linux-gnu/libpthread.a ; then
       LIBS="$LIBMONITOR_LIBS /usr/$abi_libdir/x86_64-linux-gnu/libpthread.a"
    elif test -f /usr/$alt_abi_libdir/x86_64-linux-gnu/libpthread.a ; then
       LIBS="$LIBMONITOR_LIBS /usr/$alt_abi_libdir/x86_64-linux-gnu/libpthread.a"
    else
       LIBS="$LIBMONITOR_LIBS -lpthread"
    fi

    AC_MSG_CHECKING([for libmonitor library and headers])

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
        #include <monitor.h>
        ]], [[
        monitor_init_library();
        ]])], [ AC_MSG_RESULT(yes)

            AM_CONDITIONAL(HAVE_LIBMONITOR, true)
            AC_DEFINE(HAVE_LIBMONITOR, 1, [Define to 1 if you have libmonitor.])

        ], [ AC_MSG_RESULT(no)

            AM_CONDITIONAL(HAVE_LIBMONITOR, false)
            LIBMONITOR_CPPFLAGS=""
            LIBMONITOR_LDFLAGS=""
            LIBMONITOR_LIBS=""
            LIBMONITOR_DIR=""

        ]
    )

    CPPFLAGS=$libmonitor_saved_CPPFLAGS
    LDFLAGS=$libmonitor_saved_LDFLAGS
    LIBS=$libmonitor_saved_LIBS

    AC_SUBST(LIBMONITOR_CPPFLAGS)
    AC_SUBST(LIBMONITOR_LDFLAGS)
    AC_SUBST(LIBMONITOR_LIBS)
    AC_SUBST(LIBMONITOR_DIR)
    AC_SUBST(LIBMONITOR_LIBDIR)

])

################################################################################
# Check for Monitor for Target Architecture (http://www.cs.utk.edu/~mucci)
################################################################################

AC_DEFUN([AX_TARGET_LIBMONITOR], [

    AC_ARG_WITH(target-libmonitor,
                AC_HELP_STRING([--with-target-libmonitor=DIR],
                               [libmonitor target architecture installation @<:@/opt@:>@]),
                target_libmonitor_dir=$withval, target_libmonitor_dir="/zzz")

    AC_MSG_CHECKING([for Targetted libmonitor support])

    found_target_libmonitor=0
    if test -f $target_libmonitor_dir/$abi_libdir/libmonitor.so -o -f $target_libmonitor_dir/$abi_libdir/libmonitor_wrap.a ; then
       found_target_libmonitor=1
       TARGET_LIBMONITOR_LDFLAGS="-L$target_libmonitor_dir/$abi_libdir"
       TARGET_LIBMONITOR_LIBDIR="$target_libmonitor_dir/$abi_libdir"
    elif test -f  $target_libmonitor_dir/$alt_abi_libdir/libmonitor.so -o -f $target_libmonitor_dir/$alt_abi_libdir/libmonitor_wrap.a ; then
       found_target_libmonitor=1
       TARGET_LIBMONITOR_LDFLAGS="-L$target_libmonitor_dir/$alt_abi_libdir"
       TARGET_LIBMONITOR_LIBDIR="$target_libmonitor_dir/$alt_abi_libdir"
    fi

    if test "$target_libmonitor_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_LIBMONITOR, false)
      TARGET_LIBMONITOR_CPPFLAGS=""
      TARGET_LIBMONITOR_LDFLAGS=""
      TARGET_LIBMONITOR_LIBS=""
      TARGET_LIBMONITOR_DIR=""
      TARGET_LIBMONITOR_LIBDIR=""
      AC_MSG_RESULT(no)
    elif test $found_target_libmonitor == 1 ; then
      AM_CONDITIONAL(HAVE_TARGET_LIBMONITOR, true)
      AC_DEFINE(HAVE_TARGET_LIBMONITOR, 1, [Define to 1 if you have a target version of LIBMONITOR.])
      TARGET_LIBMONITOR_CPPFLAGS="-I$target_libmonitor_dir/include"
      TARGET_LIBMONITOR_LDFLAGS="-L$target_libmonitor_dir/$abi_libdir"
      TARGET_LIBMONITOR_LIBS="-lmonitor"
      TARGET_LIBMONITOR_DIR="$target_libmonitor_dir"
      AC_MSG_RESULT(yes)
    else
      AM_CONDITIONAL(HAVE_TARGET_LIBMONITOR, false)
      TARGET_LIBMONITOR_CPPFLAGS=""
      TARGET_LIBMONITOR_LDFLAGS=""
      TARGET_LIBMONITOR_LIBS=""
      TARGET_LIBMONITOR_DIR=""
      TARGET_LIBMONITOR_LIBDIR=""
      AC_MSG_RESULT(no)
    fi

    AC_SUBST(TARGET_LIBMONITOR_CPPFLAGS)
    AC_SUBST(TARGET_LIBMONITOR_LDFLAGS)
    AC_SUBST(TARGET_LIBMONITOR_LIBS)
    AC_SUBST(TARGET_LIBMONITOR_DIR)
    AC_SUBST(TARGET_LIBMONITOR_LIBDIR)

])

