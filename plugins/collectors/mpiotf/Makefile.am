################################################################################
# Copyright (c) 2007 Krell Institute  All Rights Reserved.
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

#noinst_LIBRARIES = libvt.mpi.a

libvt-mpi-a :
	cp -r @VT_LIBSDIR@/libvt.mpi.a libvt.mpi.a
	ar d libvt.mpi.a libvt_mpi_a-vt_mpiwrap.o

pkglib_LTLIBRARIES = mpiotf.la
if HAVE_LAM
pkglib_LTLIBRARIES += mpiotf-rt-lam.la
endif
if HAVE_LAMPI
pkglib_LTLIBRARIES += mpiotf-rt-lampi.la
endif
if HAVE_MPICH
pkglib_LTLIBRARIES += mpiotf-rt-mpich.la
endif
if HAVE_MPICH2
pkglib_LTLIBRARIES += mpiotf-rt-mpich2.la
endif
if HAVE_MPT
pkglib_LTLIBRARIES += mpiotf-rt-mpt.la
endif
if HAVE_OPENMPI
pkglib_LTLIBRARIES += mpiotf-rt-openmpi.la
endif

mpiotf_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-framework

mpiotf_la_LDFLAGS = \
	-no-undefined -module -avoid-version

mpiotf_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	MPIOTFCollector.hxx MPIOTFCollector.cxx \
	MPIOTFDetail.hxx

# Common definitions for all MPI implementations
#
# See comments in plugins/collectors/mpi/Makefile.am.  Remarks there
# concerning the generated wrapper sources also apply to wrappers.c
# here.

rt_CFLAGS_generate = \
	-I$(top_srcdir)/libopenss-runtime \
	@VT_CPPFLAGS@ \
	$(1) \
        @OTF_CPPFLAGS@ \
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
	-lpthread \
	-L$(top_srcdir)/plugins/collectors/mpiotf \
	@VT_LIBS@ \
        @OTF_LDFLAGS@ \
        @OTF_LIBS@ \
        @OTF_LIBZ_LDFLAGS@ \
	@OTF_LIBZ_LIBS@ \
	-lrt 

rt_sources = \
	blobs.x $(BUILT_SOURCES) \
	runtime.h runtime.c  \
	vt_mpiwrap.c 



if HAVE_LAM
mpiotf_rt_lam_la_CFLAGS = $(call rt_CFLAGS_generate,@LAM_CPPFLAGS@)
mpiotf_rt_lam_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@LAM_LDFLAGS@)
mpiotf_rt_lam_la_LIBADD = $(call rt_LIBADD_generate,@LAM_LIBS@)
mpiotf_rt_lam_la_DEPENDENCIES =
mpiotf_rt_lam_la_SOURCES = $(rt_sources)
endif



if HAVE_LAMPI
mpiotf_rt_lampi_la_CFLAGS = $(call rt_CFLAGS_generate,@LAMPI_CPPFLAGS@)
mpiotf_rt_lampi_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@LAMPI_LDFLAGS@)
mpiotf_rt_lampi_la_LIBADD = $(call rt_LIBADD_generate,@LAMPI_LIBS@)
mpiotf_rt_lampi_la_DEPENDENCIES =
mpiotf_rt_lampi_la_SOURCES = $(rt_sources)
endif



if HAVE_MPICH
mpiotf_rt_mpich_la_CFLAGS = $(call rt_CFLAGS_generate,@MPICH_CPPFLAGS@)
mpiotf_rt_mpich_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MPICH_LDFLAGS@)
mpiotf_rt_mpich_la_LIBADD = $(call rt_LIBADD_generate,@MPICH_LIBS@)
mpiotf_rt_mpich_la_DEPENDENCIES =
mpiotf_rt_mpich_la_SOURCES = $(rt_sources)
endif


if HAVE_MPICH2
mpiotf_rt_mpich2_la_CFLAGS = $(call rt_CFLAGS_generate,@MPICH2_CPPFLAGS@)
mpiotf_rt_mpich2_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MPICH2_LDFLAGS@)
mpiotf_rt_mpich2_la_LIBADD = $(call rt_LIBADD_generate,@MPICH2_LIBS@)
mpiotf_rt_mpich2_la_DEPENDENCIES =
mpiotf_rt_mpich2_la_SOURCES = $(rt_sources)
endif


if HAVE_MPT
mpiotf_rt_mpt_la_CFLAGS = $(call rt_CFLAGS_generate,@MPT_CPPFLAGS@)
mpiotf_rt_mpt_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MPT_LDFLAGS@)
mpiotf_rt_mpt_la_LIBADD = $(call rt_LIBADD_generate,@MPT_LIBS@)
mpiotf_rt_mpt_la_DEPENDENCIES =
mpiotf_rt_mpt_la_SOURCES = $(rt_sources)
endif



if HAVE_OPENMPI
mpiotf_rt_openmpi_la_CFLAGS = $(call rt_CFLAGS_generate,@OPENMPI_CPPFLAGS@)
mpiotf_rt_openmpi_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@OPENMPI_LDFLAGS@)
mpiotf_rt_openmpi_la_LIBADD = $(call rt_LIBADD_generate,@OPENMPI_LIBS@)
mpiotf_rt_openmpi_la_DEPENDENCIES =
mpiotf_rt_openmpi_la_SOURCES = $(rt_sources)
endif



SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES)


blobs.x: libvt-mpi-a

blobs.h blobs.c : blobs.x 
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

