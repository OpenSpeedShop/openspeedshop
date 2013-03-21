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

AC_DEFUN([AX_VT], [

    AC_ARG_WITH(vt,
                AC_HELP_STRING([--with-vt=DIR],
                               [VampirTrace (vt) library installation @<:@/usr@:>@]),
                vt_dir=$withval, vt_dir="/usr")

    VT_DIR="$vt_dir"
    VT_CPPFLAGS="-I$vt_dir/include"
    VT_LIBS="-lvt.mpi"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    AC_MSG_CHECKING([for VampirTrace support])

    foundVT=0
    if test -f  $vt_dir/$abi_libdir/libvt.mpi.a; then
       AC_MSG_CHECKING([found VampirTrace library])
       VT_LDFLAGS="-L$vt_dir/$abi_libdir"
       VT_LIBSDIR="$vt_dir/$abi_libdir"
       foundVT=1
    else
      if test -f  $vt_dir/$alt_abi_libdir/libvt.mpi.a; then
         AC_MSG_CHECKING([found VampirTrace library])
         VT_LDFLAGS="-L$vt_dir/$alt_abi_libdir"
         VT_LIBSDIR="$vt_dir/$alt_abi_libdir"
         foundVT=1
      else
         foundVT=0
      fi
    fi

    if test $foundVT == 1 && test -f  $vt_dir/include/vt_user.h; then
       AC_MSG_CHECKING([found VampirTrace headers])
       foundVT=1
    else
       foundVT=0
    fi

    if test $foundVT == 1; then
      AC_MSG_CHECKING([found all VampirTrace headers, libraries.])
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_VT, true)
      AC_DEFINE(HAVE_VT, 1, [Define to 1 if you have VT.])
    else
      AC_MSG_RESULT(no)
      AM_CONDITIONAL(HAVE_VT, false)
      VT_CPPFLAGS=""
      VT_DIR=""
      VT_LDFLAGS=""
      VT_LIBS=""
      VT_LIBSDIR=""
    fi


    AC_LANG_POP(C++)

    AC_SUBST(VT_DIR)
    AC_SUBST(VT_CPPFLAGS)
    AC_SUBST(VT_LDFLAGS)
    AC_SUBST(VT_LIBS)
    AC_SUBST(VT_LIBSDIR)

])
