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
# Check for Array Services (SGI Proprietary)
################################################################################

AC_DEFUN([AC_PKG_ARRAYSVCS], [

    AC_ARG_WITH(arraysvcs,
                AC_HELP_STRING([--with-arraysvcs=DIR],
                               [Array services installation @<:@/usr@:>@]),
                arraysvcs_dir=$withval, arraysvcs_dir="/usr")

    ARRAYSVCS_CPPFLAGS="-I$arraysvcs_dir/include"
    ARRAYSVCS_LDFLAGS="-L$arraysvcs_dir/lib"
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

    CPPFLAGS="$CPPFLAGS $ARRAYSVCS_CPPFLAGS"
    LDFLAGS="$CXXFLAGS $ARRAYSVCS_LDFLAGS $ARRAYSVCS_LIBS"

    AC_MSG_CHECKING([for array services library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <arraysvcs.h>
        ]], [[
        asgeterror();
        ]]), [

            AC_MSG_RESULT(yes)

            AC_SUBST(ARRAYSVCS_CPPFLAGS)
            AC_SUBST(ARRAYSVCS_LDFLAGS)
            AC_SUBST(ARRAYSVCS_LIBS)

            AC_DEFINE(HAVE_ARRAYSVCS, 1, 
                      [Define to 1 if you have array services.])

        ], [ AC_MSG_RESULT(no) ]
    )

    CPPFLAGS=$arraysvcs_saved_CPPFLAGS
    LDFLAGS=$arraysvcs_saved_LDFLAGS

    AC_LANG_POP(C++)

])

################################################################################
# Check for DPCL (http://oss.software.ibm.com/developerworks/opensource/dpcl)
################################################################################

AC_DEFUN([AC_PKG_DPCL], [

    AC_ARG_WITH(dpcl,
                AC_HELP_STRING([--with-dpcl=DIR],
                               [DPCL installation @<:@/usr@:>@]),
                dpcl_dir=$withval, dpcl_dir="/usr")

    DPCL_CPPFLAGS="-I$dpcl_dir/include/dpcl"
    DPCL_LDFLAGS="-L$dpcl_dir/lib"
    DPCL_LIBS="-ldpcl"

    case "$host" in
        ia64-*-linux*)
            DPCL_CPPFLAGS="$DPCL_CPPFLAGS -D__64BIT__"
            ;;
    esac

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

    DYNINST_CPPFLAGS="-I$dyninst_dir/include/dyninst"
    DYNINST_CPPFLAGS="$DYNINST_CPPFLAGS -DUSE_STL_VECTOR -DIBM_BPATCH_COMPAT"
    DYNINST_LDFLAGS="-L$dyninst_dir/lib"
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
# Check for SQLite (http://www.sqlite.org/)
################################################################################

AC_DEFUN([AC_PKG_SQLITE], [

    AC_ARG_WITH(sqlite,
                AC_HELP_STRING([--with-sqlite=DIR],
                               [SQLite installation @<:@/usr@:>@]),
                sqlite_dir=$withval, sqlite_dir="/usr")

    SQLITE_CPPFLAGS="-I$sqlite_dir/include"
    SQLITE_LDFLAGS="-L$sqlite_dir/lib"
    SQLITE_LIBS="-lsqlite3"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    sqlite_saved_CPPFLAGS=$CPPFLAGS
    sqlite_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $SQLITE_CPPFLAGS"
    LDFLAGS="$CXXFLAGS $SQLITE_LDFLAGS $SQLITE_LIBS"

    AC_MSG_CHECKING([for SQLite library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <sqlite3.h>
        ]], [[
	sqlite3_libversion();
        ]]), AC_MSG_RESULT(yes), [ AC_MSG_RESULT(no)
        AC_MSG_FAILURE(cannot locate SQLite library and/or headers.) ]
    )

    CPPFLAGS=$sqlite_saved_CPPFLAGS
    LDFLAGS=$sqlite_saved_LDFLAGS

    AC_LANG_POP(C++)

    AC_SUBST(SQLITE_CPPFLAGS)
    AC_SUBST(SQLITE_LDFLAGS)
    AC_SUBST(SQLITE_LIBS)

    AC_DEFINE(HAVE_SQLITE, 1, [Define to 1 if you have SQLite.])

])



################################################################################
# Check for Python minimum version 2.2 (http://www.python.org)
################################################################################
## ------------------------
## Python file handling
## From Andrew Dalke
## Updated by James Henstridge
## Modified by Jim Galarowicz
## ------------------------

# AM_PATH_PYTHON([MINIMUM-VERSION])

# Adds support for distributing Python modules and packages.  To
# install modules, copy them to $(pythondir), using the python_PYTHON
# automake variable.  To install a package with the same name as the
# automake package, install to $(pkgpythondir), or use the
# pkgpython_PYTHON automake variable.

# The variables $(pyexecdir) and $(pkgpyexecdir) are provided as
# locations to install python extension modules (shared libraries).
# Another macro is required to find the appropriate flags to compile
# extension modules.

# If your package is configured with a different prefix to python,
# users will have to add the install directory to the PYTHONPATH
# environment variable, or create a .pth file (see the python
# documentation for details).

# If the MINIUMUM-VERSION argument is passed, AM_PATH_PYTHON will
# cause an error if the version of python installed on the system
# doesn't meet the requirement.  MINIMUM-VERSION should consist of
# numbers and dots only.


AC_DEFUN([AM_PATH_PYTHON],
 [
#  AC_PROVIDE([AM_PATH_PYTHON])
  dnl minimum version 2.2 is checked for

  AC_PATH_PROG(PYTHON, python)

  dnl should we do the version check?
  ifelse([$1],[],,[
    AC_MSG_CHECKING(if Python version >= $1)
    changequote(<<, >>)dnl
    prog="
import sys, string
minver = '$1'
pyver = string.split(sys.version)[0]  # first word is version string
# split strings by '.' and convert to numeric
minver = map(string.atoi, string.split(minver, '.'))
if hasattr(sys, 'version_info'):
    pyver = sys.version_info[:3]
else:
    pyver = map(string.atoi, string.split(pyver, '.'))
# we can now do comparisons on the two lists:
if pyver >= minver:
        sys.exit(0)
else:
        sys.exit(1)"
    changequote([, ])dnl
    if $PYTHON -c "$prog" 1>&AC_FD_CC 2>&AC_FD_CC
    then
      AC_MSG_RESULT(okay)
    else
      AC_MSG_ERROR(too old)
    fi
  ])

  AC_MSG_CHECKING([local Python configuration])

  dnl Query Python for its version number.  Getting [:3] seems to be
  dnl the best way to do this; it's what "site.py" does in the standard
  dnl library.  Need to change quote character because of [:3]

  AC_SUBST(PYTHON_VERSION)
  changequote(<<, >>)dnl
  PYTHON_VERSION=`$PYTHON -c "import sys; print sys.version[:3]"`
  changequote([, ])dnl


  dnl Use the values of $prefix and $exec_prefix for the corresponding
  dnl values of PYTHON_PREFIX and PYTHON_EXEC_PREFIX.  These are made
  dnl distinct variables so they can be overridden if need be.  However,
  dnl general consensus is that you shouldn't need this ability.

  AC_SUBST(PYTHON_PREFIX)
  PYTHON_PREFIX='${prefix}'

  AC_SUBST(PYTHON_EXEC_PREFIX)
  PYTHON_EXEC_PREFIX='${exec_prefix}'

  dnl At times (like when building shared libraries) you may want
  dnl to know which OS platform Python thinks this is.

  AC_SUBST(PYTHON_PLATFORM)
  PYTHON_PLATFORM=`$PYTHON -c "import sys; print sys.platform"`

  AC_MSG_RESULT([looks good])
])


dnl a macro to check for ability to create python extensions
dnl  AM_CHECK_PYTHON_HEADERS([ACTION-IF-POSSIBLE], [ACTION-IF-NOT-POSSIBLE])
dnl function also defines PYTHON_INCLUDES
AC_DEFUN([AM_CHECK_PYTHON_HEADERS],
[AC_REQUIRE([AM_PATH_PYTHON])
AC_MSG_CHECKING(for headers required to compile python extensions)
dnl deduce PYTHON_INCLUDES
py_prefix=`$PYTHON -c "import sys; print sys.prefix"`
py_exec_prefix=`$PYTHON -c "import sys; print sys.exec_prefix"`
PYTHON_INCLUDES="-I${py_prefix}/include/python${PYTHON_VERSION}"
if test "$py_prefix" != "$py_exec_prefix"; then
  PYTHON_INCLUDES="$PYTHON_INCLUDES -I${py_exec_prefix}/include/python${PYTHON_VERSION}"
fi
AC_SUBST(PYTHON_INCLUDES)
dnl check if the headers exist:
save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $PYTHON_INCLUDES"
AC_TRY_CPP([#include <Python.h>],dnl
[AC_MSG_RESULT(found)
$1],dnl
[AC_MSG_RESULT(not found)
$2])
CPPFLAGS="$save_CPPFLAGS"
])


AC_DEFUN([AC_PKG_PYTHON], [

    AC_ARG_WITH(python,
                AC_HELP_STRING([--with-python=DIR],
                               [Python ${PYTHON_VERSION} installation @<:@/usr@:>@]),
                python_dir=$withval, python_dir="/usr")

    AC_CHECK_FILE([$python_dir/include/python${PYTHON_VERSION}/Python.h], [
        PYTHON_CPPFLAGS="-I$python_dir/include/python${PYTHON_VERSION}"
        PYTHON_LDFLAGS="-L$python_dir/lib"
    ])

    if test -d "$ROOT"; then
        AC_CHECK_FILE([$ROOT/include/python2.3/Python.h], [
            PYTHON_CPPFLAGS="-I$ROOT/include/python2.3"
            PYTHON_LDFLAGS=""
            PYTHON_VERSION="2.3"
	
        ])
    fi

    PYTHON_LIBS="-lpython${PYTHON_VERSION}"

    AC_LANG_PUSH(C++)

    FOUND_PYTHON_LIB="no" 

    if test "$FOUND_PYTHON_LIB" == "no"; then
        AC_MSG_CHECKING([for Python ${PYTHON_VERSION} headers and library (in $python_dir/lib)])
        python_saved_CPPFLAGS=$CPPFLAGS
        python_saved_LDFLAGS=$LDFLAGS
        CPPFLAGS="$CPPFLAGS $PYTHON_CPPFLAGS"
        LDFLAGS="$CXXFLAGS $PYTHON_LDFLAGS $PYTHON_LIBS"
        AC_LINK_IFELSE(AC_LANG_PROGRAM([[
            #include <Python.h>
            ]], [[
            Py_Initialize();
            ]]), [ AC_MSG_RESULT(yes); FOUND_PYTHON_LIB="yes" ], [ AC_MSG_RESULT(no) ]
        )
        CPPFLAGS=$python_saved_CPPFLAGS
        LDFLAGS=$python_saved_LDFLAGS
    fi

    if test "$FOUND_PYTHON_LIB" == "no"; then

        PYTHON_LDFLAGS="-L$python_dir/lib/python${PYTHON_VERSION}/config"

        AC_MSG_CHECKING([for Python ${PYTHON_VERSION} headers and library (in /usr/lib/python${PYTHON_VERSION}/config)])
        python_saved_CPPFLAGS=$CPPFLAGS
        python_saved_LDFLAGS=$LDFLAGS
        CPPFLAGS="$CPPFLAGS $PYTHON_CPPFLAGS"
        LDFLAGS="$CXXFLAGS $PYTHON_LDFLAGS $PYTHON_LIBS"
        AC_LINK_IFELSE(AC_LANG_PROGRAM([[
            #include <Python.h>
            ]], [[
            Py_Initialize();
            ]]), [ AC_MSG_RESULT(yes); FOUND_PYTHON_LIB="yes" ], [ AC_MSG_RESULT(no) ] 
        )
        CPPFLAGS=$python_saved_CPPFLAGS
        LDFLAGS=$python_saved_LDFLAGS
    fi

    if test "$FOUND_PYTHON_LIB" == "no"; then
        AC_MSG_FAILURE(cannot locate Python library and/or headers.)
    fi

    AC_LANG_POP(C++)

    AC_SUBST(PYTHON_CPPFLAGS)
    AC_SUBST(PYTHON_LDFLAGS)
    AC_SUBST(PYTHON_LIBS)

    if PYTHON_VERSION < 2.2; then
	AC_MSG_ERROR([** PYTHON version 2.2 or greater is required for the Open|SpeedShop.])
    fi

    AC_DEFINE(HAVE_PYTHON, 1, [Define to 1 if you have Python ${PYTHON_VERSION} ])

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

dnl Check if we have qglobal.h which is an indication of QT 3.3 or greater
AC_CACHE_CHECK(
[for Qt library version >= 3.3.0], ac_qtlib_version,
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
		#if QT_VERSION < 0x030300
		#error Qt library 3.3 or greater is required.
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
		AC_MSG_ERROR([** QT version 3.3 or greater is required for the Open|SpeedShop QT gui.])
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
