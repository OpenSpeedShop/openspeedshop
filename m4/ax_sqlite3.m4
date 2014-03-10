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
# Check for SQLite (http://www.sqlite.org)
################################################################################

AC_DEFUN([AX_SQLITE], [

    found_sqlite=0
    AC_ARG_WITH(sqlite,
                AC_HELP_STRING([--with-sqlite=DIR],
                               [SQLite installation @<:@/usr@:>@]),
                sqlite_dir=$withval, sqlite_dir="/usr")

    SQLITE_CPPFLAGS="-I$sqlite_dir/include"

    # Test for installations on debian/ubuntu systems
    if test -f $sqlite_dir/lib/x86_64-linux-gnu/libsqlite3.so; then
      SQLITE_LDFLAGS="-L$sqlite_dir/lib/x86_64-linux-gnu"
    else 
      SQLITE_LDFLAGS="-L$sqlite_dir/$abi_libdir"
    fi

    SQLITE_LIBS="-lsqlite3"

    sqlite_saved_CPPFLAGS=$CPPFLAGS
    sqlite_saved_LDFLAGS=$LDFLAGS
    sqlite_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $SQLITE_CPPFLAGS"
    LDFLAGS="$LDFLAGS $SQLITE_LDFLAGS"
    LIBS="$SQLITE_LIBS"

    AC_MSG_CHECKING([for SQLite library and headers])

    LIBS="${LIBS} $SQLITE_LDFLAGS $SQLITE_LIBS" 
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
        #include <sqlite3.h>
        ]], [[
	sqlite3_libversion();
        ]])],[ found_sqlite=1 ], [ found_sqlite=0 ]
    )

    CPPFLAGS=$sqlite_saved_CPPFLAGS
    LDFLAGS=$sqlite_saved_LDFLAGS
    LIBS=$sqlite_saved_LIBS

    if test $found_sqlite -eq 1; then
      AC_MSG_RESULT([yes])
      AC_DEFINE(HAVE_SQLITE, 1, [Define to 1 if you have SQLite.])
      AM_CONDITIONAL(HAVE_SQLITE, true)
    else
      AC_MSG_RESULT([cannot locate SQLite library and/or headers.]) 
      # If want this to cause configure failure and stopage, add in AC_MSG_FAILURE
      #AC_MSG_FAILURE(cannot locate SQLite library and/or headers.) 
      AC_MSG_RESULT([no]) 
      AM_CONDITIONAL(HAVE_SQLITE, false)
      SQLITE_CPPFLAGS=""
      SQLITE_LDFLAGS=""
      SQLITE_LIBS=""
    fi

    AC_SUBST(SQLITE_CPPFLAGS)
    AC_SUBST(SQLITE_LDFLAGS)
    AC_SUBST(SQLITE_LIBS)
])


#############################################################################################
# Check for sqlite for Target Architecture   (http://www.sqlite.org)
#############################################################################################

AC_DEFUN([AX_TARGET_SQLITE], [

    AC_ARG_WITH(target-sqlite,
                AC_HELP_STRING([--with-target-sqlite=DIR],
                               [sqlite target architecture installation @<:@/opt@:>@]),
                target_sqlite_dir=$withval, target_sqlite_dir="/zzz")

    AC_MSG_CHECKING([for Targetted sqlite support])

    if test "$target_sqlite_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_SQLITE, false)
      TARGET_SQLITE_CPPFLAGS=""
      TARGET_SQLITE_LDFLAGS=""
      TARGET_SQLITE_LIBS=""
      TARGET_SQLITE_DIR=""
      AC_MSG_RESULT(no)
    else
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_TARGET_SQLITE, true)
      AC_DEFINE(HAVE_TARGET_SQLITE, 1, [Define to 1 if you have a target version of SQLITE.])
      TARGET_SQLITE_CPPFLAGS="-I$target_sqlite_dir/include"
      TARGET_SQLITE_LDFLAGS="-L$target_sqlite_dir/$abi_libdir"
      TARGET_SQLITE_LIBS="-lsqlite3"
      TARGET_SQLITE_DIR="$target_sqlite_dir"
    fi


    AC_SUBST(TARGET_SQLITE_CPPFLAGS)
    AC_SUBST(TARGET_SQLITE_LDFLAGS)
    AC_SUBST(TARGET_SQLITE_LIBS)
    AC_SUBST(TARGET_SQLITE_DIR)

])
