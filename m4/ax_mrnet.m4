################################################################################
# Copyright (c) 2010-2014 Krell Institute. All Rights Reserved.
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

AC_DEFUN([AX_MRNET], [

    foundMRNET=0

    AC_ARG_WITH(mrnet-version,
                AC_HELP_STRING([--with-mrnet-version=VERS],
                               [mrnet-version installation @<:@4.0.0@:>@]),
                mrnet_vers=$withval, mrnet_vers="4.0.0")


    AC_ARG_WITH(mrnet,
                AC_HELP_STRING([--with-mrnet=DIR],
                               [MRNet installation @<:@/usr@:>@]),
                mrnet_dir=$withval, mrnet_dir="/usr")

#   Temporary fix for problems with the MRNet public header files.
#   They depend on os_linux or other flags being set
    os_release=`uname -r`
    case "$os_release" in
      2.4*)
        MRNET_CPPFLAGS="-Dos_linux=24"
        ;;
      2.6*)
        MRNET_CPPFLAGS="-Dos_linux=26"
        ;;
      *)
        MRNET_CPPFLAGS="-Dos_linux"
        ;;
    esac

#   The default is to use mrnet-2.2 cppflags and libs.  
#   Change that (the default case entry) when you change the default vers to something other than 2.2

    case "$mrnet_vers" in
	"2.0.1")
            MRNET_CPPFLAGS="$MRNET_CPPFLAGS -I$mrnet_dir/include -I$mrnet_dir/include/mrnet"
            ;;
	"2.1")
            MRNET_CPPFLAGS="$MRNET_CPPFLAGS -I$mrnet_dir/include -I$mrnet_dir/include/mrnet -DMRNET_21=1"
            ;;
	"2.2")
            MRNET_CPPFLAGS="$MRNET_CPPFLAGS -I$mrnet_dir/include -I$mrnet_dir/include/mrnet -DMRNET_22=1"
            ;;
	"3.0")
            MRNET_CPPFLAGS="$MRNET_CPPFLAGS -I$mrnet_dir/include -I$mrnet_dir/include/mrnet -DMRNET_30=1"
            ;;
	"3.0.1")
            MRNET_CPPFLAGS="$MRNET_CPPFLAGS -I$mrnet_dir/include -I$mrnet_dir/include/mrnet -DMRNET_30=1 -DMRNET_301=1"
            ;;
	"3.1.0")
            MRNET_CPPFLAGS="$MRNET_CPPFLAGS -I$mrnet_dir/include -I$mrnet_dir/include/mrnet -DMRNET_30=1 -DMRNET_301=1"
            ;;
	"20110714")
            MRNET_CPPFLAGS="$MRNET_CPPFLAGS -I$mrnet_dir/include -I$mrnet_dir/include/mrnet -DMRNET_30=1 -DMRNET_301=1"
            ;;
	"400a")
            MRNET_CPPFLAGS="$MRNET_CPPFLAGS -I$mrnet_dir/include -I$mrnet_dir/include/mrnet -I$mrnet_dir/$abi_libdir -DMRNET_30=1 -DMRNET_301=1"
            ;;
	"4.0.0")
            MRNET_CPPFLAGS="$MRNET_CPPFLAGS -I$mrnet_dir/include -I$mrnet_dir/include/mrnet -I$mrnet_dir/$abi_libdir/mrnet-4.0.0/include -I$mrnet_dir/$abi_libdir/xplat-4.0.0/include -DMRNET_30=1 -DMRNET_301=1"
            ;;
	*)
            MRNET_CPPFLAGS="$MRNET_CPPFLAGS -I$mrnet_dir/include -I$mrnet_dir/include/mrnet -I$mrnet_dir/$abi_libdir/mrnet-4.0.0/include -I$mrnet_dir/$abi_libdir/xplat-4.0.0/include -DMRNET_30=1 -DMRNET_301=1"
            ;;
    esac

    MRNET_LDFLAGS="-L$mrnet_dir/$abi_libdir"
    MRNET_LIBS="-Wl,--whole-archive -lmrnet -lxplat -Wl,--no-whole-archive"
    MRNET_LIBS="$MRNET_LIBS -lpthread -ldl"
    MRNET_LW_LIBS="-Wl,--whole-archive -lmrnet_lightweight -lxplat_lightweight -Wl,--no-whole-archive"
    MRNET_LW_LIBS="$MRNET_LW_LIBS -lpthread -ldl"

    MRNET_LWR_LIBS="-Wl,--whole-archive -lmrnet_lightweight_r -lxplat_lightweight_r -Wl,--no-whole-archive"
    MRNET_LWR_LIBS="$MRNET_LWR_LIBS -lpthread -ldl"

    MRNET_DIR="$mrnet_dir"

    if [ test -z "$SYSROOT_DIR" ]; then
      # SYSROOT_DIR was not set
      if [ test -d /usr/lib/alps ] && [ test -f $mrnet_dir/$abi_libdir/libmrnet.so -o -f $mrnet_dir/$abi_libdir/libmrnet.a ]; then
        MRNET_LDFLAGS="$MRNET_LDFLAGS -L/usr/lib/alps"
        MRNET_LDFLAGS="$MRNET_LDFLAGS -L/usr/lib64"
        MRNET_LIBS="$MRNET_LIBS -lalps -lalpslli -lalpsutil"
        MRNET_LIBS="$MRNET_LIBS -lxmlrpc-epi"
      fi
    else
      if [ test -d $SYSROOT_DIR/usr/lib/alps ] && [ test -f $mrnet_dir/$abi_libdir/libmrnet.so -o -f $mrnet_dir/$abi_libdir/libmrnet.a ]; then
        MRNET_LDFLAGS="$MRNET_LDFLAGS -L$SYSROOT_DIR/usr/lib/alps"
        MRNET_LDFLAGS="$MRNET_LDFLAGS -L$SYSROOT_DIR/usr/lib64"
        MRNET_LIBS="$MRNET_LIBS -lalps -lalpslli -lalpsutil"
        MRNET_LIBS="$MRNET_LIBS -lxmlrpc-epi"
      fi
    fi

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    mrnet_saved_CPPFLAGS=$CPPFLAGS
    mrnet_saved_LDFLAGS=$LDFLAGS
    mrnet_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $MRNET_CPPFLAGS $BOOST_CPPFLAGS"
    LDFLAGS="$LDFLAGS $MRNET_LDFLAGS"
    LIBS="$MRNET_LIBS"

    AC_MSG_CHECKING([for MRNet library and headers])

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
        #include <mrnet/MRNet.h>
        ]], [[
        MRN::set_OutputLevel(0);
        ]])], [ 
            AC_MSG_RESULT(yes)

	    foundMRNET=1

        ], [
            AC_MSG_RESULT(no)
            #AC_MSG_ERROR([MRNet could not be found.])

	    foundMRNET=0

        ])

    CPPFLAGS=$mrnet_saved_CPPFLAGS
    LDFLAGS=$mrnet_saved_LDFLAGS
    LIBS=$mrnet_saved_LIBS

    AC_LANG_POP(C++)

    AC_SUBST(MRNET_CPPFLAGS)
    AC_SUBST(MRNET_LDFLAGS)
    AC_SUBST(MRNET_LIBS)
    AC_SUBST(MRNET_LW_LIBS)
    AC_SUBST(MRNET_LWR_LIBS)
    AC_SUBST(MRNET_DIR)

    if test $foundMRNET == 1; then
        AM_CONDITIONAL(HAVE_MRNET, true)
        AC_DEFINE(HAVE_MRNET, 1, [Define to 1 if you have MRNet.])
    else
        AM_CONDITIONAL(HAVE_MRNET, false)
    fi

])

#############################################################################################
# Check for MRNet for Target Architecture 
#############################################################################################

AC_DEFUN([AX_TARGET_MRNET], [

    AC_ARG_WITH(target-mrnet,
                AC_HELP_STRING([--with-target-mrnet=DIR],
                               [MRNet target architecture installation @<:@/opt@:>@]),
                target_mrnet_dir=$withval, target_mrnet_dir="/zzz")

    AC_ARG_WITH(mrnet-version,
                AC_HELP_STRING([--with-mrnet-version=VERS],
                               [mrnet-version installation @<:@4.0.0@:>@]),
                mrnet_vers=$withval, mrnet_vers="4.0.0")



    AC_MSG_CHECKING([for Targetted MRNet support])

    found_target_mrnet=0

    if [ !test -z $SYSROOT_DIR -a test -d $SYSROOT_DIR/usr/lib/alps ] && [ test -f $target_mrnet_dir/$abi_libdir/libmrnet.so -o -f $target_mrnet_dir/$abi_libdir/libmrnet.a ]; then
       found_target_mrnet=1
       TARGET_MRNET_LDFLAGS="-L$target_mrnet_dir/$abi_libdir"
       TARGET_MRNET_LDFLAGS="$TARGET_MRNET_LDFLAGS -L/usr/lib/alps -L/opt/cray/xt-tools/lgdb/1.4/lib/alps"
       TARGET_MRNET_LIBS="-lalpslli -lalpsutil"
       TARGET_MRNET_LWR_LIBS="$TARGET_MRNET_LWR_LIBS -lalpslli -lalpsutil"
       TARGET_MRNET_LW_LIBS="$TARGET_MRNET_LW_LIBS -lalpslli -lalpsutil"
    elif [ test -d /usr/lib/alps ] && [ test -f $target_mrnet_dir/$abi_libdir/libmrnet.so -o -f $target_mrnet_dir/$abi_libdir/libmrnet.a ]; then
       found_target_mrnet=1
       TARGET_MRNET_LDFLAGS="-L$target_mrnet_dir/$abi_libdir"
       TARGET_MRNET_LDFLAGS="$TARGET_MRNET_LDFLAGS -L/usr/lib/alps -L/opt/cray/xt-tools/lgdb/1.4/lib/alps"
       TARGET_MRNET_LIBS="-lalpslli -lalpsutil"
       TARGET_MRNET_LWR_LIBS="$TARGET_MRNET_LWR_LIBS -lalpslli -lalpsutil"
       TARGET_MRNET_LW_LIBS="$TARGET_MRNET_LW_LIBS -lalpslli -lalpsutil"
    elif [ test -d /usr/lib/alps ] && [ test -f $target_mrnet_dir/$alt_abi_libdir/libmrnet.so -o -f $target_mrnet_dir/$alt_abi_libdir/libmrnet.a ]; then
       found_target_mrnet=1
       TARGET_MRNET_LDFLAGS="-L$target_mrnet_dir/$alt_abi_libdir"
       TARGET_MRNET_LDFLAGS="$TARGET_MRNET_LDFLAGS -L/usr/lib/alps -L/opt/cray/xt-tools/lgdb/1.4/lib/alps"
       TARGET_MRNET_LIBS="-lalpslli -lalpsutil"
       TARGET_MRNET_LWR_LIBS="$TARGET_MRNET_LWR_LIBS -lalpslli -lalpsutil"
       TARGET_MRNET_LW_LIBS="$TARGET_MRNET_LW_LIBS -lalpslli -lalpsutil"
    elif [ test -f $target_mrnet_dir/$abi_libdir/libmrnet.so -o -f $target_mrnet_dir/$abi_libdir/libmrnet.a ]; then
       found_target_mrnet=1
       TARGET_MRNET_LDFLAGS="-L$target_mrnet_dir/$alt_abi_libdir"
       TARGET_MRNET_LDFLAGS="$TARGET_MRNET_LDFLAGS"
    fi

    if test $found_target_mrnet == 0 && test "$target_mrnet_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_MRNET, false)
      TARGET_MRNET_CPPFLAGS=""
      TARGET_MRNET_LDFLAGS=""
      TARGET_MRNET_LIBS=""
      TARGET_MRNET_LW_LIBS=""
      TARGET_MRNET_DIR=""
      AC_MSG_RESULT(no)
    elif test $found_target_mrnet == 1 ; then
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_TARGET_MRNET, true)
      AC_DEFINE(HAVE_TARGET_MRNET, 1, [Define to 1 if you have a target version of MRNET.])
      if test -f $target_mrnet_dir/$abi_libdir/mrnet-$mrnet_vers/include/mrnet_config.h && test -f $target_mrnet_dir/$abi_libdir/xplat-$mrnet_vers/include/xplat_config.h ; then
         TARGET_MRNET_CPPFLAGS="-I$target_mrnet_dir/include -I$target_mrnet_dir/$abi_libdir/mrnet-$mrnet_vers/include -I$target_mrnet_dir/$abi_libdir/xplat-$mrnet_vers/include  -Dos_linux"
      elif test -f $target_mrnet_dir/$abi_libdir/mrnet_config.h ; then
         TARGET_MRNET_CPPFLAGS="-I$target_mrnet_dir/include -I$target_mrnet_dir/$abi_libdir -Dos_linux"
      elif test -f $target_mrnet_dir/$alt_abi_libdir/mrnet_config.h ; then
         TARGET_MRNET_CPPFLAGS="-I$target_mrnet_dir/include -I$target_mrnet_dir/$alt_abi_libdir -Dos_linux"
      else
         TARGET_MRNET_CPPFLAGS="-I$target_mrnet_dir/include -Dos_linux"
      fi

      TARGET_MRNET_LIBS="-Wl,--whole-archive -lmrnet -lxplat -Wl,--no-whole-archive"
      TARGET_MRNET_LIBS="$TARGET_MRNET_LIBS -lpthread -ldl"
      TARGET_MRNET_LW_LIBS="-Wl,--whole-archive -lmrnet_lightweight -lxplat_lightweight -Wl,--no-whole-archive"
      TARGET_MRNET_LW_LIBS="$TARGET_MRNET_LW_LIBS -lpthread -ldl"
      TARGET_MRNET_LWR_LIBS="-Wl,--whole-archive -lmrnet_lightweight_r -lxplat_lightweight_r -Wl,--no-whole-archive"
      TARGET_MRNET_LWR_LIBS="$TARGET_MRNET_LWR_LIBS -lpthread -ldl"

      TARGET_MRNET_DIR="$target_mrnet_dir"
    else 
      AM_CONDITIONAL(HAVE_TARGET_MRNET, false)
      TARGET_MRNET_CPPFLAGS=""
      TARGET_MRNET_LDFLAGS=""
      TARGET_MRNET_LIBS=""
      TARGET_MRNET_LW_LIBS=""
      TARGET_MRNET_LWR_LIBS=""
      TARGET_MRNET_DIR=""
      AC_MSG_RESULT(no)
    fi


    AC_SUBST(TARGET_MRNET_CPPFLAGS)
    AC_SUBST(TARGET_MRNET_LDFLAGS)
    AC_SUBST(TARGET_MRNET_LIBS)
    AC_SUBST(TARGET_MRNET_LW_LIBS)
    AC_SUBST(TARGET_MRNET_LWR_LIBS)
    AC_SUBST(TARGET_MRNET_DIR)

])

