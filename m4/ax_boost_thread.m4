# ===========================================================================
#         http://www.nongnu.org/autoconf-archive/ax_boost_thread.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_BOOST_THREAD
#
# DESCRIPTION
#
#   Test for Thread library from the Boost C++ libraries. The macro requires
#   a preceding call to AX_BOOST_BASE. Further documentation is available at
#   <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_THREAD_LIB)
#
#   And sets:
#
#     HAVE_BOOST_THREAD
#
# LICENSE
#
#   Copyright (c) 2009 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2009 Michael Tindal
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 14

AC_DEFUN([AX_BOOST_THREAD],
[
	AC_ARG_WITH([boost-thread],
	AS_HELP_STRING([--with-boost-thread@<:@=special-lib@:>@],
                   [use the Thread library from boost - it is possible to specify a certain library for the linker
                        e.g. --with-boost-thread=boost_thread-gcc-mt ]),
        [
        if test "$withval" = "no"; then
			want_boost="no"
        elif test "$withval" = "yes"; then
            want_boost="yes"
            ax_boost_user_thread_lib=""
        else
		    want_boost="yes"
        	ax_boost_user_thread_lib="$withval"
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
		LDFLAGS="$LDFLAGS $BOOST_LDFLAGS -lboost_system"
		export LDFLAGS

        AC_CACHE_CHECK(whether the Boost::Thread library is available,
					   ax_cv_boost_thread,
        [AC_LANG_PUSH([C++])
			 CXXFLAGS_SAVE=$CXXFLAGS

			 if test "x$build_os" = "xsolaris" ; then
  				 CXXFLAGS="-pthreads $CXXFLAGS"
			 elif test "x$build_os" = "xming32" ; then
				 CXXFLAGS="-mthreads $CXXFLAGS"
			 else
				CXXFLAGS="-pthread $CXXFLAGS"
			 fi
			 AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[@%:@include <boost/thread/thread.hpp>]],
                                   [[boost::thread_group thrds;
                                   return 0;]])],
                   ax_cv_boost_thread=yes, ax_cv_boost_thread=no)
			 CXXFLAGS=$CXXFLAGS_SAVE
             AC_LANG_POP([C++])
		])
		if test "x$ax_cv_boost_thread" = "xyes"; then
           if test "x$build_os" = "xsolaris" ; then
			  BOOST_CPPFLAGS="-pthreads $BOOST_CPPFLAGS"
		   elif test "x$build_os" = "xming32" ; then
			  BOOST_CPPFLAGS="-mthreads $BOOST_CPPFLAGS"
		   else
			  BOOST_CPPFLAGS="-pthread $BOOST_CPPFLAGS"
		   fi

			AC_SUBST(BOOST_CPPFLAGS)

			AC_DEFINE(HAVE_BOOST_THREAD,,[define if the Boost::Thread library is available])

            boost_in_usr=no
            if test "x$BOOST_LDFLAGS" = "x"; then
                boost_in_usr=yes
                BOOST_LDFLAGS="-L/usr/lib*"
            fi
            BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`

			LDFLAGS_SAVE=$LDFLAGS
                        case "x$build_os" in
                          *bsd* )
                               LDFLAGS="-pthread $LDFLAGS"
                          break;
                          ;;
                        esac


            if test "x$ax_boost_user_thread_lib" = "x"; then
		for libextension in `ls $BOOSTLIBDIR/libboost_thread*.{so,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_thread.*\)\.so.*$;\1;' -e 's;^lib\(boost_thread.*\)\.a*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, exit,
                                 [BOOST_THREAD_LIB="-l$ax_lib"; AC_SUBST(BOOST_THREAD_LIB) link_thread="yes"; break],
                                 [link_thread="no"])
  				done
                if test "x$link_thread" != "xyes"; then
		    for libextension in `ls $BOOSTLIBDIR/boost_thread*.{dll,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_thread.*\)\.dll.*$;\1;' -e 's;^\(boost_thread.*\)\.a*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, exit,
                                 [BOOST_THREAD_LIB="-l$ax_lib"; AC_SUBST(BOOST_THREAD_LIB) link_thread="yes"; break],
                                 [link_thread="no"])
  				done
                fi

            else
               for ax_lib in $ax_boost_user_thread_lib boost_thread-$ax_boost_user_thread_lib; do
				      AC_CHECK_LIB($ax_lib, exit,
                                   [BOOST_THREAD_LIB="-l$ax_lib"; AC_SUBST(BOOST_THREAD_LIB) link_thread="yes"; break],
                                   [link_thread="no"])
                  done

            fi
			if test "x$link_thread" = "xno"; then
				AC_MSG_ERROR(Could not link against $ax_lib !)
                        else
                           case "x$build_os" in
                              *bsd* )
			        BOOST_LDFLAGS="-pthread $BOOST_LDFLAGS"
                              break;
                              ;;
                           esac

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
# Check for Boost Thread for Target Architecture 
#############################################################################################

AC_DEFUN([AC_PKG_TARGET_BOOST_THREAD], [

    AC_ARG_WITH(target-boost-thread,
                AC_HELP_STRING([--with-target-boost-thread=DIR],
                               [Boost thread target architecture installation @<:@/opt@:>@]),
                target_boost_thread_dir=$withval, target_boost_thread_dir="/zzz")

    AC_MSG_CHECKING([for Targetted Boost Thread support])

    found_target_boost_thread=0
    if test -f $target_boost_thread_dir/$abi_libdir/libboost_thread.so -o -f $target_boost_thread_dir/$abi_libdir/libboost_thread.a -o -f $target_boost_thread_dir/$abi_libdir/libboost_thread.so -o -f $target_boost_thread_dir/$abi_libdir/libboost_thread.a; then
       found_target_boost_thread=1
       TARGET_BOOST_THREAD_LDFLAGS="-L$target_boost_thread_dir/$abi_libdir"
       TARGET_BOOST_THREAD_LIB="$target_boost_thread_dir/$abi_libdir"
    elif test -f $target_boost_thread_dir/$alt_abi_libdir/libboost_thread.so -o -f $target_boost_thread_dir/$alt_abi_libdir/libboost_thread.a -o -f $target_boost_thread_dir/$alt_abi_libdir/libboost_thread.so -o -f $target_boost_thread_dir/$alt_abi_libdir/libboost_thread.a; then
       found_target_boost_thread=1
       TARGET_BOOST_THREAD_LDFLAGS="-L$target_boost_thread_dir/$alt_abi_libdir"
       TARGET_BOOST_THREAD_LIB="$target_boost_thread_dir/$abi_libdir"
    fi

    if test $found_target_boost_thread == 0 && test "$target_boost_thread_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_BOOST_THREAD, false)
      TARGET_BOOST_THREAD_CPPFLAGS=""
      TARGET_BOOST_THREAD_LDFLAGS=""
      TARGET_BOOST_THREAD_LIBS=""
      TARGET_BOOST_THREAD_LIB=""
      TARGET_BOOST_THREAD_DIR=""
      AC_MSG_RESULT(no)
    elif test $found_target_boost_thread == 1 ; then
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_TARGET_BOOST_THREAD, true)
      AC_DEFINE(HAVE_TARGET_BOOST_THREAD, 1, [Define to 1 if you have a target version of BOOST_THREAD.])
      TARGET_BOOST_THREAD_CPPFLAGS="-I$target_boost_thread_dir/include/boost"
      TARGET_BOOST_THREAD_LIBS="-lboost_thread"
      TARGET_BOOST_THREAD_LIB="-lboost_thread"
      TARGET_BOOST_THREAD_DIR="$target_boost_thread_dir"
      TARGET_BOOST_DIR="$target_boost_thread_dir"
    else 
      AM_CONDITIONAL(HAVE_TARGET_BOOST_THREAD, false)
      TARGET_BOOST_THREAD_CPPFLAGS=""
      TARGET_BOOST_THREAD_LDFLAGS=""
      TARGET_BOOST_THREAD_LIBS=""
      TARGET_BOOST_THREAD_LIB=""
      TARGET_BOOST_THREAD_DIR=""
      AC_MSG_RESULT(no)
    fi

    AC_SUBST(TARGET_BOOST_THREAD_LIB)
    AC_SUBST(TARGET_BOOST_THREAD_CPPFLAGS)
    AC_SUBST(TARGET_BOOST_THREAD_LDFLAGS)
    AC_SUBST(TARGET_BOOST_THREAD_LIBS)
    AC_SUBST(TARGET_BOOST_THREAD_DIR)

])

