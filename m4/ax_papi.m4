################################################################################
# Copyright (c) 2006-2014 Krell Institute. All Rights Reserved.
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
# Check for PAPI (http://icl.cs.utk.edu/papi)
################################################################################

AC_DEFUN([AX_PAPI], [

    AC_ARG_WITH(papi,
                AC_HELP_STRING([--with-papi=DIR],
                               [PAPI installation @<:@/usr@:>@]),
                papi_dir=$withval, papi_dir="/usr")

    if test -d $papi_dir/$abi_libdir ; then
	use_abi_libdir=$papi_dir/$abi_libdir
    else
	use_abi_libdir=$papi_dir/$alt_abi_libdir
    fi

    PAPI_CPPFLAGS="-I$papi_dir/include"
    PAPI_LDFLAGS="-L$use_abi_libdir"
    PAPI_DIR="$papi_dir"

    case "$host" in
        powerpc64-*-linux*) 
	    PAPI_LIBS="-lpapi"
            ;;

        powerpc-*-linux*) 
	    PAPI_LIBS="-lpapi"
            ;;

	ia64-*-linux*)
	    PAPI_LIBS="-lpapi -lpfm"
            ;;
	*)
            if test -f $use_abi_libdir/libperfctr.so ; then
              PAPI_LIBS="-lpapi -lperfctr -lpfm"
            elif test -f $use_abi_libdir/libpfm.so \
                   -o -f $use_abi_libdir/libpfm.a ; then
              PAPI_LIBS="-lpapi -lpfm"
            else
              PAPI_LIBS="-lpapi"
            fi
            ;;
    esac

    papi_saved_CPPFLAGS=$CPPFLAGS
    papi_saved_LDFLAGS=$LDFLAGS
    papi_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $PAPI_CPPFLAGS"
    LDFLAGS="$PAPI_LDFLAGS"
    LIBS="$PAPI_LIBS"

    AC_MSG_CHECKING([for PAPI library and headers])

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
        #include <papi.h>
        ]], [[
	PAPI_is_initialized();
        ]])], [ AC_MSG_RESULT(yes)

            AM_CONDITIONAL(HAVE_PAPI, true)
            AC_DEFINE(HAVE_PAPI, 1, [Define to 1 if you have PAPI.])

        ], [ AC_MSG_RESULT(no)

            AM_CONDITIONAL(HAVE_PAPI, false)
            PAPI_CPPFLAGS=""
            PAPI_LDFLAGS=""
            PAPI_LIBS=""
            PAPI_DIR=""

        ]
    )

    CPPFLAGS=$papi_saved_CPPFLAGS
    LDFLAGS=$papi_saved_LDFLAGS
    LIBS=$papi_saved_LIBS

    AC_SUBST(PAPI_CPPFLAGS)
    AC_SUBST(PAPI_LDFLAGS)
    AC_SUBST(PAPI_LIBS)
    AC_SUBST(PAPI_DIR)

])

################################################################################
# Check for PAPI for Target Architecture (http://icl.cs.utk.edu/papi)
################################################################################

AC_DEFUN([AX_TARGET_PAPI], [

    AC_ARG_WITH(target-papi,
                AC_HELP_STRING([--with-target-papi=DIR],
                               [PAPI target architecture installation @<:@/opt@:>@]),
                target_papi_dir=$withval, target_papi_dir="/zzz")


    foundPS=0

    if test "$target_papi_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_PAPI, false)
      TARGET_PAPI_DIR=""
      TARGET_PAPI_CPPFLAGS=""
      TARGET_PAPI_LDFLAGS=""
      TARGET_PAPI_LIBS=""
      foundPS=0
      AC_MSG_CHECKING([for Targetted PAPI support])
      AC_MSG_RESULT(no)
    else

      AM_CONDITIONAL(HAVE_TARGET_PAPI, true)
      AC_DEFINE(HAVE_TARGET_PAPI, 1, [Define to 1 if you have a target version of PAPI.])
      AC_MSG_CHECKING([for Targetted PAPI support])
      AC_MSG_RESULT(yes)

      TARGET_PAPI_DIR="$target_papi_dir"
      TARGET_PAPI_CPPFLAGS="-I$target_papi_dir/include"
      if test -f $target_papi_dir/$abi_libdir/libpapi.so; then
        TARGET_PAPI_LDFLAGS="-L$target_papi_dir/$abi_libdir"
        foundPS=1
      elif test -f $target_papi_dir/$abi_libdir/libpapi.a; then
        TARGET_PAPI_LDFLAGS="-L$target_papi_dir/$abi_libdir"
        foundPS=0
      elif test -f $target_papi_dir/$alt_abi_libdir/libpapi.so; then
        TARGET_PAPI_LDFLAGS="-L$target_papi_dir/$alt_abi_libdir"
        foundPS=1
      elif test -f $target_papi_dir/$alt_abi_libdir/libpapi.a; then
        TARGET_PAPI_LDFLAGS="-L$target_papi_dir/$alt_abi_libdir"
        foundPS=0
      fi

      case "$target_os" in
	cray-xk)
	    TARGET_PAPI_LIBS="-lpapi -lpfm"
            ;;
	cray-xe)
	    TARGET_PAPI_LIBS="-lpapi -lpfm"
            ;;
	cray-xt5)
	    TARGET_PAPI_LIBS="-lpapi -lpfm"
            ;;
	*)
            if test -f $target_papi_dir/$abi_libdir/libperfctr.so; then
              TARGET_PAPI_LIBS="-lpapi -lperfctr"
              if test -f $target_papi_dir/$abi_libdir/libpfm.so ; then
                TARGET_PAPI_LIBS="-lpapi -lperfctr -lpfm"
              elif test -f $target_papi_dir/$abi_libdir/libpfm.a ; then
                TARGET_PAPI_LIBS="-lpapi -lperfctr -lpfm"
              fi
            elif test -f $target_papi_dir/$alt_abi_libdir/libperfctr.so; then
                TARGET_PAPI_LIBS="-lpapi -lperfctr"
              if test -f $target_papi_dir/$alt_abi_libdir/libpfm.so ; then
                TARGET_PAPI_LIBS="-lpapi -lperfctr -lpfm"
              elif test -f $target_papi_dir/$alt_abi_libdir/libpfm.a ; then
                TARGET_PAPI_LIBS="-lpapi -lperfctr -lpfm"
              fi
            else
              if test -f $target_papi_dir/$abi_libdir/libpfm.so; then
                TARGET_PAPI_LIBS="-lpapi -lpfm"
              elif test -f $target_papi_dir/$abi_libdir/libpfm.a ; then
                TARGET_PAPI_LIBS="-lpapi -lpfm"
              elif test -f $target_papi_dir/$alt_abi_libdir/libpfm.so; then
                TARGET_PAPI_LIBS="-lpapi -lpfm"
              elif test -f $target_papi_dir/$alt_abi_libdir/libpfm.a ; then
                TARGET_PAPI_LIBS="-lpapi -lpfm"
              else
                TARGET_PAPI_LIBS="-lpapi"
              fi
            fi
            ;;
      esac

    fi

    AC_MSG_CHECKING([for Shared Targetted PAPI support])
    if test $foundPS == 1; then
        AM_CONDITIONAL(HAVE_TARGET_PAPI_SHARED, true)
        AC_DEFINE(HAVE_TARGET_PAPI_SHARED, 1, [Define to 1 if you have a target version of PAPI that has shared libs.])
        AC_MSG_RESULT(yes)
    else
        AM_CONDITIONAL(HAVE_TARGET_PAPI_SHARED, false)
        AC_MSG_RESULT(no)
    fi

    AC_SUBST(TARGET_PAPI_DIR)
    AC_SUBST(TARGET_PAPI_CPPFLAGS)
    AC_SUBST(TARGET_PAPI_LDFLAGS)
    AC_SUBST(TARGET_PAPI_LIBS)

])
