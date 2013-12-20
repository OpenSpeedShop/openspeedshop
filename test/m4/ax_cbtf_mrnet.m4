################################################################################
# Copyright (c) 2012 Krell Institute. All Rights Reserved.
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

AC_DEFUN([AX_CBTF_MRNET], [

    AC_ARG_WITH(cbtf-mrnet,
                AC_HELP_STRING([--with-cbtf-mrnet=DIR],
                               [CBTF MRNet library installation @<:@/usr@:>@]),
                cbtf_mrnet_dir=$withval, cbtf_mrnet_dir="/usr")

    CBTF_MRNET_CPPFLAGS="-I$cbtf_mrnet_dir/include"
    CBTF_MRNET_LDFLAGS="-L$cbtf_mrnet_dir/$abi_libdir"
    CBTF_MRNET_LIBS="$BOOST_SYSTEM_LIB $MRNET_LIBS -lcbtf -lcbtf-mrnet -lcbtf-xml"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    cbtf_mrnet_saved_CPPFLAGS=$CPPFLAGS
    cbtf_mrnet_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $CBTF_MRNET_CPPFLAGS $BOOST_CPPFLAGS $MRNET_CPPFLAGS"
    LDFLAGS="$LDFLAGS $CBTF_MRNET_LDFLAGS $LIBXERCES_C_LDFLAGS $BOOST_LDFLAGS $MRNET_LDFLAGS"
    LIBS="$CBTF_MRNET_LIBS $LIBXERCES_C $BOOST_THREAD_LIB $BOOST_FILESYSTEM_LIB"

    AC_MSG_CHECKING([for CBTF MRNet library and headers])

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
        #include <KrellInstitute/CBTF/XDR.hpp>
        ]], [[
        ]])], [ 
            AC_MSG_RESULT(yes)
        ], [
            AC_MSG_RESULT(no)
            AC_MSG_ERROR([CBTF MRNet library could not be found.])
        ])

    CPPFLAGS=$cbtf_mrnet_saved_CPPFLAGS
    LDFLAGS=$cbtf_mrnet_saved_LDFLAGS
    LIBS=$cbtf_mrnet_saved_LIBS

    AC_LANG_POP(C++)

    AC_SUBST(CBTF_MRNET_CPPFLAGS)
    AC_SUBST(CBTF_MRNET_LDFLAGS)
    AC_SUBST(CBTF_MRNET_LIBS)

])
