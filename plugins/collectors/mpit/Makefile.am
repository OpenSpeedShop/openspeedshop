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
pkglib_LTLIBRARIES += mpit.la
endif
endif

if BUILD_INSTRUMENTOR
if !BUILD_CBTF

if HAVE_LAM
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpit-rt-lam.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpit-lam-rt-offline.la libmpit-lam-offline.la
endif
if HAVE_TARGET_LIBMONITOR
if HAVE_TARGET_SHARED
pkglib_LTLIBRARIES += mpit-lam-rt-offline.la
endif
pkglib_LTLIBRARIES += libmpit-lam-offline.la
endif
endif

if HAVE_LAMPI
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpit-rt-lampi.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpit-lampi-rt-offline.la libmpit-lampi-offline.la
endif
if HAVE_TARGET_LIBMONITOR
if HAVE_TARGET_SHARED
pkglib_LTLIBRARIES += mpit-lampi-rt-offline.la
endif
pkglib_LTLIBRARIES += libmpit-lampi-offline.la
endif
endif

if HAVE_MPICH
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpit-rt-mpich.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpit-mpich-rt-offline.la libmpit-mpich-offline.la
endif
if HAVE_TARGET_LIBMONITOR
if HAVE_TARGET_SHARED
pkglib_LTLIBRARIES += mpit-mpich-rt-offline.la
endif
pkglib_LTLIBRARIES += libmpit-mpich-offline.la
endif
endif

if HAVE_MPICH2
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpit-rt-mpich2.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpit-mpich2-rt-offline.la libmpit-mpich2-offline.la
endif
endif

if HAVE_TARGET_MPICH2
if HAVE_TARGET_LIBMONITOR
if HAVE_TARGET_SHARED
pkglib_LTLIBRARIES += mpit-mpich2-rt-offline.la
endif
pkglib_LTLIBRARIES += libmpit-mpich2-offline.la
endif
endif

if HAVE_MPT
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpit-rt-mpt.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpit-mpt-rt-offline.la libmpit-mpt-offline.la
endif
if HAVE_TARGET_LIBMONITOR
if HAVE_TARGET_SHARED
pkglib_LTLIBRARIES += mpit-mpt-rt-offline.la
endif
pkglib_LTLIBRARIES += libmpit-mpt-offline.la
endif
endif

if HAVE_MVAPICH
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpit-rt-mvapich.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpit-mvapich-rt-offline.la libmpit-mvapich-offline.la
endif
if HAVE_TARGET_LIBMONITOR
if HAVE_TARGET_SHARED
pkglib_LTLIBRARIES += mpit-mvapich-rt-offline.la
endif
pkglib_LTLIBRARIES += libmpit-mvapich-offline.la
endif
endif

if HAVE_MVAPICH2
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpit-rt-mvapich2.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpit-mvapich2-rt-offline.la libmpit-mvapich2-offline.la
endif
if HAVE_TARGET_LIBMONITOR
if HAVE_TARGET_SHARED
pkglib_LTLIBRARIES += mpit-mvapich2-rt-offline.la
endif
pkglib_LTLIBRARIES += libmpit-mvapich2-offline.la
endif
endif

if HAVE_OPENMPI
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpit-rt-openmpi.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpit-openmpi-rt-offline.la libmpit-openmpi-offline.la
endif
if HAVE_TARGET_LIBMONITOR
if HAVE_TARGET_SHARED
pkglib_LTLIBRARIES += mpit-openmpi-rt-offline.la
endif
pkglib_LTLIBRARIES += libmpit-openmpi-offline.la
endif
endif

endif
endif

if !BUILD_RUNTIME_ONLY
mpit_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-framework

mpit_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-framework \
	-no-undefined -module -avoid-version

mpit_la_LIBADD = \
	-lopenss-framework

mpit_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	MPITTraceableFunctions.h \
	MPITCollector.hxx MPITCollector.cxx \
	MPITDetail.hxx
endif

if !BUILD_CBTF
# Common definitions for all MPI implementations
#
# See comments in plugins/collectors/mpi/Makefile.am.  Remarks there
# concerning the generated wrapper sources also apply to wrappers.c
# here.

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
	-lpthread -lrt

rt_sources = \
	blobs.x $(BUILT_SOURCES) \
	runtime.h runtime.c \
	wrappers.c

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

if HAVE_LAM
mpit_rt_lam_la_CFLAGS = $(call rt_CFLAGS_generate,@LAM_CPPFLAGS@)
mpit_rt_lam_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@LAM_LDFLAGS@)
mpit_rt_lam_la_LIBADD = $(call rt_LIBADD_generate,@LAM_LIBS@)
mpit_rt_lam_la_DEPENDENCIES =
mpit_rt_lam_la_SOURCES = $(rt_sources)
nodist_mpit_rt_lam_la_SOURCES = 

if HAVE_LIBMONITOR
mpit_lam_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@LAM_CPPFLAGS@)
mpit_lam_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@LAM_LDFLAGS@)
mpit_lam_rt_offline_la_LIBADD = $(call rt_offline_LIBADD_generate,@LAM_LIBS@)
mpit_lam_rt_offline_la_DEPENDENCIES =
mpit_lam_rt_offline_la_SOURCES = $(rt_offline_sources)
nodist_mpit_lam_rt_offline_la_SOURCES = 

libmpit_lam_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@LAM_CPPFLAGS@)
libmpit_lam_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@LAM_LDFLAGS@)
libmpit_lam_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@LAM_LIBS@)
libmpit_lam_offline_la_DEPENDENCIES =
libmpit_lam_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpit_lam_offline_la_SOURCES =
endif
if HAVE_TARGET_LIBMONITOR
libmpit_lam_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@LAM_CPPFLAGS@)
libmpit_lam_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@LAM_LDFLAGS@)
libmpit_lam_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@LAM_LIBS@)
libmpit_lam_offline_la_DEPENDENCIES =
libmpit_lam_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpit_lam_offline_la_SOURCES =
endif
endif



if HAVE_LAMPI
mpit_rt_lampi_la_CFLAGS = $(call rt_CFLAGS_generate,@LAMPI_CPPFLAGS@)
mpit_rt_lampi_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@LAMPI_LDFLAGS@)
mpit_rt_lampi_la_LIBADD = $(call rt_LIBADD_generate,@LAMPI_LIBS@)
mpit_rt_lampi_la_DEPENDENCIES =
mpit_rt_lampi_la_SOURCES = $(rt_sources)
nodist_mpit_rt_lampi_la_SOURCES = 

if HAVE_LIBMONITOR
mpit_lampi_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@LAMPI_CPPFLAGS@)
mpit_lampi_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@LAMPI_LDFLAGS@)
mpit_lampi_rt_offline_la_LIBADD = $(call rt_offline_LIBADD_generate,@LAMPI_LIBS@)
mpit_lampi_rt_offline_la_DEPENDENCIES =
mpit_lampi_rt_offline_la_SOURCES = $(rt_offline_sources)
nodist_mpit_lampi_rt_offline_la_SOURCES = 

libmpit_lampi_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@LAMPI_CPPFLAGS@)
libmpit_lampi_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@LAMPI_LDFLAGS@)
libmpit_lampi_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@LAMPI_LIBS@)
libmpit_lampi_offline_la_DEPENDENCIES =
libmpit_lampi_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpit_lampi_offline_la_SOURCES =
endif
if HAVE_TARGET_LIBMONITOR
libmpit_lampi_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@LAMPI_CPPFLAGS@)
libmpit_lampi_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@LAMPI_LDFLAGS@)
libmpit_lampi_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@LAMPI_LIBS@)
libmpit_lampi_offline_la_DEPENDENCIES =
libmpit_lampi_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpit_lampi_offline_la_SOURCES =
endif
endif



if HAVE_MPICH
mpit_rt_mpich_la_CFLAGS = $(call rt_CFLAGS_generate,@MPICH_CPPFLAGS@)
mpit_rt_mpich_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MPICH_LDFLAGS@)
mpit_rt_mpich_la_LIBADD = $(call rt_LIBADD_generate,@MPICH_LIBS@)
mpit_rt_mpich_la_DEPENDENCIES =
mpit_rt_mpich_la_SOURCES = $(rt_sources)
nodist_mpit_rt_mpich_la_SOURCES = 

if HAVE_LIBMONITOR
mpit_mpich_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@MPICH_CPPFLAGS@)
mpit_mpich_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@MPICH_LDFLAGS@)
mpit_mpich_rt_offline_la_LIBADD = $(call rt_offline_LIBADD_generate,@MPICH_LIBS@)
mpit_mpich_rt_offline_la_DEPENDENCIES =
mpit_mpich_rt_offline_la_SOURCES = $(rt_offline_sources)
nodist_mpit_mpich_rt_offline_la_SOURCES = 

libmpit_mpich_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@MPICH_CPPFLAGS@)
libmpit_mpich_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MPICH_LDFLAGS@)
libmpit_mpich_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@MPICH_LIBS@)
libmpit_mpich_offline_la_DEPENDENCIES =
libmpit_mpich_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpit_mpich_offline_la_SOURCES =
endif
if HAVE_TARGET_LIBMONITOR
libmpit_mpich_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@MPICH_CPPFLAGS@)
libmpit_mpich_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MPICH_LDFLAGS@)
libmpit_mpich_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@MPICH_LIBS@)
libmpit_mpich_offline_la_DEPENDENCIES =
libmpit_mpich_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpit_mpich_offline_la_SOURCES =
endif
endif

if HAVE_MPICH2
mpit_rt_mpich2_la_CFLAGS = $(call rt_CFLAGS_generate,@MPICH2_CPPFLAGS@)
mpit_rt_mpich2_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MPICH2_LDFLAGS@)
mpit_rt_mpich2_la_LIBADD = $(call rt_LIBADD_generate,@MPICH2_LIBS@)
mpit_rt_mpich2_la_DEPENDENCIES =
mpit_rt_mpich2_la_SOURCES = $(rt_sources)
nodist_mpit_rt_mpich2_la_SOURCES = 

if HAVE_LIBMONITOR
mpit_mpich2_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@MPICH2_CPPFLAGS@)
mpit_mpich2_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@MPICH2_LDFLAGS@)
mpit_mpich2_rt_offline_la_LIBADD = $(call rt_offline_LIBADD_generate,@MPICH2_LIBS@)
mpit_mpich2_rt_offline_la_DEPENDENCIES =
mpit_mpich2_rt_offline_la_SOURCES = $(rt_offline_sources)
nodist_mpit_mpich2_rt_offline_la_SOURCES = 

libmpit_mpich2_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@MPICH2_CPPFLAGS@)
libmpit_mpich2_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MPICH2_LDFLAGS@)
libmpit_mpich2_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@MPICH2_LIBS@)
libmpit_mpich2_offline_la_DEPENDENCIES =
libmpit_mpich2_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpit_mpich2_offline_la_SOURCES =
endif
endif

if HAVE_TARGET_MPICH2
if HAVE_TARGET_LIBMONITOR

if HAVE_TARGET_SHARED
mpit_mpich2_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@TARGET_MPICH2_CPPFLAGS@)
mpit_mpich2_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@TARGET_MPICH2_LDFLAGS@)
mpit_mpich2_rt_offline_la_LIBADD = $(call rt_offline_LIBADD_generate,@TARGET_MPICH2_LIBS@)
mpit_mpich2_rt_offline_la_DEPENDENCIES =
mpit_mpich2_rt_offline_la_SOURCES = $(rt_offline_sources)
nodist_mpit_mpich2_rt_offline_la_SOURCES = 
endif

libmpit_mpich2_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@TARGET_MPICH2_CPPFLAGS@)
libmpit_mpich2_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@TARGET_MPICH2_LDFLAGS@)
libmpit_mpich2_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@TARGET_MPICH2_LIBS@)
libmpit_mpich2_offline_la_DEPENDENCIES =
libmpit_mpich2_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpit_mpich2_offline_la_SOURCES =

endif
endif


if HAVE_MPT
mpit_rt_mpt_la_CFLAGS = -DSGI_MPT $(call rt_CFLAGS_generate,@MPT_CPPFLAGS@)
mpit_rt_mpt_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MPT_LDFLAGS@)
mpit_rt_mpt_la_LIBADD = $(call rt_LIBADD_generate,@MPT_LIBS@)
mpit_rt_mpt_la_DEPENDENCIES =
mpit_rt_mpt_la_SOURCES = $(rt_sources)
nodist_mpit_rt_mpt_la_SOURCES = 

if HAVE_LIBMONITOR
mpit_mpt_rt_offline_la_CFLAGS = -DSGI_MPT $(call rt_offline_CFLAGS_generate,@MPT_CPPFLAGS@)
mpit_mpt_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@MPT_LDFLAGS@)
mpit_mpt_rt_offline_la_LIBADD = $(call rt_offline_LIBADD_generate,@MPT_LIBS@)
mpit_mpt_rt_offline_la_DEPENDENCIES =
mpit_mpt_rt_offline_la_SOURCES = $(rt_offline_sources)
nodist_mpit_mpt_rt_offline_la_SOURCES = 

libmpit_mpt_offline_la_CFLAGS = -DSGI_MPT -static $(call lib_offline_CFLAGS_generate,@MPT_CPPFLAGS@)
libmpit_mpt_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MPT_LDFLAGS@)
libmpit_mpt_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@MPT_LIBS@)
libmpit_mpt_offline_la_DEPENDENCIES =
libmpit_mpt_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpit_mpt_offline_la_SOURCES =
endif
if HAVE_TARGET_LIBMONITOR
libmpit_mpt_offline_la_CFLAGS = -DSGI_MPT -static $(call lib_offline_CFLAGS_generate,@MPT_CPPFLAGS@)
libmpit_mpt_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MPT_LDFLAGS@)
libmpit_mpt_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@MPT_LIBS@)
libmpit_mpt_offline_la_DEPENDENCIES =
libmpit_mpt_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpit_mpt_offline_la_SOURCES =
endif
endif

if HAVE_MVAPICH
mpit_rt_mvapich_la_CFLAGS = $(call rt_CFLAGS_generate,@MVAPICH_CPPFLAGS@)
mpit_rt_mvapich_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MVAPICH_LDFLAGS@)
mpit_rt_mvapich_la_LIBADD = $(call rt_LIBADD_generate,@MVAPICH_LIBS@)
mpit_rt_mvapich_la_DEPENDENCIES =
mpit_rt_mvapich_la_SOURCES = $(rt_sources)
nodist_mpit_rt_mvapich_la_SOURCES =

if HAVE_LIBMONITOR
mpit_mvapich_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@MVAPICH_CPPFLAGS@)
mpit_mvapich_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@MVAPICH_LDFLAGS@)
mpit_mvapich_rt_offline_la_LIBADD = $(call rt_offline_LIBADD_generate,@MVAPICH_LIBS@)
mpit_mvapich_rt_offline_la_DEPENDENCIES =
mpit_mvapich_rt_offline_la_SOURCES = $(rt_offline_sources)
nodist_mpit_mvapich_rt_offline_la_SOURCES =

libmpit_mvapich_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@MVAPICH_CPPFLAGS@)
libmpit_mvapich_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MVAPICH_LDFLAGS@)
libmpit_mvapich_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@MVAPICH_LIBS@)
libmpit_mvapich_offline_la_DEPENDENCIES =
libmpit_mvapich_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpit_mvapich_offline_la_SOURCES =
endif
if HAVE_TARGET_LIBMONITOR
libmpit_mvapich_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@MVAPICH_CPPFLAGS@)
libmpit_mvapich_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MVAPICH_LDFLAGS@)
libmpit_mvapich_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@MVAPICH_LIBS@)
libmpit_mvapich_offline_la_DEPENDENCIES =
libmpit_mvapich_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpit_mvapich_offline_la_SOURCES =
endif
endif


if HAVE_MVAPICH2
mpit_rt_mvapich2_la_CFLAGS = $(call rt_CFLAGS_generate,@MVAPICH2_CPPFLAGS@)
mpit_rt_mvapich2_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MVAPICH2_LDFLAGS@)
mpit_rt_mvapich2_la_LIBADD = $(call rt_LIBADD_generate,@MVAPICH2_LIBS@)
mpit_rt_mvapich2_la_DEPENDENCIES =
mpit_rt_mvapich2_la_SOURCES = $(rt_sources)
nodist_mpit_rt_mvapich2_la_SOURCES =

if HAVE_LIBMONITOR
mpit_mvapich2_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@MVAPICH2_CPPFLAGS@)
mpit_mvapich2_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@MVAPICH2_LDFLAGS@)
mpit_mvapich2_rt_offline_la_LIBADD = $(call rt_offline_LIBADD_generate,@MVAPICH2_LIBS@)
mpit_mvapich2_rt_offline_la_DEPENDENCIES =
mpit_mvapich2_rt_offline_la_SOURCES = $(rt_offline_sources)
nodist_mpit_mvapich2_rt_offline_la_SOURCES =

libmpit_mvapich2_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@MVAPICH2_CPPFLAGS@)
libmpit_mvapich2_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MVAPICH2_LDFLAGS@)
libmpit_mvapich2_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@MVAPICH2_LIBS@)
libmpit_mvapich2_offline_la_DEPENDENCIES =
libmpit_mvapich2_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpit_mvapich2_offline_la_SOURCES =
endif
if HAVE_TARGET_LIBMONITOR
libmpit_mvapich2_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@MVAPICH2_CPPFLAGS@)
libmpit_mvapich2_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MVAPICH2_LDFLAGS@)
libmpit_mvapich2_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@MVAPICH2_LIBS@)
libmpit_mvapich2_offline_la_DEPENDENCIES =
libmpit_mvapich2_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpit_mvapich2_offline_la_SOURCES =
endif
endif


if HAVE_OPENMPI
mpit_rt_openmpi_la_CFLAGS = $(call rt_CFLAGS_generate,@OPENMPI_CPPFLAGS@)
mpit_rt_openmpi_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@OPENMPI_LDFLAGS@)
mpit_rt_openmpi_la_LIBADD = $(call rt_LIBADD_generate,@OPENMPI_LIBS@)
mpit_rt_openmpi_la_DEPENDENCIES =
mpit_rt_openmpi_la_SOURCES = $(rt_sources)
nodist_mpit_rt_openmpi_la_SOURCES = 

if HAVE_LIBMONITOR
mpit_openmpi_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@OPENMPI_CPPFLAGS@)
mpit_openmpi_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@OPENMPI_LDFLAGS@)
mpit_openmpi_rt_offline_la_LIBADD = $(call rt_offline_LIBADD_generate,@OPENMPI_LIBS@)
mpit_openmpi_rt_offline_la_DEPENDENCIES =
mpit_openmpi_rt_offline_la_SOURCES = $(rt_offline_sources)
nodist_mpit_openmpi_rt_offline_la_SOURCES = 

libmpit_openmpi_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@OPENMPI_CPPFLAGS@)
libmpit_openmpi_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@OPENMPI_LDFLAGS@)
libmpit_openmpi_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@OPENMPI_LIBS@)
libmpit_openmpi_offline_la_DEPENDENCIES =
libmpit_openmpi_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpit_openmpi_offline_la_SOURCES = 
endif
if HAVE_TARGET_LIBMONITOR
libmpit_openmpi_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@OPENMPI_CPPFLAGS@)
libmpit_openmpi_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@OPENMPI_LDFLAGS@)
libmpit_openmpi_offline_la_LIBADD = $(call lib_offline_LIBADD_generate,@OPENMPI_LIBS@)
libmpit_openmpi_offline_la_DEPENDENCIES =
libmpit_openmpi_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpit_openmpi_offline_la_SOURCES = 
endif
endif

#endif for !BUILD_OFFLINE
#endif for !BUILD_CBTF
endif

SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES)

blobs.h blobs.c : blobs.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<
