# ===========================================================================
#    http://www.gnu.org/software/autoconf-archive/ax_boost_date_time.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_BOOST_DATE_TIME
#
# DESCRIPTION
#
#   Test for Date_Time library from the Boost C++ libraries. The macro
#   requires a preceding call to AX_BOOST_BASE. Further documentation is
#   available at <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_DATE_TIME_LIB)
#
#   And sets:
#
#     HAVE_BOOST_DATE_TIME
#
# LICENSE
#
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2008 Michael Tindal
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 12

AC_DEFUN([AX_BOOST_DATE_TIME],
[
	AC_ARG_WITH([boost-date-time],
	AS_HELP_STRING([--with-boost-date-time@<:@=special-lib@:>@],
                   [use the Date_Time library from boost - it is possible to specify a certain library for the linker
                        e.g. --with-boost-date-time=boost_date_time-gcc-mt-d-1_33_1 ]),
        [
        if test "$withval" = "no"; then
			want_boost="no"
        elif test "$withval" = "yes"; then
            want_boost="yes"
            ax_boost_user_date_time_lib=""
        else
		    want_boost="yes"
        	ax_boost_user_date_time_lib="$withval"
		fi
        ],
        [want_boost="yes"]
	)

	if test "x$want_boost" = "xyes"; then
        AC_REQUIRE([AC_PROG_CC])
		CPPFLAGS_SAVED="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
		export CPPFLAGS

		LDFLAGS_SAVED="$LDFLAGS"
		LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
		export LDFLAGS

        AC_CACHE_CHECK(whether the Boost::Date_Time library is available,
					   ax_cv_boost_date_time,
        [AC_LANG_PUSH([C++])
		 AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[@%:@include <boost/date_time/gregorian/gregorian_types.hpp>]],
                                   [[using namespace boost::gregorian; date d(2002,Jan,10);
                                     return 0;
                                   ]]),
         ax_cv_boost_date_time=yes, ax_cv_boost_date_time=no)
         AC_LANG_POP([C++])
		])
		if test "x$ax_cv_boost_date_time" = "xyes"; then
			AC_DEFINE(HAVE_BOOST_DATE_TIME,,[define if the Boost::Date_Time library is available])

	    boost_in_usr=no
	    if test "x$BOOST_LDFLAGS" = "x"; then
	        boost_in_usr=yes
		BOOST_LDFLAGS="-L/usr/lib*"
	    fi

            BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`
            if test "x$ax_boost_user_date_time_lib" = "x"; then
                for libextension in `ls -r $BOOSTLIBDIR/libboost_date_time*.{so,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_date_time.*\)\.so.*$;\1;' -e 's;^lib\(boost_date_time.*\)\.a*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, exit,
                                 [BOOST_DATE_TIME_LIB="-l$ax_lib"; AC_SUBST(BOOST_DATE_TIME_LIB) link_date_time="yes"; break],
                                 [link_date_time="no"])
  				done
                if test "x$link_date_time" != "xyes"; then
                for libextension in `ls -r $BOOSTLIBDIR/boost_date_time*.{dll,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_date_time.*\)\.dll.*$;\1;' -e 's;^\(boost_date_time.*\)\.a*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, exit,
                                 [BOOST_DATE_TIME_LIB="-l$ax_lib"; AC_SUBST(BOOST_DATE_TIME_LIB) link_date_time="yes"; break],
                                 [link_date_time="no"])
  				done
                fi

            else
               for ax_lib in $ax_boost_user_date_time_lib boost_date_time-$ax_boost_user_date_time_lib; do
				      AC_CHECK_LIB($ax_lib, main,
                                   [BOOST_DATE_TIME_LIB="-l$ax_lib"; AC_SUBST(BOOST_DATE_TIME_LIB) link_date_time="yes"; break],
                                   [link_date_time="no"])
                  done

            fi

			if test "x$link_date_time" != "xyes"; then
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
# Check for Boost Date and Time for Target Architecture 
#############################################################################################

AC_DEFUN([AC_PKG_TARGET_BOOST_DATE_TIME], [

    AC_ARG_WITH(target-boost-date-time,
                AC_HELP_STRING([--with-target-boost-date-time=DIR],
                               [Boost date_time target architecture installation @<:@/opt@:>@]),
                target_boost_date_time_dir=$withval, target_boost_date_time_dir="/zzz")

    AC_MSG_CHECKING([for Targetted Boost Date and Time support])

    found_target_boost_date_time=0
    if test -f $target_boost_date_time_dir/$abi_libdir/libboost_date_time.so -o -f $target_boost_date_time_dir/$abi_libdir/libboost_date_time.a -o -f $target_boost_date_time_dir/$abi_libdir/libboost_date_time.so -o -f $target_boost_date_time_dir/$abi_libdir/libboost_date_time.a; then
       found_target_boost_date_time=1
       TARGET_BOOST_DATE_TIME_LDFLAGS="-L$target_boost_date_time_dir/$abi_libdir"
       TARGET_BOOST_DATE_TIME_LIB="$target_boost_date_time_dir/$abi_libdir"
    elif test -f $target_boost_date_time_dir/$alt_abi_libdir/libboost_date_time.so -o -f $target_boost_date_time_dir/$alt_abi_libdir/libboost_date_time.a -o -f $target_boost_date_time_dir/$alt_abi_libdir/libboost_date_time.so -o -f $target_boost_date_time_dir/$alt_abi_libdir/libboost_date_time.a; then
       found_target_boost_date_time=1
       TARGET_BOOST_DATE_TIME_LDFLAGS="-L$target_boost_date_time_dir/$alt_abi_libdir"
       TARGET_BOOST_DATE_TIME_LIB="$target_boost_date_time_dir/$abi_libdir"
    fi

    if test $found_target_boost_date_time == 0 && test "$target_boost_date_time_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_BOOST_DATE_TIME, false)
      TARGET_BOOST_DATE_TIME_CPPFLAGS=""
      TARGET_BOOST_DATE_TIME_LDFLAGS=""
      TARGET_BOOST_DATE_TIME_LIBS=""
      TARGET_BOOST_DATE_TIME_LIB=""
      TARGET_BOOST_DATE_TIME_DIR=""
      AC_MSG_RESULT(no)
    elif test $found_target_boost_date_time == 1 ; then
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_TARGET_BOOST_DATE_TIME, true)
      AC_DEFINE(HAVE_TARGET_BOOST_DATE_TIME, 1, [Define to 1 if you have a target version of BOOST_DATE_TIME.])
      TARGET_BOOST_DATE_TIME_CPPFLAGS="-I$target_boost_date_time_dir/include/boost"
      TARGET_BOOST_DATE_TIME_LIBS="-lboost_date_time"
      TARGET_BOOST_DATE_TIME_LIB="-lboost_date_time"
      TARGET_BOOST_DIR="$target_boost_date_time_dir"
    else 
      AM_CONDITIONAL(HAVE_TARGET_BOOST_DATE_TIME, false)
      TARGET_BOOST_DATE_TIME_CPPFLAGS=""
      TARGET_BOOST_DATE_TIME_LDFLAGS=""
      TARGET_BOOST_DATE_TIME_LIBS=""
      TARGET_BOOST_DATE_TIME_LIB=""
      TARGET_BOOST_DATE_TIME_DIR=""
      AC_MSG_RESULT(no)
    fi

    AC_SUBST(TARGET_BOOST_DATE_TIME_LIB)
    AC_SUBST(TARGET_BOOST_DATE_TIME_CPPFLAGS)
    AC_SUBST(TARGET_BOOST_DATE_TIME_LDFLAGS)
    AC_SUBST(TARGET_BOOST_DATE_TIME_LIBS)
    AC_SUBST(TARGET_BOOST_DATE_TIME_DIR)

])

