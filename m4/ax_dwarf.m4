#################################################################################
# Copyright (c) 2010-2014 Krell Institute. All Rights Reserved.
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

    AC_ARG_WITH([libdwarf-libdir],
                AS_HELP_STRING([--with-libdwarf-libdir=LIB_DIR],
                [Force given directory for libdwarf libraries. Note that this will overwrite library path detection, so use this parameter only if default library detection fails and you know exactly where your libdwarf libraries are located.]),
                [
                if test -d $withval 
                then
                        ac_libdwarf_lib_path="$withval"
                else
                        AC_MSG_ERROR(--with-libdwarf-libdir expected directory name)
                fi ], 
                [ac_libdwarf_lib_path=""])


    if test "x$ac_libdwarf_lib_path" == "x"; then
       LIBDWARF_LDFLAGS="-L$libdwarf_dir/$abi_libdir"
    else
       LIBDWARF_LDFLAGS="-L$ac_libdwarf_lib_path"
    fi


    found_libdwarf=0

    if test "x$libdwarf_dir" == "x/usr"; then
	LIBDWARF_CPPFLAGS=""
	LIBDWARF_LDFLAGS=""
    else
	LIBDWARF_CPPFLAGS="-I$libdwarf_dir/include"
    fi

    LIBDWARF_LIBS="-ldwarf"

    libdwarf_saved_CPPFLAGS=$CPPFLAGS
    libdwarf_saved_LDFLAGS=$LDFLAGS
    libdwarf_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $LIBDWARF_CPPFLAGS"
    LDFLAGS="$LDFLAGS $LIBDWARF_LDFLAGS $LIBELF_LDFLAGS"
    LIBS="$LIBDWARF_LIBS $LIBELF_LIBS -lpthread"

    AC_MSG_CHECKING([for libdwarf library and headers])

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
        #include <dwarf.h>
        ]], [[
        if (DW_ID_up_case != DW_ID_down_case) {
           int mycase = DW_ID_up_case;
        }
        ]])], [ found_libdwarf=1 ], [ found_libdwarf=0 ])

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

    CPPFLAGS=$libdwarf_saved_CPPFLAGS 
    LDFLAGS=$libdwarf_saved_LDFLAGS
    LIBS=$libdwarf_saved_LIBS

    AC_SUBST(LIBDWARF_CPPFLAGS)
    AC_SUBST(LIBDWARF_LDFLAGS)
    AC_SUBST(LIBDWARF_LIBS)

])
