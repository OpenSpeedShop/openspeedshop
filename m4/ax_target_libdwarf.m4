#############################################################################################
# Check for libdwarf for Target Architecture (http://www.reality.sgiweb.org/davea/dwarf.html)
#############################################################################################

AC_DEFUN([AX_TARGET_LIBDWARF], [

    AC_ARG_WITH(target-libdwarf,
                AC_HELP_STRING([--with-target-libdwarf=DIR],
                               [libdwarf target architecture installation @<:@/opt@:>@]),
                target_libdwarf_dir=$withval, target_libdwarf_dir="/zzz")

    AC_MSG_CHECKING([for Targetted libdwarf support])

    found_target_libdwarf=0
    if test -f $target_libdwarf_dir/$abi_libdir/libdwarf.a || test -f $target_libdwarf_dir/$abi_libdir/libdwarf.so; then
       found_target_libdwarf=1
       TARGET_LIBDWARF_LDFLAGS="-L$target_libdwarf_dir/$abi_libdir"
    elif test -f $target_libdwarf_dir/$alt_abi_libdir/libdwarf.a || test -f $target_libdwarf_dir/$alt_abi_libdir/libdwarf.so; then
       found_target_libdwarf=1
       TARGET_LIBDWARF_LDFLAGS="-L$target_libdwarf_dir/$alt_abi_libdir"
    fi

    if test $found_target_libdwarf == 0 && test "$target_libdwarf_dir" == "/zzz" ; then
      AM_CONDITIONAL(HAVE_TARGET_LIBDWARF, false)
      TARGET_LIBDWARF_CPPFLAGS=""
      TARGET_LIBDWARF_LDFLAGS=""
      TARGET_LIBDWARF_LIBS=""
      TARGET_LIBDWARF_DIR=""
      AC_MSG_RESULT(no)
    elif test $found_target_libdwarf == 1 ; then
      AM_CONDITIONAL(HAVE_TARGET_LIBDWARF, true)
      AC_DEFINE(HAVE_TARGET_LIBDWARF, 1, [Define to 1 if you have a target version of LIBDWARF.])
      TARGET_LIBDWARF_CPPFLAGS="-I$target_libdwarf_dir/include"
      TARGET_LIBDWARF_LIBS="-ldwarf"
      TARGET_LIBDWARF_DIR="$target_libdwarf_dir"
      AC_MSG_RESULT(yes)
    else
      AM_CONDITIONAL(HAVE_TARGET_LIBDWARF, false)
      TARGET_LIBDWARF_CPPFLAGS=""
      TARGET_LIBDWARF_LDFLAGS=""
      TARGET_LIBDWARF_LIBS=""
      TARGET_LIBDWARF_DIR=""
      AC_MSG_RESULT(no)
    fi


    AC_SUBST(TARGET_LIBDWARF_CPPFLAGS)
    AC_SUBST(TARGET_LIBDWARF_LDFLAGS)
    AC_SUBST(TARGET_LIBDWARF_LIBS)
    AC_SUBST(TARGET_LIBDWARF_DIR)

])



