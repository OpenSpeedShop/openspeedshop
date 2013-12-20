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
# Check for Array Services (SGI Proprietary)
################################################################################

AC_DEFUN([AX_ARRAYSVCS], [

    AC_ARG_WITH(arraysvcs,
                AC_HELP_STRING([--with-arraysvcs=DIR],
                               [array services installation @<:@/usr@:>@]),
                arraysvcs_dir=$withval, arraysvcs_dir="/usr")

    ARRAYSVCS_CPPFLAGS="-I$arraysvcs_dir/include"
    ARRAYSVCS_LDFLAGS="-L$arraysvcs_dir/$abi_libdir"
    ARRAYSVCS_LIBS="-larray"

    case "$host" in
        *linux*)
            ARRAYSVCS_CPPFLAGS="$ARRAYSVCS_CPPFLAGS -DLINUX"
            ARRAYSVCS_CPPFLAGS="$ARRAYSVCS_CPPFLAGS -D_LANGUAGE_C_PLUS_PLUS"
	    ;;
    esac

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    arraysvcs_saved_CPPFLAGS=$CPPFLAGS
    arraysvcs_saved_LDFLAGS=$LDFLAGS
    arraysvcs_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $ARRAYSVCS_CPPFLAGS"
    LDFLAGS="$LDFLAGS $ARRAYSVCS_LDFLAGS"
    LIBS="$ARRAYSVCS_LIBS"

    AC_MSG_CHECKING([for array services library and headers])

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
        #include <arraysvcs.h>
        ]], [[
        asgeterror();
        ]])], [ AC_MSG_RESULT(yes)

            AC_DEFINE(HAVE_ARRAYSVCS, 1, 
                      [Define to 1 if you have array services.])

        ], [ AC_MSG_RESULT(no) 

            ARRAYSVCS_CPPFLAGS=""
            ARRAYSVCS_LDFLAGS=""
            ARRAYSVCS_LIBS=""

        ]
    )

    CPPFLAGS=$arraysvcs_saved_CPPFLAGS
    LDFLAGS=$arraysvcs_saved_LDFLAGS
    LIBS=$arraysvcs_saved_LIBS

    AC_LANG_POP(C++)

    AC_SUBST(ARRAYSVCS_CPPFLAGS)
    AC_SUBST(ARRAYSVCS_LDFLAGS)
    AC_SUBST(ARRAYSVCS_LIBS)

])
