

################################################################################
# Check for LAMPI (http://public.lanl.gov/lampi)
################################################################################

AC_DEFUN([AC_PKG_LAMPI], [

    AC_ARG_WITH(lampi,
		AC_HELP_STRING([--with-lampi=DIR],
			       [LAMPI installation @<:@/usr/local@:>@]),
		lampi_dir=$withval, lampi_dir="/usr/local")

    AC_MSG_CHECKING([for LAMPI library and headers])

    found_lampi=0

    LAMPI_CC="$lampi_dir/bin/mpicc"
    LAMPI_CPPFLAGS="-I$lampi_dir/include"
    LAMPI_LDFLAGS="-L$lampi_dir/$abi_libdir"
    LAMPI_LIBS="-lmpi"
    LAMPI_HEADER="$lampi_dir/include/mpi.h"
    LAMPI_DIR="$lampi_dir"
         
# LAMPI has a level of indirection.  The mpi collector
# searches the header file for MPI functions
    if test -f $lampi_dir/include/mpi/mpi.h; then
      LAMPI_HEADER="$lampi_dir/include/mpi/mpi.h"
    fi 

    lampi_saved_CC=$CC
    lampi_saved_CPPFLAGS=$CPPFLAGS
    lampi_saved_LDFLAGS=$LDFLAGS

    CC="$LAMPI_CC"
    CPPFLAGS="$CPPFLAGS $LAMPI_CPPFLAGS"
    LDFLAGS="$LDFLAGS $LAMPI_LDFLAGS $LAMPI_LIBS"

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
	#include <mpi.h>
	]], [[
	MPI_Initialized((int*)0);
	]]),

	if nm $lampi_dir/$abi_libdir/libmpi.so \
		| cut -d' ' -f3 | grep "^lampi_init" >/dev/null; then
	    found_lampi=1
	fi
 
	, )

    CC=$lampi_saved_CC
    CPPFLAGS=$lampi_saved_CPPFLAGS
    LDFLAGS=$lampi_saved_LDFLAGS

    if test $found_lampi -eq 1; then
	AC_MSG_RESULT(yes)
	AM_CONDITIONAL(HAVE_LAMPI, true)
	AC_DEFINE(HAVE_LAMPI, 1, [Define to 1 if you have LAMPI.])	
    else
# Try again with $alt_abi_libdir instead
         found_lampi=0

         LAMPI_CC="$lampi_dir/bin/mpicc"
         LAMPI_CPPFLAGS="-I$lampi_dir/include"
         LAMPI_LDFLAGS="-L$lampi_dir/$alt_abi_libdir"
         LAMPI_LIBS="-lmpi"
         LAMPI_HEADER="$lampi_dir/include/mpi.h"
         LAMPI_DIR="$lampi_dir"
         
# LAMPI has a level of indirection.  The mpi collector
# searches the header file for MPI functions
         if test -f $lampi_dir/include/mpi/mpi.h; then
             LAMPI_HEADER="$lampi_dir/include/mpi/mpi.h"
         fi 

         lampi_saved_CC=$CC
         lampi_saved_CPPFLAGS=$CPPFLAGS
         lampi_saved_LDFLAGS=$LDFLAGS

         CC="$LAMPI_CC"
         CPPFLAGS="$CPPFLAGS $LAMPI_CPPFLAGS"
         LDFLAGS="$LDFLAGS $LAMPI_LDFLAGS $LAMPI_LIBS"

         AC_LINK_IFELSE(AC_LANG_PROGRAM([[
             #include <mpi.h>
             ]], [[
             MPI_Initialized((int*)0);
             ]]),

             if nm $lampi_dir/$alt_abi_libdir/libmpi.so \
                | cut -d' ' -f3 | grep "^lampi_init" >/dev/null; then
                 found_lampi=1
             fi


             , )

         CC=$lampi_saved_CC
         CPPFLAGS=$lampi_saved_CPPFLAGS
         LDFLAGS=$lampi_saved_LDFLAGS

         if test $found_lampi -eq 1; then
             AC_MSG_RESULT(yes)
             AM_CONDITIONAL(HAVE_LAMPI, true)
             AC_DEFINE(HAVE_LAMPI, 1, [Define to 1 if you have LAMPI.])
         else
          AC_MSG_RESULT(no)
          AM_CONDITIONAL(HAVE_LAMPI, false)
          LAMPI_CC=""
          LAMPI_CPPFLAGS=""
          LAMPI_LDFLAGS=""
          LAMPI_LIBS=""
          LAMPI_HEADER=""
          LAMPI_DIR=""
        fi
    fi

    AC_SUBST(LAMPI_CC)
    AC_SUBST(LAMPI_CPPFLAGS)
    AC_SUBST(LAMPI_LDFLAGS)
    AC_SUBST(LAMPI_LIBS)
    AC_SUBST(LAMPI_HEADER)
    AC_SUBST(LAMPI_DIR)

])


################################################################################
# Check for MPICH (http://www-unix.mcs.anl.gov/mpi/mpich1)
################################################################################

AC_DEFUN([AC_PKG_MPICH], [

    AC_ARG_WITH(mpich,
		AC_HELP_STRING([--with-mpich=DIR],
			       [MPICH installation @<:@/opt/mpich@:>@]),
		mpich_dir=$withval, mpich_dir="/opt/mpich")

    AC_ARG_WITH(mpich-driver,
		AC_HELP_STRING([--with-mpich-driver=NAME],
			       [MPICH driver name @<:@ch-p4@:>@]),
		mpich_driver=$withval, mpich_driver="ch-p4")

    AC_MSG_CHECKING([for MPICH library and headers])

    found_mpich=0

    # Put -shlib into MPICH_CC, since it is needed when building the
    # tests, where $MPICH_CC is used, and is not needed when building
    # the MPI-related plugins, where $MPICH_CC is not used.
    MPICH_CC="$mpich_dir/$mpich_driver/bin/mpicc -shlib"
    MPICH_CPPFLAGS="-I$mpich_dir/include"
    MPICH_LDFLAGS="-L$mpich_dir/$mpich_driver/$abi_libdir/shared"
    MPICH_LIBS="-lmpich"
    MPICH_HEADER="$mpich_dir/include/mpi.h"
    MPICH_DIR="$mpich_dir"

    # On the systems "mcr" and "thunder" at LLNL they have an MPICH variant
    # that has things moved around a bit. Handle this by allowing a "llnl"
    # pseudo-driver that makes the necessary configuration changes.
    if test x"$mpich_driver" == x"llnl"; then
	MPICH_CC="$mpich_dir/bin/mpicc -shlib"
        MPICH_LDFLAGS="-L$mpich_dir/$abi_libdir"
    fi

    mpich_saved_CC=$CC
    mpich_saved_CPPFLAGS=$CPPFLAGS
    mpich_saved_LDFLAGS=$LDFLAGS

    CC="$MPICH_CC"
    CPPFLAGS="$CPPFLAGS $MPICH_CPPFLAGS"
    LDFLAGS="$LDFLAGS $MPICH_LDFLAGS $MPICH_LIBS"

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
	#include <mpi.h>
	]], [[
	MPI_Initialized((int*)0);
	]]),

	found_mpich=1

	, )

    CC=$mpich_saved_CC
    CPPFLAGS=$mpich_saved_CPPFLAGS
    LDFLAGS=$mpich_saved_LDFLAGS

    if test $found_mpich -eq 1; then
	AC_MSG_RESULT(yes)
	AM_CONDITIONAL(HAVE_MPICH, true)
	AC_DEFINE(HAVE_MPICH, 1, [Define to 1 if you have MPICH.])	
    else
	AC_MSG_RESULT(no)
	AM_CONDITIONAL(HAVE_MPICH, false)
	MPICH_CC=""
	MPICH_CPPFLAGS=""
	MPICH_LDFLAGS=""
	MPICH_LIBS=""
	MPICH_HEADER=""
	MPICH_DIR=""
    fi

    AC_SUBST(MPICH_CC)
    AC_SUBST(MPICH_CPPFLAGS)
    AC_SUBST(MPICH_LDFLAGS)
    AC_SUBST(MPICH_LIBS)
    AC_SUBST(MPICH_HEADER)
    AC_SUBST(MPICH_DIR)

])


################################################################################
# Check for MPICH2 (http://www-unix.mcs.anl.gov/mpi/mpich2)
################################################################################

AC_DEFUN([AC_PKG_MPICH2], [

    AC_ARG_WITH(mpich2,
		AC_HELP_STRING([--with-mpich2=DIR],
			       [MPICH2 installation @<:@/usr/local@:>@]),
		mpich2_dir=$withval, mpich2_dir="/usr/local")

#    AC_ARG_WITH(mpich2-driver,
#		AC_HELP_STRING([--with-mpich2-driver=NAME],
#			       [MPICH2 driver name @<:@ch-p4@:>@]),
#		mpich2_driver=$withval, mpich2_driver="")

    AC_MSG_CHECKING([for MPICH2 library and headers])

    found_mpich2=0

    # Put -shlib into MPICH2_CC, since it is needed when building the
    # tests, where $MPICH2_CC is used, and is not needed when building
    # the MPI-related plugins, where $MPICH2_CC is not used.
    MPICH2_CC="$mpich2_dir/bin/mpicc -shlib"
    MPICH2_CPPFLAGS="-I$mpich2_dir/include"
    MPICH2_LDFLAGS="-L$mpich2_dir/$abi_libdir"
    MPICH2_LIBS="-lmpich"
    MPICH2_HEADER="$mpich2_dir/include/mpi.h"
    MPICH2_DIR="$mpich2_dir"

    # On the systems "mcr" and "thunder" at LLNL they have an MPICH variant
    # that has things moved around a bit. Handle this by allowing a "llnl"
    # pseudo-driver that makes the necessary configuration changes.
    if test x"$mpich2_driver" == x"llnl"; then
	MPICH2_CC="$mpich2_dir/bin/mpicc -shlib"
        MPICH2_LDFLAGS="-L$mpich2_dir/$abi_libdir"
    fi

    mpich2_saved_CC=$CC
    mpich2_saved_CPPFLAGS=$CPPFLAGS
    mpich2_saved_LDFLAGS=$LDFLAGS

    CC="$MPICH2_CC"
    CPPFLAGS="$CPPFLAGS $MPICH2_CPPFLAGS"
    LDFLAGS="$LDFLAGS $MPICH2_LDFLAGS $MPICH_LIBS"

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
	#include <mpi.h>
	]], [[
	MPI_Initialized((int*)0);
	]]),

	found_mpich2=1

	, )

    CC=$mpich2_saved_CC
    CPPFLAGS=$mpich2_saved_CPPFLAGS
    LDFLAGS=$mpich2_saved_LDFLAGS

    if test $found_mpich2 -eq 1; then
	AC_MSG_RESULT(yes)
	AM_CONDITIONAL(HAVE_MPICH2, true)
	AC_DEFINE(HAVE_MPICH2, 1, [Define to 1 if you have MPICH.])	
    else
	AC_MSG_RESULT(no)
	AM_CONDITIONAL(HAVE_MPICH2, false)
	MPICH2_CC=""
	MPICH2_CPPFLAGS=""
	MPICH2_LDFLAGS=""
	MPICH2_LIBS=""
	MPICH2_HEADER=""
	MPICH2_DIR=""
    fi

    AC_SUBST(MPICH2_CC)
    AC_SUBST(MPICH2_CPPFLAGS)
    AC_SUBST(MPICH2_LDFLAGS)
    AC_SUBST(MPICH2_LIBS)
    AC_SUBST(MPICH2_HEADER)
    AC_SUBST(MPICH2_DIR)

])


################################################################################
# Check for MPT (http://www.sgi.com/products/software/mpt)
################################################################################

AC_DEFUN([AC_PKG_MPT], [

    AC_ARG_WITH(mpt,
		AC_HELP_STRING([--with-mpt=DIR],
			       [MPT installation @<:@/usr@:>@]),
		mpt_dir=$withval, mpt_dir="/usr")

    AC_MSG_CHECKING([for MPT library and headers])

    found_mpt=0

    MPT_CC="$CC"
    MPT_CPPFLAGS="-I$mpt_dir/include"
    MPT_LDFLAGS="-L$mpt_dir/$abi_libdir"
    MPT_LIBS="-lmpi"
    MPT_HEADER="$mpt_dir/include/mpi.h"
    MPT_DIR="$mpt_dir"

    mpt_saved_CC=$CC
    mpt_saved_CPPFLAGS=$CPPFLAGS
    mpt_saved_LDFLAGS=$LDFLAGS

    CC="$MPT_CC"
    CPPFLAGS="$CPPFLAGS $MPT_CPPFLAGS"
    LDFLAGS="$LDFLAGS $MPT_LDFLAGS $MPT_LIBS"

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
	#include <mpi.h>
	]], [[
	MPI_Initialized((int*)0);
	]]),

	if nm $mpt_dir/$abi_libdir/libmpi.so \
		| grep MPI_debug_rank >/dev/null; then
	    found_mpt=1
	fi

	, )

    CC=$mpt_saved_CC
    CPPFLAGS=$mpt_saved_CPPFLAGS
    LDFLAGS=$mpt_saved_LDFLAGS

    if test $found_mpt -eq 1; then
	AC_MSG_RESULT(yes)
	AM_CONDITIONAL(HAVE_MPT, true)
	AC_DEFINE(HAVE_MPT, 1, [Define to 1 if you have MPT.])	
    else
	AC_MSG_RESULT(no)
	AM_CONDITIONAL(HAVE_MPT, false)
	MPT_CC=""
	MPT_CPPFLAGS=""
	MPT_LDFLAGS=""
	MPT_LIBS=""
	MPT_HEADER=""
	MPT_DIR=""
    fi

    AC_SUBST(MPT_CC)
    AC_SUBST(MPT_CPPFLAGS)
    AC_SUBST(MPT_LDFLAGS)
    AC_SUBST(MPT_LIBS)
    AC_SUBST(MPT_HEADER)
    AC_SUBST(MPT_DIR)

])


################################################################################
# Check for OpenMPI (http://www.open-mpi.org)
################################################################################

AC_DEFUN([AC_PKG_OPENMPI], [

    AC_ARG_WITH(openmpi,
		AC_HELP_STRING([--with-openmpi=DIR],
			       [OpenMPI installation @<:@/usr/local@:>@]),
		openmpi_dir=$withval, openmpi_dir="/usr/local")

    AC_MSG_CHECKING([for OpenMPI library and headers])

    found_openmpi=0

    OPENMPI_CC="$openmpi_dir/bin/mpicc"
    OPENMPI_CPPFLAGS="-I$openmpi_dir/include"
    OPENMPI_LDFLAGS="-L$openmpi_dir/$abi_libdir"
    OPENMPI_LIBS="-lmpi"
    OPENMPI_HEADER="$openmpi_dir/include/mpi.h"
    OPENMPI_DIR="$openmpi_dir"

    openmpi_saved_CC=$CC
    openmpi_saved_CPPFLAGS=$CPPFLAGS
    openmpi_saved_LDFLAGS=$LDFLAGS

    CC="$OPENMPI_CC"
    CPPFLAGS="$CPPFLAGS $OPENMPI_CPPFLAGS"
    LDFLAGS="$LDFLAGS $OPENMPI_LDFLAGS $OPENMPI_LIBS"

    AC_LINK_IFELSE(AC_LANG_PROGRAM([[
	#include <mpi.h>
	]], [[
	MPI_Initialized((int*)0);
	]]),

	if nm $openmpi_dir/$abi_libdir/libmpi.so \
		| cut -d' ' -f3 | grep "^ompi_mpi" >/dev/null; then
	    found_openmpi=1
	fi

	, )

    CC=$openmpi_saved_CC
    CPPFLAGS=$openmpi_saved_CPPFLAGS
    LDFLAGS=$openmpi_saved_LDFLAGS

    if test $found_openmpi -eq 1; then
	AC_MSG_RESULT(yes)
	AM_CONDITIONAL(HAVE_OPENMPI, true)
	AC_DEFINE(HAVE_OPENMPI, 1, [Define to 1 if you have OpenMPI.])	
    else
	AC_MSG_RESULT(no)
	AM_CONDITIONAL(HAVE_OPENMPI, false)
	OPENMPI_CC=""
	OPENMPI_CPPFLAGS=""
	OPENMPI_LDFLAGS=""
	OPENMPI_LIBS=""
	OPENMPI_HEADER=""
	OPENMPI_DIR=""
    fi

    AC_SUBST(OPENMPI_CC)
    AC_SUBST(OPENMPI_CPPFLAGS)
    AC_SUBST(OPENMPI_LDFLAGS)
    AC_SUBST(OPENMPI_LIBS)
    AC_SUBST(OPENMPI_HEADER)
    AC_SUBST(OPENMPI_DIR)

])


################################################################################
# Check for MPI (http://www.mpi-forum.org/)
################################################################################

AC_DEFUN([AC_PKG_MPI], [

    AC_PKG_LAMPI()
    AC_PKG_MPICH()
    AC_PKG_MPICH2()
    AC_PKG_MPT()
    AC_PKG_OPENMPI()

    default_mpi=""
    all_mpi_names=""

    # Test for MPI implementations in reverse order of priority for
    # the default choice (ordering compatible with historical
    # AC_PKG_MPI behavior), so we can simultaneously gather all of
    # them and easily set $default_mpi* correctly.

    if test x"$OPENMPI_LIBS" != x""; then
	default_mpi=OPENMPI;  default_mpi_name=openmpi
	all_mpi_names=" openmpi $all_mpi_names"
    fi
    if test x"$LAMPI_LIBS" != x""; then
	default_mpi=LAMPI;    default_mpi_name=lampi
	all_mpi_names=" lampi $all_mpi_names"
    fi
    if test x"$MPICH2_LIBS" != x""; then
	default_mpi=MPICH2;    default_mpi_name=mpich2
	all_mpi_names=" mpich2 $all_mpi_names"
    fi
    if test x"$MPICH_LIBS" != x""; then
	default_mpi=MPICH;    default_mpi_name=mpich
	all_mpi_names=" mpich $all_mpi_names"
    fi
    if test x"$MPT_LIBS" != x""; then
	default_mpi=MPT;      default_mpi_name=mpt
	all_mpi_names=" mpt $all_mpi_names"
    fi

    if test x"$default_mpi" != x""; then
	AM_CONDITIONAL(HAVE_MPI, true)
	AC_DEFINE(HAVE_MPI, 1, [Define to 1 if you have MPI.])	
	AC_SUBST(DEFAULT_MPI_IMPL, $default_mpi)
	AC_DEFINE_UNQUOTED(ALL_MPI_IMPL_NAMES, "$all_mpi_names",
			   [Names of all MPI Implementations])
	AC_DEFINE_UNQUOTED(DEFAULT_MPI_IMPL_NAME, "$default_mpi_name",
			   [Name of default MPI Implementation])
    else
	AM_CONDITIONAL(HAVE_MPI, false)
    fi

])


