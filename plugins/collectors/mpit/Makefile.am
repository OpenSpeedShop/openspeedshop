################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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

BUILT_SOURCES = \
	blobs.h blobs.c	

pkglib_LTLIBRARIES = mpit.la
if HAVE_LAMPI
pkglib_LTLIBRARIES += mpit-rt-lampi.la
endif
if HAVE_MPICH
pkglib_LTLIBRARIES += mpit-rt-mpich.la
endif
if HAVE_MPT
pkglib_LTLIBRARIES += mpit-rt-mpt.la
endif
if HAVE_OPENMPI
pkglib_LTLIBRARIES += mpit-rt-openmpi.la
endif

mpit_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-framework

mpit_la_LDFLAGS = \
	-no-undefined -module -avoid-version

mpit_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	MPITCollector.hxx MPITCollector.cxx \
	MPITDetail.hxx


# Common definitions for all MPI implementations
#
# See comments in plugins/collectors/mpi/Makefile.am.  Remarks there
# concerning the generated wrapper sources also apply to wrappers.c
# here.

rt_CFLAGS_generate = \
	-I$(top_srcdir)/libopenss-runtime \
	$(1) \
	@LIBUNWIND_CPPFLAGS@

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


# LAMPI

if HAVE_LAMPI

mpit_rt_lampi_la_CFLAGS = $(call rt_CFLAGS_generate,@LAMPI_CPPFLAGS@)
mpit_rt_lampi_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@LAMPI_LDFLAGS@)
mpit_rt_lampi_la_LIBADD = $(call rt_LIBADD_generate,@LAMPI_LIBS@)
mpit_rt_lampi_la_DEPENDENCIES =
mpit_rt_lampi_la_SOURCES = $(rt_sources)

endif


# MPICH

if HAVE_MPICH

mpit_rt_mpich_la_CFLAGS = $(call rt_CFLAGS_generate,@MPICH_CPPFLAGS@)
mpit_rt_mpich_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MPICH_LDFLAGS@)
mpit_rt_mpich_la_LIBADD = $(call rt_LIBADD_generate,@MPICH_LIBS@)
mpit_rt_mpich_la_DEPENDENCIES =
mpit_rt_mpich_la_SOURCES = $(rt_sources)

endif


# MPT

if HAVE_MPT

mpit_rt_mpt_la_CFLAGS = $(call rt_CFLAGS_generate,@MPT_CPPFLAGS@)
mpit_rt_mpt_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MPT_LDFLAGS@)
mpit_rt_mpt_la_LIBADD = $(call rt_LIBADD_generate,@MPT_LIBS@)
mpit_rt_mpt_la_DEPENDENCIES =
mpit_rt_mpt_la_SOURCES = $(rt_sources)

endif


# OpenMPI

if HAVE_OPENMPI

mpit_rt_openmpi_la_CFLAGS = $(call rt_CFLAGS_generate,@OPENMPI_CPPFLAGS@)
mpit_rt_openmpi_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@OPENMPI_LDFLAGS@)
mpit_rt_openmpi_la_LIBADD = $(call rt_LIBADD_generate,@OPENMPI_LIBS@)
mpit_rt_openmpi_la_DEPENDENCIES =
mpit_rt_openmpi_la_SOURCES = $(rt_sources)

endif



SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES)

blobs.h blobs.c : blobs.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<
