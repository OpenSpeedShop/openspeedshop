# ============================================================================
#  http://www.gnu.org/software/autoconf-archive/ax_boost_program_options.html
# ============================================================================
#
# SYNOPSIS
#
#   AX_BOOST_PROGRAM_OPTIONS
#
# DESCRIPTION
#
#   Test for program options library from the Boost C++ libraries. The macro
#   requires a preceding call to AX_BOOST_BASE. Further documentation is
#   available at <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_PROGRAM_OPTIONS_LIB)
#
#   And sets:
#
#     HAVE_BOOST_PROGRAM_OPTIONS
#
# LICENSE
#
#   Copyright (c) 2009 Thomas Porschberg <thomas@randspringer.de>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 20

AC_DEFUN([AX_BOOST_PROGRAM_OPTIONS],
[
	AC_ARG_WITH([boost-program-options],
		AS_HELP_STRING([--with-boost-program-options@<:@=special-lib@:>@],
                       [use the program options library from boost - it is possible to specify a certain library for the linker
                        e.g. --with-boost-program-options=boost_program_options-gcc-mt-1_33_1 ]),
        [
        if test "$withval" = "no"; then
			want_boost="no"
        elif test "$withval" = "yes"; then
            want_boost="yes"
            ax_boost_user_program_options_lib=""
        else
		    want_boost="yes"
		ax_boost_user_program_options_lib="$withval"
		fi
        ],
        [want_boost="yes"]
	)

	if test "x$want_boost" = "xyes"; then
        AC_REQUIRE([AC_PROG_CC])
	    export want_boost
		CPPFLAGS_SAVED="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
		export CPPFLAGS
		LDFLAGS_SAVED="$LDFLAGS"
		LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
		export LDFLAGS
		AC_CACHE_CHECK([whether the Boost::Program_Options library is available],
					   ax_cv_boost_program_options,
					   [AC_LANG_PUSH(C++)
				AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[@%:@include <boost/program_options.hpp>
                                                          ]],
                                  [[boost::program_options::options_description generic("Generic options");
                                   return 0;]])],
                           ax_cv_boost_program_options=yes, ax_cv_boost_program_options=no)
					AC_LANG_POP([C++])
		])

            boost_in_usr=no
            if test "x$BOOST_LDFLAGS" = "x"; then
                boost_in_usr=yes
                BOOST_LDFLAGS="-L/usr/lib*"
            fi

		if test "$ax_cv_boost_program_options" = yes; then
				AC_DEFINE(HAVE_BOOST_PROGRAM_OPTIONS,,[define if the Boost::PROGRAM_OPTIONS library is available])
                  BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`
                if test "x$ax_boost_user_program_options_lib" = "x"; then
                for libextension in `ls $BOOSTLIBDIR/libboost_program_options*.so* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_program_options.*\)\.so.*$;\1;'` `ls $BOOSTLIBDIR/libboost_program_options*.a* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_program_options.*\)\.a*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, exit,
                                 [BOOST_PROGRAM_OPTIONS_LIB="-l$ax_lib"; AC_SUBST(BOOST_PROGRAM_OPTIONS_LIB) link_program_options="yes"; break],
                                 [link_program_options="no"])
				done
                if test "x$link_program_options" != "xyes"; then
                for libextension in `ls $BOOSTLIBDIR/boost_program_options*.dll* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_program_options.*\)\.dll.*$;\1;'` `ls $BOOSTLIBDIR/boost_program_options*.a* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_program_options.*\)\.a*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, exit,
                                 [BOOST_PROGRAM_OPTIONS_LIB="-l$ax_lib"; AC_SUBST(BOOST_PROGRAM_OPTIONS_LIB) link_program_options="yes"; break],
                                 [link_program_options="no"])
				done
                fi
                else
                  for ax_lib in $ax_boost_user_program_options_lib boost_program_options-$ax_boost_user_program_options_lib; do
				      AC_CHECK_LIB($ax_lib, main,
                                   [BOOST_PROGRAM_OPTIONS_LIB="-l$ax_lib"; AC_SUBST(BOOST_PROGRAM_OPTIONS_LIB) link_program_options="yes"; break],
                                   [link_program_options="no"])
                  done
                fi
            if test "x$ax_lib" = "x"; then
                AC_MSG_ERROR(Could not find a version of the library!)
            fi
				if test "x$link_program_options" != "xyes"; then
					AC_MSG_ERROR([Could not link against [$ax_lib] !])
				fi
		fi
		CPPFLAGS="$CPPFLAGS_SAVED"
	LDFLAGS="$LDFLAGS_SAVED"
	fi

                       if test "x$boost_in_usr" = "xyes"; then
                            BOOST_LDFLAGS=""
                        fi

])

#############################################################################################
# Check for Boost Program Options for Target Architecture 
#############################################################################################

AC_DEFUN([AC_PKG_TARGET_BOOST_PROGRAM_OPTIONS], [

    AC_ARG_WITH(target-boost-program-options,
                AC_HELP_STRING([--with-target-boost-program-options=DIR],
                               [Boost program-options target architecture installation @<:@/opt@:>@]),
                target_boost_program_options_dir=$withval, target_boost_program_options_dir="/zzz")

    AC_MSG_CHECKING([for Targetted Boost Program Options support])

    found_target_boost_program_options=0
    if test -f $target_boost_program_options_dir/$abi_libdir/libboost_program_options.so -o -f $target_boost_program_options_dir/$abi_libdir/libboost_program_options.a -o -f $target_boost_program_options_dir/$abi_libdir/libboost_program_options.so -o -f $target_boost_program_options_dir/$abi_libdir/libboost_program_options.a; then
       found_target_boost_program_options=1
       TARGET_BOOST_PROGRAM_OPTIONS_LDFLAGS="-L$target_boost_program_options_dir/$abi_libdir"
       TARGET_BOOST_PROGRAM_OPTIONS_LIB="$target_boost_program_options_dir/$abi_libdir"
    elif test -f $target_boost_program_options_dir/$alt_abi_libdir/libboost_program_options.so -o -f $target_boost_program_options_dir/$alt_abi_libdir/libboost_program_options.a -o -f $target_boost_program_options_dir/$alt_abi_libdir/libboost_program_options.so -o -f $target_boost_program_options_dir/$alt_abi_libdir/libboost_program_options.a; then
       found_target_boost_program_options=1
       TARGET_BOOST_PROGRAM_OPTIONS_LDFLAGS="-L$target_boost_program_options_dir/$alt_abi_libdir"
       TARGET_BOOST_PROGRAM_OPTIONS_LIB="$target_boost_program_options_dir/$abi_libdir"
    fi

    if test $found_target_boost_program_options == 0 && test "$target_boost_program_options_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_BOOST_PROGRAM_OPTIONS, false)
      TARGET_BOOST_PROGRAM_OPTIONS_CPPFLAGS=""
      TARGET_BOOST_PROGRAM_OPTIONS_LDFLAGS=""
      TARGET_BOOST_PROGRAM_OPTIONS_LIBS=""
      TARGET_BOOST_PROGRAM_OPTIONS_LIB=""
      TARGET_BOOST_PROGRAM_OPTIONS_DIR=""
      AC_MSG_RESULT(no)
    elif test $found_target_boost_program_options == 1 ; then
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_TARGET_BOOST_PROGRAM_OPTIONS, true)
      AC_DEFINE(HAVE_TARGET_BOOST_PROGRAM_OPTIONS, 1, [Define to 1 if you have a target version of BOOST_PROGRAM_OPTIONS.])
      TARGET_BOOST_PROGRAM_OPTIONS_CPPFLAGS="-I$target_boost_program_options_dir/include/boost"
      TARGET_BOOST_PROGRAM_OPTIONS_LIBS="-lboost_program_options"
      TARGET_BOOST_PROGRAM_OPTIONS_LIB="-lboost_program_options"
      TARGET_BOOST_DIR="$target_boost_program_options_dir"
    else 
      AM_CONDITIONAL(HAVE_TARGET_BOOST_PROGRAM_OPTIONS, false)
      TARGET_BOOST_PROGRAM_OPTIONS_CPPFLAGS=""
      TARGET_BOOST_PROGRAM_OPTIONS_LDFLAGS=""
      TARGET_BOOST_PROGRAM_OPTIONS_LIBS=""
      TARGET_BOOST_PROGRAM_OPTIONS_LIB=""
      TARGET_BOOST_PROGRAM_OPTIONS_DIR=""
      AC_MSG_RESULT(no)
    fi

    AC_SUBST(TARGET_BOOST_PROGRAM_OPTIONS_LIB)
    AC_SUBST(TARGET_BOOST_PROGRAM_OPTIONS_CPPFLAGS)
    AC_SUBST(TARGET_BOOST_PROGRAM_OPTIONS_LDFLAGS)
    AC_SUBST(TARGET_BOOST_PROGRAM_OPTIONS_LIBS)
    AC_SUBST(TARGET_BOOST_PROGRAM_OPTIONS_DIR)

])

