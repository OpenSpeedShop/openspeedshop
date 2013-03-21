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

#############################################################################################
# Check for librt for Target Architecture 
#############################################################################################

AC_DEFUN([AC_PKG_TARGET_LIBRT], [

    AC_ARG_WITH(target-librt,
                AC_HELP_STRING([--with-target-librt=DIR],
                               [librt target architecture installation @<:@/opt@:>@]),
                target_librt_dir=$withval, target_librt_dir="/zzz")

    AC_MSG_CHECKING([for Targetted librt support])

    found_target_librt=0
    if test -f $target_librt_dir/$abi_libdir/librt.so; then
       TARGET_LIBRT_LDFLAGS="-L$target_librt_dir/$abi_libdir"
       found_target_librt=1
    elif test -f $target_librt_dir/$alt_abi_libdir/librt.so; then
       TARGET_LIBRT_LDFLAGS="-L$target_librt_dir/$alt_abi_libdir"
       found_target_librt=1
    fi

    if test $found_target_librt == 0 && test "$target_librt_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_LIBRT, false)
      TARGET_LIBRT_CPPFLAGS=""
      TARGET_LIBRT_LDFLAGS=""
      TARGET_LIBRT_LIBS="-lrt"
      TARGET_LIBRT_DIR=""
      AC_MSG_RESULT(no)
    elif test $found_target_librt == 1 ; then
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_TARGET_LIBRT, true)
      AC_DEFINE(HAVE_TARGET_LIBRT, 1, [Define to 1 if you have a target version of LIBRT.])
      TARGET_LIBRT_CPPFLAGS="-I$target_librt_dir/include -DUNW_LOCAL_ONLY"
      TARGET_LIBRT_LIBS="-lrt"
      TARGET_LIBRT_DIR="$target_librt_dir"
    else 
      AM_CONDITIONAL(HAVE_TARGET_LIBRT, false)
      TARGET_LIBRT_CPPFLAGS=""
      TARGET_LIBRT_LDFLAGS=""
      TARGET_LIBRT_LIBS="-lrt"
      TARGET_LIBRT_DIR=""
      AC_MSG_RESULT(no)
    fi


    AC_SUBST(TARGET_LIBRT_CPPFLAGS)
    AC_SUBST(TARGET_LIBRT_LDFLAGS)
    AC_SUBST(TARGET_LIBRT_LIBS)
    AC_SUBST(TARGET_LIBRT_DIR)

])
