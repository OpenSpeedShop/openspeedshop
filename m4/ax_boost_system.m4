# ===========================================================================
#         http://www.nongnu.org/autoconf-archive/ax_boost_system.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_BOOST_SYSTEM
#
# DESCRIPTION
#
#   Test for System library from the Boost C++ libraries. The macro requires
#   a preceding call to AX_BOOST_BASE. Further documentation is available at
#   <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_SYSTEM_LIB)
#
#   And sets:
#
#     HAVE_BOOST_SYSTEM
#
# LICENSE
#
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2008 Michael Tindal
#   Copyright (c) 2008 Daniel Casimiro <dan.casimiro@gmail.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 6

AC_DEFUN([AX_BOOST_SYSTEM],
[
	AC_ARG_WITH([boost-system],
	AS_HELP_STRING([--with-boost-system@<:@=special-lib@:>@],
                   [use the System library from boost - it is possible to specify a certain library for the linker
                        e.g. --with-boost-system=boost_system-gcc-mt ]),
        [
        if test "$withval" = "no"; then
			want_boost="no"
        elif test "$withval" = "yes"; then
            want_boost="yes"
            ax_boost_user_system_lib=""
        else
		    want_boost="yes"
        	ax_boost_user_system_lib="$withval"
		fi
        ],
        [want_boost="yes"]
	)

	if test "x$want_boost" = "xyes"; then
        AC_REQUIRE([AC_PROG_CC])
        AC_REQUIRE([AC_CANONICAL_BUILD])
		CPPFLAGS_SAVED="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
		export CPPFLAGS

		LDFLAGS_SAVED="$LDFLAGS"
		LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
		export LDFLAGS

        AC_CACHE_CHECK(whether the Boost::System library is available,
					   ax_cv_boost_system,
        [AC_LANG_PUSH([C++])
			 CXXFLAGS_SAVE=$CXXFLAGS

			 AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[@%:@include <boost/system/error_code.hpp>]],
                                   [[boost::system::system_category]])],
                   ax_cv_boost_system=yes, ax_cv_boost_system=no)
			 CXXFLAGS=$CXXFLAGS_SAVE
             AC_LANG_POP([C++])
		])

            boost_in_usr=no
            if test "x$BOOST_LDFLAGS" = "x"; then
                boost_in_usr=yes
                BOOST_LDFLAGS="-L/usr/lib*"
            fi

		if test "x$ax_cv_boost_system" = "xyes"; then
			AC_SUBST(BOOST_CPPFLAGS)

			AC_DEFINE(HAVE_BOOST_SYSTEM,,[define if the Boost::System library is available])
            BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`

			LDFLAGS_SAVE=$LDFLAGS
            if test "x$ax_boost_user_system_lib" = "x"; then
                for libextension in `ls $BOOSTLIBDIR/libboost_system*.{so,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_system.*\)\.so.*$;\1;' -e 's;^lib\(boost_system.*\)\.a*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, exit,
                                 [BOOST_SYSTEM_LIB="-l$ax_lib"; AC_SUBST(BOOST_SYSTEM_LIB) link_system="yes"; break],
                                 [link_system="no"])
  				done
                if test "x$link_system" != "xyes"; then
                for libextension in `ls $BOOSTLIBDIR/boost_system*.{dll,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_system.*\)\.dll.*$;\1;' -e 's;^\(boost_system.*\)\.a*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, exit,
                                 [BOOST_SYSTEM_LIB="-l$ax_lib"; AC_SUBST(BOOST_SYSTEM_LIB) link_system="yes"; break],
                                 [link_system="no"])
  				done
                fi

            else
               for ax_lib in $ax_boost_user_system_lib boost_system-$ax_boost_user_system_lib; do
				      AC_CHECK_LIB($ax_lib, exit,
                                   [BOOST_SYSTEM_LIB="-l$ax_lib"; AC_SUBST(BOOST_SYSTEM_LIB) link_system="yes"; break],
                                   [link_system="no"])
                  done

            fi
			if test "x$link_system" = "xno"; then
				AC_MSG_ERROR(Could not link against $ax_lib !)
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
# Check for Boost System for Target Architecture 
#############################################################################################

AC_DEFUN([AC_PKG_TARGET_BOOST_SYSTEM], [

    AC_ARG_WITH(target-boost-system,
                AC_HELP_STRING([--with-target-boost-system=DIR],
                               [Boost system target architecture installation @<:@/opt@:>@]),
                target_boost_system_dir=$withval, target_boost_system_dir="/zzz")

    AC_MSG_CHECKING([for Targetted Boost System support])

    found_target_boost_system=0
    if test -f $target_boost_system_dir/$abi_libdir/libboost_system.so -o -f $target_boost_system_dir/$abi_libdir/libboost_system.a -o -f $target_boost_system_dir/$abi_libdir/libboost_system.so -o -f $target_boost_system_dir/$abi_libdir/libboost_system.a; then
       found_target_boost_system=1
       TARGET_BOOST_SYSTEM_LDFLAGS="-L$target_boost_system_dir/$abi_libdir"
       TARGET_BOOST_SYSTEM_LIB="$target_boost_system_dir/$abi_libdir"
    elif test -f $target_boost_system_dir/$alt_abi_libdir/libboost_system.so -o -f $target_boost_system_dir/$alt_abi_libdir/libboost_system.a -o -f $target_boost_system_dir/$alt_abi_libdir/libboost_system.so -o -f $target_boost_system_dir/$alt_abi_libdir/libboost_system.a; then
       found_target_boost_system=1
       TARGET_BOOST_SYSTEM_LDFLAGS="-L$target_boost_system_dir/$alt_abi_libdir"
       TARGET_BOOST_SYSTEM_LIB="$target_boost_system_dir/$abi_libdir"
    fi

    if test $found_target_boost_system == 0 && test "$target_boost_system_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_BOOST_SYSTEM, false)
      TARGET_BOOST_SYSTEM_CPPFLAGS=""
      TARGET_BOOST_SYSTEM_LDFLAGS=""
      TARGET_BOOST_SYSTEM_LIBS=""
      TARGET_BOOST_SYSTEM_LIB=""
      TARGET_BOOST_SYSTEM_DIR=""
      AC_MSG_RESULT(no)
    elif test $found_target_boost_system == 1 ; then
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_TARGET_BOOST_SYSTEM, true)
      AC_DEFINE(HAVE_TARGET_BOOST_SYSTEM, 1, [Define to 1 if you have a target version of BOOST_SYSTEM.])
      TARGET_BOOST_SYSTEM_CPPFLAGS="-I$target_boost_system_dir/include/boost"
      TARGET_BOOST_SYSTEM_LIBS="-lboost_system"
      TARGET_BOOST_SYSTEM_LIB="-lboost_system"
      TARGET_BOOST_DIR="$target_boost_system_dir"
    else 
      AM_CONDITIONAL(HAVE_TARGET_BOOST_SYSTEM, false)
      TARGET_BOOST_SYSTEM_CPPFLAGS=""
      TARGET_BOOST_SYSTEM_LDFLAGS=""
      TARGET_BOOST_SYSTEM_LIBS=""
      TARGET_BOOST_SYSTEM_LIB=""
      TARGET_BOOST_SYSTEM_DIR=""
      AC_MSG_RESULT(no)
    fi

    AC_SUBST(TARGET_BOOST_SYSTEM_LIB)
    AC_SUBST(TARGET_BOOST_SYSTEM_CPPFLAGS)
    AC_SUBST(TARGET_BOOST_SYSTEM_LDFLAGS)
    AC_SUBST(TARGET_BOOST_SYSTEM_LIBS)
    AC_SUBST(TARGET_BOOST_SYSTEM_DIR)

])

