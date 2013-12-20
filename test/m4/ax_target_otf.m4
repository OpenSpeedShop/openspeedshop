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
# Check for OTF (http://www.paratools.com/otf)
################################################################################

AC_DEFUN([AX_TARGET_OTF], [

    AC_ARG_WITH(target-otf,
                AC_HELP_STRING([--with-target-otf=DIR],
                               [Targetted OTF Open Trace Format library installation @<:@/zzz@:>@]),
                target_otf_dir=$withval, target_otf_dir="/zzz")

    AC_ARG_WITH(target-libz,
                AC_HELP_STRING([--with-target-libz=DIR],
                               [Targetted libz installation @<:@/zzz@:>@]),
                target_libz_dir=$withval, target_libz_dir="/zzz")

    TARGET_OTF_DIR="$target_otf_dir"
    TARGET_OTF_LIBSDIR="$target_otf_dir/$abi_libdir"
    TARGET_OTF_CPPFLAGS="-I$target_otf_dir/include"
    TARGET_OTF_LDFLAGS="-L$target_otf_dir/$abi_libdir"
    TARGET_OTF_LIBS="-lotf"

    TARGET_OTF_LIBZ_LDFLAGS="-L/usr/$abi_libdir"
    TARGET_OTF_LIBZ_LIBS="-lz"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    AC_MSG_CHECKING([for Targetted OTF support])

    found_target_otf=0
    if test -f  $target_otf_dir/$abi_libdir/libotf.a; then
       AC_MSG_CHECKING([found Targetted OTF library])
       found_target_otf=1
    else 
	if test -f $target_otf_dir/$alt_abi_libdir/libotf.a; then
          AC_MSG_CHECKING([found Targetted OTF library])
          TARGET_OTF_LIBSDIR="$target_otf_dir/$alt_abi_libdir"
          TARGET_OTF_LDFLAGS="-L$target_otf_dir/$alt_abi_libdir"
          found_target_otf=1
       else
       AC_MSG_CHECKING([FAILED to find Targetted OTF library])
          found_target_otf=0
       fi
    fi

    if test $found_target_otf == 1 && test -f $target_otf_dir/include/otf.h; then
       AC_MSG_CHECKING([found Targetted OTF headers])
       found_target_otf=1
    else
       AC_MSG_CHECKING([FAILED to find Targetted OTF headers])
       found_target_otf=0
    fi

    if test $found_target_otf == 1 && test -f $target_libz_dir/$abi_libdir/libz.so; then
       AC_MSG_CHECKING([found Targetted LIBZ library used by OTF])
       found_target_otf=1
    else
       AC_MSG_CHECKING([FAILED to find Targetted LIBZ library used by OTF])
       found_target_otf=0
    fi

    if test $found_target_otf == 1; then
      AC_MSG_CHECKING([found all Targetted OTF headers, libraries and supporting libz library])
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_TARGET_OTF, true)
      AC_DEFINE(HAVE_TARGET_OTF, 1, [Define to 1 if you have OTF.])
    else
      AC_MSG_CHECKING([FAILED to find all Targetted OTF headers, libraries and supporting libz library])
      AC_MSG_RESULT(no)
      AM_CONDITIONAL(HAVE_TARGET_OTF, false)
      TARGET_OTF_DIR=""
      TARGET_OTF_LIBSDIR=""
      TARGET_OTF_CPPFLAGS=""
      TARGET_OTF_LDFLAGS=""
      TARGET_OTF_LIBS=""
      TARGET_OTF_LIBZ_LDFLAGS=""
      TARGET_OTF_LIBZ_LIBS=""
    fi

    AC_LANG_POP(C++)

    AC_SUBST(TARGET_OTF_DIR)
    AC_SUBST(TARGET_OTF_LIBSDIR)
    AC_SUBST(TARGET_OTF_CPPFLAGS)
    AC_SUBST(TARGET_OTF_LDFLAGS)
    AC_SUBST(TARGET_OTF_LIBS)
    AC_SUBST(TARGET_OTF_LIBZ_LDFLAGS)
    AC_SUBST(TARGET_OTF_LIBZ_LIBS)

])
