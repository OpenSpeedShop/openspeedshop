################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2007 William Hachfeld. All Rights Reserved.
# Copyright (c) 2006,2007,2008,2009 The Krell Institute. All Rights Reserved.
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 2.1 of the License, or (at your option)
# any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
################################################################################

# Note: See note in "libopenss-runtime/Makefile.am" for why "-O0" is used here.

CFLAGS = -g -O0
LDFLAGS =

BUILT_SOURCES = \
	blobs.h blobs.c

#noinst_PROGRAMS = mkwrapper

#mkwrapper_SOURCES = \
#	mkwrapper.cxx

pkglib_LTLIBRARIES =

if BUILD_INSTRUMENTOR
if !BUILD_CBTF
if HAVE_LIBMONITOR
OFFLINE_SOURCES = \
	offline.c
endif

if HAVE_TARGET_LIBMONITOR
OFFLINE_SOURCES = \
	offline.c
endif
endif
endif

if !BUILD_RUNTIME_ONLY
if !HAVE_TARGET_LIBMONITOR
pkglib_LTLIBRARIES += mpi.la
endif
endif

if BUILD_INSTRUMENTOR
if !BUILD_CBTF

if HAVE_LAM
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpi-rt-lam.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpi-lam-rt-offline.la libmpi-lam-offline.la
endif
if HAVE_TARGET_LIBMONITOR
if HAVE_TARGET_SHARED
pkglib_LTLIBRARIES += mpi-lam-rt-offline.la
endif
pkglib_LTLIBRARIES += libmpi-lam-offline.la
endif
endif

if HAVE_LAMPI
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpi-rt-lampi.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpi-lampi-rt-offline.la libmpi-lampi-offline.la
endif
if HAVE_TARGET_LIBMONITOR
if HAVE_TARGET_SHARED
pkglib_LTLIBRARIES += mpi-lampi-rt-offline.la
endif
pkglib_LTLIBRARIES += libmpi-lampi-offline.la
endif
endif

if HAVE_MPICH
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpi-rt-mpich.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpi-mpich-rt-offline.la libmpi-mpich-offline.la
endif
endif

#if HAVE_TARGET_MPICH
#if HAVE_TARGET_LIBMONITOR
#if HAVE_TARGET_SHARED
#pkglib_LTLIBRARIES += mpi-mpich-rt-offline.la
#endif
#pkglib_LTLIBRARIES += libmpi-mpich-offline.la
#endif
#endif

if HAVE_MPICH2
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpi-rt-mpich2.la
endif

if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpi-mpich2-rt-offline.la libmpi-mpich2-offline.la
endif
endif

if HAVE_TARGET_MPICH2
if HAVE_TARGET_LIBMONITOR
if HAVE_TARGET_SHARED
pkglib_LTLIBRARIES += mpi-mpich2-rt-offline.la
endif
pkglib_LTLIBRARIES += libmpi-mpich2-offline.la
endif
endif


if HAVE_MPT
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpi-rt-mpt.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpi-mpt-rt-offline.la libmpi-mpt-offline.la
endif
endif

#if HAVE_TARGET_MPT
#if HAVE_TARGET_LIBMONITOR
#if HAVE_TARGET_SHARED
#pkglib_LTLIBRARIES += mpi-mpt-rt-offline.la
#endif
#pkglib_LTLIBRARIES += libmpi-mpt-offline.la
#endif
#endif

if HAVE_OPENMPI
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpi-rt-openmpi.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpi-openmpi-rt-offline.la libmpi-openmpi-offline.la
endif
endif

#if HAVE_TARGET_OPENMPI
#if HAVE_TARGET_LIBMONITOR
#if HAVE_TARGET_SHARED
#pkglib_LTLIBRARIES += mpi-openmpi-rt-offline.la
#endif
#pkglib_LTLIBRARIES += libmpi-openmpi-offline.la
#endif
#endif

if HAVE_MVAPICH
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpi-rt-mvapich.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpi-mvapich-rt-offline.la libmpi-mvapich-offline.la
endif
endif

#if HAVE_TARGET_MVAPICH
#if HAVE_TARGET_LIBMONITOR
#if HAVE_TARGET_SHARED
#pkglib_LTLIBRARIES += mpi-mvapich-rt-offline.la
#endif
#pkglib_LTLIBRARIES += libmpi-mvapich-offline.la
#endif
#endif

if HAVE_MVAPICH2
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpi-rt-mvapich2.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpi-mvapich2-rt-offline.la libmpi-mvapich2-offline.la
endif
endif

#if HAVE_TARGET_MVAPICH2
#if HAVE_TARGET_LIBMONITOR
#if HAVE_TARGET_SHARED
#pkglib_LTLIBRARIES += mpi-mvapich2-rt-offline.la
#endif
#pkglib_LTLIBRARIES += libmpi-mvapich2-offline.la
#endif
#endif

endif
endif

if !BUILD_RUNTIME_ONLY
mpi_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-framework

mpi_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-framework \
	-no-undefined -module -avoid-version

mpi_la_LIBADD = \
	-lopenss-framework

mpi_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	MPITraceableFunctions.h \
	MPICollector.hxx MPICollector.cxx \
	MPIDetail.hxx
endif

if !BUILD_CBTF
# Common definitions for all MPI implementations
#
# We have to build a separate version of the plugin's runtime library
# for each MPI implementation on the system.  Various compilation
# flags for the different implementations are provided by the
# configuration process, in the autoconf variables *_CPPFLAGS,
# *_LDFLAGS, and *_LIBS, where * comes from the following set:
#
#     LAMPI, MPICH, MPT, OPENMPI
#
# The *_generate definitions below are GNU make functions which are
# used to produce values for the automake library variables for each
# plugin runtime library, given the implementation-specific flags as
# arguments.  However, these functions are not invoked until make
# time, so their final results are not seen by automake.  Because of
# this, the "-module" flag, despite being common to all the libraries,
# is moved out into each invocation of rt_LDFLAGS_generate, so that
# automake will see it and not issue warnings about the libraries'
# names not beginning with "lib" (GNU automake, 7.3.6).  Also, the
# definitions below for each specific implementation give empty
# *_DEPENDENCIES definitions, since automake will not see the final
# values of *_LIBADD and automatically set *_DEPENDENCIES correctly
# (GNU automake, 7.4, entry for _DEPENDENCIES).
#
# The generation of the wrapper sources is based on reading mpi.h in
# its entirety, and therefore it is possible for the results to be
# different if mpi.h differs among the implementations.  In practice,
# the only wrapper source differences are in the parameter lists for
# the MPI_Test{all,any,some} functions: parameter types of
# MPI_Request* v.  MPI_Request[], for example.  These are equivalent
# as parameter types, and the compilers do not currently complain, so
# the same wrapper sources can be used for all implementations.

if BUILD_MRNET
rt_CFLAGS_generate = \
	-I$(top_srcdir)/libopenss-runtime \
	$(1) \
	@LIBUNWIND_CPPFLAGS@
else

if !HAVE_TARGET_LIBMONITOR
rt_CFLAGS_generate = \
	-I$(top_srcdir)/libopenss-runtime \
	$(1) \
	@LIBUNWIND_CPPFLAGS@
endif

endif


if !HAVE_TARGET_LIBMONITOR
rt_LDFLAGS_generate = \
	-L$(top_srcdir)/libopenss-runtime \
	$(1) \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined -avoid-version

rt_LIBADD_generate = \
	-lopenss-runtime \
	-lopenss-runtime-unwind \
	$(1) \
	@LIBUNWIND_LIBS@ \
	-lrt -lpthread

rt_sources = \
	blobs.x $(BUILT_SOURCES) \
	runtime.h runtime.c \
	wrappers.c
endif

if HAVE_LIBMONITOR
rt_offline_CFLAGS_generate = \
	-DOPENSS_OFFLINE \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	$(1) \
	@LIBUNWIND_CPPFLAGS@

rt_offline_LDFLAGS_generate = \
	-L$(top_srcdir)/libopenss-runtime \
	-L$(top_srcdir)/libopenss-runtime/offline \
	$(1) \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined -avoid-version

rt_offline_LIBADD_generate = \
	-lopenss-runtime-offline -lopenss-runtime-monitor \
	-lopenss-runtime-unwind \
	$(1) \
	@LIBUNWIND_LIBS@ \
	-lrt -lpthread

rt_offline_sources = \
	blobs.x $(BUILT_SOURCES) $(OFFLINE_SOURCES) \
	wrappers.c \
	wrappers-fortran.c \
	runtime.h runtime.c

lib_offline_CFLAGS_generate = \
	-static \
	-DOPENSS_OFFLINE -DOPENSS_STATIC \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	$(1) \
	@LIBUNWIND_CPPFLAGS@

lib_offline_LDFLAGS_generate = \
	-static \
	-L$(top_srcdir)/libopenss-runtime \
	-L$(top_srcdir)/libopenss-runtime/offline \
	$(1) \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined

lib_offline_LIBADD_generate = \
	-lopenss-runtime-offline \
	-lopenss-runtime-monitor \
	-lopenss-runtime-unwind \
	$(1) \
	@LIBUNWIND_LIBS@ \
	-lrt -lpthread

lib_offline_sources = \
	blobs.x $(BUILT_SOURCES) $(OFFLINE_SOURCES) \
	wrappers.c \
	wrappers-fortran.c \
	runtime.h runtime.c

endif

if HAVE_TARGET_LIBMONITOR

if HAVE_TARGET_SHARED
rt_offline_CFLAGS_generate = \
	-DOPENSS_OFFLINE \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	$(1) \
	@TARGET_LIBUNWIND_CPPFLAGS@

rt_offline_LDFLAGS_generate = \
	-L$(top_srcdir)/libopenss-runtime \
	-L$(top_srcdir)/libopenss-runtime/offline \
	$(1) \
	@TARGET_LIBUNWIND_LDFLAGS@ \
	-no-undefined -avoid-version

rt_offline_LIBADD_generate = \
	-lopenss-runtime-offline -lopenss-runtime-monitor \
	-lopenss-runtime-unwind \
	$(1) \
	@TARGET_LIBUNWIND_LIBS@ \
	-lrt -lpthread

rt_offline_sources = \
	blobs.x $(BUILT_SOURCES) $(OFFLINE_SOURCES) \
	wrappers.c \
	wrappers-fortran.c \
	runtime.h runtime.c
endif

lib_offline_CFLAGS_generate = \
	-static \
	-DOPENSS_OFFLINE -DOPENSS_STATIC \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	$(1) \
	@TARGET_LIBUNWIND_CPPFLAGS@

lib_offline_LDFLAGS_generate = \
	-static \
	-L$(top_srcdir)/libopenss-runtime \
	-L$(top_srcdir)/libopenss-runtime/offline \
	$(1) \
	@TARGET_LIBUNWIND_LDFLAGS@ \
	-no-undefined

lib_offline_LIBADD_generate = \
	-lopenss-runtime-offline \
	-lopenss-runtime-monitor \
	-lopenss-runtime-unwind \
	$(1) \
	@TARGET_LIBUNWIND_LIBS@ \
	-lrt -lpthread

lib_offline_sources = \
	blobs.x $(BUILT_SOURCES) $(OFFLINE_SOURCES) \
	wrappers.c \
	wrappers-fortran.c \
	runtime.h runtime.c
endif

wrapper_sources = \
	$(NULL)


if HAVE_LAM
mpi_rt_lam_la_CFLAGS = $(call rt_CFLAGS_generate,@LAM_CPPFLAGS@)
mpi_rt_lam_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@LAM_LDFLAGS@)
mpi_rt_lam_la_LIBADD = $(call rt_LIBADD_generate,@LAM_LIBS@)
mpi_rt_lam_la_DEPENDENCIES =
mpi_rt_lam_la_SOURCES = $(rt_sources)
nodist_mpi_rt_lam_la_SOURCES = $(wrapper_sources)

if HAVE_LIBMONITOR
mpi_lam_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@LAM_CPPFLAGS@)
mpi_lam_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@LAM_LDFLAGS@)
mpi_lam_rt_offline_la_LIBADD = $(call rt_offline_LIBADD_generate,@LAM_LIBS@)
mpi_lam_rt_offline_la_DEPENDENCIES =
mpi_lam_rt_offline_la_SOURCES = $(rt_offline_sources)
nodist_mpi_lam_rt_offline_la_SOURCES = $(wrapper_sources)

libmpi_lam_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@LAM_CPPFLAGS@)
libmpi_lam_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@LAM_LDFLAGS@)
libmpi_lam_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@LAM_LIBS@)
libmpi_lam_offline_la_DEPENDENCIES =
libmpi_lam_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpi_lam_offline_la_SOURCES = $(wrapper_sources)
endif

endif



if HAVE_LAMPI
mpi_rt_lampi_la_CFLAGS = $(call rt_CFLAGS_generate,@LAMPI_CPPFLAGS@)
mpi_rt_lampi_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@LAMPI_LDFLAGS@)
mpi_rt_lampi_la_LIBADD = $(call rt_LIBADD_generate,@LAMPI_LIBS@)
mpi_rt_lampi_la_DEPENDENCIES =
mpi_rt_lampi_la_SOURCES = $(rt_sources)
nodist_mpi_rt_lampi_la_SOURCES = $(wrapper_sources)

if HAVE_LIBMONITOR
mpi_lampi_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@LAMPI_CPPFLAGS@)
mpi_lampi_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@LAMPI_LDFLAGS@)
mpi_lampi_rt_offline_la_LIBADD = $(call rt_offline_LIBADD_generate,@LAMPI_LIBS@)
mpi_lampi_rt_offline_la_DEPENDENCIES =
mpi_lampi_rt_offline_la_SOURCES = $(rt_offline_sources)
nodist_mpi_lampi_rt_offline_la_SOURCES = $(wrapper_sources)

libmpi_lampi_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@LAMPI_CPPFLAGS@)
libmpi_lampi_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@LAMPI_LDFLAGS@)
libmpi_lampi_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@LAMPI_LIBS@)
libmpi_lampi_offline_la_DEPENDENCIES =
libmpi_lampi_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpi_lampi_offline_la_SOURCES = $(wrapper_sources)
endif

endif



if HAVE_MPICH
mpi_rt_mpich_la_CFLAGS = $(call rt_CFLAGS_generate,@MPICH_CPPFLAGS@)
mpi_rt_mpich_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MPICH_LDFLAGS@)
mpi_rt_mpich_la_LIBADD = $(call rt_LIBADD_generate,@MPICH_LIBS@)
mpi_rt_mpich_la_DEPENDENCIES =
mpi_rt_mpich_la_SOURCES = $(rt_sources)
nodist_mpi_rt_mpich_la_SOURCES = $(wrapper_sources)

if HAVE_LIBMONITOR
mpi_mpich_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@MPICH_CPPFLAGS@)
mpi_mpich_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@MPICH_LDFLAGS@)
mpi_mpich_rt_offline_la_LIBADD = $(call rt_offline_LIBADD_generate,@MPICH_LIBS@)
mpi_mpich_rt_offline_la_DEPENDENCIES =
mpi_mpich_rt_offline_la_SOURCES = $(rt_offline_sources)
nodist_mpi_mpich_rt_offline_la_SOURCES = $(wrapper_sources)

libmpi_mpich_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@MPICH_CPPFLAGS@)
libmpi_mpich_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MPICH_LDFLAGS@)
libmpi_mpich_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@MPICH_LIBS@)
libmpi_mpich_offline_la_DEPENDENCIES =
libmpi_mpich_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpi_mpich_offline_la_SOURCES = $(wrapper_sources)
endif

endif

#
# MPICH2 SETUP
#
if HAVE_MPICH2
mpi_rt_mpich2_la_CFLAGS = $(call rt_CFLAGS_generate,@MPICH2_CPPFLAGS@)
mpi_rt_mpich2_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MPICH2_LDFLAGS@)
mpi_rt_mpich2_la_LIBADD = $(call rt_LIBADD_generate,@MPICH2_LIBS@)
mpi_rt_mpich2_la_DEPENDENCIES =
mpi_rt_mpich2_la_SOURCES = $(rt_sources)
nodist_mpi_rt_mpich2_la_SOURCES = $(wrapper_sources)
 
if HAVE_LIBMONITOR
mpi_mpich2_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@MPICH2_CPPFLAGS@)
mpi_mpich2_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@MPICH2_LDFLAGS@)
mpi_mpich2_rt_offline_la_LIBADD = $(call rt_offline_LIBADD_generate,@MPICH2_LIBS@)
mpi_mpich2_rt_offline_la_DEPENDENCIES =
mpi_mpich2_rt_offline_la_SOURCES = $(rt_offline_sources)
nodist_mpi_mpich2_rt_offline_la_SOURCES = $(wrapper_sources)
 
libmpi_mpich2_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@MPICH2_CPPFLAGS@)
libmpi_mpich2_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MPICH2_LDFLAGS@)
libmpi_mpich2_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@MPICH2_LIBS@)
libmpi_mpich2_offline_la_DEPENDENCIES =
libmpi_mpich2_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpi_mpich2_offline_la_SOURCES = $(wrapper_sources)
endif
endif

#
# For targetted MPICH2
#

if HAVE_TARGET_MPICH2
if HAVE_TARGET_LIBMONITOR
libmpi_mpich2_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@TARGET_MPICH2_CPPFLAGS@)
libmpi_mpich2_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@TARGET_MPICH2_LDFLAGS@)
libmpi_mpich2_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@TARGET_MPICH2_LIBS@)
libmpi_mpich2_offline_la_DEPENDENCIES =
libmpi_mpich2_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpi_mpich2_offline_la_SOURCES = $(wrapper_sources)

if HAVE_TARGET_SHARED
mpi_mpich2_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@TARGET_MPICH2_CPPFLAGS@)
mpi_mpich2_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@TARGET_MPICH2_LDFLAGS@)
mpi_mpich2_rt_offline_la_LIBADD = $(call rt_offline_LIBADD_generate,@TARGET_MPICH2_LIBS@)
mpi_mpich2_rt_offline_la_DEPENDENCIES =
mpi_mpich2_rt_offline_la_SOURCES = $(rt_offline_sources)
nodist_mpi_mpich2_rt_offline_la_SOURCES = $(wrapper_sources)
endif

endif
endif


if HAVE_MPT
mpi_rt_mpt_la_CFLAGS = -DSGI_MPT $(call rt_CFLAGS_generate,@MPT_CPPFLAGS@)
mpi_rt_mpt_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MPT_LDFLAGS@)
mpi_rt_mpt_la_LIBADD = $(call rt_LIBADD_generate,@MPT_LIBS@)
mpi_rt_mpt_la_DEPENDENCIES =
mpi_rt_mpt_la_SOURCES = $(rt_sources)
nodist_mpi_rt_mpt_la_SOURCES = $(wrapper_sources)

if HAVE_LIBMONITOR
mpi_mpt_rt_offline_la_CFLAGS = -DSGI_MPT $(call rt_offline_CFLAGS_generate,@MPT_CPPFLAGS@)
mpi_mpt_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@MPT_LDFLAGS@)
mpi_mpt_rt_offline_la_LIBADD = $(call rt_offline_LIBADD_generate,@MPT_LIBS@)
mpi_mpt_rt_offline_la_DEPENDENCIES =
mpi_mpt_rt_offline_la_SOURCES = $(rt_offline_sources)
nodist_mpi_mpt_rt_offline_la_SOURCES = $(wrapper_sources)

libmpi_mpt_offline_la_CFLAGS = -static -DSGI_MPT $(call lib_offline_CFLAGS_generate,@MPT_CPPFLAGS@)
libmpi_mpt_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MPT_LDFLAGS@)
libmpi_mpt_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@MPT_LIBS@)
libmpi_mpt_offline_la_DEPENDENCIES =
libmpi_mpt_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpi_mpt_offline_la_SOURCES = $(wrapper_sources)
endif

endif

# Not supported yet - need to add --with-target-mpt support
#if HAVE_TARGET_MPT
#if HAVE_TARGET_LIBMONITOR
#libmpi_mpt_offline_la_CFLAGS = -DSGI_MPT -static $(call lib_offline_CFLAGS_generate,@MPT_CPPFLAGS@)
#libmpi_mpt_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MPT_LDFLAGS@)
#libmpi_mpt_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@MPT_LIBS@)
#libmpi_mpt_offline_la_DEPENDENCIES =
#libmpi_mpt_offline_la_SOURCES = $(lib_offline_sources)
#nodist_libmpi_mpt_offline_la_SOURCES = $(wrapper_sources)
#endif
#endif

if HAVE_MVAPICH
mpi_rt_mvapich_la_CFLAGS = $(call rt_CFLAGS_generate,@MVAPICH_CPPFLAGS@)
mpi_rt_mvapich_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MVAPICH_LDFLAGS@)
mpi_rt_mvapich_la_LIBADD = $(call rt_LIBADD_generate,@MVAPICH_LIBS@)
mpi_rt_mvapich_la_DEPENDENCIES =
mpi_rt_mvapich_la_SOURCES = $(rt_sources)
nodist_mpi_rt_mvapich_la_SOURCES = $(wrapper_sources)

if HAVE_LIBMONITOR
mpi_mvapich_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@MVAPICH_CPPFLAGS@)
mpi_mvapich_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@MVAPICH_LDFLAGS@)
mpi_mvapich_rt_offline_la_LIBADD = $(call rt_offline_LIBADD_generate,@MVAPICH_LIBS@)
mpi_mvapich_rt_offline_la_DEPENDENCIES =
mpi_mvapich_rt_offline_la_SOURCES = $(rt_offline_sources)
nodist_mpi_mvapich_rt_offline_la_SOURCES = $(wrapper_sources)

libmpi_mvapich_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@MVAPICH_CPPFLAGS@)
libmpi_mvapich_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MVAPICH_LDFLAGS@)
libmpi_mvapich_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@MVAPICH_LIBS@)
libmpi_mvapich_offline_la_DEPENDENCIES =
libmpi_mvapich_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpi_mvapich_offline_la_SOURCES = $(wrapper_sources)
endif
endif

# Not supported yet - need to add --with-target-mvapich support
#if HAVE_TARGERT_MVAPICH
#if HAVE_TARGET_LIBMONITOR
#libmpi_mvapich_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@MVAPICH_CPPFLAGS@)
#libmpi_mvapich_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MVAPICH_LDFLAGS@)
#libmpi_mvapich_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@MVAPICH_LIBS@)
#libmpi_mvapich_offline_la_DEPENDENCIES =
#libmpi_mvapich_offline_la_SOURCES = $(lib_offline_sources)
#nodist_libmpi_mvapich_offline_la_SOURCES = $(wrapper_sources)
#endif
#endif

if HAVE_MVAPICH2
mpi_rt_mvapich2_la_CFLAGS = $(call rt_CFLAGS_generate,@MVAPICH2_CPPFLAGS@)
mpi_rt_mvapich2_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MVAPICH2_LDFLAGS@)
mpi_rt_mvapich2_la_LIBADD = $(call rt_LIBADD_generate,@MVAPICH2_LIBS@)
mpi_rt_mvapich2_la_DEPENDENCIES =
mpi_rt_mvapich2_la_SOURCES = $(rt_sources)
nodist_mpi_rt_mvapich2_la_SOURCES = $(wrapper_sources)

if HAVE_LIBMONITOR
mpi_mvapich2_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@MVAPICH2_CPPFLAGS@)
mpi_mvapich2_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@MVAPICH2_LDFLAGS@)
mpi_mvapich2_rt_offline_la_LIBADD = $(call rt_offline_LIBADD_generate,@MVAPICH2_LIBS@)
mpi_mvapich2_rt_offline_la_DEPENDENCIES =
mpi_mvapich2_rt_offline_la_SOURCES = $(rt_offline_sources)
nodist_mpi_mvapich2_rt_offline_la_SOURCES = $(wrapper_sources)

libmpi_mvapich2_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@MVAPICH2_CPPFLAGS@)
libmpi_mvapich2_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MVAPICH2_LDFLAGS@)
libmpi_mvapich2_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@MVAPICH2_LIBS@)
libmpi_mvapich2_offline_la_DEPENDENCIES =
libmpi_mvapich2_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpi_mvapich2_offline_la_SOURCES = $(wrapper_sources)
endif
endif

# Not supported yet - need to add --with-target-mvapich2 support
#if HAVE_TARGET_MVAPICH2
#if HAVE_TARGET_LIBMONITOR
#libmpi_mvapich2_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@MVAPICH2_CPPFLAGS@)
#libmpi_mvapich2_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MVAPICH2_LDFLAGS@)
#libmpi_mvapich2_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@MVAPICH2_LIBS@)
#libmpi_mvapich2_offline_la_DEPENDENCIES =
#libmpi_mvapich2_offline_la_SOURCES = $(lib_offline_sources)
#nodist_libmpi_mvapich2_offline_la_SOURCES = $(wrapper_sources)
#endif
#endif


if HAVE_OPENMPI
mpi_rt_openmpi_la_CFLAGS = $(call rt_CFLAGS_generate,@OPENMPI_CPPFLAGS@)
mpi_rt_openmpi_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@OPENMPI_LDFLAGS@)
mpi_rt_openmpi_la_LIBADD = $(call rt_LIBADD_generate,@OPENMPI_LIBS@)
mpi_rt_openmpi_la_DEPENDENCIES =
mpi_rt_openmpi_la_SOURCES = $(rt_sources)
nodist_mpi_rt_openmpi_la_SOURCES = $(wrapper_sources)

if HAVE_LIBMONITOR
mpi_openmpi_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@OPENMPI_CPPFLAGS@)
mpi_openmpi_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@OPENMPI_LDFLAGS@)
mpi_openmpi_rt_offline_la_LIBADD = $(call rt_offline_LIBADD_generate,@OPENMPI_LIBS@)
mpi_openmpi_rt_offline_la_DEPENDENCIES =
mpi_openmpi_rt_offline_la_SOURCES = $(rt_offline_sources)
nodist_mpi_openmpi_rt_offline_la_SOURCES = $(wrapper_sources)

libmpi_openmpi_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@OPENMPI_CPPFLAGS@)
libmpi_openmpi_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@OPENMPI_LDFLAGS@)
libmpi_openmpi_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@OPENMPI_LIBS@)
libmpi_openmpi_offline_la_DEPENDENCIES =
libmpi_openmpi_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpi_openmpi_offline_la_SOURCES = $(wrapper_sources)
endif
endif

# Not supported yet - need to add --with-target-openmpi support
#if HAVE_TARGET_OPENMPI
#if HAVE_TARGET_LIBMONITOR
#libmpi_openmpi_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@OPENMPI_CPPFLAGS@)
#libmpi_openmpi_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@OPENMPI_LDFLAGS@)
#libmpi_openmpi_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@OPENMPI_LIBS@)
#libmpi_openmpi_offline_la_DEPENDENCIES =
#libmpi_openmpi_offline_la_SOURCES = $(lib_offline_sources)
#nodist_libmpi_openmpi_offline_la_SOURCES = $(wrapper_sources)
#endif
#endif

# if !BUILD_CBTF
endif

#EXTRA_DIST = mkwrapper.template
SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES) $(wrapper_sources)

blobs.h blobs.c : blobs.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<


#
# If we have targetted MPI then use those variables.  Can not build both from same configure.
#
if HAVE_TARGET_MPI
ANY_MPI_IMPL = @DEFAULT_TARGET_MPI_IMPL@
ANY_MPI_HEADER = $(TARGET_$(ANY_MPI_IMPL)_HEADER)
else
ANY_MPI_IMPL = @DEFAULT_MPI_IMPL@
ANY_MPI_HEADER = $($(ANY_MPI_IMPL)_HEADER)
endif

#$(wrapper_sources) : mkwrapper mkwrapper.template
#	./mkwrapper \
#		` echo $@ | sed -e 's/\.c//' ` \
#		$(ANY_MPI_HEADER) \
#		mkwrapper.template \
#		> $@


