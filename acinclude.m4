################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
# Check for DPCL (http://oss.software.ibm.com/developerworks/opensource/dpcl)
################################################################################

AC_DEFUN([AC_PKG_DPCL], [

    AC_ARG_WITH(dpcl,
                AC_HELP_STRING([--with-dpcl=DIR],
                               [DPCL installation @<:@/opt/dpcl@:>@]),
                dpcl_dir=$withval, dpcl_dir="/opt/dpcl")

    AC_CHECK_FILE([$dpcl_dir/include/dpcl.h], [
        DPCL_CPPFLAGS="-I$dpcl_dir/include"
        DPCL_LDFLAGS="-L$dpcl_dir/lib"
    ])

    AC_CHECK_FILE([$dpcl_dir/include/dpcl/dpcl.h], [
        DPCL_CPPFLAGS="-I$dpcl_dir/include/dpcl"
        DPCL_LDFLAGS="-L$dpcl_dir/lib"
    ])

    if test -d "$ROOT"; then
        AC_CHECK_FILE([$ROOT/include/dpcl/dpcl.h], [
            DPCL_CPPFLAGS="-I$ROOT/include/dpcl"
            DPCL_LDFLAGS=""
        ])
    fi

    case "$host" in
        ia64-*-linux-*)
            DPCL_CPPFLAGS="$DPCL_CPPFLAGS -D__64BIT__"
            ;;
    esac

    DPCL_LIBS="-ldpcl"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    dpcl_saved_CPPFLAGS=$CPPFLAGS
    dpcl_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $DPCL_CPPFLAGS"
    LDFLAGS="$CXXFLAGS $DPCL_LDFLAGS $DPCL_LIBS"

    AC_MSG_CHECKING([for DPCL client library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <dpcl.h>
        ]], [[
        Ais_initialize();
        ]]), AC_MSG_RESULT(yes), [ AC_MSG_RESULT(no)
        AC_MSG_FAILURE(cannot locate DPCL client library and/or headers.) ]
    )

    CPPFLAGS=$dpcl_saved_CPPFLAGS
    LDFLAGS=$dpcl_saved_LDFLAGS

    AC_LANG_POP(C++)

    AC_SUBST(DPCL_CPPFLAGS)
    AC_SUBST(DPCL_LDFLAGS)
    AC_SUBST(DPCL_LIBS)

    AC_DEFINE(HAVE_DPCL, 1, [Define to 1 if you have DPCL.])

])

################################################################################
# Check for Dyninst (http://www.dyninst.org)
################################################################################

AC_DEFUN([AC_PKG_DYNINST], [

    AC_ARG_WITH(dyninst,
                AC_HELP_STRING([--with-dyninst=DIR],
                               [Dyninst installation @<:@/usr/dyninst@:>@]),
                dyninst_dir=$withval, dyninst_dir="/usr/dyninst")

    case "$host" in
	i386-*-linux-*)
	    dyninst_platform="i386-unknown-linux2.4"
	    ;;
        ia64-*-linux-*)
	    dyninst_platform="ia64-unknown-linux2.4"
            ;;
    esac

    AC_CHECK_FILE([$dyninst_dir/core/dyninstAPI/h/BPatch.h], [
        DYNINST_CPPFLAGS="-I$dyninst_dir/core/dyninstAPI/h"
        DYNINST_LDFLAGS="-L$dyninst_dir/$dyninst_platform/lib"
    ])

    if test -d "$ROOT"; then
        AC_CHECK_FILE([$ROOT/include/dyninst/BPatch.h], [
            DYNINST_CPPFLAGS="-I$ROOT/include/dyninst"
            DYNINST_LDFLAGS=""
        ])
    fi

    DYNINST_LIBS="-ldyninstAPI"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    dyninst_saved_CPPFLAGS=$CPPFLAGS
    dyninst_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $DYNINST_CPPFLAGS"
    LDFLAGS="$CXXFLAGS $DYNINST_LDFLAGS $DYNINST_LIBS"

    AC_MSG_CHECKING([for Dyninst API library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
	#include <BPatch.h>
        ]], [[
	BPatch bpatch();
        ]]), AC_MSG_RESULT(yes), [ AC_MSG_RESULT(no)
        AC_MSG_FAILURE(cannot locate Dyninst API library and/or headers.) ]
    )

    CPPFLAGS=$dyninst_saved_CPPFLAGS
    LDFLAGS=$dyninst_saved_LDFLAGS

    AC_LANG_POP(C++)

    AC_SUBST(DYNINST_CPPFLAGS)
    AC_SUBST(DYNINST_LDFLAGS)
    AC_SUBST(DYNINST_LIBS)

    AC_DEFINE(HAVE_DYNINST, 1, [Define to 1 if you have Dyninst.])

])
