#################################################################################
# Copyright (c) 2010-2012 Krell Institute. All Rights Reserved.
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
# Check for libdwarf (http://www.reality.sgiweb.org/davea/dwarf.html)
################################################################################

AC_DEFUN([AX_LIBDWARF], [

    AC_ARG_WITH(libdwarf,
                AC_HELP_STRING([--with-libdwarf=DIR],
                               [libdwarf installation @<:@/usr@:>@]),
                libdwarf_dir=$withval, libdwarf_dir="/usr")

    found_libdwarf=0

    LIBDWARF_CPPFLAGS="-I$libdwarf_dir/include"
    LIBDWARF_LDFLAGS="-L$libdwarf_dir/$abi_libdir"
    LIBDWARF_LIBS="-ldwarf"

    libdwarf_saved_CPPFLAGS=$CPPFLAGS
    libdwarf_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $LIBDWARF_CPPFLAGS"
    LDFLAGS="$LDFLAGS $LIBDWARF_LDFLAGS $LIBDWARF_LIBS -lelf -lpthread"

    AC_MSG_CHECKING([for libdwarf library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <dwarf.h>
        ]], [[
        if (DW_ID_up_case != DW_ID_down_case) {
           int mycase = DW_ID_up_case;
        }
        ]]), [ found_libdwarf=1 ], [ found_libdwarf=0 ])

    if test $found_libdwarf -eq 1; then
        AC_MSG_RESULT(yes)
        AM_CONDITIONAL(HAVE_LIBDWARF, true)
        AC_DEFINE(HAVE_LIBDWARF, 1, [Define to 1 if you have LIBDWARF.])
    else
# Try again with the traditional path instead
         found_libdwarf=0
         LIBDWARF_CPPFLAGS="-I$libdwarf_dir/include"
         LIBDWARF_LDFLAGS="-L$libdwarf_dir/$abi_libdir"

         CPPFLAGS="$CPPFLAGS $LIBDWARF_CPPFLAGS"
         LDFLAGS="$LDFLAGS $LIBDWARF_LDFLAGS $LIBDWARF_LIBS -lelf -lpthread"

         AC_MSG_CHECKING([for libdwarf library and headers])

         AC_LINK_IFELSE(AC_LANG_PROGRAM([[
             #include <dwarf.h>
             ]], [[
             if (DW_ID_up_case != DW_ID_down_case) {
                int mycase = DW_ID_up_case;
             }
             ]]), [ found_libdwarf=1

             ], [ found_libdwarf=0 ])

         if test $found_libdwarf -eq 1; then
             AC_MSG_RESULT(yes)
             AM_CONDITIONAL(HAVE_LIBDWARF, true)
             AC_DEFINE(HAVE_LIBDWARF, 1, [Define to 1 if you have LIBDWARF.])
         else
             AC_MSG_RESULT(no)
             AM_CONDITIONAL(HAVE_LIBDWARF, false)
             AC_DEFINE(HAVE_LIBDWARF, 0, [Define to 0 if you do not have LIBDWARF.])
             LIBDWARF_CPPFLAGS=""
             LIBDWARF_LDFLAGS=""
             LIBDWARF_LIBS=""
         fi
    fi

    CPPFLAGS=$libdwarf_saved_CPPFLAGS LDFLAGS=$libdwarf_saved_LDFLAGS

    AC_SUBST(LIBDWARF_CPPFLAGS)
    AC_SUBST(LIBDWARF_LDFLAGS)
    AC_SUBST(LIBDWARF_LIBS)

])


#######################################################################################
# Check for libdwarf for Target Architecture (http://www.reality.sgiweb.org/davea/dwarf.html)
#######################################################################################

AC_DEFUN([AC_PKG_TARGET_LIBDWARF], [

    AC_ARG_WITH(target-libdwarf,
                AC_HELP_STRING([--with-target-libdwarf=DIR],
                               [libdwarf target architecture installation @<:@/opt@:>@]),
                target_libdwarf_dir=$withval, target_libdwarf_dir="/zzz")

    AC_MSG_CHECKING([for Targetted libdwarf support])


    found_target_libdwarf=0
    if test -f  $target_libdwarf_dir/$abi_libdir/libdwarf.so -o -f $target_libdwarf_dir/$abi_libdir/libdwarf.a; then
       found_target_libdwarf=1
       TARGET_LIBDWARF_LDFLAGS="-L$target_libdwarf_dir/$abi_libdir"
    elif test -f  $target_libdwarf_dir/$alt_abi_libdir/libdwarf.so -o -f $target_libdwarf_dir/$alt_abi_libdir/libdwarf.a; then
       found_target_libdwarf=1
       TARGET_LIBDWARF_LDFLAGS="-L$target_libdwarf_dir/$alt_abi_libdir"
    fi

    if test $found_target_libdwarf == 0 && test "$target_libdwarf_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_LIBDWARF, false)
      TARGET_LIBDWARF_CPPFLAGS=""
      TARGET_LIBDWARF_LDFLAGS=""
      TARGET_LIBDWARF_LIBS=""
      TARGET_LIBDWARF_DIR=""
      AC_MSG_RESULT(no)
    elif test $found_target_libdwarf == 1 ; then
      AM_CONDITIONAL(HAVE_TARGET_LIBDWARF, true)
      AC_DEFINE(HAVE_TARGET_LIBDWARF, 1, [Define to 1 if you have a target version of LIBDWARF.])
      TARGET_LIBDWARF_CPPFLAGS="-I$target_libdwarf_dir/include"
      TARGET_LIBDWARF_LIBS="-ldwarf"
      TARGET_LIBDWARF_DIR="$target_libdwarf_dir"
      AC_MSG_RESULT(yes)
    else
      AM_CONDITIONAL(HAVE_TARGET_LIBDWARF, false)
      TARGET_LIBDWARF_CPPFLAGS=""
      TARGET_LIBDWARF_LDFLAGS=""
      TARGET_LIBDWARF_LIBS=""
      TARGET_LIBDWARF_DIR=""
      AC_MSG_RESULT(no)
    fi


    AC_SUBST(TARGET_LIBDWARF_CPPFLAGS)
    AC_SUBST(TARGET_LIBDWARF_LDFLAGS)
    AC_SUBST(TARGET_LIBDWARF_LIBS)
    AC_SUBST(TARGET_LIBDWARF_DIR)

])

