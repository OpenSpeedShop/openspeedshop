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
                               [DPCL installation @<:@/usr@:>@]),
                dpcl_dir=$withval, dpcl_dir="/usr")

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
        ia64-*-linux*)
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
                               [Dyninst installation @<:@/usr@:>@]),
                dyninst_dir=$withval, dyninst_dir="/usr/")

    case "$host" in
	i386-*-linux-*)
	    dyninst_platform="i386-unknown-linux2.4"
	    ;;
        ia64-*-linux-*)
	    dyninst_platform="ia64-unknown-linux2.4"
            ;;
    esac

    AC_CHECK_FILE([$dyninst_dir/include/dyninst/BPatch.h], [
        DYNINST_CPPFLAGS="-I$dyninst_dir/include/dyninst"
        DYNINST_LDFLAGS="-L$dyninst_dir/lib"
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



################################################################################
# Check for Python 2.3 (http://www.python.org)
################################################################################

AC_DEFUN([AC_PKG_PYTHON], [

    AC_ARG_WITH(python,
                AC_HELP_STRING([--with-python=DIR],
                               [Python 2.3 installation @<:@/usr@:>@]),
                python_dir=$withval, python_dir="/usr")

    AC_CHECK_FILE([$python_dir/include/python2.3/Python.h], [
        PYTHON_CPPFLAGS="-I$python_dir/include/python2.3"
        PYTHON_LDFLAGS="-L$python_dir/lib"
    ])

    if test -d "$ROOT"; then
        AC_CHECK_FILE([$ROOT/include/python2.3/Python.h], [
            PYTHON_CPPFLAGS="-I$ROOT/include/python2.3"
            PYTHON_LDFLAGS=""
        ])
    fi

    PYTHON_LIBS="-lpython2.3"

    AC_LANG_PUSH(C++)

    python_saved_CPPFLAGS=$CPPFLAGS
    python_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $PYTHON_CPPFLAGS"
    LDFLAGS="$CXXFLAGS $PYTHON_LDFLAGS $PYTHON_LIBS"

    AC_MSG_CHECKING([for Python 2.3 library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <Python.h>
        ]], [[
        PyDoc_STR("xxx");
        ]]), AC_MSG_RESULT(yes), [ AC_MSG_RESULT(no)
        AC_MSG_FAILURE(cannot locate Python 2.3 library and/or headers.) ]
    )

    CPPFLAGS=$python_saved_CPPFLAGS
    LDFLAGS=$python_saved_LDFLAGS

    AC_LANG_POP(C++)

    AC_SUBST(PYTHON_CPPFLAGS)
    AC_SUBST(PYTHON_LDFLAGS)
    AC_SUBST(PYTHON_LIBS)

    AC_DEFINE(HAVE_PYTHON, 1, [Define to 1 if you have Python 2.3.])

])

##################################################################################
# Check for QT libraries
# Some of this code was found by an internet search for qt configuration/checking,
# so credit to those who originally created parts of the code below.
##################################################################################

AC_DEFUN([AC_PKG_QTLIB], [

dnl Check for QTDIR environment variable.
AC_MSG_CHECKING([whether QTDIR environment variable is set])
if test "x$QTDIR" = "x"; then
  AC_MSG_RESULT([no])
  AC_MSG_ERROR([QTDIR must be properly set.])
else
  AC_MSG_RESULT([$QTDIR])
fi

dnl Checks for Qt library.
AC_CACHE_CHECK([for Qt library],
  ac_qtlib, [
  for X in qt-mt qt; do
    if test "x$ac_qtlib" = "x"; then
      if test -f $QTDIR/lib/lib$X.so -o -f $QTDIR/lib/lib$X.a; then
        ac_qtlib=$X
      fi
    fi
  done
])
AC_MSG_RESULT([Temporary output: ac_qtlib=$ac_qtlib])

if test "x$ac_qtlib" = "x"; then
  AC_MSG_ERROR([Qt library not found. Maybe QTDIR isn't properly set.])
fi
AC_SUBST(ac_qtlib)

dnl Check for Qt multi-thread support.
if test "x$ac_qtlib" = "xqt-mt"; then
  ac_thread="thread"
fi
AC_SUBST(ac_thread)

dnl Set initial values for QTLIB exports
QTLIB_CFLAGS="$CFLAGS -I$QTDIR/include -I$KDEDIR/include"
QTLIB_CPPFLAGS="$CPPFLAGS -I$QTDIR/include -I$KDEDIR/include"
QTLIB_LIBS="-L$QTDIR/lib -L/usr/X11R6/lib"
QTLIB_LDFLAGS="-l$ac_qtlib"

dnl Save the current CPPFLAGS and LDFLAGS variables prior to qt version test
qtlib_saved_CPPFLAGS=$CPPFLAGS
qtlib_saved_LDFLAGS=$LDFLAGS

dnl Use QT CPPFLAGS and LDFLAGS variables for the qt version test
CPPFLAGS="$CPPFLAGS $QTLIB_CPPFLAGS"
LDFLAGS="$CXXFLAGS $QTLIB_LDFLAGS $QTLIB_LIBS"

dnl Check if we have qglobal.h which is an indication of QT 3.0 or greater
AC_CACHE_CHECK(
[for Qt library version >= 3.0.0], ac_qtlib_version,
[
    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
	#include <qglobal.h>
	        ]], [[
	        ]]), AC_MSG_RESULT(yes), [ AC_MSG_RESULT(no)
	        AC_MSG_FAILURE(Can not locate Qt library and/or the headers.) ]
   	    )
	AC_TRY_LINK(
	[
		#include "qglobal.h"
	],
	[
		#if QT_VERSION < 0x030000
		#error Qt library 3.0 or greater is required.
		#endif
	],
	[
		ac_qtlib_version="yes"
		QT_CFLAGS="$CFLAGS"
		QT_LIBS="-l$ac_qtlib"
		QT_LDFLAGS="$LIBS"
		],
	[
		QT_CFLAGS=""
		QT_LIBS=""
		QT_LDFLAGS=""
		AC_MSG_ERROR([** QT version 3.0 or greater is required for the Open|SpeedShop QT gui.])
	])
	]
)

dnl A common error message:
ac_qtdir_errmsg="Not found in current PATH. Maybe QT development environment isn't available (qt3-devel)."

dnl Check for Qt qmake utility.
AC_PATH_PROG(ac_qmake, qmake, [no], $ac_qtdir/bin:${PATH})
if test "x$ac_qmake" = "xno"; then
   AC_MSG_ERROR([qmake $ac_qtdir_errmsg])
fi

    AC_SUBST(ac_qmake)

dnl Support the user enabled --with-qtlib option.
    AC_ARG_WITH(qtlib,
                AC_HELP_STRING([--with-qtlib=DIR],
                               [QTLIB installation @<:@/usr@:>@]),
                qtlib_dir=$withval, qtlib_dir="ac_qtlib")

dnl This only needed if we continue the build environment ROOT support
    if test -d "$ROOT"; then
        AC_CHECK_FILE([$ROOT/include/qt-3.3/qtwidget.h], [
            QTLIB_CPPFLAGS="-I$ROOT/include/qt-3.3"
            QTLIB_LDFLAGS="-L$ROOT/lib"
            QTLIB_LIBS="-lqt-mt"
        ])
    fi

dnl Restore saved flags after the QT version check program compilation
    CPPFLAGS=$qtlib_saved_CPPFLAGS
    LDFLAGS=$qtlib_saved_LDFLAGS

dnl Make the QTLIB flags/libs available
    AC_SUBST(QTLIB_CPPFLAGS)
    AC_SUBST(QTLIB_LDFLAGS)
    AC_SUBST(QTLIB_LIBS)
    AC_DEFINE(HAVE_QTLIB, 1, [Define to 1 if you have Qt library 3.1 >])

])
