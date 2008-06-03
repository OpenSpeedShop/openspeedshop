################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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

BUILT_SOURCES = \
	blobs.h blobs.c

noinst_PROGRAMS = mkwrapper

mkwrapper_SOURCES = \
	mkwrapper.cxx

pkglib_LTLIBRARIES =

if HAVE_LIBMONITOR
OFFLINE_SOURCES = \
	mpi_offline.c mpi_offline.h

endif


#if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpi.la

if HAVE_LAM
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpi-rt-lam.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpi-rt-offline-lam.la
endif
endif

if HAVE_LAMPI
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpi-rt-lampi.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpi-rt-offline-lampi.la
endif
endif

if HAVE_MPICH
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpi-rt-mpich.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpi-rt-offline-mpich.la
endif
endif

if HAVE_MPICH2
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpi-rt-mpich2.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpi-rt-offline-mpich2.la
endif
endif

if HAVE_MPT
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpi-rt-mpt.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpi-rt-offline-mpt.la
endif
endif

if HAVE_OPENMPI
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpi-rt-openmpi.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpi-rt-offline-openmpi.la libmpi-offline-openmpi.la
endif
endif

if HAVE_MVAPICH
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpi-rt-mvapich.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpi-rt-offline-mvapich.la libmpi-offline-mvapich.la
endif
endif

mpi_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-framework

mpi_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-framework \
	-no-undefined -module -avoid-version

mpi_la_LIBADD = \
	-lopenss-framework

mpi_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	MPICollector.hxx MPICollector.cxx \
	MPIDetail.hxx

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
	-DOPENSS_USE_FILEIO -I$(top_srcdir)/libopenss-runtime/fileio  \
	-I$(top_srcdir)/libopenss-runtime \
	$(1) \
	@LIBUNWIND_CPPFLAGS@
else
rt_CFLAGS_generate = \
	-I$(top_srcdir)/libopenss-runtime \
	$(1) \
	@LIBUNWIND_CPPFLAGS@
endif

rt_LDFLAGS_generate = \
	-L$(top_srcdir)/libopenss-runtime \
	$(1) \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined -avoid-version

rt_LIBADD_generate = \
	-lopenss-runtime \
	$(1) \
	@LIBUNWIND_LIBS@ \
	-lpthread -lrt

rt_sources = \
	blobs.x $(BUILT_SOURCES) \
	runtime.h runtime.c \
	wrappers.c

if HAVE_LIBMONITOR

rt_offline_CFLAGS_generate = \
	-DOPENSS_OFFLINE -DOPENSS_USE_FILEIO \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/fileio \
	-I$(top_srcdir)/libopenss-runtime/offline \
	$(1) \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@

rt_offline_LDFLAGS_generate = \
	-L$(top_srcdir)/libopenss-runtime \
	$(1) \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined -avoid-version

rt_offline_LIBADD_generate = \
	$(1) \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	$(top_srcdir)/libopenss-runtime/libopenss-runtime-offline.la \
	-lpthread -lrt

rt_offline_sources = \
	blobs.x $(BUILT_SOURCES) $(OFFLINE_SOURCES) \
	wrappers.c \
	runtime.h runtime.c

lib_offline_CFLAGS_generate = \
	-DOPENSS_OFFLINE -DOPENSS_USE_FILEIO \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/fileio \
	-I$(top_srcdir)/libopenss-runtime/offline \
	$(1) \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@

lib_offline_LDFLAGS_generate = \
	-static -L$(top_srcdir)/libopenss-runtime \
	$(1) \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined

lib_offline_LIBADD_generate = \
	-lopenss-runtime-offline \
	$(1) \
	@LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	-lpthread -lrt

lib_offline_sources = \
	blobs.x $(BUILT_SOURCES) $(OFFLINE_SOURCES) \
	wrappers.c \
	runtime.h runtime.c

endif

wrapper_sources = \
	MPI_Allgather.c \
	MPI_Allgatherv.c \
	MPI_Allreduce.c \
	MPI_Alltoall.c \
	MPI_Alltoallv.c \
	MPI_Barrier.c \
	MPI_Bcast.c \
	MPI_Bsend.c \
	MPI_Bsend_init.c \
	MPI_Cancel.c \
	MPI_Cart_create.c \
	MPI_Cart_sub.c \
	MPI_Comm_create.c \
	MPI_Comm_dup.c \
	MPI_Comm_free.c \
	MPI_Comm_split.c \
	MPI_Finalize.c \
	MPI_Gather.c \
	MPI_Gatherv.c \
	MPI_Get_count.c \
	MPI_Graph_create.c \
	MPI_Ibsend.c \
	MPI_Init.c \
	MPI_Intercomm_create.c \
	MPI_Intercomm_merge.c \
	MPI_Iprobe.c \
	MPI_Irecv.c \
	MPI_Irsend.c \
	MPI_Isend.c \
	MPI_Issend.c \
	MPI_Pack.c \
	MPI_Probe.c \
	MPI_Recv.c \
	MPI_Recv_init.c \
	MPI_Reduce.c \
	MPI_Reduce_scatter.c \
	MPI_Request_free.c \
	MPI_Rsend.c \
	MPI_Rsend_init.c \
	MPI_Scan.c \
	MPI_Scatter.c \
	MPI_Scatterv.c \
	MPI_Send.c \
	MPI_Send_init.c \
	MPI_Sendrecv.c \
	MPI_Sendrecv_replace.c \
	MPI_Ssend.c \
	MPI_Ssend_init.c \
	MPI_Start.c \
	MPI_Startall.c \
	MPI_Test.c \
	MPI_Testall.c \
	MPI_Testany.c \
	MPI_Testsome.c \
	MPI_Unpack.c \
	MPI_Wait.c \
	MPI_Waitall.c \
	MPI_Waitany.c \
	MPI_Waitsome.c \
	$(NULL)


if HAVE_LAM
mpi_rt_lam_la_CFLAGS = $(call rt_CFLAGS_generate,@LAM_CPPFLAGS@)
mpi_rt_lam_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@LAM_LDFLAGS@)
mpi_rt_lam_la_LIBADD = $(call rt_LIBADD_generate,@LAM_LIBS@)
mpi_rt_lam_la_DEPENDENCIES =
mpi_rt_lam_la_SOURCES = $(rt_sources)
nodist_mpi_rt_lam_la_SOURCES = $(wrapper_sources)

if HAVE_LIBMONITOR
mpi_rt_offline_lam_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@LAM_CPPFLAGS@)
mpi_rt_offline_lam_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@LAM_LDFLAGS@)
mpi_rt_offline_lam_la_LIBADD = $(call rt_offline_LIBADD_generate,@LAM_LIBS@)
mpi_rt_offline_lam_la_DEPENDENCIES =
mpi_rt_offline_lam_la_SOURCES = $(rt_offline_sources)
nodist_mpi_rt_offline_lam_la_SOURCES = $(wrapper_sources)
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
mpi_rt_offline_lampi_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@LAMPI_CPPFLAGS@)
mpi_rt_offline_lampi_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@LAMPI_LDFLAGS@)
mpi_rt_offline_lampi_la_LIBADD = $(call rt_offline_LIBADD_generate,@LAMPI_LIBS@)
mpi_rt_offline_lampi_la_DEPENDENCIES =
mpi_rt_offline_lampi_la_SOURCES = $(rt_offline_sources)
nodist_mpi_rt_offline_lampi_la_SOURCES = $(wrapper_sources)
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
mpi_rt_offline_mpich_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@MPICH_CPPFLAGS@)
mpi_rt_offline_mpich_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@MPICH_LDFLAGS@)
mpi_rt_offline_mpich_la_LIBADD = $(call rt_offline_LIBADD_generate,@MPICH_LIBS@)
mpi_rt_offline_mpich_la_DEPENDENCIES =
mpi_rt_offline_mpich_la_SOURCES = $(rt_offline_sources)
nodist_mpi_rt_offline_mpich_la_SOURCES = $(wrapper_sources)
endif
endif

if HAVE_MPICH2
mpi_rt_mpich2_la_CFLAGS = $(call rt_CFLAGS_generate,@MPICH2_CPPFLAGS@)
mpi_rt_mpich2_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MPICH2_LDFLAGS@)
mpi_rt_mpich2_la_LIBADD = $(call rt_LIBADD_generate,@MPICH2_LIBS@)
mpi_rt_mpich2_la_DEPENDENCIES =
mpi_rt_mpich2_la_SOURCES = $(rt_sources)
nodist_mpi_rt_mpich2_la_SOURCES = $(wrapper_sources)

if HAVE_LIBMONITOR
mpi_rt_offline_mpich2_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@MPICH2_CPPFLAGS@)
mpi_rt_offline_mpich2_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@MPICH2_LDFLAGS@)
mpi_rt_offline_mpich2_la_LIBADD = $(call rt_offline_LIBADD_generate,@MPICH2_LIBS@)
mpi_rt_offline_mpich2_la_DEPENDENCIES =
mpi_rt_offline_mpich2_la_SOURCES = $(rt_offline_sources)
nodist_mpi_rt_offline_mpich2_la_SOURCES = $(wrapper_sources)
endif
endif


if HAVE_MPT
mpi_rt_mpt_la_CFLAGS = $(call rt_CFLAGS_generate,@MPT_CPPFLAGS@)
mpi_rt_mpt_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MPT_LDFLAGS@)
mpi_rt_mpt_la_LIBADD = $(call rt_LIBADD_generate,@MPT_LIBS@)
mpi_rt_mpt_la_DEPENDENCIES =
mpi_rt_mpt_la_SOURCES = $(rt_sources)
nodist_mpi_rt_mpt_la_SOURCES = $(wrapper_sources)

if HAVE_LIBMONITOR
mpi_rt_offline_mpt_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@MPT_CPPFLAGS@)
mpi_rt_offline_mpt_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@MPT_LDFLAGS@)
mpi_rt_offline_mpt_la_LIBADD = $(call rt_offline_LIBADD_generate,@MPT_LIBS@)
mpi_rt_offline_mpt_la_DEPENDENCIES =
mpi_rt_offline_mpt_la_SOURCES = $(rt_offline_sources)
nodist_mpi_rt_offline_mpt_la_SOURCES = $(wrapper_sources)
endif
endif

if HAVE_MVAPICH
mpi_rt_mvapich_la_CFLAGS = $(call rt_CFLAGS_generate,@MVAPICH_CPPFLAGS@)
mpi_rt_mvapich_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MVAPICH_LDFLAGS@)
mpi_rt_mvapich_la_LIBADD = $(call rt_LIBADD_generate,@MVAPICH_LIBS@)
mpi_rt_mvapich_la_DEPENDENCIES =
mpi_rt_mvapich_la_SOURCES = $(rt_sources)
nodist_mpi_rt_mvapich_la_SOURCES = $(wrapper_sources)
endif


if HAVE_OPENMPI
mpi_rt_openmpi_la_CFLAGS = $(call rt_CFLAGS_generate,@OPENMPI_CPPFLAGS@)
mpi_rt_openmpi_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@OPENMPI_LDFLAGS@)
mpi_rt_openmpi_la_LIBADD = $(call rt_LIBADD_generate,@OPENMPI_LIBS@)
mpi_rt_openmpi_la_DEPENDENCIES =
mpi_rt_openmpi_la_SOURCES = $(rt_sources)
nodist_mpi_rt_openmpi_la_SOURCES = $(wrapper_sources)

if HAVE_LIBMONITOR
mpi_rt_offline_openmpi_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@OPENMPI_CPPFLAGS@)
mpi_rt_offline_openmpi_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@OPENMPI_LDFLAGS@)
mpi_rt_offline_openmpi_la_LIBADD = $(call rt_offline_LIBADD_generate,@OPENMPI_LIBS@)
mpi_rt_offline_openmpi_la_DEPENDENCIES =
mpi_rt_offline_openmpi_la_SOURCES = $(rt_offline_sources)
nodist_mpi_rt_offline_openmpi_la_SOURCES = $(wrapper_sources)

libmpi_offline_openmpi_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@OPENMPI_CPPFLAGS@)
libmpi_offline_openmpi_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@OPENMPI_LDFLAGS@)
libmpi_offline_openmpi_la_LIBADD = $(call lib_offline_LIBADD_generate,@OPENMPI_LIBS@)
libmpi_offline_openmpi_la_DEPENDENCIES =
libmpi_offline_openmpi_la_SOURCES = $(lib_offline_sources)
nodist_libmpi_offline_openmpi_la_SOURCES = $(wrapper_sources)
endif
endif

#endif

EXTRA_DIST = mkwrapper.template
SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES) $(wrapper_sources)

blobs.h blobs.c : blobs.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

ANY_MPI_IMPL = @DEFAULT_MPI_IMPL@
ANY_MPI_HEADER = $($(ANY_MPI_IMPL)_HEADER)

$(wrapper_sources) : mkwrapper mkwrapper.template
	./mkwrapper \
		` echo $@ | sed -e 's/\.c//' ` \
		$(ANY_MPI_HEADER) \
		mkwrapper.template \
		> $@
