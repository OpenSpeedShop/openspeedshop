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
# Check for VampirTrace  (http://tu-dresden.de/die_tu_dresden/zentrale_einrichtungen
#/zih/forschung/software_werkzeuge_zur_unterstuetzung_von_programmierung_und_optimierung
#/vampirtrace?set_language=en&cl=en)
################################################################################

AC_DEFUN([AX_TARGET_VT], [

    AC_ARG_WITH(target-vt,
                AC_HELP_STRING([--with-target-vt=DIR],
                               [Targetted VampirTrace (vt) library installation @<:@/zzz@:>@]),
                target_vt_dir=$withval, target_vt_dir="/zzz")

    TARGET_VT_DIR="$target_vt_dir"
    TARGET_VT_CPPFLAGS="-I$target_vt_dir/include"
    TARGET_VT_LIBS="-lvt.mpi"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    AC_MSG_CHECKING([for Targetted VampirTrace support])

    found_target_vt=0
    if test -f  $target_vt_dir/$abi_libdir/libvt.mpi.a; then
       AC_MSG_CHECKING([found VampirTrace library])
       TARGET_VT_LDFLAGS="-L$target_vt_dir/$abi_libdir"
       TARGET_VT_LIBSDIR="$target_vt_dir/$abi_libdir"
       found_target_vt=1
    else
      if test -f  $target_vt_dir/$alt_abi_libdir/libvt.mpi.a; then
         AC_MSG_CHECKING([found VampirTrace library])
         TARGET_VT_LDFLAGS="-L$target_vt_dir/$alt_abi_libdir"
         TARGET_VT_LIBSDIR="$target_vt_dir/$alt_abi_libdir"
         found_target_vt=1
      else
         found_target_vt=0
      fi
    fi

    if test $found_target_vt == 1 && test -f  $target_vt_dir/include/vt_user.h; then
       AC_MSG_CHECKING([found Targetted VampirTrace headers])
       found_target_vt=1
    else
       found_target_vt=0
    fi

    if test $found_target_vt == 1; then
      AC_MSG_CHECKING([found all Targetted VampirTrace headers, libraries.])
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_TARGET_VT, true)
      AC_DEFINE(HAVE_TARGET_VT, 1, [Define to 1 if you have TARGET_VT.])
    else
      AC_MSG_RESULT(no)
      AM_CONDITIONAL(HAVE_TARGET_VT, false)
      TARGET_VT_CPPFLAGS=""
      TARGET_VT_DIR=""
      TARGET_VT_LDFLAGS=""
      TARGET_VT_LIBS=""
      TARGET_VT_LIBSDIR=""
    fi


    AC_LANG_POP(C++)

    AC_SUBST(TARGET_VT_DIR)
    AC_SUBST(TARGET_VT_CPPFLAGS)
    AC_SUBST(TARGET_VT_LDFLAGS)
    AC_SUBST(TARGET_VT_LIBS)
    AC_SUBST(TARGET_VT_LIBSDIR)

])
