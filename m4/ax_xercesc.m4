
AC_DEFUN([AX_XERCESC], [

    AC_ARG_WITH(libxerces-c-prefix,
                AC_HELP_STRING([--with-libxerces-c-prefix=DIR],
                               [XERCES installation @<:@/usr@:>@]),
                xerces_dir=$withval, xerces_dir="/usr")

    LIBXERCES_C_DIR="$xerces_dir"
    LIBXERCES_C_LIBSDIR="$xerces_dir/$abi_libdir"

    if test "x$LIBXERCES_C_DIR" == "x/usr"; then
	if test -f $LIBXERCES_C_DIR/lib/x86_64-linux-gnu/libxerces-c.so; then
          LIBXERCES_C_CPPFLAGS="-I$xerces_dir/include"
          LIBXERCES_C_LDFLAGS="-L$xerces_dir/lib/x86_64-linux-gnu"
        else
          LIBXERCES_C_CPPFLAGS=""
          LIBXERCES_C_LDFLAGS=""
        fi
    else
        LIBXERCES_C_CPPFLAGS="-I$xerces_dir/include"
        LIBXERCES_C_LDFLAGS="-L$xerces_dir/$abi_libdir"
    fi

    LIBXERCES_C="-lxerces-c"
    LTLIBXERCES_C="-lxerces-c"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    xerces_saved_CPPFLAGS=$CPPFLAGS
    xerces_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $LIBXERCES_C_CPPFLAGS"
    LDFLAGS="$LDFLAGS $LIBXERCES_C_LDFLAGS $LIBXERCES_C"

    AC_MSG_CHECKING([for XERCES support])

    foundXERCES=0
    if test -f  $xerces_dir/$abi_libdir/libxerces-c.so; then
       AC_MSG_CHECKING([found xerces library])
       foundXERCES=1
    else 
	if test -f $xerces_dir/$alt_abi_libdir/libxerces-c.so; then
          AC_MSG_CHECKING([found xerces library])
          LIBXERCES_C_LIBSDIR="$xerces_dir/$alt_abi_libdir"
          LIBXERCES_C_LDFLAGS="-L$xerces_dir/$alt_abi_libdir"
          foundXERCES=1
          AC_MSG_CHECKING([found xerces lib (so)])
	elif test -f $xerces_dir/lib/x86_64-linux-gnu/libxerces-c.so; then
          AC_MSG_CHECKING([found xerces library])
          LIBXERCES_C_LIBSDIR="$xerces_dir/lib/x86_64-linux-gnu"
          LIBXERCES_C_LDFLAGS="-L$xerces_dir/lib/x86_64-linux-gnu"
          foundXERCES=1
          AC_MSG_CHECKING([found xerces lib (so)])
       else
          foundXERCES=0
          AC_MSG_CHECKING([did not find xerces lib (so)])
       fi
    fi

    if test $foundXERCES == 1 && test -f  $xerces_dir/include/xercesc/util/XMLString.hpp; then
       AC_MSG_CHECKING([found xerces headers])
       foundXERCES=1
    else
       foundXERCES=0
       AC_MSG_CHECKING([did not find xerces headers])
    fi

    if test $foundXERCES == 1; then
      AC_MSG_CHECKING([found all xerces headers, libraries and supporting libz library])
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_XERCES, true)
      AC_DEFINE(HAVE_XERCES, 1, [Define to 1 if you have XERCES.])
    else
      AC_MSG_RESULT(no)
      AM_CONDITIONAL(HAVE_XERCES, false)
      LIBXERCES_C_DIR=""
      LIBXERCES_C_LIBSDIR=""
      LIBXERCES_C_CPPFLAGS=""
      LIBXERCES_C_LDFLAGS=""
      LIBXERCES_C=""
      LTLIBXERCES_C=""
    fi


    CPPFLAGS=$xerces_saved_CPPFLAGS
    LDFLAGS=$xerces_saved_LDFLAGS

    AC_LANG_POP(C++)

    AC_SUBST(LIBXERCES_C_DIR)
    AC_SUBST(LIBXERCES_C_LIBSDIR)
    AC_SUBST(LIBXERCES_C_CPPFLAGS)
    AC_SUBST(LIBXERCES_C_LDFLAGS)
    AC_SUBST(LIBXERCES_C)
    AC_SUBST(LTLIBXERCES_C)

])

#############################################################################################
# Check for XercesC for Target Architecture 
#############################################################################################

AC_DEFUN([AX_TARGET_XERCESC], [

    AC_ARG_WITH(target-libxerces-c-prefix,
                AC_HELP_STRING([--with-target-libxerces-c-prefix=DIR],
                               [xercesc target architecture installation @<:@/opt@:>@]),
                target_xercesc_dir=$withval, target_xercesc_dir="/zzz")

    AC_MSG_CHECKING([for Targetted Xercesc support])

    found_target_xercesc=0
    if test -f $target_xercesc_dir/$abi_libdir/libxerces-c.so -o -f $target_xercesc_dir/$abi_libdir/libxerces-c.a; then
       found_target_xercesc=1
       TARGET_LIBXERCES_C_LDFLAGS="-L$target_xercesc_dir/$abi_libdir"
       TARGET_LIBXERCES_C_LIB="$target_xercesc_dir/$abi_libdir"
    elif test -f $target_xercesc_dir/$alt_abi_libdir/libxerces-c.so -o -f $target_xercesc_dir/$alt_abi_libdir/libxerces-c.a; then
       found_target_xercesc=1
       TARGET_LIBXERCES_C_LDFLAGS="-L$target_xercesc_dir/$alt_abi_libdir"
       TARGET_LIBXERCES_C_LIB="$target_xercesc_dir/$abi_libdir"
    fi

    if test $found_target_xercesc == 0 && test "$target_xercesc_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_LIBXERCES_C, false)
      TARGET_LIBXERCES_C_CPPFLAGS=""
      TARGET_LIBXERCES_C_LDFLAGS=""
      TARGET_LIBXERCES_C_LIBS=""
      TARGET_LIBXERCES_C=""
      TARGET_LIBXERCES_C_LIB=""
      TARGET_LIBXERCES_C_DIR=""
      AC_MSG_RESULT(no)
    elif test $found_target_xercesc == 1 ; then
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_TARGET_LIBXERCES_C, true)
      AC_DEFINE(HAVE_TARGET_LIBXERCES_C, 1, [Define to 1 if you have a target version of LIBXERCES_C.])
      TARGET_LIBXERCES_C_CPPFLAGS="-I$target_xercesc_dir/include/xercesc -I$target_xercesc_dir/include/"
      TARGET_LIBXERCES_C="-lxerces-c"
      TARGET_LIBXERCES_C_LIBS="-lxerces-c"
      TARGET_LIBXERCES_C_LIB="-lxerces-c"
      TARGET_LIBXERCES_C_DIR="$target_xercesc_dir"
    else 
      AM_CONDITIONAL(HAVE_TARGET_LIBXERCES_C, false)
      TARGET_LIBXERCES_C_CPPFLAGS=""
      TARGET_LIBXERCES_C_LDFLAGS=""
      TARGET_LIBXERCES_C_LIBS=""
      TARGET_LIBXERCES_C=""
      TARGET_LIBXERCES_C_LIB=""
      TARGET_LIBXERCES_C_DIR=""
      AC_MSG_RESULT(no)
    fi

    AC_SUBST(TARGET_LIBXERCES_C)
    AC_SUBST(TARGET_LIBXERCES_C_LIB)
    AC_SUBST(TARGET_LIBXERCES_C_CPPFLAGS)
    AC_SUBST(TARGET_LIBXERCES_C_LDFLAGS)
    AC_SUBST(TARGET_LIBXERCES_C_LIBS)
    AC_SUBST(TARGET_LIBXERCES_C_DIR)

])
