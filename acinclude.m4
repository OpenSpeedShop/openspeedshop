################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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
# Check for Array Services (SGI Proprietary)
################################################################################

AC_DEFUN([AC_PKG_ARRAYSVCS], [

    AC_ARG_WITH(arraysvcs,
                AC_HELP_STRING([--with-arraysvcs=DIR],
                               [array services installation @<:@/usr@:>@]),
                arraysvcs_dir=$withval, arraysvcs_dir="/usr")

    ARRAYSVCS_CPPFLAGS="-I$arraysvcs_dir/include"
    ARRAYSVCS_LDFLAGS="-L$arraysvcs_dir/$abi_libdir"
    ARRAYSVCS_LIBS="-larray"

    case "$host" in
        *linux*)
            ARRAYSVCS_CPPFLAGS="$ARRAYSVCS_CPPFLAGS -DLINUX"
            ARRAYSVCS_CPPFLAGS="$ARRAYSVCS_CPPFLAGS -D_LANGUAGE_C_PLUS_PLUS"
	    ;;
    esac

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    arraysvcs_saved_CPPFLAGS=$CPPFLAGS
    arraysvcs_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $ARRAYSVCS_CPPFLAGS"
    LDFLAGS="$CXXFLAGS $ARRAYSVCS_LDFLAGS $ARRAYSVCS_LIBS"

    AC_MSG_CHECKING([for array services library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <arraysvcs.h>
        ]], [[
        asgeterror();
        ]]), [ AC_MSG_RESULT(yes)

            AC_DEFINE(HAVE_ARRAYSVCS, 1, 
                      [Define to 1 if you have array services.])

        ], [ AC_MSG_RESULT(no) 

            ARRAYSVCS_CPPFLAGS=""
            ARRAYSVCS_LDFLAGS=""
            ARRAYSVCS_LIBS=""

        ]
    )

    CPPFLAGS=$arraysvcs_saved_CPPFLAGS
    LDFLAGS=$arraysvcs_saved_LDFLAGS

    AC_LANG_POP(C++)

    AC_SUBST(ARRAYSVCS_CPPFLAGS)
    AC_SUBST(ARRAYSVCS_LDFLAGS)
    AC_SUBST(ARRAYSVCS_LIBS)

])

################################################################################
# Check for Binutils (http://www.gnu.org/software/binutils)
################################################################################

AC_DEFUN([AC_PKG_BINUTILS], [

    AC_ARG_WITH(binutils,
                AC_HELP_STRING([--with-binutils=DIR],
                               [binutils installation @<:@/usr@:>@]),
                binutils_dir=$withval, binutils_dir="/usr")

    AC_ARG_WITH(binutils-version,
                AC_HELP_STRING([--with-binutils-version=VERS],
                               [binutils-version installation @<:@@:>@]),
                binutils_vers=-$withval, binutils_vers="")

    case "$host" in
	ia64-*-linux*)
	    binutils_required="false"
            BINUTILS_DIR=""
	    BINUTILS_CPPFLAGS=""
	    BINUTILS_LDFLAGS=""
	    BINUTILS_LIBS=""
            ;;
	x86_64-*-linux*)
	    binutils_required="true"
            BINUTILS_DIR="$binutils_dir"
	    BINUTILS_CPPFLAGS="-I$binutils_dir/include"
	    BINUTILS_LDFLAGS="-L$binutils_dir/$abi_libdir"
	    BINUTILS_LIBS="-lopcodes$binutils_vers -lbfd$binutils_vers -liberty"
            ;;
	*)
	    binutils_required="true"
            BINUTILS_DIR="$binutils_dir"
	    BINUTILS_CPPFLAGS="-I$binutils_dir/include"
	    BINUTILS_LDFLAGS="-L$binutils_dir/$abi_libdir"
	    BINUTILS_LIBS="-lopcodes$binutils_vers -lbfd$binutils_vers -liberty"
            ;;
    esac

    binutils_saved_CPPFLAGS=$CPPFLAGS
    binutils_saved_LDFLAGS=$LDFLAGS
    binutils_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $BINUTILS_CPPFLAGS"
    LDFLAGS="$LDFLAGS $BINUTILS_LDFLAGS $BINUTILS_LIBS"

    AC_MSG_CHECKING([for binutils librarys and headers])

    AC_SEARCH_LIBS(bfd_init, [bfd$binutils_vers], 
        [ AC_MSG_RESULT(yes)

            AM_CONDITIONAL(HAVE_BINUTILS, true)
            AC_DEFINE(HAVE_BINUTILS, 1, [Define to 1 if you have BINUTILS.])

        ], [ AC_MSG_RESULT(no)

	    if test x"$binutils_required" == x"true"; then
		AM_CONDITIONAL(HAVE_BINUTILS, false)
	    else
		AM_CONDITIONAL(HAVE_BINUTILS, true)
	    fi
            BINUTILS_CPPFLAGS=""
            BINUTILS_LDFLAGS=""
            BINUTILS_LIBS=""
            BINUTILS_DIR=""

        ]
    )

    CPPFLAGS=$binutils_saved_CPPFLAGS
    LDFLAGS=$binutils_saved_LDFLAGS
    LIBS=$binutils_saved_LIBS

    AC_SUBST(BINUTILS_CPPFLAGS)
    AC_SUBST(BINUTILS_LDFLAGS)
    AC_SUBST(BINUTILS_LIBS)
    AC_SUBST(BINUTILS_DIR)

])

################################################################################
# Check for DPCL (http://oss.software.ibm.com/developerworks/opensource/dpcl)
################################################################################

AC_DEFUN([AC_PKG_DPCL], [

    AC_ARG_WITH(dpcl,
                AC_HELP_STRING([--with-dpcl=DIR],
                               [DPCL installation @<:@/usr@:>@]),
                dpcl_dir=$withval, dpcl_dir="/usr")

    DPCL_CPPFLAGS="-I$dpcl_dir/include/dpcl -D_DYNINST -DENABLE_MANUAL_START"
    DPCL_LDFLAGS="-L$dpcl_dir/$abi_libdir"

    case "$host" in
	x86_64-*-linux* | ia64-*-linux*)
            DPCL_CPPFLAGS="$DPCL_CPPFLAGS -D__64BIT__"
	    DPCL_LIBS="-ldpcl64 -lpthread"
            ;;
	*)
	    DPCL_LIBS="-ldpcl -lpthread"
            ;;
    esac

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    dpcl_saved_CPPFLAGS=$CPPFLAGS
    dpcl_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $DPCL_CPPFLAGS"
    LDFLAGS="$CXXFLAGS $DPCL_LDFLAGS $DPCL_LIBS"

    AC_MSG_CHECKING([for DPCL client library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <dpcl.h>
        ]], [[
        Ais_initialize();
        ]]), AC_MSG_RESULT(yes), [ AC_MSG_RESULT(no)
        AC_MSG_FAILURE(cannot locate DPCL client library and/or headers.) ]
    )

    CPPFLAGS=$dpcl_saved_CPPFLAGS
    LDFLAGS=$dpcl_saved_LDFLAGS

    AC_LANG_POP(C++)

    AC_SUBST(DPCL_CPPFLAGS)
    AC_SUBST(DPCL_LDFLAGS)
    AC_SUBST(DPCL_LIBS)

    AC_DEFINE(HAVE_DPCL, 1, [Define to 1 if you have DPCL.])

])

################################################################################
# Check for Dyninst (http://www.dyninst.org)
################################################################################

AC_DEFUN([AC_PKG_DYNINST], [

    AC_ARG_WITH(dyninst,
                AC_HELP_STRING([--with-dyninst=DIR],
                               [Dyninst installation @<:@/usr@:>@]),
                dyninst_dir=$withval, dyninst_dir="/usr")

    DYNINST_CPPFLAGS="-I$dyninst_dir/include/dyninst"
    DYNINST_CPPFLAGS="$DYNINST_CPPFLAGS -DUSE_STL_VECTOR -DIBM_BPATCH_COMPAT"
    DYNINST_LDFLAGS="-L$dyninst_dir/$abi_libdir"
    DYNINST_LIBS="-ldyninstAPI -lcommon -lsymtabAPI"
    DYNINST_DIR="$dyninst_dir"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    dyninst_saved_CPPFLAGS=$CPPFLAGS
    dyninst_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $DYNINST_CPPFLAGS"
    LDFLAGS="$CXXFLAGS $DYNINST_LDFLAGS $DYNINST_LIBS -liberty $LIBDWARF_LDFLAGS $LIBDWARF_LIBS"

    AC_MSG_CHECKING([for Dyninst API library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
	#include <BPatch.h>
        ]], [[
	BPatch bpatch();
        ]]), AC_MSG_RESULT(yes), [ AC_MSG_RESULT(no)
        AC_MSG_FAILURE(cannot locate Dyninst API library and/or headers.) ]
    )

    CPPFLAGS=$dyninst_saved_CPPFLAGS
    LDFLAGS=$dyninst_saved_LDFLAGS

    AC_LANG_POP(C++)

    AC_SUBST(DYNINST_CPPFLAGS)
    AC_SUBST(DYNINST_LDFLAGS)
    AC_SUBST(DYNINST_LIBS)
    AC_SUBST(DYNINST_DIR)

    AC_DEFINE(HAVE_DYNINST, 1, [Define to 1 if you have Dyninst.])

])

################################################################################
# Check for Libunwind (http://www.hpl.hp.com/research/linux/libunwind)
################################################################################

AC_DEFUN([AC_PKG_LIBUNWIND], [

    AC_ARG_WITH(libunwind,
                AC_HELP_STRING([--with-libunwind=DIR],
                               [libunwind installation @<:@/usr@:>@]),
                libunwind_dir=$withval, libunwind_dir="/usr")

    LIBUNWIND_CPPFLAGS="-I$libunwind_dir/include -DUNW_LOCAL_ONLY"
    LIBUNWIND_LDFLAGS="-L$libunwind_dir/$abi_libdir"
    LIBUNWIND_LIBS="-lunwind"

    libunwind_saved_CPPFLAGS=$CPPFLAGS
    libunwind_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $LIBUNWIND_CPPFLAGS"
    LDFLAGS="$LDFLAGS $LIBUNWIND_LDFLAGS $LIBUNWIND_LIBS"

    AC_MSG_CHECKING([for libunwind library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <libunwind.h>
        ]], [[
        unw_init_local((void*)0, (void*)0);
        ]]), [ AC_MSG_RESULT(yes)

            AM_CONDITIONAL(HAVE_LIBUNWIND, true)
            AC_DEFINE(HAVE_LIBUNWIND, 1, [Define to 1 if you have libunwind.])

        ], [ AC_MSG_RESULT(no)

            AM_CONDITIONAL(HAVE_LIBUNWIND, false)
            LIBUNWIND_CPPFLAGS=""
            LIBUNWIND_LDFLAGS=""
            LIBUNWIND_LIBS=""

        ]
    )

    CPPFLAGS=$libunwind_saved_CPPFLAGS
    LDFLAGS=$libunwind_saved_LDFLAGS

    AC_SUBST(LIBUNWIND_CPPFLAGS)
    AC_SUBST(LIBUNWIND_LDFLAGS)
    AC_SUBST(LIBUNWIND_LIBS)

])

################################################################################
# Check for MPI (http://www.mpi-forum.org)
################################################################################

m4_include(ac_pkg_mpi.m4)

################################################################################
# Check for MRNet (http://www.paradyn.org/mrnet)
################################################################################

AC_DEFUN([AC_PKG_MRNET], [

    AC_ARG_WITH(mrnet,
                AC_HELP_STRING([--with-mrnet=DIR],
                               [MRNet installation @<:@/usr@:>@]),
                mrnet_dir=$withval, mrnet_dir="/usr")

    MRNET_CPPFLAGS="-I$mrnet_dir/include"
    MRNET_LDFLAGS="-L$mrnet_dir/$abi_libdir"
    MRNET_LIBS="-Wl,--whole-archive -lmrnet -lxplat -Wl,--no-whole-archive"
    MRNET_LIBS="$MRNET_LIBS -lpthread -ldl"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    mrnet_saved_CPPFLAGS=$CPPFLAGS
    mrnet_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $MRNET_CPPFLAGS"
    LDFLAGS="$CXXFLAGS $MRNET_LDFLAGS $MRNET_LIBS"

    AC_MSG_CHECKING([for MRNet library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
	#include <mrnet/MRNet.h>
        ]], [[
	MRN::set_OutputLevel(0);
        ]]), [ AC_MSG_RESULT(yes)

            AC_DEFINE(HAVE_MRNET, 1, 
                      [Define to 1 if you have MRNet.])

        ], [ AC_MSG_RESULT(no) 

            MRNET_CPPFLAGS=""
            MRNET_LDFLAGS=""
            MRNET_LIBS=""

        ]
    )

    CPPFLAGS=$mrnet_saved_CPPFLAGS
    LDFLAGS=$mrnet_saved_LDFLAGS

    AC_LANG_POP(C++)

    AC_SUBST(MRNET_CPPFLAGS)
    AC_SUBST(MRNET_LDFLAGS)
    AC_SUBST(MRNET_LIBS)

])

################################################################################
# Check for OpenMP (http://www.openmp.org)
################################################################################

AC_DEFUN([AC_PKG_OPENMP], [

    OPENMP_CPPFLAGS="-fopenmp"
    OPENMP_LDFLAGS=""
    OPENMP_LIBS="-lgomp"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    openmp_saved_CPPFLAGS=$CPPFLAGS
    openmp_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $OPENMP_CPPFLAGS"
    LDFLAGS="$LDFLAGS $OPENMP_LDFLAGS $OPENMP_LIBS"

    AC_MSG_CHECKING([for OpenMP support])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
	#include <omp.h>
	#include <stdio.h>
        ]], [[
	#pragma omp parallel for
	for(int i = 0; i < 100; ++i)
	    printf("[%d] i = %d\n", omp_get_thread_num(), i);
        ]]), [ AC_MSG_RESULT(yes)

            AM_CONDITIONAL(HAVE_OPENMP, true)
	    AC_DEFINE(HAVE_OPENMP, 1, [Define to 1 if you have OpenMP.])

        ], [ AC_MSG_RESULT(no)

            AM_CONDITIONAL(HAVE_OPENMP, false)
	    OPENMP_CPPFLAGS=""
            OPENMP_LDFLAGS=""
            OPENMP_LIBS=""

        ]
    )

    CPPFLAGS=$openmp_saved_CPPFLAGS
    LDFLAGS=$openmp_saved_LDFLAGS

    AC_LANG_POP(C++)

    AC_SUBST(OPENMP_CPPFLAGS)
    AC_SUBST(OPENMP_LDFLAGS)
    AC_SUBST(OPENMP_LIBS)

])

################################################################################
# Check for OTF (http://www.paratools.com/otf)
################################################################################

AC_DEFUN([AC_PKG_OTF], [

    AC_ARG_WITH(otf,
                AC_HELP_STRING([--with-otf=DIR],
                               [OTF Open Trace Format library installation @<:@/usr@:>@]),
                otf_dir=$withval, otf_dir="/usr")

    AC_ARG_WITH(libz,
                AC_HELP_STRING([--with-libz=DIR],
                               [libz installation @<:@/usr@:>@]),
                libz_dir=$withval, libz_dir="/usr")

    OTF_DIR="$otf_dir"
    OTF_LIBSDIR="$otf_dir/$abi_libdir"
    OTF_CPPFLAGS="-I$otf_dir/include"
    OTF_LDFLAGS="-L$otf_dir/$abi_libdir"
    OTF_LIBS="-lotf"

    OTF_LIBZ_LDFLAGS="-L/usr/$abi_libdir"
    OTF_LIBZ_LIBS="-lz"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    otf_saved_CPPFLAGS=$CPPFLAGS
    otf_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $OTF_CPPFLAGS"
    LDFLAGS="$LDFLAGS $OTF_LDFLAGS $OTF_LIBS"

    AC_MSG_CHECKING([for OTF support])

    foundOTF=0
    if test -f  $otf_dir/$abi_libdir/libotf.a; then
       AC_MSG_CHECKING([found otf library])
       foundOTF=1
    else 
	if test -f $otf_dir/$alt_abi_libdir/libotf.a; then
          AC_MSG_CHECKING([found otf library])
          OTF_LIBSDIR="$otf_dir/$alt_abi_libdir"
          OTF_LDFLAGS="-L$otf_dir/$alt_abi_libdir"
          foundOTF=1
       else
          foundOTF=0
       fi
    fi

    if test $foundOTF == 1 && test -f  $otf_dir/include/otf.h; then
       AC_MSG_CHECKING([found otf headers])
       foundOTF=1
    else
       foundOTF=0
    fi

    if test $foundOTF == 1 && test -f  $libz_dir/$abi_libdir/libz.so; then
       AC_MSG_CHECKING([found libz library used by otf])
       foundOTF=1
    else
       foundOTF=0
    fi

    if test $foundOTF == 1; then
      AC_MSG_CHECKING([found all otf headers, libraries and supporting libz library])
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_OTF, true)
      AC_DEFINE(HAVE_OTF, 1, [Define to 1 if you have OTF.])
    else
      AC_MSG_RESULT(no)
      AM_CONDITIONAL(HAVE_OTF, false)
      OTF_DIR=""
      OTF_LIBSDIR=""
      OTF_CPPFLAGS=""
      OTF_LDFLAGS=""
      OTF_LIBS=""
      OTF_LIBZ_LDFLAGS=""
      OTF_LIBZ_LIBS=""
    fi


    CPPFLAGS=$otf_saved_CPPFLAGS
    LDFLAGS=$otf_saved_LDFLAGS

    AC_LANG_POP(C++)

    AC_SUBST(OTF_DIR)
    AC_SUBST(OTF_LIBSDIR)
    AC_SUBST(OTF_CPPFLAGS)
    AC_SUBST(OTF_LDFLAGS)
    AC_SUBST(OTF_LIBS)
    AC_SUBST(OTF_LIBZ_LDFLAGS)
    AC_SUBST(OTF_LIBZ_LIBS)

])


################################################################################
# Check for VampirTrace  (http://tu-dresden.de/die_tu_dresden/zentrale_einrichtungen
#/zih/forschung/software_werkzeuge_zur_unterstuetzung_von_programmierung_und_optimierung
#/vampirtrace?set_language=en&cl=en)
################################################################################

AC_DEFUN([AC_PKG_VT], [

    AC_ARG_WITH(vt,
                AC_HELP_STRING([--with-vt=DIR],
                               [VampirTrace (vt) library installation @<:@/usr@:>@]),
                vt_dir=$withval, vt_dir="/usr")

    VT_DIR="$vt_dir"
    VT_CPPFLAGS="-I$vt_dir/include"
    VT_LIBS="-lvt.mpi"

    AC_LANG_PUSH(C++)
    AC_REQUIRE_CPP

    AC_MSG_CHECKING([for VampirTrace support])

    foundVT=0
    if test -f  $vt_dir/$abi_libdir/libvt.mpi.a; then
       AC_MSG_CHECKING([found VampirTrace library])
       VT_LDFLAGS="-L$vt_dir/$abi_libdir"
       VT_LIBSDIR="$vt_dir/$abi_libdir"
       foundVT=1
    else
      if test -f  $vt_dir/$alt_abi_libdir/libvt.mpi.a; then
         AC_MSG_CHECKING([found VampirTrace library])
         VT_LDFLAGS="-L$vt_dir/$alt_abi_libdir"
         VT_LIBSDIR="$vt_dir/$alt_abi_libdir"
         foundVT=1
      else
         foundVT=0
      fi
    fi

    if test $foundVT == 1 && test -f  $vt_dir/include/vt_user.h; then
       AC_MSG_CHECKING([found VampirTrace headers])
       foundVT=1
    else
       foundVT=0
    fi

    if test $foundVT == 1; then
      AC_MSG_CHECKING([found all VampirTrace headers, libraries.])
      AC_MSG_RESULT(yes)
      AM_CONDITIONAL(HAVE_VT, true)
      AC_DEFINE(HAVE_VT, 1, [Define to 1 if you have VT.])
    else
      AC_MSG_RESULT(no)
      AM_CONDITIONAL(HAVE_VT, false)
      VT_CPPFLAGS=""
      VT_DIR=""
      VT_LDFLAGS=""
      VT_LIBS=""
      VT_LIBSDIR=""
    fi


    AC_LANG_POP(C++)

    AC_SUBST(VT_DIR)
    AC_SUBST(VT_CPPFLAGS)
    AC_SUBST(VT_LDFLAGS)
    AC_SUBST(VT_LIBS)
    AC_SUBST(VT_LIBSDIR)

])

################################################################################
# Check for PAPI (http://icl.cs.utk.edu/papi)
################################################################################

AC_DEFUN([AC_PKG_PAPI], [

    AC_ARG_WITH(papi,
                AC_HELP_STRING([--with-papi=DIR],
                               [PAPI installation @<:@/usr@:>@]),
                papi_dir=$withval, papi_dir="/usr")

    PAPI_CPPFLAGS="-I$papi_dir/include"
    PAPI_LDFLAGS="-L$papi_dir/$abi_libdir"

    case "$host" in
	ia64-*-linux*)
	    PAPI_LIBS="-lpapi -lpfm"
            ;;
	*)
	    PAPI_LIBS="-lpapi"
            ;;
    esac

    papi_saved_CPPFLAGS=$CPPFLAGS
    papi_saved_LDFLAGS=$LDFLAGS
    papi_saved_LIBS=$LIBS

    CPPFLAGS="$CPPFLAGS $PAPI_CPPFLAGS"
#    LDFLAGS="$LDFLAGS $PAPI_LDFLAGS $PAPI_LIBS"
    LDFLAGS="$PAPI_LDFLAGS $PAPI_LIBS"
    LIBS=""

    AC_MSG_CHECKING([for PAPI library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <papi.h>
        ]], [[
	PAPI_is_initialized();
        ]]), [ AC_MSG_RESULT(yes)

            AM_CONDITIONAL(HAVE_PAPI, true)
            AC_DEFINE(HAVE_PAPI, 1, [Define to 1 if you have PAPI.])

        ], [ AC_MSG_RESULT(no)

            AM_CONDITIONAL(HAVE_PAPI, false)
            PAPI_CPPFLAGS=""
            PAPI_LDFLAGS=""
            PAPI_LIBS=""

        ]
    )

    CPPFLAGS=$papi_saved_CPPFLAGS
    LDFLAGS=$papi_saved_LDFLAGS
    LIBS=$papi_saved_LIBS

    AC_SUBST(PAPI_CPPFLAGS)
    AC_SUBST(PAPI_LDFLAGS)
    AC_SUBST(PAPI_LIBS)

])

################################################################################
# Check for SQLite (http://www.sqlite.org)
################################################################################

AC_DEFUN([AC_PKG_SQLITE], [

    AC_ARG_WITH(sqlite,
                AC_HELP_STRING([--with-sqlite=DIR],
                               [SQLite installation @<:@/usr@:>@]),
                sqlite_dir=$withval, sqlite_dir="/usr")

    SQLITE_CPPFLAGS="-I$sqlite_dir/include"
    SQLITE_LDFLAGS="-L$sqlite_dir/$abi_libdir"
    SQLITE_LIBS="-lsqlite3"

    sqlite_saved_CPPFLAGS=$CPPFLAGS
    sqlite_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $SQLITE_CPPFLAGS"
    LDFLAGS="$LDFLAGS $SQLITE_LDFLAGS $SQLITE_LIBS"

    AC_MSG_CHECKING([for SQLite library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <sqlite3.h>
        ]], [[
	sqlite3_libversion();
        ]]), AC_MSG_RESULT(yes), [ AC_MSG_RESULT(no)
        AC_MSG_FAILURE(cannot locate SQLite library and/or headers.) ]
    )

    CPPFLAGS=$sqlite_saved_CPPFLAGS
    LDFLAGS=$sqlite_saved_LDFLAGS

    AC_SUBST(SQLITE_CPPFLAGS)
    AC_SUBST(SQLITE_LDFLAGS)
    AC_SUBST(SQLITE_LIBS)

    AC_DEFINE(HAVE_SQLITE, 1, [Define to 1 if you have SQLite.])

])



################################################################################
# Check for PYTHON libraries
# Some of this code was found by an internet search for python configuration/
# checking, so credit to those who originally created parts or all 
# of the code below.
################################################################################
dnl @synopsis AC_PYTHON_DEVEL([version])
dnl
dnl Checks for Python and tries to get the include path to 'Python.h'.
dnl It provides the $(PYTHON_CPPFLAGS) and $(PYTHON_LDFLAGS) output
dnl variables. Also exports $(PYTHON_EXTRA_LIBS) and
dnl $(PYTHON_EXTRA_LDFLAGS) for embedding Python in your code.
dnl
dnl You can search for some particular version of Python by passing a
dnl parameter to this macro, for example ">= '2.3.1'", or "== '2.4'".
dnl Please note that you *have* to pass also an operator along with the
dnl version to match, and pay special attention to the single quotes
dnl surrounding the version number.
dnl
dnl If the user wants to employ a particular version of Python, she can
dnl now pass to configure the PYTHON_VERSION environment variable. This
dnl is only limited by the macro parameter set by the packager.
dnl
dnl This macro should work for all versions of Python >= 2.1.0. You can
dnl disable the check for the python version by setting the
dnl PYTHON_NOVERSIONCHECK environment variable to something else than
dnl the empty string.
dnl
dnl If you need to use this macro for an older Python version, please
dnl contact the authors. We're always open for feedback.
dnl
dnl @category InstalledPackages
dnl @author Sebastian Huber <sebastian-huber@web.de>
dnl @author Alan W. Irwin <irwin@beluga.phys.uvic.ca>
dnl @author Rafael Laboissiere <laboissiere@psy.mpg.de>
dnl @author Andrew Collier <colliera@nu.ac.za>
dnl @author Matteo Settenvini <matteo@member.fsf.org>
dnl @version 2006-02-05
dnl @license GPLWithACException

AC_DEFUN([AC_PYTHON_DEVEL],[
        #
        # Allow the use of a (user set) custom python version
        #
        AC_ARG_VAR([PYTHON_VERSION],[The installed Python
                version to use, for example '2.3'. This string
                will be appended to the Python interpreter
                canonical name.])

dnl Support the user enabled --with-python option.
    AC_MSG_CHECKING(for --with-python)
    AC_ARG_WITH(python,
                AC_HELP_STRING([--with-python=PYTHON],
                               [path name of Python libraries]),
                [ if test "$withval" != "yes"
                  then
                    PYTHON="$withval/bin/python"
                    AC_MSG_RESULT($withval)
                  else
                    AC_MSG_RESULT(no)
                  fi
                ],
                [ AC_MSG_RESULT(no)
                ])

        AC_PATH_PROG([PYTHON],[python[$PYTHON_VERSION]])
        if test -z "$PYTHON"; then
           AC_MSG_ERROR([Cannot find python$PYTHON_VERSION in your system path])
        fi

        #
        # Check for a version of Python >= 2.1.0
        #
        AC_MSG_CHECKING([for a version of Python >= '2.1.0'])
        ac_supports_python_ver=`$PYTHON -c "import sys, string; \
                ver = string.split(sys.version)[[0]]; \
                print ver >= '2.1.0'"`
        if test "$ac_supports_python_ver" != "True"; then
                if test -z "$PYTHON_NOVERSIONCHECK"; then
                        AC_MSG_RESULT([no])
                        AC_MSG_FAILURE([
This version of the AC@&t@_PYTHON_DEVEL macro
doesn't work properly with versions of Python before
2.1.0. You may need to re-run configure, setting the
variables PYTHON_CPPFLAGS, PYTHON_LDFLAGS, PYTHON_SITE_PKG,
PYTHON_EXTRA_LIBS and PYTHON_EXTRA_LDFLAGS by hand.
Moreover, to disable this check, set PYTHON_NOVERSIONCHECK
to something else than an empty string.
])dnl
                else
                        AC_MSG_RESULT([skip at user request])
                fi
        else
                AC_MSG_RESULT([yes])
        fi

        #
        # if the macro parameter ``version'' is set, honour it
        #
        if test -n "$1"; then
                AC_MSG_CHECKING([for a version of Python $1])
                ac_supports_python_ver=`$PYTHON -c "import sys, string; \
                        ver = string.split(sys.version)[[0]]; \
                        print ver $1"`
                if test "$ac_supports_python_ver" = "True"; then
                   AC_MSG_RESULT([yes])
                else
                        AC_MSG_RESULT([no])
                        AC_MSG_ERROR([this package requires Python $1.
If you have it installed, but it isn't the default Python
interpreter in your system path, please pass the PYTHON_VERSION
variable to configure. See ``configure --help'' for reference.
])dnl
                fi
        fi

        #
        # Check if you have distutils, else fail
        #
        AC_MSG_CHECKING([for the distutils Python package])
        ac_distutils_result=`$PYTHON -c "import distutils" 2>&1`
        if test -z "$ac_distutils_result"; then
                AC_MSG_RESULT([yes])
        else
                AC_MSG_RESULT([no])
                AC_MSG_ERROR([cannot import Python module "distutils".
Please check your Python installation. The error was:
$ac_distutils_result])
        fi

        #
        # Check for Python include path
        #
        AC_MSG_CHECKING([for Python include path])
        if test -z "$PYTHON_CPPFLAGS"; then
                python_path=`$PYTHON -c "import distutils.sysconfig; \
                        print distutils.sysconfig.get_python_inc();"`
                if test -n "${python_path}"; then
                        python_path="-I$python_path"
                fi
                PYTHON_CPPFLAGS=$python_path
        fi
        AC_MSG_RESULT([$PYTHON_CPPFLAGS])
        AC_SUBST([PYTHON_CPPFLAGS])

        #
        # Check for Python library path
        #
        AC_MSG_CHECKING([for Python library path])
        if test -z "$PYTHON_LDFLAGS"; then
                # (makes two attempts to ensure we've got a version number
                # from the interpreter)
                py_version=`$PYTHON -c "from distutils.sysconfig import *; \
                        from string import join; \
                        print join(get_config_vars('VERSION'))"`
                if test "$py_version" == "[None]"; then
                        if test -n "$PYTHON_VERSION"; then
                                py_version=$PYTHON_VERSION
                        else
                                py_version=`$PYTHON -c "import sys; \
                                        print sys.version[[:3]]"`
                        fi
                fi

                PYTHON_LDFLAGS=`$PYTHON -c "from distutils.sysconfig import *; \
                        from string import join; \
                        print '-L' + get_python_lib(0,1), \
                        '-lpython';"`$py_version
        fi
        AC_MSG_RESULT([$PYTHON_LDFLAGS])
        AC_SUBST([PYTHON_LDFLAGS])

        #
        # Check for site packages
        #
        AC_MSG_CHECKING([for Python site-packages path])
        if test -z "$PYTHON_SITE_PKG"; then
                PYTHON_SITE_PKG=`$PYTHON -c "import distutils.sysconfig; \
                        print distutils.sysconfig.get_python_lib(0,0);"`
        fi
        AC_MSG_RESULT([$PYTHON_SITE_PKG])
        AC_SUBST([PYTHON_SITE_PKG])

        #
        # libraries which must be linked in when embedding
        #
        AC_MSG_CHECKING(python extra libraries)
        if test -z "$PYTHON_EXTRA_LIBS"; then
           PYTHON_EXTRA_LIBS=`$PYTHON -c "import distutils.sysconfig; \
                conf = distutils.sysconfig.get_config_var; \
                print conf('LOCALMODLIBS'), conf('LIBS')"`
        fi
        AC_MSG_RESULT([$PYTHON_EXTRA_LIBS])
        AC_SUBST(PYTHON_EXTRA_LIBS)

        #
        # linking flags needed when embedding
        #
        AC_MSG_CHECKING(python extra linking flags)
        if test -z "$PYTHON_EXTRA_LDFLAGS"; then
                PYTHON_EXTRA_LDFLAGS=`$PYTHON -c "import distutils.sysconfig; \
                        conf = distutils.sysconfig.get_config_var; \
                        print conf('LINKFORSHARED')"`
        fi
        AC_MSG_RESULT([$PYTHON_EXTRA_LDFLAGS])
        AC_SUBST(PYTHON_EXTRA_LDFLAGS)
])


################################################################################
# Check for QT libraries
# Some of this code was found by an internet search for qt configuration/
# checking, so credit to those who originally created parts of the code below.
################################################################################

AC_DEFUN([AC_PKG_QTLIB], [

LQTDIR=$QTDIR
dnl Support the user enabled --with-qtlib option.
    AC_ARG_WITH(qtlib,
                AC_HELP_STRING([--with-qtlib=DIR],
                               [QTLIB installation @<:@/usr@:>@]),
                [LQTDIR=$withval AC_SUBST($LQTDIR) qtlib_dir=$withval], qtlib_dir="ac_qtlib")

dnl Check for QTDIR environment variable.
AC_MSG_CHECKING([whether QTDIR environment variable is set])
if test "x$LQTDIR" = "x"; then
  AC_MSG_RESULT([no])
  AC_MSG_ERROR([QTDIR must be properly set.])
else
  AC_MSG_RESULT([$LQTDIR])
fi

dnl Use temporary variable to hold initial library abi setting.
dnl This will be changed if initial checks don't find qt libs.
dnl Then the temporary variable will be checked for the alternative lib location.
tmp_abi_libdir=$abi_libdir

dnl Checks for Qt library.
AC_CACHE_CHECK([for Qt library],
  ac_qtlib, [
  for X in qt-mt qt; do
    if test "x$ac_qtlib" = "x"; then
      if test -f $LQTDIR/$abi_libdir/lib$X.so -o -f $LQTDIR/$abi_libdir/lib$X.a; then
        ac_qtlib=$X
      fi
    fi
  done
])
AC_MSG_RESULT([Temporary output: ac_qtlib=$ac_qtlib])

if test "x$ac_qtlib" = "x"; then
dnl Conduct another check for the QT libs HERE look in lib or lib64
dnl depending on where the first check looked. See configure.ac
dnl section for determining abi_libdir.
  AC_CACHE_CHECK([for Qt library],
    ac_qtlib, [
    for X in qt-mt qt; do
      if test "x$ac_qtlib" = "x"; then
        if test -f $LQTDIR/$alt_abi_libdir/lib$X.so -o -f $LQTDIR/$alt_abi_libdir/lib$X.a; then
          ac_qtlib=$X
	  tmp_abi_libdir=$alt_abi_libdir
        fi
      fi
    done
  ])
  AC_MSG_RESULT([Temporary output: ac_qtlib=$ac_qtlib])
  if test "x$ac_qtlib" = "x"; then
    AC_MSG_ERROR([Qt library not found. Maybe QTDIR isn't properly set.])
  fi
fi
AC_SUBST(ac_qtlib)

dnl Check for Qt multi-thread support.
if test "x$ac_qtlib" = "xqt-mt"; then
  ac_thread="thread"
fi
AC_SUBST(ac_thread)

dnl Set initial values for QTLIB exports
QTLIB_CFLAGS="$CFLAGS -I$LQTDIR/include -I$KDEDIR/include"
# QTLIB_CPPFLAGS="$CPPFLAGS -I$QTDIR/include -I$KDEDIR/include -D_REENTRANT  -DQT_NO_DEBUG -DQT_THREAD_SUPPORT -DQT_SHARED"
QTLIB_CPPFLAGS="$CPPFLAGS -I$LQTDIR/include -I$KDEDIR/include -DQT_NO_DEBUG -DQT_THREAD_SUPPORT -DQT_SHARED"

dnl Use temporary abi library setting to set the QT library variables
QTLIB_LIBS="-L$LQTDIR/$tmp_abi_libdir -L/usr/X11R6/$tmp_abi_libdir"

QTLIB_LDFLAGS="-l$ac_qtlib"

dnl Save the current CPPFLAGS and LDFLAGS variables prior to qt version test
qtlib_saved_CPPFLAGS=$CPPFLAGS
qtlib_saved_LDFLAGS=$LDFLAGS

dnl Use QT CPPFLAGS and LDFLAGS variables for the qt version test
CPPFLAGS="$CPPFLAGS $QTLIB_CPPFLAGS"
LDFLAGS="$CXXFLAGS $QTLIB_LDFLAGS $QTLIB_LIBS"

dnl Check if we have qglobal.h which is an indication of QT 3.3 or greater
AC_CACHE_CHECK(
[for Qt library version >= 3.3.0], ac_qtlib_version,
[
    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
	#include <qglobal.h>
	        ]], [[
	        ]]), AC_MSG_RESULT(yes), [ AC_MSG_RESULT(no)
	        AC_MSG_FAILURE(Can not locate Qt library and/or the headers.) ]
   	    )
	AC_TRY_LINK(
	[
		#include "qglobal.h"
	],
	[
		#if QT_VERSION < 0x030300
		#error Qt library 3.3 or greater is required.
		#endif
	],
	[
		ac_qtlib_version="yes"
		QT_CFLAGS="$CFLAGS"
		QT_LIBS="-l$ac_qtlib"
		QT_LDFLAGS="$LIBS"
		],
	[
		QT_CFLAGS=""
		QT_LIBS=""
		QT_LDFLAGS=""
		AC_MSG_ERROR([** QT version 3.3 or greater is required for the Open|SpeedShop QT gui.])
	])
	]
)

dnl A common error message:
ac_qtdir_errmsg="Not found in current PATH. Maybe QT development environment isn't available (qt3-devel)."

dnl Check for Qt qmake utility.
AC_PATH_PROG(ac_qmake, qmake, [no], $ac_qtdir/bin:${PATH})
if test "x$ac_qmake" = "xno"; then
   AC_MSG_ERROR([qmake $ac_qtdir_errmsg])
fi

    AC_SUBST(ac_qmake)

dnl Support the user enabled --with-qtlib option.
    AC_ARG_WITH(qtlib,
                AC_HELP_STRING([--with-qtlib=DIR],
                               [QTLIB installation @<:@/usr@:>@]),
                qtlib_dir=$withval, qtlib_dir="ac_qtlib")

dnl This only needed if we continue the build environment ROOT support
    if test -d "$ROOT"; then
        AC_CHECK_FILE([$ROOT/include/qt-3.3/qtwidget.h], [
            QTLIB_CPPFLAGS="-I$ROOT/include/qt-3.3"
            QTLIB_LDFLAGS="-L$ROOT/$abi_libdir"
            QTLIB_LIBS="-lqt-mt"
        ])
    fi

dnl Restore saved flags after the QT version check program compilation
    CPPFLAGS=$qtlib_saved_CPPFLAGS
    LDFLAGS=$qtlib_saved_LDFLAGS

dnl Make the QTLIB flags/libs available
    AC_SUBST(QTLIB_CPPFLAGS)
    AC_SUBST(QTLIB_LDFLAGS)
    AC_SUBST(QTLIB_LIBS)
    AC_DEFINE(HAVE_QTLIB, 1, [Define to 1 if you have Qt library 3.3 >])

])

################################################################################
# Check for Monitor (http://www.cs.utk.edu/~mucci)
################################################################################

AC_DEFUN([AC_PKG_LIBMONITOR], [

    AC_ARG_WITH(libmonitor,
                AC_HELP_STRING([--with-libmonitor=DIR],
                               [libmonitor installation @<:@/usr@:>@]),
                libmonitor_dir=$withval, libmonitor_dir="/usr")

    LIBMONITOR_CPPFLAGS="-I$libmonitor_dir/include"
    LIBMONITOR_LDFLAGS="-L$libmonitor_dir/$abi_libdir"
    LIBMONITOR_LIBS="-lmonitor"

    libmonitor_saved_CPPFLAGS=$CPPFLAGS
    libmonitor_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $LIBMONITOR_CPPFLAGS"
    LDFLAGS="$LDFLAGS $LIBMONITOR_LDFLAGS $LIBMONITOR_LIBS -lpthread"

    AC_MSG_CHECKING([for libmonitor library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <monitor.h>
        ]], [[
        monitor_fini_process();
        ]]), [ AC_MSG_RESULT(yes)

            AM_CONDITIONAL(HAVE_LIBMONITOR, true)
            AC_DEFINE(HAVE_LIBMONITOR, 1, [Define to 1 if you have libmonitor.])

        ], [ AC_MSG_RESULT(no)

            AM_CONDITIONAL(HAVE_LIBMONITOR, false)
            LIBMONITOR_CPPFLAGS=""
            LIBMONITOR_LDFLAGS=""
            LIBMONITOR_LIBS=""

        ]
    )

    CPPFLAGS=$libmonitor_saved_CPPFLAGS
    LDFLAGS=$libmonitor_saved_LDFLAGS

    AC_SUBST(LIBMONITOR_CPPFLAGS)
    AC_SUBST(LIBMONITOR_LDFLAGS)
    AC_SUBST(LIBMONITOR_LIBS)

])

################################################################################
# Check for libdwarf (http://www.reality.sgiweb.org/davea/dwarf.html)
################################################################################

AC_DEFUN([AC_PKG_LIBDWARF], [

    AC_ARG_WITH(libdwarf,
                AC_HELP_STRING([--with-libdwarf=DIR],
                               [libdwarf installation @<:@/usr@:>@]),
                libdwarf_dir=$withval, libdwarf_dir="/usr")

    found_libdwarf=0

    LIBDWARF_CPPFLAGS="-I$libdwarf_dir/$abi_libdir/libdwarf/include"
    LIBDWARF_LDFLAGS="-L$libdwarf_dir/$abi_libdir/libdwarf/$abi_libdir"
    LIBDWARF_LIBS="-ldwarf"

    libdwarf_saved_CPPFLAGS=$CPPFLAGS
    libdwarf_saved_LDFLAGS=$LDFLAGS

    CPPFLAGS="$CPPFLAGS $LIBDWARF_CPPFLAGS"
    LDFLAGS="$LDFLAGS $LIBDWARF_LDFLAGS $LIBDWARF_LIBS -lelf -lpthread"

    AC_MSG_CHECKING([for libdwarf library and headers])

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
        #include <dwarf.h>
        ]], [[
        if (DW_ID_up_case != DW_ID_down_case) {
           int mycase = DW_ID_up_case;
        }
        ]]), [ found_libdwarf=1 ], [ found_libdwarf=0 ])

    if test $found_libdwarf -eq 1; then
        AC_MSG_RESULT(yes)
        AM_CONDITIONAL(HAVE_LIBDWARF, true)
        AC_DEFINE(HAVE_LIBDWARF, 1, [Define to 1 if you have LIBDWARF.])
    else
# Try again with the traditional path instead
         found_libdwarf=0
         LIBDWARF_CPPFLAGS="-I$libdwarf_dir/include"
         LIBDWARF_LDFLAGS="-L$libdwarf_dir/$abi_libdir"

         CPPFLAGS="$CPPFLAGS $LIBDWARF_CPPFLAGS"
         LDFLAGS="$LDFLAGS $LIBDWARF_LDFLAGS $LIBDWARF_LIBS -lelf -lpthread"

         AC_MSG_CHECKING([for libdwarf library and headers])

         AC_LINK_IFELSE(AC_LANG_PROGRAM([[
             #include <dwarf.h>
             ]], [[
             if (DW_ID_up_case != DW_ID_down_case) {
                int mycase = DW_ID_up_case;
             }
             ]]), [ found_libdwarf=1

             ], [ found_libdwarf=0 ])

         if test $found_libdwarf -eq 1; then
             AC_MSG_RESULT(yes)
             AM_CONDITIONAL(HAVE_LIBDWARF, true)
             AC_DEFINE(HAVE_LIBDWARF, 1, [Define to 1 if you have LIBDWARF.])
         else
             AC_MSG_RESULT(no)
             AM_CONDITIONAL(HAVE_LIBDWARF, false)
             AC_DEFINE(HAVE_LIBDWARF, 0, [Define to 0 if you do not have LIBDWARF.])
             LIBDWARF_CPPFLAGS=""
             LIBDWARF_LDFLAGS=""
             LIBDWARF_LIBS=""
         fi
    fi

    CPPFLAGS=$libdwarf_saved_CPPFLAGS LDFLAGS=$libdwarf_saved_LDFLAGS

    AC_SUBST(LIBDWARF_CPPFLAGS)
    AC_SUBST(LIBDWARF_LDFLAGS)
    AC_SUBST(LIBDWARF_LIBS)

])

