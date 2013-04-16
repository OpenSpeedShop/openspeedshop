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
# Check for QT libraries
# Some of this code was found by an internet search for qt configuration/
# checking, so credit to those who originally created parts of the code below.
################################################################################


AC_DEFUN([AX_QTLIB], [ 
dnl if QTDIR is not default to /usr 

found_all_qt=1

if test "x$QTDIR" = "x"; then
   LQTDIR=/usr
else
   LQTDIR=$QTDIR
fi

dnl Support the user enabled --with-qtlib option.
    AC_ARG_WITH(qtlib,
                AC_HELP_STRING([--with-qtlib=DIR],
                               [QTLIB installation @<:@/usr@:>@]),
                [LQTDIR=$withval AC_SUBST(LQTDIR) qtlib_dir=$withval], qtlib_dir="ac_cv_qtlib")

dnl Check for QTDIR environment variable.
AC_MSG_CHECKING([whether QTDIR environment variable is set])
#if test "x$LQTDIR" = "x"; then
#  AC_MSG_RESULT([no])
#  AC_MSG_ERROR([QTDIR must be properly set.])
#else
#  AC_MSG_RESULT([$LQTDIR])
#fi

dnl Use temporary variable to hold initial library abi setting.
dnl This will be changed if initial checks don't find qt libs.
dnl Then the temporary variable will be checked for the alternative lib location.
dnl
tmp_abi_libdir=$abi_libdir

dnl Checks for Qt library.
dnl Start checking assuming $LQTDIR is $QTDIR, if we don't find anything with that we'll
dnl start trying in /usr/lib{64}/qt3 and later /usr/lib{64}/qt-3.3
AC_CACHE_CHECK([for Qt library],
  ac_cv_qtlib, [
  for X in qt-mt qt; do
    if test "x$ac_cv_qtlib" = "x"; then
      if test -f $LQTDIR/$abi_libdir/lib$X.so -o -f $LQTDIR/$abi_libdir/lib$X.a; then
        ac_cv_qtlib=$X
        tmp_lqtdir=$LQTDIR
      fi
    fi
  done
])

AC_MSG_RESULT([Temporary output: ac_cv_qtlib=$ac_cv_qtlib])

if test "x$ac_cv_qtlib" = "x"; then
dnl Conduct another check for the QT libs HERE look in lib or lib64
dnl depending on where the first check looked. See configure.ac
dnl section for determining abi_libdir.
  AC_CACHE_CHECK([for Qt library],
    ac_cv_qtlib, [
    for X in qt-mt qt; do
      if test "x$ac_cv_qtlib" = "x"; then
        if test -f $LQTDIR/$alt_abi_libdir/lib$X.so -o -f $LQTDIR/$alt_abi_libdir/lib$X.a; then
          ac_cv_qtlib=$X
	  tmp_abi_libdir=$alt_abi_libdir
	  tmp_lqtdir=$LQTDIR
        fi
      fi
    done
  ])

  AC_MSG_RESULT([Temporary output: ac_cv_qtlib=$ac_cv_qtlib])

#  if test "x$ac_cv_qtlib" = "x"; then
#    AC_MSG_ERROR([Qt library not found. Maybe QTDIR isn't properly set.])
#  fi

AC_MSG_RESULT([Temporary output: ac_cv_qtlib=$ac_cv_qtlib])

#
# Start trying /usr/lib/qt3 and /usr/lib64/qt3 locations
LQTDIR=/usr/$abi_libdir/qt3

if test "x$ac_cv_qtlib" = "x"; then
dnl Conduct another check for the QT libs HERE look in lib or lib64
dnl depending on where the first check looked. See configure.ac
dnl section for determining abi_libdir.
  AC_CACHE_CHECK([for Qt library],
    ac_cv_qtlib, [
    for X in qt-mt qt; do
      if test "x$ac_cv_qtlib" = "x"; then
        if test -f $LQTDIR/$abi_libdir/lib$X.so -o -f $LQTDIR/$abi_libdir/lib$X.a; then
          ac_cv_qtlib=$X
	  tmp_abi_libdir=$abi_libdir
	  tmp_lqtdir=$LQTDIR
        fi
      fi
    done
  ])

  AC_MSG_RESULT([Temporary output: ac_cv_qtlib=$ac_cv_qtlib])
fi

# Start trying /usr/lib/qt3 and /usr/lib64/qt3 locations

if test "x$ac_cv_qtlib" = "x"; then
dnl Conduct another check for the QT libs HERE look in lib or lib64
dnl depending on where the first check looked. See configure.ac
dnl section for determining abi_libdir.
  AC_CACHE_CHECK([for Qt library],
    ac_cv_qtlib, [
    for X in qt-mt qt; do
      if test "x$ac_cv_qtlib" = "x"; then
        if test -f $LQTDIR/$alt_abi_libdir/lib$X.so -o -f $LQTDIR/$alt_abi_libdir/lib$X.a; then
          ac_cv_qtlib=$X
	  tmp_abi_libdir=$alt_abi_libdir
	  tmp_lqtdir=$LQTDIR
        fi
      fi
    done
  ])

  AC_MSG_RESULT([Temporary output: ac_cv_qtlib=$ac_cv_qtlib])
fi

# Start trying /usr/lib/qt3 and /usr/lib64/qt3 locations

LQTDIR=/usr/$alt_abi_libdir/qt3.3

if test "x$ac_cv_qtlib" = "x"; then
dnl Conduct another check for the QT libs HERE look in lib or lib64
dnl depending on where the first check looked. See configure.ac
dnl section for determining abi_libdir.
  AC_CACHE_CHECK([for Qt library],
    ac_cv_qtlib, [
    for X in qt-mt qt; do
      if test "x$ac_cv_qtlib" = "x"; then
        if test -f $LQTDIR/$abi_libdir/lib$X.so -o -f $LQTDIR/$abi_libdir/lib$X.a; then
          ac_cv_qtlib=$X
	  tmp_abi_libdir=$abi_libdir
	  tmp_lqtdir=$LQTDIR
        fi
      fi
    done
  ])

  AC_MSG_RESULT([Temporary output: ac_cv_qtlib=$ac_cv_qtlib])
fi

# Start trying /usr/lib/qt-3.3 and /usr/lib64/qt-3.3 locations

if test "x$ac_cv_qtlib" = "x"; then
dnl Conduct another check for the QT libs HERE look in lib or lib64
dnl depending on where the first check looked. See configure.ac
dnl section for determining abi_libdir.
  AC_CACHE_CHECK([for Qt library],
    ac_cv_qtlib, [
    for X in qt-mt qt; do
      if test "x$ac_cv_qtlib" = "x"; then
        if test -f $LQTDIR/$alt_abi_libdir/lib$X.so -o -f $LQTDIR/$alt_abi_libdir/lib$X.a; then
          ac_cv_qtlib=$X
	  tmp_abi_libdir=$alt_abi_libdir
	  tmp_lqtdir=$LQTDIR
        fi
      fi
    done
  ])

  AC_MSG_RESULT([Temporary output: ac_cv_qtlib=$ac_cv_qtlib])

#  if test "x$ac_cv_qtlib" = "x"; then
#    AC_MSG_ERROR([Qt library not found. Maybe QTDIR isn't properly set.])
#  fi

fi
#
# Now start trying /usr/lib/qt-3.3 and /usr/lib64/qt-3.3 locations

LQTDIR=/usr/$abi_libdir/qt-3.3

if test "x$ac_cv_qtlib" = "x"; then
dnl Conduct another check for the QT libs HERE look in lib or lib64
dnl depending on where the first check looked. See configure.ac
dnl section for determining abi_libdir.
  AC_CACHE_CHECK([for Qt library],
    ac_cv_qtlib, [
    for X in qt-mt qt; do
      if test "x$ac_cv_qtlib" = "x"; then
        if test -f $LQTDIR/$abi_libdir/lib$X.so -o -f $LQTDIR/$abi_libdir/lib$X.a; then
          ac_cv_qtlib=$X
	  tmp_abi_libdir=$abi_libdir
	  tmp_lqtdir=$LQTDIR
        fi
      fi
    done
  ])

  AC_MSG_RESULT([Temporary output: ac_cv_qtlib=$ac_cv_qtlib])
fi

# Try alt_abi_libdir now

if test "x$ac_cv_qtlib" = "x"; then
dnl Conduct another check for the QT libs HERE look in lib or lib64
dnl depending on where the first check looked. See configure.ac
dnl section for determining abi_libdir.
  AC_CACHE_CHECK([for Qt library],
    ac_cv_qtlib, [
    for X in qt-mt qt; do
      if test "x$ac_cv_qtlib" = "x"; then
        if test -f $LQTDIR/$alt_abi_libdir/lib$X.so -o -f $LQTDIR/$alt_abi_libdir/lib$X.a; then
          ac_cv_qtlib=$X
	  tmp_abi_libdir=$alt_abi_libdir
	  tmp_lqtdir=$LQTDIR
        fi
      fi
    done
  ])

  AC_MSG_RESULT([Temporary output: ac_cv_qtlib=$ac_cv_qtlib])
fi

# Start trying /usr/lib64/qt-3.3 locations

LQTDIR=/usr/$alt_abi_libdir/qt3.3

if test "x$ac_cv_qtlib" = "x"; then
dnl Conduct another check for the QT libs HERE look in lib or lib64
dnl depending on where the first check looked. See configure.ac
dnl section for determining abi_libdir.
  AC_CACHE_CHECK([for Qt library],
    ac_cv_qtlib, [
    for X in qt-mt qt; do
      if test "x$ac_cv_qtlib" = "x"; then
        if test -f $LQTDIR/$abi_libdir/lib$X.so -o -f $LQTDIR/$abi_libdir/lib$X.a; then
          ac_cv_qtlib=$X
	  tmp_abi_libdir=$abi_libdir
	  tmp_lqtdir=$LQTDIR
        fi
      fi
    done
  ])

  AC_MSG_RESULT([Temporary output: ac_cv_qtlib=$ac_cv_qtlib])
fi

# Try alt_abi_libdir for  /usr/lib64/qt-3.3

if test "x$ac_cv_qtlib" = "x"; then
dnl Conduct another check for the QT libs HERE look in lib or lib64
dnl depending on where the first check looked. See configure.ac
dnl section for determining abi_libdir.
  AC_CACHE_CHECK([for Qt library],
    ac_cv_qtlib, [
    for X in qt-mt qt; do
      if test "x$ac_cv_qtlib" = "x"; then
        if test -f $LQTDIR/$alt_abi_libdir/lib$X.so -o -f $LQTDIR/$alt_abi_libdir/lib$X.a; then
          ac_cv_qtlib=$X
	  tmp_abi_libdir=$alt_abi_libdir
	  tmp_lqtdir=$LQTDIR
        fi
      fi
    done
  ])

  AC_MSG_RESULT([Temporary output: ac_cv_qtlib=$ac_cv_qtlib])

#  if test "x$ac_cv_qtlib" = "x"; then
#    AC_MSG_ERROR([Qt library not found. Maybe QTDIR isn't properly set.])
#  fi

fi

AC_SUBST(ac_cv_qtlib)

dnl Check for Qt multi-thread support.

#  if test "x$ac_cv_qtlib" = "x"; then
#    AC_MSG_ERROR([Qt library not found. Maybe QTDIR isn't properly set.])
#  fi
fi

AC_SUBST(ac_cv_qtlib)
dnl Now we know the path to the library for QT, it is in $tmp_lqtdir

AC_MSG_RESULT([Temporary output: tmp_lqtdir=$tmp_lqtdir])

dnl Check for Qt multi-thread support.
if test "x$ac_cv_qtlib" = "xqt-mt"; then
  ac_thread="thread"
fi
AC_SUBST(ac_thread)

dnl Set initial values for QTLIB exports
QTLIB_CFLAGS="$CFLAGS -I$tmp_lqtdir/include -I$KDEDIR/include"
QTLIB_CPPFLAGS="$CPPFLAGS -I$tmp_lqtdir/include -I$KDEDIR/include -DQT_NO_DEBUG -DQT_THREAD_SUPPORT -DQT_SHARED"


dnl Use temporary abi library setting to set the QT library variables
QTLIB_LIBS="-L$tmp_lqtdir/$tmp_abi_libdir -L/usr/X11R6/$tmp_abi_libdir"
QTLIB_LDFLAGS="-l$ac_cv_qtlib"

dnl Save the current CPPFLAGS and LDFLAGS variables prior to qt version test
qtlib_saved_CPPFLAGS=$CPPFLAGS
qtlib_saved_LDFLAGS=$LDFLAGS
qtlib_saved_LIBS=$LIBS

dnl Use QT CPPFLAGS and LDFLAGS variables for the qt version test
CPPFLAGS="$CPPFLAGS $QTLIB_CPPFLAGS"
LDFLAGS="$LDFLAGS $QTLIB_LDFLAGS"
LIBS="$QTLIB_LIBS"

dnl Check if we have qglobal.h which is an indication of QT 3.3 or greater
AC_CACHE_CHECK( [for Qt library version >= 3.3.0], ac_cv_qtlib_version,
[
#    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#	#include <qglobal.h>
#	        ]], [[
#	        ]])], AC_MSG_RESULT(yes), [ AC_MSG_RESULT(no)
#	        AC_MSG_FAILURE(Can not locate Qt library and/or the headers.) ]
#   	    )
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
	#include <qglobal.h>
	        ]], [[
	        ]])], found_qt_inc=1, [ found_qt_inc=0 ]
   	    )

    if test $found_qt_inc -eq 1; then
      AC_MSG_RESULT(yes)
    else
      QTLIB_CFLAGS="$CFLAGS -I$tmp_lqtdir/include/qt3 -I$KDEDIR/include"
      QTLIB_CPPFLAGS="$CPPFLAGS -I$tmp_lqtdir/include/qt3 -I$KDEDIR/include -DQT_NO_DEBUG -DQT_THREAD_SUPPORT -DQT_SHARED"
dnl Use QT CPPFLAGS and LDFLAGS variables for the qt version test
      CPPFLAGS="$CPPFLAGS $QTLIB_CPPFLAGS"
      LDFLAGS="$LDFLAGS $QTLIB_LDFLAGS"
      LIBS="$QTLIB_LIBS"

      AC_LINK_IFELSE([AC_LANG_PROGRAM([[
  	#include <qglobal.h>
	        ]], [[
	        ]])], found_qt_inc=1, [ found_qt_inc=0 ]
   	    )
    fi

    if test $found_qt_inc -eq 1; then
      AC_MSG_RESULT(yes)
    else
        found_all_qt=0
        AC_MSG_RESULT(no)
#	AC_MSG_FAILURE(Can not locate Qt library and/or the headers.) 
        AC_MSG_RESULT([Can not locate Qt library and/or the headers.])
    fi

    AC_TRY_LINK( [
		#include "qglobal.h"
	], [
		#if QT_VERSION < 0x030300
		#error Qt library 3.3 or greater is required.
		#endif
	], [
		ac_cv_qtlib_version="yes"
		QT_CFLAGS="$CFLAGS"
		QT_LIBS="-l$ac_cv_qtlib"
		QT_LDFLAGS="$LIBS"
	], [
		QT_CFLAGS=""
		QT_LIBS=""
		QT_LDFLAGS=""
		#AC_MSG_ERROR([** QT version 3.3 or greater is required for the Open|SpeedShop QT gui.])
#	        AC_MSG_FAILURE(QT version 3.3 or greater was not found.) 
                AC_MSG_RESULT([QT version 3.3 or greater was not found.])
                found_all_qt=0
	])

] )

dnl A common error message:
ac_qtdir_errmsg="Not found in current PATH. Maybe QT development environment isn't available (qt3-devel)."

dnl Check for Qt qmake utility.
AC_PATH_PROG(ac_qmake, qmake, [no], $tmp_lqtdir/bin:${PATH})
if test "x$ac_qmake" = "xno"; then
   #AC_MSG_ERROR([qmake $ac_qtdir_errmsg])
#   AC_MSG_FAILURE(QT qmake was not found.) 
   AC_MSG_RESULT([QT qmake was not found.])
   found_all_qt=0
else
   QTLIB_HOME=$tmp_lqtdir
fi

    AC_SUBST(ac_qmake)

dnl Support the user enabled --with-qtlib option.
    AC_ARG_WITH(qtlib,
                AC_HELP_STRING([--with-qtlib=DIR],
                               [QTLIB installation @<:@/usr@:>@]),
                qtlib_dir=$withval, qtlib_dir="ac_cv_qtlib")


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
    LIBS=$qtlib_saved_LIBS

dnl Make the QTLIB flags/libs available
    
    if test $found_all_qt -eq 0; then
	QTLIB_CFLAGS=""
	QTLIB_LIBS=""
	QTLIB_LDFLAGS=""
	QTLIB_CPPFLAGS=""
	QTLIB_HOME=""
        AM_CONDITIONAL(HAVE_QTLIB, false)
    fi

    AC_SUBST(QTLIB_HOME)
    AC_SUBST(QTLIB_CPPFLAGS)
    AC_SUBST(QTLIB_LDFLAGS)
    AC_SUBST(QTLIB_LIBS)
    if test $found_all_qt -eq 1; then
      AM_CONDITIONAL(HAVE_QTLIB, true)
      AC_DEFINE(HAVE_QTLIB, 1, [Define to 1 if you have Qt library 3.3 >])
    fi

])
