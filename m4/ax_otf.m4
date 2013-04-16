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

AC_DEFUN([AX_OTF], [

    AC_ARG_WITH(otf,
                AC_HELP_STRING([--with-otf=DIR],
                               [OTF Open Trace Format library installation @<:@/usr@:>@]),
                otf_dir=$withval, otf_dir="/usr")

    AC_ARG_WITH(libz,
                AC_HELP_STRING([--with-libz=DIR],
                               [libz installation @<:@/usr@:>@]),
                libz_dir=$withval, libz_dir="/usr")

    OTF_DIR="$otf_dir"
    OTF_LIBSDIR="$otf_dir/$abi_libdir"
    OTF_CPPFLAGS="-I$otf_dir/include"
    OTF_LDFLAGS="-L$otf_dir/$abi_libdir"
    OTF_LIBS="-lotf"

    OTF_LIBZ_LDFLAGS="-L/usr/$abi_libdir"
    OTF_LIBZ_LIBS="-lz"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    otf_saved_CPPFLAGS=$CPPFLAGS
    otf_saved_LDFLAGS=$LDFLAGS
    otf_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $OTF_CPPFLAGS"
    LDFLAGS="$LDFLAGS $OTF_LDFLAGS"
    LIBS="$OTF_LIBS"

    AC_MSG_CHECKING([for OTF support])

    foundOTF=0
    if test -f  $otf_dir/$abi_libdir/libotf.a; then
       AC_MSG_CHECKING([found otf library])
       foundOTF=1
    else 
	if test -f $otf_dir/$alt_abi_libdir/libotf.a; then
          AC_MSG_CHECKING([found otf library])
          OTF_LIBSDIR="$otf_dir/$alt_abi_libdir"
          OTF_LDFLAGS="-L$otf_dir/$alt_abi_libdir"
          foundOTF=1
       else
          foundOTF=0
       fi
    fi

    if test $foundOTF == 1 && test -f  $otf_dir/include/otf.h; then
       AC_MSG_CHECKING([found otf headers])
       foundOTF=1
    else
       foundOTF=0
    fi

    if test $foundOTF == 1 && test -f  $libz_dir/$abi_libdir/libz.so; then
       AC_MSG_CHECKING([found libz library used by otf])
       foundOTF=1
    else
       foundOTF=0
    fi

    if test $foundOTF == 1; then
      AC_MSG_CHECKING([found all otf headers, libraries and supporting libz library])
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_OTF, true)
      AC_DEFINE(HAVE_OTF, 1, [Define to 1 if you have OTF.])
    else
      AC_MSG_RESULT(no)
      AM_CONDITIONAL(HAVE_OTF, false)
      OTF_DIR=""
      OTF_LIBSDIR=""
      OTF_CPPFLAGS=""
      OTF_LDFLAGS=""
      OTF_LIBS=""
      OTF_LIBZ_LDFLAGS=""
      OTF_LIBZ_LIBS=""
    fi


    CPPFLAGS=$otf_saved_CPPFLAGS
    LDFLAGS=$otf_saved_LDFLAGS
    LIBS=$otf_saved_LIBS

    AC_LANG_POP(C++)

    AC_SUBST(OTF_DIR)
    AC_SUBST(OTF_LIBSDIR)
    AC_SUBST(OTF_CPPFLAGS)
    AC_SUBST(OTF_LDFLAGS)
    AC_SUBST(OTF_LIBS)
    AC_SUBST(OTF_LIBZ_LDFLAGS)
    AC_SUBST(OTF_LIBZ_LIBS)

])
