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
# Check for symtabAPI (http://www.dyninst.org)
################################################################################

AC_DEFUN([AX_SYMTABAPI], [

    AC_ARG_WITH(symtabapi,
                AC_HELP_STRING([--with-symtabapi=DIR],
                               [symtabAPI installation @<:@/usr@:>@]),
                symtabapi_dir=$withval, symtabapi_dir="/usr")

    AC_ARG_WITH(symtabapi-version,
                AC_HELP_STRING([--with-symtabapi-version=VERS],
                               [symtabapi-version installation @<:@8.1.2@:>@]),
                symtabapi_vers=$withval, symtabapi_vers="8.1.2")

    AC_ARG_WITH([symtabapi-libdir],
                AS_HELP_STRING([--with-symtabapi-libdir=LIB_DIR],
                [Force given directory for symtabapi libraries. Note that this will overwrite library path detection, so use this parameter only if default library detection fails and you know exactly where your symtabapi libraries are located.]),
                [
                if test -d $withval 
                then
                        ac_symtabapi_lib_path="$withval"
                else
                        AC_MSG_ERROR(--with-symtabapi-libdir expected directory name)
                fi ], 
                [ac_symtabapi_lib_path=""])



    if test "x${symtabapi_dir}" == "x" || test "x${symtabapi_dir}" == "x/usr" ; then
      SYMTABAPI_CPPFLAGS=""
      SYMTABAPI_DIR=""
      SYMTABAPI_LDFLAGS=""
    else
      SYMTABAPI_CPPFLAGS="-I$symtabapi_dir/include -std=c++0x"
      SYMTABAPI_DIR="$symtabapi_dir" 
      SYMTABAPI_CPPFLAGS="$SYMTABAPI_CPPFLAGS -DUSE_STL_VECTOR"
      if test "x$ac_symtabapi_lib_path" == "x"; then
         SYMTABAPI_LDFLAGS="-L$symtabapi_dir/$abi_libdir"
      else
         SYMTABAPI_LDFLAGS="-L$ac_symtabapi_lib_path"
      fi
      SYMTABAPI_LDFLAGS+=" $LIBDWARF_LDFLAGS $LIBELF_LDFLAGS"
    fi

    case "$symtabapi_vers" in
	"7.0.1")
            SYMTABAPI_LIBS="-lsymtabAPI -lcommon" 
            ;;
	"8.0.0")
            SYMTABAPI_LIBS="-lsymtabAPI -lcommon -ldynDwarf -ldynElf" 
            ;;
	"8.1")
            SYMTABAPI_LIBS="-lsymtabAPI -lcommon -ldynDwarf -ldynElf" 
            ;;
	"8.1.1")
            SYMTABAPI_LIBS="-lsymtabAPI -lcommon -ldynDwarf -ldynElf" 
            ;;
	"8.1.2")
            SYMTABAPI_LIBS="-lsymtabAPI -lcommon -ldynDwarf -ldynElf" 
            SYMTABAPI_LIBS+=" $LIBDWARF_LIBS $LIBELF_LIBS"
            ;;
	"8.2")
            SYMTABAPI_LIBS="-lsymtabAPI -lcommon -ldynDwarf -ldynElf" 
            SYMTABAPI_LIBS+=" $LIBDWARF_LIBS $LIBELF_LIBS"
            ;;
	*)
            SYMTABAPI_LIBS="-lsymtabAPI -lcommon -ldynDwarf -ldynElf" 
            SYMTABAPI_LIBS+=" $LIBDWARF_LIBS $LIBELF_LIBS"
            ;;
    esac


    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    symtabapi_saved_CPPFLAGS=$CPPFLAGS
    symtabapi_saved_LDFLAGS=$LDFLAGS
    symtabapi_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $SYMTABAPI_CPPFLAGS $BOOST_CPPFLAGS"
    LDFLAGS="$LDFLAGS $SYMTABAPI_LDFLAGS $BINUTILS_LDFLAGS"
    LIBS="$SYMTABAPI_LIBS $BINUTILS_IBERTY_LIB" 

    found_symtabapi=0

    AC_MSG_CHECKING([for symtabAPI API library and headers using $abi_libdir])

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
	#include "dyninst/Symbol.h"
	#include "dyninst/Symtab.h"
	using namespace Dyninst;
	using namespace SymtabAPI;
	
        ]], [[
        Symtab symtab = Symtab();
        ]])], 
        [ found_symtabapi=1
        ],
        [ found_symtabapi=0
        ]
    )

    if test $found_symtabapi -eq 1; then
        AC_MSG_RESULT(yes) 
        AM_CONDITIONAL(HAVE_SYMTABAPI, true)
        AC_DEFINE(HAVE_SYMTABAPI, 1, [Define to 1 if you have symtabAPI.])
    else
        AC_MSG_RESULT(no)
        AM_CONDITIONAL(HAVE_SYMTABAPI, false)
        SYMTABAPI_CPPFLAGS=""
        SYMTABAPI_LDFLAGS=""
        SYMTABAPI_LIBS=""
        SYMTABAPI_DIR=""
    fi



    CPPFLAGS=$symtabapi_saved_CPPFLAGS
    LDFLAGS=$symtabapi_saved_LDFLAGS
    LIBS=$symtabapi_saved_LIBS

    AC_LANG_POP(C++)

    AC_SUBST(SYMTABAPI_CPPFLAGS)
    AC_SUBST(SYMTABAPI_LDFLAGS)
    AC_SUBST(SYMTABAPI_LIBS)
    AC_SUBST(SYMTABAPI_DIR)


])


#############################################################################################
# Check for symtabAPI for Target Architecture 
#############################################################################################

AC_DEFUN([AX_TARGET_SYMTABAPI], [

    AC_ARG_WITH(target-symtabapi,
                AC_HELP_STRING([--with-target-symtabapi=DIR],
                               [symtabapi target architecture installation @<:@/opt@:>@]),
                target_symtabapi_dir=$withval, target_symtabapi_dir="/zzz")

    AC_MSG_CHECKING([for Targetted symtabapi support])

    found_target_symtabapi=0
    if test -f $target_symtabapi_dir/$abi_libdir/libsymtabAPI.a || test -f $target_symtabapi_dir/$abi_libdir/libsymtabAPI.so; then
       TARGET_SYMTABAPI_LDFLAGS="-L$target_symtabapi_dir/$abi_libdir"
       found_target_symtabapi=1
    elif test -f $target_symtabapi_dir/$alt_abi_libdir/libsymtabAPI.a || test -f $target_symtabapi_dir/$alt_abi_libdir/libsymtabAPI.so; then
       TARGET_SYMTABAPI_LDFLAGS="-L$target_symtabapi_dir/$alt_abi_libdir"
       found_target_symtabapi=1
    fi

    if test $found_target_symtabapi == 0 && test "$target_symtabapi_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_SYMTABAPI, false)
      TARGET_SYMTABAPI_CPPFLAGS=""
      TARGET_SYMTABAPI_LDFLAGS=""
      TARGET_SYMTABAPI_LIBS=""
      TARGET_SYMTABAPI_DIR=""
      AC_MSG_RESULT(no)
    elif test $found_target_symtabapi == 1 ; then
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_TARGET_SYMTABAPI, true)
      AC_DEFINE(HAVE_TARGET_SYMTABAPI, 1, [Define to 1 if you have a target version of SYMTABAPI.])
      TARGET_SYMTABAPI_CPPFLAGS="-I$target_symtabapi_dir/include -I$target_symtabapi_dir/include/dyninst -DUNW_LOCAL_ONLY"
      TARGET_SYMTABAPI_LIBS="-lsymtabAPI"
      TARGET_SYMTABAPI_DIR="$target_symtabapi_dir"
    else 
      AM_CONDITIONAL(HAVE_TARGET_SYMTABAPI, false)
      TARGET_SYMTABAPI_CPPFLAGS=""
      TARGET_SYMTABAPI_LDFLAGS=""
      TARGET_SYMTABAPI_LIBS=""
      TARGET_SYMTABAPI_DIR=""
      AC_MSG_RESULT(no)
    fi


    AC_SUBST(TARGET_SYMTABAPI_CPPFLAGS)
    AC_SUBST(TARGET_SYMTABAPI_LDFLAGS)
    AC_SUBST(TARGET_SYMTABAPI_LIBS)
    AC_SUBST(TARGET_SYMTABAPI_DIR)

])
