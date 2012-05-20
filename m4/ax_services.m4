################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2007 William Hachfeld. All Rights Reserved.
# Copyright (c) 2006-2011 Krell Institute. All Rights Reserved.
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
# Check for Binutils (http://www.gnu.org/software/binutils)
################################################################################

AC_DEFUN([AC_PKG_BINUTILS], [

    AC_ARG_WITH(binutils,
                AC_HELP_STRING([--with-binutils=DIR],
                               [binutils installation @<:@/usr@:>@]),
                binutils_dir=$withval, binutils_dir="/usr")


    vers_info_needed=1
    if test -f  $binutils_dir/$abi_libdir/libbfd.so && test -x $binutils_dir/$abi_libdir/libbfd.so; then
		vers_info_needed=0
    fi

    AC_ARG_WITH(binutils-version,
                AC_HELP_STRING([--with-binutils-version=VERS],
                               [binutils-version installation @<:@@:>@]),
                binutils_vers=-$withval, binutils_vers="")

    if test x"$binutils_vers" == x"" && test $vers_info_needed == 1; then
	binutils_vers="help"
        binutils_vers="`ls $binutils_dir/$abi_libdir/libbfd*.so | cut -d/ -f 4 | cut -c7-99 | sed 's/.so//'`"
    fi

# Chose the pic version over the normal version - binutils-2.20 divides the main library into pic and non-pic
    if test -f $binutils_dir/$abi_libdir/libiberty_pic.a; then
       BINUTILS_IBERTY_LIB="-liberty_pic"
       BINUTILS_IBERTY_LDFLAGS="-L$binutils_dir/$abi_libdir"
       binutils_iberty_lib="-liberty_pic"
    elif test -f $binutils_dir/$abi_libdir/libiberty.a; then
       BINUTILS_IBERTY_LIB="-liberty"
       BINUTILS_IBERTY_LDFLAGS="-L$binutils_dir/$abi_libdir"
       binutils_iberty_lib="-liberty"
    elif test -f $binutils_dir/$alt_abi_libdir/libiberty_pic.a; then
       BINUTILS_IBERTY_LIB="-liberty_pic"
       BINUTILS_IBERTY_LDFLAGS="-L$binutils_dir/$alt_abi_libdir"
       binutils_iberty_lib="-liberty_pic"
    elif test -f $binutils_dir/$alt_abi_libdir/libiberty.a; then
       BINUTILS_IBERTY_LIB="-liberty"
       BINUTILS_IBERTY_LDFLAGS="-L$binutils_dir/$alt_abi_libdir"
       binutils_iberty_lib="-liberty"
    else
       BINUTILS_IBERTY_LIB=
       BINUTILS_IBERTY_LDFLAGS=
       binutils_iberty_lib=
    fi

    case "$host" in
	ia64-*-linux*)
	    binutils_required="true"
            BINUTILS_DIR="$binutils_dir"
	    BINUTILS_CPPFLAGS="-I$binutils_dir/include"
	    BINUTILS_LDFLAGS="-L$binutils_dir/$abi_libdir"
	    BINUTILS_LIBS="-lopcodes$binutils_vers -lbfd$binutils_vers $binutils_iberty_lib"
	    BINUTILS_OPCODES_LIB="-lopcodes$binutils_vers"
	    BINUTILS_BFD_LIB="-lbfd$binutils_vers"
            ;;
	x86_64-*-linux*)
	    binutils_required="true"
            BINUTILS_DIR="$binutils_dir"
	    BINUTILS_CPPFLAGS="-I$binutils_dir/include"
	    BINUTILS_LDFLAGS="-L$binutils_dir/$abi_libdir"
	    BINUTILS_LIBS="-lopcodes$binutils_vers -lbfd$binutils_vers $binutils_iberty_lib"
	    BINUTILS_OPCODES_LIB="-lopcodes$binutils_vers"
	    BINUTILS_BFD_LIB="-lbfd$binutils_vers"
            ;;
	*)
	    binutils_required="true"
            BINUTILS_DIR="$binutils_dir"
	    BINUTILS_CPPFLAGS="-I$binutils_dir/include"
	    BINUTILS_LDFLAGS="-L$binutils_dir/$abi_libdir"
	    BINUTILS_LIBS="-lopcodes$binutils_vers -lbfd$binutils_vers $binutils_iberty_lib"
	    BINUTILS_OPCODES_LIB="-lopcodes$binutils_vers"
	    BINUTILS_BFD_LIB="-lbfd$binutils_vers"
            ;;
    esac

    binutils_saved_CPPFLAGS=$CPPFLAGS
    binutils_saved_LDFLAGS=$LDFLAGS
    binutils_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $BINUTILS_CPPFLAGS"
    LDFLAGS="$LDFLAGS $BINUTILS_LDFLAGS $BINUTILS_LIBS"

    AC_MSG_CHECKING([for binutils librarys and headers])

    AC_SEARCH_LIBS(bfd_init, [bfd$binutils_vers], 
        [ AC_MSG_RESULT(yes)

            AM_CONDITIONAL(HAVE_BINUTILS, true)
            AC_DEFINE(HAVE_BINUTILS, 1, [Define to 1 if you have BINUTILS.])

        ], [ AC_MSG_RESULT(no)

	    if test x"$binutils_required" == x"true"; then
		AM_CONDITIONAL(HAVE_BINUTILS, false)
	    else
		AM_CONDITIONAL(HAVE_BINUTILS, true)
	    fi
            BINUTILS_CPPFLAGS=""
            BINUTILS_LDFLAGS=""
            BINUTILS_LIBS=""
	    BINUTILS_OPCODES_LIB=""
	    BINUTILS_BFD_LIB=""
            BINUTILS_DIR=""
            BINUTILS_IBERTY_LIB=""
            BINUTILS_IBERTY_LDFLAGS=""

        ]
    )

    CPPFLAGS=$binutils_saved_CPPFLAGS
    LDFLAGS=$binutils_saved_LDFLAGS
    LIBS=$binutils_saved_LIBS

    AC_SUBST(BINUTILS_CPPFLAGS)
    AC_SUBST(BINUTILS_LDFLAGS)
    AC_SUBST(BINUTILS_LIBS)
    AC_SUBST(BINUTILS_OPCODES_LIB)
    AC_SUBST(BINUTILS_BFD_LIB)
    AC_SUBST(BINUTILS_DIR)
    AC_SUBST(BINUTILS_IBERTY_LIB)
    AC_SUBST(BINUTILS_IBERTY_LDFLAGS)

])

################################################################################
# Check for Binutils for Target Architecture (http://www.gnu.org/software/binutils)
################################################################################

AC_DEFUN([AC_PKG_TARGET_BINUTILS], [

    AC_ARG_WITH(target-binutils,
                AC_HELP_STRING([--with-target-binutils=DIR],
                               [binutils target architecture installation @<:@/opt@:>@]),
                target_binutils_dir=$withval, target_binutils_dir="/zzz")

    AC_MSG_CHECKING([for Targetted binutils support])

    if test "$target_binutils_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_BINUTILS, false)
      TARGET_BINUTILS_DIR=""
      TARGET_BINUTILS_CPPFLAGS=""
      TARGET_BINUTILS_LDFLAGS=""
      TARGET_BINUTILS_LIBS=""
      AC_MSG_RESULT(no)
    else
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_TARGET_BINUTILS, true)
      AC_DEFINE(HAVE_TARGET_BINUTILS, 1, [Define to 1 if you have a target version of BINUTILS.])
      case "$target_os" in
	cray-xt5)
            TARGET_BINUTILS_DIR="$target_binutils_dir"
	    TARGET_BINUTILS_CPPFLAGS="-I$target_binutils_dir/include"
	    TARGET_BINUTILS_LDFLAGS="-L$target_binutils_dir/$abi_libdir"
	    TARGET_BINUTILS_LIBS="-lopcodes-lbfd -liberty"
            ;;
	*)
            TARGET_BINUTILS_DIR="$target_binutils_dir"
	    TARGET_BINUTILS_CPPFLAGS="-I$target_binutils_dir/include"
	    TARGET_BINUTILS_LDFLAGS="-L$target_binutils_dir/$abi_libdir"
	    TARGET_BINUTILS_LIBS="-lopcodes -lbfd -liberty"
            ;;
      esac
    fi


    AC_SUBST(TARGET_BINUTILS_CPPFLAGS)
    AC_SUBST(TARGET_BINUTILS_LDFLAGS)
    AC_SUBST(TARGET_BINUTILS_LIBS)
    AC_SUBST(TARGET_BINUTILS_DIR)

])

################################################################################
# Check for Libunwind (http://www.hpl.hp.com/research/linux/libunwind)
################################################################################

AC_DEFUN([AC_PKG_LIBUNWIND], [

    AC_ARG_WITH(libunwind,
                AC_HELP_STRING([--with-libunwind=DIR],
                               [libunwind installation @<:@/usr@:>@]),
                libunwind_dir=$withval, libunwind_dir="/usr")

    LIBUNWIND_CPPFLAGS="-I$libunwind_dir/include -DUNW_LOCAL_ONLY"
    LIBUNWIND_LDFLAGS="-L$libunwind_dir/$abi_libdir"
    LIBUNWIND_LIBS="-lunwind"
    LIBUNWIND_DIR="$libunwind_dir"

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
            LIBUNWIND_DIR=""

        ]
    )

    CPPFLAGS=$libunwind_saved_CPPFLAGS
    LDFLAGS=$libunwind_saved_LDFLAGS

    AC_SUBST(LIBUNWIND_CPPFLAGS)
    AC_SUBST(LIBUNWIND_LDFLAGS)
    AC_SUBST(LIBUNWIND_LIBS)
    AC_SUBST(LIBUNWIND_DIR)

])

#############################################################################################
# Check for Libunwind for Target Architecture (http://www.hpl.hp.com/research/linux/libunwind)
#############################################################################################

AC_DEFUN([AC_PKG_TARGET_LIBUNWIND], [

    AC_ARG_WITH(target-libunwind,
                AC_HELP_STRING([--with-target-libunwind=DIR],
                               [libunwind target architecture installation @<:@/opt@:>@]),
                target_libunwind_dir=$withval, target_libunwind_dir="/zzz")

    AC_MSG_CHECKING([for Targetted libunwind support])

    if test "$target_libunwind_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_LIBUNWIND, false)
      TARGET_LIBUNWIND_CPPFLAGS=""
      TARGET_LIBUNWIND_LDFLAGS=""
      TARGET_LIBUNWIND_LIBS=""
      TARGET_LIBUNWIND_DIR=""
      AC_MSG_RESULT(no)
    else
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_TARGET_LIBUNWIND, true)
      AC_DEFINE(HAVE_TARGET_LIBUNWIND, 1, [Define to 1 if you have a target version of LIBUNWIND.])
      TARGET_LIBUNWIND_CPPFLAGS="-I$target_libunwind_dir/include -DUNW_LOCAL_ONLY"
      TARGET_LIBUNWIND_LDFLAGS="-L$target_libunwind_dir/$abi_libdir"
      TARGET_LIBUNWIND_LIBS="-lunwind"
      TARGET_LIBUNWIND_DIR="$target_libunwind_dir"
    fi


    AC_SUBST(TARGET_LIBUNWIND_CPPFLAGS)
    AC_SUBST(TARGET_LIBUNWIND_LDFLAGS)
    AC_SUBST(TARGET_LIBUNWIND_LIBS)
    AC_SUBST(TARGET_LIBUNWIND_DIR)

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
            if test -f $papi_dir/$abi_libdir/libperfctr.so -o -f /usr/$abi_libdir/libperfctr.so; then
              PAPI_LIBS="-lpapi -lperfctr -lpfm"
            elif test -f $papi_dir/$abi_libdir/libpfm.so -o -f /usr/$abi_libdir/libpfm.so
                   -o -f $papi_dir/$abi_libdir/libpfm.a -o -f /usr/$abi_libdir/libpfm.a; then
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

AC_DEFUN([AC_PKG_TARGET_PAPI], [

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
      if test -d $target_papi_dir/$abi_libdir; then
        TARGET_PAPI_LDFLAGS="-L$target_papi_dir/$abi_libdir"
      elif test -d $target_papi_dir/$alt_abi_libdir; then
        TARGET_PAPI_LDFLAGS="-L$target_papi_dir/$alt_abi_libdir"
      fi

      if test -f $target_papi_dir/$abi_libdir/libpapi.so; then
        foundPS=1
      else
        foundPS=0
      fi

      case "$target_os" in
	cray-xt5)
	    TARGET_PAPI_LIBS="-lpapi -lpfm"
            ;;
	*)
            if test -f $target_papi_dir/$abi_libdir/libperfctr.so; then
              TARGET_PAPI_LIBS="-lpapi -lperfctr -lpfm"
            elif test -f $target_papi_dir/$alt_abi_libdir/libperfctr.so; then
              TARGET_PAPI_LIBS="-lpapi -lperfctr -lpfm"
            else
              TARGET_PAPI_LIBS="-lpapi -lpfm"
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

################################################################################
# Check for Monitor (http://www.cs.utk.edu/~mucci)
################################################################################

AC_DEFUN([AC_PKG_LIBMONITOR], [

    AC_ARG_WITH(libmonitor,
                AC_HELP_STRING([--with-libmonitor=DIR],
                               [libmonitor installation @<:@/usr@:>@]),
                libmonitor_dir=$withval, libmonitor_dir="/usr")

    LIBMONITOR_CPPFLAGS="-I$libmonitor_dir/include"
    LIBMONITOR_LDFLAGS="-L$libmonitor_dir/$abi_libdir"
    LIBMONITOR_LIBS="-lmonitor"
    LIBMONITOR_DIR="$libmonitor_dir"

    libmonitor_saved_CPPFLAGS=$CPPFLAGS
    libmonitor_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $LIBMONITOR_CPPFLAGS"
    LDFLAGS="$LDFLAGS $LIBMONITOR_LDFLAGS $LIBMONITOR_LIBS -lpthread"

    AC_MSG_CHECKING([for libmonitor library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <monitor.h>
        ]], [[
        monitor_init_library();
        ]]), [ AC_MSG_RESULT(yes)

            AM_CONDITIONAL(HAVE_LIBMONITOR, true)
            AC_DEFINE(HAVE_LIBMONITOR, 1, [Define to 1 if you have libmonitor.])

        ], [ AC_MSG_RESULT(no)

            AM_CONDITIONAL(HAVE_LIBMONITOR, false)
            LIBMONITOR_CPPFLAGS=""
            LIBMONITOR_LDFLAGS=""
            LIBMONITOR_LIBS=""
            LIBMONITOR_DIR=""

        ]
    )

    CPPFLAGS=$libmonitor_saved_CPPFLAGS
    LDFLAGS=$libmonitor_saved_LDFLAGS

    AC_SUBST(LIBMONITOR_CPPFLAGS)
    AC_SUBST(LIBMONITOR_LDFLAGS)
    AC_SUBST(LIBMONITOR_LIBS)
    AC_SUBST(LIBMONITOR_DIR)

])

################################################################################
# Check for Monitor for Target Architecture (http://www.cs.utk.edu/~mucci)
################################################################################

AC_DEFUN([AC_PKG_TARGET_LIBMONITOR], [

    AC_ARG_WITH(target-libmonitor,
                AC_HELP_STRING([--with-target-libmonitor=DIR],
                               [libmonitor target architecture installation @<:@/opt@:>@]),
                target_libmonitor_dir=$withval, target_libmonitor_dir="/zzz")

    AC_MSG_CHECKING([for Targetted libmonitor support])

    if test "$target_libmonitor_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_LIBMONITOR, false)
      TARGET_LIBMONITOR_CPPFLAGS=""
      TARGET_LIBMONITOR_LDFLAGS=""
      TARGET_LIBMONITOR_LIBS=""
      TARGET_LIBMONITOR_DIR=""
      AC_MSG_RESULT(no)
    else
      AM_CONDITIONAL(HAVE_TARGET_LIBMONITOR, true)
      AC_DEFINE(HAVE_TARGET_LIBMONITOR, 1, [Define to 1 if you have a target version of LIBMONITOR.])
      TARGET_LIBMONITOR_CPPFLAGS="-I$target_libmonitor_dir/include"
      TARGET_LIBMONITOR_LDFLAGS="-L$target_libmonitor_dir/$abi_libdir"
      TARGET_LIBMONITOR_LIBS="-lmonitor"
      TARGET_LIBMONITOR_DIR="$target_libmonitor_dir"
      AC_MSG_RESULT(yes)
    fi


    AC_SUBST(TARGET_LIBMONITOR_CPPFLAGS)
    AC_SUBST(TARGET_LIBMONITOR_LDFLAGS)
    AC_SUBST(TARGET_LIBMONITOR_LIBS)
    AC_SUBST(TARGET_LIBMONITOR_DIR)

])

################################################################################
# Check for EPYDOC (http://epydoc.sourceforge.net/)   
################################################################################

AC_DEFUN([AC_PKG_EPYDOC], [

	AC_MSG_CHECKING([for epydoc binary])
	if epydoc --version >/dev/null 2>/dev/null ; then
      		AC_MSG_CHECKING([found epydoc binary])
		      AC_MSG_RESULT(yes)
		      AM_CONDITIONAL(HAVE_EPYDOC, true)
		      AC_DEFINE(HAVE_EPYDOC, 1, [Define to 1 if you have EPYDOC.])
	else
                AM_CONDITIONAL(HAVE_EPYDOC, false)
	fi
])

AC_DEFUN([AX_CBTF_SERVICES], [

    AC_ARG_WITH(cbtf-services,
            AC_HELP_STRING([--with-cbtf-services=DIR],
                           [CBTF services library installation @<:@/usr@:>@]),
                services_dir=$withval, services_dir="/usr"
		)

    CBTF_SERVICES_CPPFLAGS="-I$services_dir/include"
    CBTF_SERVICES_LDFLAGS="-L$services_dir/$abi_libdir"
    CBTF_SERVICES_BINUTILS_LIBS="-lcbtf-services-binutils"
    CBTF_SERVICES_COMMON_LIBS="-lcbtf-services-common"
    CBTF_SERVICES_DATA_LIBS="-lcbtf-services-data"
    CBTF_SERVICES_FILEIO_LIBS="-lcbtf-services-fileio"
    CBTF_SERVICES_FPE_LIBS="-lcbtf-services-fpe"
    CBTF_SERVICES_MONITOR_LIBS="-lcbtf-services-monitor"
    CBTF_SERVICES_MRNET_LIBS="-lcbtf-services-mrnet"
    CBTF_SERVICES_OFFLINE_LIBS="-lcbtf-services-offline"
    CBTF_SERVICES_PAPI_LIBS="-lcbtf-services-papi"
    CBTF_SERVICES_SEND_LIBS="-lcbtf-services-send"
    CBTF_SERVICES_TIMER_LIBS="-lcbtf-services-timer"
    CBTF_SERVICES_UNWIND_LIBS="-lcbtf-services-unwind"


    CBTF_SERVICES_LIBS="$CBTF_SERVICES_BINUTILS_LIBS $CBTF_SERVICES_COMMON_LIBS $CBTF_SERVICES_DATA_LIBS $CBTF_SERVICES_FILEIO_LIBS $CBTF_SERVICES_FPE_LIBS $CBTF_SERVICES_MONITOR_LIBS $CBTF_SERVICES_MRNET_LIBS $CBTF_SERVICES_OFFLINE_LIBS $CBTF_SERVICES_PAPI_LIBS $CBTF_SERVICES_SEND_LIBS $CBTF_SERVICES_TIMER_LIBS $CBTF_SERVICES_UNWIND_LIBS"

    services_saved_CPPFLAGS=$CPPFLAGS
    services_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $CBTF_SERVICES_CPPFLAGS"
    LDFLAGS="$LDFLAGS $CBTF_SERVICES_LDFLAGS $CBTF_SERVICES_COMMON_LIBS -lrt -lpthread"

    AC_MSG_CHECKING([for CBTF SERVICES library and headers])

    AC_SEARCH_LIBS(CBTF_GetPCFromContext, cbtf-services-common[], 
        [ AC_MSG_RESULT(yes)

        ], [ AC_MSG_RESULT(no)

            AC_MSG_ERROR([CBTF services library could not be found.])
        ]
    )

    CPPFLAGS=$services_saved_CPPFLAGS
    LDFLAGS=$services_saved_LDFLAGS

    AC_SUBST(CBTF_SERVICES_CPPFLAGS)
    AC_SUBST(CBTF_SERVICES_LDFLAGS)
    AC_SUBST(CBTF_SERVICES_LIBS)
    AC_SUBST(CBTF_SERVICES_BINUTILS_LIBS)
    AC_SUBST(CBTF_SERVICES_COMMON_LIBS)
    AC_SUBST(CBTF_SERVICES_DATA_LIBS)
    AC_SUBST(CBTF_SERVICES_FILEIO_LIBS)
    AC_SUBST(CBTF_SERVICES_FPE_LIBS)
    AC_SUBST(CBTF_SERVICES_MONITOR_LIBS)
    AC_SUBST(CBTF_SERVICES_MRNET_LIBS)
    AC_SUBST(CBTF_SERVICES_PAPI_LIBS)
    AC_SUBST(CBTF_SERVICES_SEND_LIBS)
    AC_SUBST(CBTF_SERVICES_TIMER_LIBS)
    AC_SUBST(CBTF_SERVICES_UNWIND_LIBS)

])
