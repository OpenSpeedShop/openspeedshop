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
# Check for STDC_PLUS_PLUS for Target Architecture
#############################################################################################

AC_DEFUN([AX_TARGET_STDC_PLUSPLUS], [

    AC_ARG_WITH(target-stdc-plusplus,
                AC_HELP_STRING([--with-target-stdc-plusplus=DIR],
                               [stdc-plusplus target architecture installation @<:@/usr@:>@]),
                target_stdc_plusplus_dir=$withval, target_stdc_plusplus_dir="/usr")

    AC_MSG_CHECKING([for Targetted stdc++ support])

    if test "$target_stdc_plusplus_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_STDC_PLUSPLUS, false)
      TARGET_STDC_PLUSPLUS_CPPFLAGS=""
      TARGET_STDC_PLUSPLUS_LDFLAGS=""
      TARGET_STDC_PLUSPLUS_LIBS="-lstdc++"
      TARGET_STDC_PLUSPLUS_DIR=""
      AC_MSG_RESULT(no)
    else
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_TARGET_STDC_PLUSPLUS, true)
      AC_DEFINE(HAVE_TARGET_STDC_PLUSPLUS, 1, [Define to 1 if you have a target version of STDC_PLUSPLUS.])
      TARGET_STDC_PLUSPLUS_CPPFLAGS="-I$target_stdc_plusplus_dir/include -DUNW_LOCAL_ONLY"
      if test -f  $target_stdc_plusplus_dir/$abi_libdir/libstdc++.so; then
        TARGET_STDC_PLUSPLUS_LDFLAGS="-L$target_stdc_plusplus_dir/$abi_libdir"
      elif test -f  $target_stdc_plusplus_dir/$alt_abi_libdir/libstdc++.so; then
        TARGET_STDC_PLUSPLUS_LDFLAGS="-L$target_stdc_plusplus_dir/$alt_abi_libdir"
      fi
      TARGET_STDC_PLUSPLUS_LIBS="-lstdc++"
      TARGET_STDC_PLUSPLUS_DIR="$target_stdc_plusplus_dir"
    fi


    AC_SUBST(TARGET_STDC_PLUSPLUS_CPPFLAGS)
    AC_SUBST(TARGET_STDC_PLUSPLUS_LDFLAGS)
    AC_SUBST(TARGET_STDC_PLUSPLUS_LIBS)
    AC_SUBST(TARGET_STDC_PLUSPLUS_DIR)

])
