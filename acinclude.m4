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

    CPPFLAGS="$CPPFLAGS $ARRAYSVCS_CPPFLAGS"
    LDFLAGS="$CXXFLAGS $ARRAYSVCS_LDFLAGS $ARRAYSVCS_LIBS"

    AC_MSG_CHECKING([for array services library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <arraysvcs.h>
        ]], [[
        asgeterror();
        ]]), [ AC_MSG_RESULT(yes)

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

    AC_LANG_POP(C++)

    AC_SUBST(ARRAYSVCS_CPPFLAGS)
    AC_SUBST(ARRAYSVCS_LDFLAGS)
    AC_SUBST(ARRAYSVCS_LIBS)

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
    DPCL_LDFLAGS="-L$dpcl_dir/$abi_libdir"

    case "$host" in
	x86_64-*-linux* | ia64-*-linux*)
            DPCL_CPPFLAGS="$DPCL_CPPFLAGS -D__64BIT__"
	    DPCL_LIBS="-ldpcl64 -lpthread"
            ;;
	*)
	    DPCL_LIBS="-ldpcl -lpthread"
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
    DYNINST_LDFLAGS="-L$dyninst_dir/$abi_libdir"
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
# Check for MPI (http://www.mpi-forum.org/)
################################################################################

AC_DEFUN([AC_PKG_MPI], [

    AC_ARG_WITH(mpi,
                AC_HELP_STRING([--with-mpi=DIR],
                               [MPI installation @<:@/usr@:>@]),
                mpi_dir=$withval, mpi_dir="/usr")

    AC_ARG_WITH(mpich-driver,
                AC_HELP_STRING([--with-mpich-driver=NAME],
                               [MPICH driver name @<:@ch-p4@:>@]),
                mpich_driver=$withval, mpich_driver="ch-p4")

    AC_MSG_CHECKING([for MPI library and headers])

    mpi_saved_CPPFLAGS=$CPPFLAGS
    mpi_saved_LDFLAGS=$LDFLAGS

    MPI_CPPFLAGS="-I$mpi_dir/include"
    MPI_LDFLAGS="-L$mpi_dir/$abi_libdir"
    MPI_LIBS="-lmpi"

    CPPFLAGS="$CPPFLAGS $MPI_CPPFLAGS"
    LDFLAGS="$LDFLAGS $MPI_LDFLAGS $MPI_LIBS"

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <mpi.h>
        ]], [[
	MPI_Initialized((int*)0);
        ]]), , [

            MPI_CPPFLAGS=""
            MPI_LDFLAGS=""
            MPI_LIBS=""

        ]
    )

    if test x"$MPI_LIBS" == x""; then

        MPI_CPPFLAGS="-I$mpi_dir/include"
        MPI_LDFLAGS="-L$mpi_dir/$mpich_driver/$abi_libdir/shared"
        MPI_LIBS="-lmpich"

        CPPFLAGS="$mpi_saved_CPPFLAGS $MPI_CPPFLAGS"
        LDFLAGS="$mpi_saved_LDFLAGS $MPI_LDFLAGS $MPI_LIBS"

        AC_LINK_IFELSE(AC_LANG_PROGRAM([[
            #include <mpi.h>
            ]], [[
            MPI_Initialized((int*)0);
            ]]), , [

                MPI_CPPFLAGS=""
                MPI_LDFLAGS=""
                MPI_LIBS=""

            ]
        )

    fi

    CPPFLAGS=$mpi_saved_CPPFLAGS
    LDFLAGS=$mpi_saved_LDFLAGS

    if test x"$MPI_LIBS" == x""; then
        AC_MSG_RESULT(no)
        AM_CONDITIONAL(HAVE_MPI, false)
    else
        AC_MSG_RESULT(yes)
        AM_CONDITIONAL(HAVE_MPI, true)
        AC_DEFINE(HAVE_MPI, 1, [Define to 1 if you have MPI.])	
    fi

    AC_SUBST(MPI_CPPFLAGS)
    AC_SUBST(MPI_LDFLAGS)
    AC_SUBST(MPI_LIBS)

])

################################################################################
# Check for Libunwind (http://www.hpl.hp.com/research/linux/libunwind/)
################################################################################

AC_DEFUN([AC_PKG_LIBUNWIND], [

    AC_ARG_WITH(libunwind,
                AC_HELP_STRING([--with-libunwind=DIR],
                               [libunwind installation @<:@/usr@:>@]),
                libunwind_dir=$withval, libunwind_dir="/usr")

    LIBUNWIND_CPPFLAGS="-I$libunwind_dir/include -DUNW_LOCAL_ONLY"
    LIBUNWIND_LDFLAGS="-L$libunwind_dir/$abi_libdir"
    LIBUNWIND_LIBS="-lunwind"

    libunwind_saved_CPPFLAGS=$CPPFLAGS
    libunwind_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $LIBUNWIND_CPPFLAGS"
    LDFLAGS="$LDFLAGS $LIBUNWIND_LDFLAGS $LIBUNWIND_LIBS"

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

        ]
    )

    CPPFLAGS=$libunwind_saved_CPPFLAGS
    LDFLAGS=$libunwind_saved_LDFLAGS

    AC_SUBST(LIBUNWIND_CPPFLAGS)
    AC_SUBST(LIBUNWIND_LDFLAGS)
    AC_SUBST(LIBUNWIND_LIBS)

])

################################################################################
# Check for PAPI (http://icl.cs.utk.edu/papi)
################################################################################

AC_DEFUN([AC_PKG_PAPI], [

    AC_ARG_WITH(papi,
                AC_HELP_STRING([--with-papi=DIR],
                               [PAPI installation @<:@/usr@:>@]),
                papi_dir=$withval, papi_dir="/usr")

    PAPI_CPPFLAGS="-I$papi_dir/include"
    PAPI_LDFLAGS="-L$papi_dir/$abi_libdir"
    PAPI_LIBS="-lpapi"

    papi_saved_CPPFLAGS=$CPPFLAGS
    papi_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $PAPI_CPPFLAGS"
    LDFLAGS="$LDFLAGS $PAPI_LDFLAGS $PAPI_LIBS"

    AC_MSG_CHECKING([for PAPI library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <papi.h>
        ]], [[
	PAPI_is_initialized();
        ]]), [ AC_MSG_RESULT(yes)

            AM_CONDITIONAL(HAVE_PAPI, true)
            AC_DEFINE(HAVE_PAPI, 1, [Define to 1 if you have PAPI.])

        ], [ AC_MSG_RESULT(no)

            AM_CONDITIONAL(HAVE_PAPI, false)
            PAPI_CPPFLAGS=""
            PAPI_LDFLAGS=""
            PAPI_LIBS=""

        ]
    )

    CPPFLAGS=$papi_saved_CPPFLAGS
    LDFLAGS=$papi_saved_LDFLAGS

    AC_SUBST(PAPI_CPPFLAGS)
    AC_SUBST(PAPI_LDFLAGS)
    AC_SUBST(PAPI_LIBS)

])

################################################################################
# Check for SQLite (http://www.sqlite.org)
################################################################################

AC_DEFUN([AC_PKG_SQLITE], [

    AC_ARG_WITH(sqlite,
                AC_HELP_STRING([--with-sqlite=DIR],
                               [SQLite installation @<:@/usr@:>@]),
                sqlite_dir=$withval, sqlite_dir="/usr")

    SQLITE_CPPFLAGS="-I$sqlite_dir/include"
    SQLITE_LDFLAGS="-L$sqlite_dir/$abi_libdir"
    SQLITE_LIBS="-lsqlite3"

    sqlite_saved_CPPFLAGS=$CPPFLAGS
    sqlite_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $SQLITE_CPPFLAGS"
    LDFLAGS="$LDFLAGS $SQLITE_LDFLAGS $SQLITE_LIBS"

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

    AC_SUBST(SQLITE_CPPFLAGS)
    AC_SUBST(SQLITE_LDFLAGS)
    AC_SUBST(SQLITE_LIBS)

    AC_DEFINE(HAVE_SQLITE, 1, [Define to 1 if you have SQLite.])

])





################################################################################
# Check for python (http://www.python.org)
################################################################################
## Find the install dirs for the python installation.

## Credits for this particular version of AM_CHECK_PYTHON_HEADERS 
## By James Henstridge from http://source.macgimp.org/acinclude.m4

dnl a macro to check for ability to create python extensions
dnl AM_CHECK_PYTHON_HEADERS([ACTION-IF-POSSIBLE], [ACTION-IF-NOT-POSSIBLE])
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
        PYTHON_LDFLAGS="-L$python_dir/$abi_libdir"
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
        AC_MSG_CHECKING([for Python ${PYTHON_VERSION} headers and library (in $python_dir/$abi_libdir)])
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

        PYTHON_LDFLAGS="-L$python_dir/$abi_libdir/python${PYTHON_VERSION}/config"

        AC_MSG_CHECKING([for Python ${PYTHON_VERSION} headers and library (in /usr/$abi_libdir/python${PYTHON_VERSION}/config)])
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

################################################################################
# Check for QT libraries
# Some of this code was found by an internet search for qt configuration/
# checking, so credit to those who originally created parts of the code below.
################################################################################

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
      if test -f $QTDIR/$abi_libdir/lib$X.so -o -f $QTDIR/$abi_libdir/lib$X.a; then
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
# QTLIB_CPPFLAGS="$CPPFLAGS -I$QTDIR/include -I$KDEDIR/include -D_REENTRANT  -DQT_NO_DEBUG -DQT_THREAD_SUPPORT -DQT_SHARED"
QTLIB_CPPFLAGS="$CPPFLAGS -I$QTDIR/include -I$KDEDIR/include -DQT_NO_DEBUG -DQT_THREAD_SUPPORT -DQT_SHARED"
QTLIB_LIBS="-L$QTDIR/$abi_libdir -L/usr/X11R6/$abi_libdir"
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
            QTLIB_LDFLAGS="-L$ROOT/$abi_libdir"
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
