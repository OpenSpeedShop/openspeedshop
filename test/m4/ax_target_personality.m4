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
# Check for the location of the BG personality includes for Target Architecture 
################################################################################

AC_DEFUN([AX_TARGET_PERSONALITY], [

    AC_ARG_WITH(target-personality,
                AC_HELP_STRING([--with-target-personality=DIR],
                               [personality target architecture installation @<:@/opt@:>@]),
                target_personality_dir=$withval, target_personality_dir="/zzz")

    AC_MSG_CHECKING([for Targetted personality support])

    if test "$target_personality_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_PERSONALITY, false)
      TARGET_PERSONALITY_DIR=""
      TARGET_PERSONALITY_CPPFLAGS=""
      AC_MSG_RESULT(no)
    else
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_TARGET_PERSONALITY, true)
      AC_DEFINE(HAVE_TARGET_PERSONALITY, 1, [Define to 1 if you have a target version of PERSONALITY.])
      case "$target_os" in
	cray-xk)
            TARGET_PERSONALITY_DIR="$target_personality_dir"
	    TARGET_PERSONALITY_CPPFLAGS="-I$target_personality_dir/include"
	    TARGET_PERSONALITY_OS="$target_os"
            ;;
	cray-xe)
            TARGET_PERSONALITY_DIR="$target_personality_dir"
	    TARGET_PERSONALITY_CPPFLAGS="-I$target_personality_dir/include"
	    TARGET_PERSONALITY_OS="$target_os"
            ;;
	cray-xt5)
            TARGET_PERSONALITY_DIR="$target_personality_dir"
	    TARGET_PERSONALITY_CPPFLAGS="-I$target_personality_dir/include"
	    TARGET_PERSONALITY_OS="$target_os"
            ;;
	bgq)
            TARGET_PERSONALITY_DIR="$target_personality_dir"
	    TARGET_PERSONALITY_CPPFLAGS="-I$target_personality_dir -I$target_personality_dir/spi/include -I$target_personality_dir/spi/include/kernel/cnk -I$target_personality_dir/spi/include/kernel"
	    TARGET_PERSONALITY_OS="$target_os"
            ;;
	*)
            TARGET_PERSONALITY_DIR="$target_personality_dir"
	    TARGET_PERSONALITY_CPPFLAGS="-I$target_personality_dir -I$target_personality_dir/include"
	    TARGET_PERSONALITY_OS="$target_os"
            ;;
      esac
    fi


    AC_SUBST(TARGET_PERSONALITY_CPPFLAGS)
    AC_SUBST(TARGET_PERSONALITY_DIR)
    AC_SUBST(TARGET_PERSONALITY_OS)

])
