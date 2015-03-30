################################################################################
# Copyright (c) 2014 Argo Navis Technologies. All Rights Reserved.
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

AC_DEFUN([AX_CBTF_MESSAGES_CUDA], [

   AC_ARG_WITH(cbtf-messages-cuda,
               AC_HELP_STRING([--with-cbtf-messages-cuda=DIR],
                   [CBTF CUDA messages library installation @<:@/usr@:>@]),
               messages_cuda_dir=$withval, messages_cuda_dir="/usr")

    if test "x${messages_cuda_dir}" == "x" || test "x${messages_cuda_dir}" == "x/usr" ; then
      MESSAGES_CUDA_CPPFLAGS=""
      MESSAGES_CUDA_LDFLAGS=""
    else
      MESSAGES_CUDA_CPPFLAGS="-I$messages_cuda_dir/include"
      MESSAGES_CUDA_LDFLAGS="-L$messages_cuda_dir/$abi_libdir"
    fi

    MESSAGES_CUDA_LIBS="-lcbtf-messages-cuda"

    messages_cuda_saved_CPPFLAGS=$CPPFLAGS
    messages_cuda_saved_LDFLAGS=$LDFLAGS
    messages_cuda_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $MESSAGES_CUDA_CPPFLAGS"
    LDFLAGS="$LDFLAGS $MESSAGES_CUDA_LDFLAGS $MESSAGES_LDFLAGS"
    LIBS="$MESSAGES_CUDA_LIBS $MESSAGES_BASE_LIBS"

    AC_MSG_CHECKING([for CBTF CUDA messages library and headers])

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
        #include <KrellInstitute/Messages/CUDA_data.h>
        ]], [[
        ]])], [
            AC_MSG_RESULT(yes)
            AM_CONDITIONAL(HAVE_CBTF_MESSAGES_CUDA, true)
            AC_DEFINE(HAVE_CBTF_MESSAGES_CUDA, 1,
                      [Define to 1 if you have CBTF CUDA Messages])
        ], [
            AC_MSG_RESULT(no)
            AM_CONDITIONAL(HAVE_CBTF_MESSAGES_CUDA, false)
            AC_DEFINE(HAVE_CBTF_MESSAGES_CUDA, 0,
                      [Define to 1 if you have CBTF CUDA Messages])
        ])

    CPPFLAGS=$messages_cuda_saved_CPPFLAGS
    LDFLAGS=$messages_cuda_saved_LDFLAGS
    LIBS=$messages_cuda_saved_LIBS

    AC_SUBST(MESSAGES_CUDA_CPPFLAGS)
    AC_SUBST(MESSAGES_CUDA_LDFLAGS)
    AC_SUBST(MESSAGES_CUDA_LIBS)

])
