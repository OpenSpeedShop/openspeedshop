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
# Check for libelf 
################################################################################

AC_DEFUN([AX_LIBELF], [

    AC_ARG_WITH(libelf,
                AC_HELP_STRING([--with-libelf=DIR],
                               [libelf installation @<:@/usr@:>@]),
                libelf_dir=$withval, libelf_dir="/usr")

    found_libelf=0

    if test "x${libelf_dir}" == "x" || test "x${libelf_dir}" == "x/usr" ; then
      LIBELF_CPPFLAGS=""
      LIBELF_LDFLAGS=""
    else
      LIBELF_CPPFLAGS="-I$libelf_dir/include"
      LIBELF_LDFLAGS="-L$libelf_dir/$abi_libdir"
    fi

    LIBELF_LIBS="-lelf"

    libelf_saved_CPPFLAGS=$CPPFLAGS
    libelf_saved_LDFLAGS=$LDFLAGS
    libelf_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $LIBELF_CPPFLAGS"
    LDFLAGS="$LDFLAGS $LIBELF_LDFLAGS"
    LIBS="$LIBELF_LIBS"

    AC_MSG_CHECKING([for libelf library and headers])

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
        #include <libelf.h>
        #include <stdio.h>
        ]], [[ if (elf_version(EV_CURRENT) == EV_NONE) {
	        printf("ELF library initialization failed: %s", elf_errmsg(-1));
              }
        ]])], [ found_libelf=1 ], [ found_libelf=0 ])

    if test $found_libelf -eq 1; then
        AC_MSG_RESULT(yes)
        AM_CONDITIONAL(HAVE_LIBELF, true)
        AC_DEFINE(HAVE_LIBELF, 1, [Define to 1 if you have LIBELF.])
    else
# FIXME.  if we expect libraries in spme specific directory then
# create an option to set it rather than override the not found above
# due to lib lib64 inconsistencies.
# Try again with the traditional library path (lib???) instead
         found_libelf=0
         if test "x${libelf_dir}" == "x" || test "x${libelf_dir}" == "x/usr" ; then
           LIBELF_CPPFLAGS=""
           LIBELF_LDFLAGS=""
         else
           LIBELF_CPPFLAGS="-I$libelf_dir/include"
           LIBELF_LDFLAGS="-L$libelf_dir/$alt_abi_libdir"
         fi 

         CPPFLAGS="$CPPFLAGS $LIBELF_CPPFLAGS"
         LDFLAGS="$LDFLAGS $LIBELF_LDFLAGS"
         LIBS="$LIBS $LIBELF_LIBS"

         AC_MSG_CHECKING([for libelf library and headers])

         AC_LINK_IFELSE([AC_LANG_PROGRAM([[
             #include <libelf.h>
             ]], [[ if (elf_version(EV_CURRENT) == EV_NONE) {
	             printf("ELF library initialization failed: %s", elf_errmsg(-1));
                   }
             ]])], [ found_libelf=1 ], [ found_libelf=0 ])

         if test $found_libelf -eq 1; then
             AC_MSG_RESULT(yes)
             AM_CONDITIONAL(HAVE_LIBELF, true)
             AC_DEFINE(HAVE_LIBELF, 1, [Define to 1 if you have LIBELF.])
         else
             AC_MSG_RESULT(no)
             AM_CONDITIONAL(HAVE_LIBELF, false)
             AC_DEFINE(HAVE_LIBELF, 0, [Define to 0 if you do not have LIBELF.])
             LIBELF_CPPFLAGS=""
             LIBELF_LDFLAGS=""
             LIBELF_LIBS=""
         fi
    fi

    CPPFLAGS=$libelf_saved_CPPFLAGS 
    LDFLAGS=$libelf_saved_LDFLAGS
    LIBS=$libelf_saved_LIBS

    AC_SUBST(LIBELF_CPPFLAGS)
    AC_SUBST(LIBELF_LDFLAGS)
    AC_SUBST(LIBELF_LIBS)

])

