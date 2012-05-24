################################################################################
# Copyright (c) 2011-2011 Krell Institute. All Rights Reserved.
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

AC_DEFUN([AX_XERCESC_VERSION], [

    AC_MSG_CHECKING([for libxerces-c >= 3.0.0])
    
    AC_LANG_PUSH(C++)

    xerces_saved_CPPFLAGS=$CPPFLAGS
    CPPFLAGS="$CPPFLAGS $LIBXERCES_C_CPPFLAGS"

    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
        #include "xercesc/util/XercesVersion.hpp"
        ]], [[
        #if _XERCES_VERSION < 3000
            #error "libxerces-c version is too old!"
        #endif
        ]])], AC_MSG_RESULT(yes), [ AC_MSG_RESULT(no)
        AC_MSG_FAILURE(libxerces-c version isn't 3.0.0 or higher.)
        ]
    )

    CPPFLAGS=$xerces_saved_CPPFLAGS

    AC_LANG_POP(C++)

])



AC_DEFUN([AX_TARGET_XERCESC_VERSION], [

    AC_MSG_CHECKING([for libxerces-c >= 3.0.0])
    
    AC_LANG_PUSH(C++)

    xerces_saved_CPPFLAGS=$CPPFLAGS
    CPPFLAGS="$CPPFLAGS $TARGET_LIBXERCES_C_CPPFLAGS"

    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
        #include "xercesc/util/XercesVersion.hpp"
        ]], [[
        #if _XERCES_VERSION < 3000
            #error "libxerces-c version is too old!"
        #endif
        ]])], AC_MSG_RESULT(yes), [ AC_MSG_RESULT(no)
        AC_MSG_FAILURE(libxerces-c version isn't 3.0.0 or higher.)
        ]
    )

    CPPFLAGS=$xerces_saved_CPPFLAGS

    AC_LANG_POP(C++)

])
