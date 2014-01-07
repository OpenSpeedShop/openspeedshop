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
# Check for libpthread for Target Architecture 
#############################################################################################

AC_DEFUN([AX_PKG_TARGET_LIBPTHREAD], [

    AC_ARG_WITH(target-libpthread,
                AC_HELP_STRING([--with-target-libpthread=DIR],
                               [libpthread target architecture installation @<:@/opt@:>@]),
                target_libpthread_dir=$withval, target_libpthread_dir="/zzz")

    AC_MSG_CHECKING([for Targetted libpthread support])

    found_target_libpthread=0
    if test -f $target_libpthread_dir/$abi_libdir/libpthread.a || test -f $target_libpthread_dir/$abi_libdir/libpthread.so; then
       found_target_libpthread=1
       TARGET_LIBPTHREAD_LDFLAGS="-L$target_libpthread_dir/$abi_libdir"
    elif test -f $target_libpthread_dir/$alt_abi_libdir/libpthread.a || test -f $target_libpthread_dir/$alt_abi_libdir/libpthread.so; then
       found_target_libpthread=1
       TARGET_LIBPTHREAD_LDFLAGS="-L$target_libpthread_dir/$alt_abi_libdir"
    fi

    if test $found_target_libpthread == 0 && test "$target_libpthread_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_LIBPTHREAD, false)
      TARGET_LIBPTHREAD_CPPFLAGS=""
      TARGET_LIBPTHREAD_LDFLAGS=""
      TARGET_LIBPTHREAD_LIBS="-lpthread"
      TARGET_LIBPTHREAD_DIR=""
      AC_MSG_RESULT(no)
    elif test $found_target_libpthread == 1 ; then
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_TARGET_LIBPTHREAD, true)
      AC_DEFINE(HAVE_TARGET_LIBPTHREAD, 1, [Define to 1 if you have a target version of LIBPTHREAD.])
      TARGET_LIBPTHREAD_CPPFLAGS="-I$target_libpthread_dir/include -DUNW_LOCAL_ONLY"
      TARGET_LIBPTHREAD_LIBS="-lpthread"
      TARGET_LIBPTHREAD_DIR="$target_libpthread_dir"
    else 
      AM_CONDITIONAL(HAVE_TARGET_LIBPTHREAD, false)
      TARGET_LIBPTHREAD_CPPFLAGS=""
      TARGET_LIBPTHREAD_LDFLAGS=""
      TARGET_LIBPTHREAD_LIBS="-lpthread"
      TARGET_LIBPTHREAD_DIR=""
      AC_MSG_RESULT(no)
    fi


    AC_SUBST(TARGET_LIBPTHREAD_CPPFLAGS)
    AC_SUBST(TARGET_LIBPTHREAD_LDFLAGS)
    AC_SUBST(TARGET_LIBPTHREAD_LIBS)
    AC_SUBST(TARGET_LIBPTHREAD_DIR)

])
