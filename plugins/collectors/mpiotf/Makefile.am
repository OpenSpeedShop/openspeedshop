################################################################################
# Copyright (c) 2007,2008,2009 Krell Institute  All Rights Reserved.
# Copyright (c) 2007 William Hachfeld. All Rights Reserved.
# Copyright (c) 2006-2014 The Krell Institute. All Rights Reserved.
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

CFLAGS =  -DBINDIR=\"@VT_DIR@/bin\" -DDATADIR=\"@VT_DIR@/share/vampirtrace\" -DVT_MPI -O0 -g -DRFG -DVT_BFDSYMCOLLECT -DVT_MEMHOOK 
#CFLAGS =    -DBINDIR=\"@VT_DIR@/bin\" -DDATADIR=\"@VT_DIR@/share/vampirtrace\" -DVT_MPI -O0 -g -fPIC -shared -DRFG -DVT_BFDSYMCOLLECT 
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

#noinst_LIBRARIES = libvt.mpi.a

if !BUILD_RUNTIME_ONLY
if !HAVE_TARGET_LIBMONITOR
pkglib_LTLIBRARIES += mpiotf.la
endif
endif

if !BUILD_CBTF
if HAVE_LAM

if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpiotf-rt-lam.la
endif

if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpiotf-lam-rt-offline.la libmpiotf-lam-offline.la
endif

if HAVE_TARGET_LIBMONITOR
pkglib_LTLIBRARIES += libmpiotf-lam-offline.la
endif

endif # endif for HAVE_LAM

if HAVE_LAMPI

if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpiotf-rt-lampi.la
endif

if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpiotf-lampi-rt-offline.la libmpiotf-lampi-offline.la
endif

if HAVE_TARGET_LIBMONITOR
pkglib_LTLIBRARIES += libmpiotf-lampi-offline.la
endif

endif # endif for HAVE_LAMPI

if HAVE_MPICH

if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpiotf-rt-mpich.la
endif

if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpiotf-mpich-rt-offline.la libmpiotf-mpich-offline.la
endif

if HAVE_TARGET_LIBMONITOR
pkglib_LTLIBRARIES += libmpiotf-mpich-offline.la
endif

endif # endif for HAVE_MPICH


if HAVE_MPICH2

if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpiotf-rt-mpich2.la
endif

if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpiotf-mpich2-rt-offline.la libmpiotf-mpich2-offline.la
endif

endif

if HAVE_TARGET_MPICH2
if HAVE_TARGET_LIBMONITOR
if HAVE_TARGET_SHARED
pkglib_LTLIBRARIES += mpiotf-mpich2-rt-offline.la
endif
pkglib_LTLIBRARIES += libmpiotf-mpich2-offline.la
endif

endif

if HAVE_MPT
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpiotf-rt-mpt.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpiotf-mpt-rt-offline.la libmpiotf-mpt-offline.la
endif

if HAVE_TARGET_LIBMONITOR
pkglib_LTLIBRARIES += libmpiotf-mpt-offline.la
endif
endif

if HAVE_MVAPICH
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpiotf-rt-mvapich.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpiotf-mvapich-rt-offline.la libmpiotf-mvapich-offline.la
endif

if HAVE_TARGET_LIBMONITOR
pkglib_LTLIBRARIES += libmpiotf-mvapich-offline.la
endif
endif

if HAVE_MVAPICH2
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpiotf-rt-mvapich2.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpiotf-mvapich2-rt-offline.la libmpiotf-mvapich2-offline.la
endif

if HAVE_TARGET_LIBMONITOR
pkglib_LTLIBRARIES += libmpiotf-mvapich2-offline.la
endif

endif

if HAVE_OPENMPI
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += mpiotf-rt-openmpi.la
endif
if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += mpiotf-openmpi-rt-offline.la libmpiotf-openmpi-offline.la
endif

if HAVE_TARGET_LIBMONITOR
pkglib_LTLIBRARIES += libmpiotf-openmpi-offline.la
endif
endif

endif

if !BUILD_RUNTIME_ONLY
mpiotf_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-framework

mpiotf_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-framework \
	-no-undefined -module -avoid-version

mpiotf_la_LIBADD = \
	-lopenss-framework

mpiotf_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	MPIOTFTraceableFunctions.h \
	MPIOTFCollector.hxx MPIOTFCollector.cxx \
	MPIOTFDetail.hxx
endif

if !BUILD_CBTF
if HAVE_LAM
libvt-lam-mpi-a:
	cp -r @VT_LIBSDIR@/libvt.mpi.a libvt.mpi.lam.a
	ar d libvt.mpi.lam.a libvt_mpi_a-vt_mpiwrap.o
	ar d libvt.mpi.lam.a libvt_mpi_a-vt_sync.o
	ar d libvt.mpi.lam.a libvt_mpi_a-vt_otf_trc.o
	ar d libvt.mpi.lam.a libvt_mpi_a-vt_mpireq.o
	ar d libvt.mpi.lam.a libvt_mpi_a-vt_mpireg.o
	ar d libvt.mpi.lam.a libvt_mpi_a-vt_mpicom.o
	ar d libvt.mpi.lam.a libvt_mpi_a-vt_fmpiwrap.o
endif

if HAVE_LAMPI
libvt-lampi-mpi-a :
	cp -r @VT_LIBSDIR@/libvt.mpi.a libvt.mpi.lampi.a
	ar d libvt.mpi.lampi.a libvt_mpi_a-vt_mpiwrap.o
	ar d libvt.mpi.lampi.a libvt_mpi_a-vt_sync.o
	ar d libvt.mpi.lampi.a libvt_mpi_a-vt_otf_trc.o
	ar d libvt.mpi.lampi.a libvt_mpi_a-vt_mpireq.o
	ar d libvt.mpi.lampi.a libvt_mpi_a-vt_mpireg.o
	ar d libvt.mpi.lampi.a libvt_mpi_a-vt_mpicom.o
	ar d libvt.mpi.lampi.a libvt_mpi_a-vt_fmpiwrap.o
endif

if HAVE_MPICH2
libvt-mpich2-mpi-a:
	cp -r @VT_LIBSDIR@/libvt.mpi.a libvt.mpi.mpich2.a
	ar d libvt.mpi.mpich2.a libvt_mpi_a-vt_mpiwrap.o
	ar d libvt.mpi.mpich2.a libvt_mpi_a-vt_sync.o
	ar d libvt.mpi.mpich2.a libvt_mpi_a-vt_otf_trc.o
	ar d libvt.mpi.mpich2.a libvt_mpi_a-vt_mpireq.o
	ar d libvt.mpi.mpich2.a libvt_mpi_a-vt_mpireg.o
	ar d libvt.mpi.mpich2.a libvt_mpi_a-vt_mpicom.o
	ar d libvt.mpi.mpich2.a libvt_mpi_a-vt_fmpiwrap.o
endif

if HAVE_TARGET_MPICH2
libvt-mpich2-mpi-a:
	cp -r @TARGET_VT_LIBSDIR@/libvt.mpi.a libvt.mpi.mpich2.a
	ar d libvt.mpi.mpich2.a libvt_mpi_a-vt_mpiwrap.o
	ar d libvt.mpi.mpich2.a libvt_mpi_a-vt_sync.o
	ar d libvt.mpi.mpich2.a libvt_mpi_a-vt_otf_trc.o
	ar d libvt.mpi.mpich2.a libvt_mpi_a-vt_mpireq.o
	ar d libvt.mpi.mpich2.a libvt_mpi_a-vt_mpireg.o
	ar d libvt.mpi.mpich2.a libvt_mpi_a-vt_mpicom.o
	ar d libvt.mpi.mpich2.a libvt_mpi_a-vt_fmpiwrap.o
endif

if HAVE_MPICH
libvt-mpich-mpi-a:
	cp -r @VT_LIBSDIR@/libvt.mpi.a libvt.mpi.mpich.a
	ar d libvt.mpi.mpich.a libvt_mpi_a-vt_mpiwrap.o
	ar d libvt.mpi.mpich.a libvt_mpi_a-vt_sync.o
	ar d libvt.mpi.mpich.a libvt_mpi_a-vt_otf_trc.o
	ar d libvt.mpi.mpich.a libvt_mpi_a-vt_mpireq.o
	ar d libvt.mpi.mpich.a libvt_mpi_a-vt_mpireg.o
	ar d libvt.mpi.mpich.a libvt_mpi_a-vt_mpicom.o
	ar d libvt.mpi.mpich.a libvt_mpi_a-vt_fmpiwrap.o
endif

if HAVE_MPT
libvt-mpt-mpi-a:
	cp -r @VT_LIBSDIR@/libvt.mpi.a libvt.mpi.mpt.a
	ar d libvt.mpi.mpt.a libvt_mpi_a-vt_mpiwrap.o
	ar d libvt.mpi.mpt.a libvt_mpi_a-vt_sync.o
	ar d libvt.mpi.mpt.a libvt_mpi_a-vt_otf_trc.o
	ar d libvt.mpi.mpt.a libvt_mpi_a-vt_mpireq.o
	ar d libvt.mpi.mpt.a libvt_mpi_a-vt_mpireg.o
	ar d libvt.mpi.mpt.a libvt_mpi_a-vt_mpicom.o
	ar d libvt.mpi.mpt.a libvt_mpi_a-vt_fmpiwrap.o
endif

if HAVE_MVAPICH
libvt-mvapich-mpi-a:
	cp -r @VT_LIBSDIR@/libvt.mpi.a libvt.mpi.mvapich.a
	ar d libvt.mpi.mvapich.a libvt_mpi_a-vt_mpiwrap.o
	ar d libvt.mpi.mvapich.a libvt_mpi_a-vt_sync.o
	ar d libvt.mpi.mvapich.a libvt_mpi_a-vt_otf_trc.o
	ar d libvt.mpi.mvapich.a libvt_mpi_a-vt_mpireq.o
	ar d libvt.mpi.mvapich.a libvt_mpi_a-vt_mpireg.o
	ar d libvt.mpi.mvapich.a libvt_mpi_a-vt_mpicom.o
	ar d libvt.mpi.mvapich.a libvt_mpi_a-vt_fmpiwrap.o
endif


if HAVE_MVAPICH2
libvt-mvapich2-mpi-a:
	cp -r @VT_LIBSDIR@/libvt.mpi.a libvt.mpi.mvapich2.a
	ar d libvt.mpi.mvapich2.a libvt_mpi_a-vt_mpiwrap.o
	ar d libvt.mpi.mvapich2.a libvt_mpi_a-vt_sync.o
	ar d libvt.mpi.mvapich2.a libvt_mpi_a-vt_otf_trc.o
	ar d libvt.mpi.mvapich2.a libvt_mpi_a-vt_mpireq.o
	ar d libvt.mpi.mvapich2.a libvt_mpi_a-vt_mpireg.o
	ar d libvt.mpi.mvapich2.a libvt_mpi_a-vt_mpicom.o
	ar d libvt.mpi.mvapich2.a libvt_mpi_a-vt_fmpiwrap.o
endif

if HAVE_OPENMPI
libvt-openmpi-mpi-a:
	cp -r @OTF_LIBSDIR@/libotf.a libotf.openmpi.a
	cp -r @VT_LIBSDIR@/libvt.mpi.a libvt.mpi.openmpi.a
	ar d libvt.mpi.openmpi.a libvt_mpi_a-vt_mpiwrap.o
	ar d libvt.mpi.openmpi.a libvt_mpi_a-vt_sync.o
	ar d libvt.mpi.openmpi.a libvt_mpi_a-vt_otf_trc.o
	ar d libvt.mpi.openmpi.a libvt_mpi_a-vt_mpireq.o
	ar d libvt.mpi.openmpi.a libvt_mpi_a-vt_mpireg.o
	ar d libvt.mpi.openmpi.a libvt_mpi_a-vt_mpicom.o
	ar d libvt.mpi.openmpi.a libvt_mpi_a-vt_fmpiwrap.o
endif

# Common definitions for all MPI implementations
#
# See comments in plugins/collectors/mpi/Makefile.am.  Remarks there
# concerning the generated wrapper sources also apply to wrappers.c
# here.

if HAVE_LIBMONITOR

rt_offline_CFLAGS_generate = \
	-DOPENSS_OFFLINE \
	$(1) \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	@VT_CPPFLAGS@ \
        @OTF_CPPFLAGS@


rt_offline_LDFLAGS_generate = \
	$(1) \
        -L$(top_srcdir)/libopenss-runtime \
        -L$(top_srcdir)/libopenss-runtime/offline \
        -L$(top_srcdir)/plugins/collectors/mpiotf \
	-no-undefined -avoid-version

rt_offline_sources = \
        blobs.x $(BUILT_SOURCES) $(OFFLINE_SOURCES) \
        runtime.h runtime.c  \
        vt_mpiwrap-offline.c vt_sync.c vt_otf_trc.c \
	vt_mpireq.c vt_mpireg.c vt_mpicom.c


rt_offline_LIBADD_generate = \
        $(1) \
        -lopenss-runtime-offline -lopenss-runtime-monitor \
        @VT_LIBS@ \
        @OTF_LDFLAGS@ \
        @OTF_LIBS@ \
        @OTF_LIBZ_LDFLAGS@ \
        @OTF_LIBZ_LIBS@ \
        -lrt

#        @OTF_LDFLAGS@ \
#        @OTF_LIBS@ \
#
rt_offline_lam_LIBADD_generate = \
	$(1) \
	-lopenss-runtime-offline \
	-lopenss-runtime-monitor \
	-L./ -lvt.mpi.lam  \
        @OTF_LIBSDIR@/libotf.a \
        @OTF_LIBZ_LDFLAGS@ \
	@OTF_LIBZ_LIBS@ \
	-lrt 

rt_offline_lampi_LIBADD_generate = \
	$(1) \
	-lopenss-runtime-offline \
	-lopenss-runtime-monitor \
	-L./ -lvt.mpi.lampi  \
        @OTF_LIBSDIR@/libotf.a \
        @OTF_LIBZ_LDFLAGS@ \
	@OTF_LIBZ_LIBS@ \
	-lrt 
#
rt_offline_mpich_LIBADD_generate = \
	$(1) \
	-lopenss-runtime-offline \
	-lopenss-runtime-monitor \
	-L./ -lvt.mpi.mpich  \
        @OTF_LIBSDIR@/libotf.a \
        @OTF_LIBZ_LDFLAGS@ \
	@OTF_LIBZ_LIBS@ \
	-lrt 
#
rt_offline_mpich2_LIBADD_generate = \
	$(1) \
	-lopenss-runtime-offline \
	-lopenss-runtime-monitor \
	-L./ -lvt.mpi.mpich2  \
        @OTF_LIBSDIR@/libotf.a \
        @OTF_LIBZ_LDFLAGS@ \
	@OTF_LIBZ_LIBS@ \
	-lrt 
#
rt_offline_mpt_LIBADD_generate = \
	$(1) \
	-lopenss-runtime-offline \
	-lopenss-runtime-monitor \
	-L./ -lvt.mpi.mpt  \
        @OTF_LIBSDIR@/libotf.a \
        @OTF_LIBZ_LDFLAGS@ \
	@OTF_LIBZ_LIBS@ \
	-lrt 
#
rt_offline_mvapich_LIBADD_generate = \
	$(1) \
	-lopenss-runtime-offline \
	-lopenss-runtime-monitor \
	-L./ -lvt.mpi.mvapich  \
        @OTF_LIBSDIR@/libotf.a \
        @OTF_LIBZ_LDFLAGS@ \
	@OTF_LIBZ_LIBS@ \
	-lrt 
#
rt_offline_mvapich2_LIBADD_generate = \
	$(1) \
	-lopenss-runtime-offline \
	-lopenss-runtime-monitor \
	-L./ -lvt.mpi.mvapich2  \
        @OTF_LIBSDIR@/libotf.a \
        @OTF_LIBZ_LDFLAGS@ \
	@OTF_LIBZ_LIBS@ \
	-lrt 

rt_offline_openmpi_LIBADD_generate = \
	$(1) \
	-lopenss-runtime-offline \
	-lopenss-runtime-monitor \
	-L./ -lvt.mpi.openmpi  \
        @OTF_LIBSDIR@/libotf.a \
        @OTF_LIBZ_LDFLAGS@ \
	@OTF_LIBZ_LIBS@ \
	-lrt 

#  --- from  mpit below

lib_offline_CFLAGS_generate = \
        -static \
        -DOPENSS_OFFLINE \
        -I$(top_srcdir)/libopenss-runtime \
        -I$(top_srcdir)/libopenss-runtime/offline \
        $(1) \
	@VT_CPPFLAGS@ \
        @OTF_CPPFLAGS@

lib_offline_LDFLAGS_generate = \
        -static \
	-L$(top_srcdir)/libopenss-runtime \
	-L$(top_srcdir)/libopenss-runtime/offline \
        $(1) \
        @VT_LDFLAGS@ \
        @OTF_LDFLAGS@ \
        @OTF_LIBZ_LDFLAGS@ \
        -no-undefined

lib_offline_lam_LIBADD_generate = \
        -lopenss-runtime-offline \
        -lopenss-runtime-monitor \
        $(1) \
	./libvt.mpi.lam.a  \
        @OTF_LIBSDIR@/libotf.a \
	@OTF_LIBZ_LIBS@ \
        -lpthread -lrt

lib_offline_lampi_LIBADD_generate = \
        -lopenss-runtime-offline \
        -lopenss-runtime-monitor \
        $(1) \
	./libvt.mpi.lampi.a  \
        @OTF_LIBSDIR@/libotf.a \
	@OTF_LIBZ_LIBS@ \
        -lpthread -lrt

lib_offline_mpich_LIBADD_generate = \
        -lopenss-runtime-offline \
        -lopenss-runtime-monitor \
        $(1) \
	./libvt.mpi.mpich.a  \
        @OTF_LIBSDIR@/libotf.a \
	@OTF_LIBZ_LIBS@ \
        -lpthread -lrt

lib_offline_mpich2_LIBADD_generate = \
        -lopenss-runtime-offline \
        -lopenss-runtime-monitor \
        $(1) \
	./libvt.mpi.mpich2.a  \
        @OTF_LIBSDIR@/libotf.a \
	@OTF_LIBZ_LIBS@ \
        -lpthread -lrt

lib_offline_mpt_LIBADD_generate = \
        -lopenss-runtime-offline \
        -lopenss-runtime-monitor \
        $(1) \
	./libvt.mpi.mpt.a  \
        @OTF_LIBSDIR@/libotf.a \
	@OTF_LIBZ_LIBS@ \
        -lpthread -lrt

lib_offline_mvapich_LIBADD_generate = \
        -lopenss-runtime-offline \
        -lopenss-runtime-monitor \
        $(1) \
	./libvt.mpi.mvapich.a  \
        @OTF_LIBSDIR@/libotf.a \
	@OTF_LIBZ_LIBS@ \
        -lpthread -lrt

lib_offline_mvapich2_LIBADD_generate = \
        -lopenss-runtime-offline \
        -lopenss-runtime-monitor \
        $(1) \
	./libvt.mpi.mvapich2.a  \
        @OTF_LIBSDIR@/libotf.a \
	@OTF_LIBZ_LIBS@ \
        -lpthread -lrt

lib_offline_openmpi_LIBADD_generate = \
        -lopenss-runtime-offline \
        -lopenss-runtime-monitor \
        $(1) \
	./libvt.mpi.openmpi.a  \
        @OTF_LIBSDIR@/libotf.a \
	@OTF_LIBZ_LIBS@ \
        -lpthread -lrt

lib_offline_sources = \
        blobs.x $(BUILT_SOURCES) $(OFFLINE_SOURCES) \
        vt_mpiwrap-offline.c \
        runtime.h runtime.c

endif

if HAVE_TARGET_LIBMONITOR

if HAVE_TARGET_MPICH2
if HAVE_TARGET_SHARED
rt_offline_mpich2_LIBADD_generate = \
	$(1) \
	-lopenss-runtime-offline \
	-lopenss-runtime-monitor \
	-L./ -lvt.mpi.mpich2  \
        @TARGET_OTF_LIBSDIR@/libotf.a \
        @TARGET_OTF_LIBZ_LDFLAGS@ \
	@TARGET_OTF_LIBZ_LIBS@ \
	-lrt 
endif
endif

lib_offline_CFLAGS_generate = \
        -static \
        -DOPENSS_OFFLINE \
        -I$(top_srcdir)/libopenss-runtime \
        -I$(top_srcdir)/libopenss-runtime/offline \
        $(1) \
	@TARGET_VT_CPPFLAGS@ \
        @TARGET_OTF_CPPFLAGS@

lib_offline_LDFLAGS_generate = \
        -static \
	-L$(top_srcdir)/libopenss-runtime \
	-L$(top_srcdir)/libopenss-runtime/offline \
        $(1) \
        @TARGET_VT_LDFLAGS@ \
        @TARGET_OTF_LDFLAGS@ \
        @TARGET_OTF_LIBZ_LDFLAGS@ \
        -no-undefined

lib_offline_lam_LIBADD_generate = \
        -lopenss-runtime-offline \
        -lopenss-runtime-monitor \
        $(1) \
	./libvt.mpi.lam.a  \
        @TARGET_OTF_LIBSDIR@/libotf.a \
	@TARGET_OTF_LIBZ_LIBS@ \
        -lpthread -lrt

lib_offline_lampi_LIBADD_generate = \
        -lopenss-runtime-offline \
        -lopenss-runtime-monitor \
        $(1) \
	./libvt.mpi.lampi.a  \
        @TARGET_OTF_LIBSDIR@/libotf.a \
	@TARGET_OTF_LIBZ_LIBS@ \
        -lpthread -lrt

lib_offline_mpich_LIBADD_generate = \
        -lopenss-runtime-offline \
        -lopenss-runtime-monitor \
        $(1) \
	./libvt.mpi.mpich.a  \
        @TARGET_OTF_LIBSDIR@/libotf.a \
	@TARGET_OTF_LIBZ_LIBS@ \
        -lpthread -lrt

lib_offline_mpich2_LIBADD_generate = \
        -lopenss-runtime-offline \
        -lopenss-runtime-monitor \
        $(1) \
	./libvt.mpi.mpich2.a  \
        @TARGET_OTF_LIBSDIR@/libotf.a \
	@TARGET_OTF_LIBZ_LIBS@ \
        -lpthread -lrt

lib_offline_mpt_LIBADD_generate = \
        -lopenss-runtime-offline \
        -lopenss-runtime-monitor \
        $(1) \
	./libvt.mpi.mpt.a  \
        @TARGET_OTF_LIBSDIR@/libotf.a \
	@TARGET_OTF_LIBZ_LIBS@ \
        -lpthread -lrt

lib_offline_mvapich_LIBADD_generate = \
        -lopenss-runtime-offline \
        -lopenss-runtime-monitor \
        $(1) \
	./libvt.mpi.mvapich.a  \
        @TARGET_OTF_LIBSDIR@/libotf.a \
	@TARGET_OTF_LIBZ_LIBS@ \
        -lpthread -lrt

lib_offline_mvapich2_LIBADD_generate = \
        -lopenss-runtime-offline \
        -lopenss-runtime-monitor \
        $(1) \
	./libvt.mpi.mvapich2.a  \
        @TARGET_OTF_LIBSDIR@/libotf.a \
	@TARGET_OTF_LIBZ_LIBS@ \
        -lpthread -lrt

lib_offline_openmpi_LIBADD_generate = \
        -lopenss-runtime-offline \
        -lopenss-runtime-monitor \
        $(1) \
	./libvt.mpi.openmpi.a  \
        @TARGET_OTF_LIBSDIR@/libotf.a \
	@TARGET_OTF_LIBZ_LIBS@ \
        -lpthread -lrt

lib_offline_sources = \
        blobs.x $(BUILT_SOURCES) $(OFFLINE_SOURCES) \
        vt_mpiwrap-offline.c \
        runtime.h runtime.c
endif


rt_CFLAGS_generate = \
	-I$(top_srcdir)/libopenss-runtime \
	@VT_CPPFLAGS@ \
	$(1) \
        @OTF_CPPFLAGS@

rt_LDFLAGS_generate = \
	-L$(top_srcdir)/libopenss-runtime \
	$(1) \
	-L$(top_srcdir)/plugins/collectors/mpiotf \
        @OTF_LDFLAGS@ \
        @OTF_LIBZ_LDFLAGS@ \
	-no-undefined -avoid-version


rt_LIBADD_generate = \
	-lopenss-runtime \
	-lpthread \
        @OTF_LIBS@ \
	@OTF_LIBZ_LIBS@ \
	$(1) \
	-lrt 

rt_sources = \
	blobs.x $(BUILT_SOURCES) \
	runtime.h runtime.c  \
	vt_mpiwrap.c vt_sync.c vt_otf_trc.c \
	vt_mpireq.c vt_mpireg.c vt_mpicom.c 


rt_openmpi_LIBADD_generate = \
	-lopenss-runtime \
	-lpthread \
	-L./ -lvt.mpi.openmpi  \
	@PAPI_LDFLAGS@ @PAPI_LIBS@ \
	-L./ -lotf.openmpi  \
	@OTF_LIBZ_LIBS@ \
	$(1) \
	-lrt 


rt_lam_LIBADD_generate = \
	-lopenss-runtime \
	$(1) \
	-lpthread \
	@LAM_LDFLAGS@ @LAM_LIBS@ \
	-L./ -lvt.mpi.lam  \
	@LAM_LDFLAGS@ @LAM_LIBS@ \
        @OTF_LDFLAGS@ \
        @OTF_LIBS@ \
        @OTF_LIBZ_LDFLAGS@ \
	@OTF_LIBZ_LIBS@ \
	-lrt 

rt_lampi_LIBADD_generate = \
	-lopenss-runtime \
	$(1) \
	-lpthread \
	-L./ -lvt.mpi.lampi  \
	@LAMPI_LDFLAGS@ @LAMPI_LIBS@ \
        @OTF_LDFLAGS@ \
        @OTF_LIBS@ \
        @OTF_LIBZ_LDFLAGS@ \
	@OTF_LIBZ_LIBS@ \
	-lrt 


rt_mpich_LIBADD_generate = \
	-lopenss-runtime \
	$(1) \
	-lpthread \
	-L./ -lvt.mpi.mpich  \
        @OTF_LDFLAGS@ \
        @OTF_LIBS@ \
        @OTF_LIBZ_LDFLAGS@ \
	@OTF_LIBZ_LIBS@ \
	-lrt 

rt_mvapich_LIBADD_generate = \
	-lopenss-runtime \
	$(1) \
	-lpthread \
	-L./ -lvt.mpi.mvapich  \
        @OTF_LDFLAGS@ \
        @OTF_LIBS@ \
        @OTF_LIBZ_LDFLAGS@ \
	@OTF_LIBZ_LIBS@ \
	-lrt 

rt_mpich2_LIBADD_generate = \
	-lopenss-runtime \
	$(1) \
	-lpthread \
	-L./ -lvt.mpi.mpich2  \
        @OTF_LDFLAGS@ \
        @OTF_LIBS@ \
        @OTF_LIBZ_LDFLAGS@ \
	@OTF_LIBZ_LIBS@ \
	-lrt 

rt_mpt_LIBADD_generate = \
	-lopenss-runtime \
	$(1) \
	-lpthread \
	-L./ -lvt.mpi.mpt  \
        @OTF_LDFLAGS@ \
        @OTF_LIBS@ \
        @OTF_LIBZ_LDFLAGS@ \
	@OTF_LIBZ_LIBS@ \
	-lrt 


if HAVE_LAM
mpiotf_rt_lam_la_CFLAGS = $(call rt_CFLAGS_generate,@LAM_CPPFLAGS@)
mpiotf_rt_lam_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@LAM_LDFLAGS@)
mpiotf_rt_lam_la_LIBADD = $(call rt_lam_LIBADD_generate,@LAM_LIBS@)
mpiotf_rt_lam_la_DEPENDENCIES = libvt-lam-mpi-a
mpiotf_rt_lam_la_SOURCES = $(rt_sources)

if HAVE_LIBMONITOR
mpiotf_lam_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@LAM_CPPFLAGS@)
mpiotf_lam_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@LAM_LDFLAGS@)
mpiotf_lam_rt_offline_la_LIBADD = $(call rt_offline_lam_LIBADD_generate,@LAM_LIBS@)
mpiotf_lam_rt_offline_la_DEPENDENCIES = libvt-lam-mpi-a
mpiotf_lam_rt_offline_la_SOURCES = $(rt_offline_sources)

libmpiotf_lam_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@LAM_CPPFLAGS@)
libmpiotf_lam_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@LAM_LDFLAGS@)
libmpiotf_lam_offline_la_LIBADD = $(call lib_offline_lam_LIBADD_generate,@LAM_LIBS@)
libmpiotf_lam_offline_la_DEPENDENCIES = libvt-lam-mpi-a
libmpiotf_lam_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpiotf_lam_offline_la_SOURCES =
endif

if HAVE_TARGET_LIBMONITOR
libmpiotf_lam_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@LAM_CPPFLAGS@)
libmpiotf_lam_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@LAM_LDFLAGS@)
libmpiotf_lam_offline_la_LIBADD = $(call lib_offline_lam_LIBADD_generate,@LAM_LIBS@)
libmpiotf_lam_offline_la_DEPENDENCIES = libvt-lam-mpi-a
libmpiotf_lam_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpiotf_lam_offline_la_SOURCES =
endif

endif



if HAVE_LAMPI
mpiotf_rt_lampi_la_CFLAGS = $(call rt_CFLAGS_generate,@LAMPI_CPPFLAGS@)
mpiotf_rt_lampi_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@LAMPI_LDFLAGS@)
mpiotf_rt_lampi_la_LIBADD = $(call rt_lampi_LIBADD_generate,@LAMPI_LIBS@)
mpiotf_rt_lampi_la_DEPENDENCIES = libvt-lampi-mpi-a
mpiotf_rt_lampi_la_SOURCES = $(rt_sources)

if HAVE_LIBMONITOR
mpiotf_lampi_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@LAMPI_CPPFLAGS@)
mpiotf_lampi_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@LAMPI_LDFLAGS@)
mpiotf_lampi_rt_offline_la_LIBADD = $(call rt_offline_lampi_LIBADD_generate,@LAMPI_LIBS@)
mpiotf_lampi_rt_offline_la_DEPENDENCIES = libvt-lampi-mpi-a
mpiotf_lampi_rt_offline_la_SOURCES = $(rt_offline_sources)

libmpiotf_lampi_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@LAMPI_CPPFLAGS@)
libmpiotf_lampi_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@LAMPI_LDFLAGS@)
libmpiotf_lampi_offline_la_LIBADD = $(call lib_offline_lampi_LIBADD_generate,@LAMPI_LIBS@)
libmpiotf_lampi_offline_la_DEPENDENCIES = libvt-lampi-mpi-a
libmpiotf_lampi_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpiotf_lampi_offline_la_SOURCES =
endif

if HAVE_TARGET_LIBMONITOR
libmpiotf_lampi_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@LAMPI_CPPFLAGS@)
libmpiotf_lampi_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@LAMPI_LDFLAGS@)
libmpiotf_lampi_offline_la_LIBADD = $(call lib_offline_lampi_LIBADD_generate,@LAMPI_LIBS@)
libmpiotf_lampi_offline_la_DEPENDENCIES = libvt-lampi-mpi-a
libmpiotf_lampi_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpiotf_lampi_offline_la_SOURCES =
endif

endif



if HAVE_MPICH
mpiotf_rt_mpich_la_CFLAGS = $(call rt_CFLAGS_generate,@MPICH_CPPFLAGS@)
mpiotf_rt_mpich_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MPICH_LDFLAGS@)
mpiotf_rt_mpich_la_LIBADD = $(call rt_mpich_LIBADD_generate,@MPICH_LIBS@)
mpiotf_rt_mpich_la_DEPENDENCIES = libvt-mpich-mpi-a
mpiotf_rt_mpich_la_SOURCES = $(rt_sources)

if HAVE_LIBMONITOR
mpiotf_mpich_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@MPICH_CPPFLAGS@)
mpiotf_mpich_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@MPICH_LDFLAGS@)
mpiotf_mpich_rt_offline_la_LIBADD = $(call rt_offline_mpich_LIBADD_generate,@MPICH_LIBS@)
mpiotf_mpich_rt_offline_la_DEPENDENCIES = libvt-mpich-mpi-a
mpiotf_mpich_rt_offline_la_SOURCES = $(rt_offline_sources)

libmpiotf_mpich_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@MPICH_CPPFLAGS@)
libmpiotf_mpich_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MPICH_LDFLAGS@)
libmpiotf_mpich_offline_la_LIBADD = $(call lib_offline_mpich_LIBADD_generate,@MPICH_LIBS@)
libmpiotf_mpich_offline_la_DEPENDENCIES = libvt-mpich-mpi-a
libmpiotf_mpich_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpiotf_mpich_offline_la_SOURCES =
endif
if HAVE_TARGET_LIBMONITOR
libmpiotf_mpich_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@MPICH_CPPFLAGS@)
libmpiotf_mpich_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MPICH_LDFLAGS@)
libmpiotf_mpich_offline_la_LIBADD = $(call lib_offline_mpich_LIBADD_generate,@MPICH_LIBS@)
libmpiotf_mpich_offline_la_DEPENDENCIES = libvt-mpich-mpi-a
libmpiotf_mpich_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpiotf_mpich_offline_la_SOURCES =
endif

endif


if HAVE_MPICH2
mpiotf_rt_mpich2_la_CFLAGS = $(call rt_CFLAGS_generate,@MPICH2_CPPFLAGS@)
mpiotf_rt_mpich2_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MPICH2_LDFLAGS@)
mpiotf_rt_mpich2_la_LIBADD = $(call rt_mpich2_LIBADD_generate,@MPICH2_LIBS@)
mpiotf_rt_mpich2_la_DEPENDENCIES = libvt-mpich2-mpi-a
mpiotf_rt_mpich2_la_SOURCES = $(rt_sources)

if HAVE_LIBMONITOR
mpiotf_mpich2_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@MPICH2_CPPFLAGS@)
mpiotf_mpich2_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@MPICH2_LDFLAGS@)
mpiotf_mpich2_rt_offline_la_LIBADD = $(call rt_offline_mpich2_LIBADD_generate,@MPICH2_LIBS@)
mpiotf_mpich2_rt_offline_la_DEPENDENCIES = libvt-mpich2-mpi-a
mpiotf_mpich2_rt_offline_la_SOURCES = $(rt_offline_sources)

libmpiotf_mpich2_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@MPICH2_CPPFLAGS@)
libmpiotf_mpich2_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MPICH2_LDFLAGS@)
libmpiotf_mpich2_offline_la_LIBADD = $(call lib_offline_mpich2_LIBADD_generate,@MPICH2_LIBS@)
libmpiotf_mpich2_offline_la_DEPENDENCIES = libvt-mpich2-mpi-a
libmpiotf_mpich2_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpiotf_mpich2_offline_la_SOURCES =
endif

endif

if HAVE_TARGET_MPICH2
if HAVE_TARGET_LIBMONITOR

if HAVE_TARGET_SHARED
mpiotf_mpich2_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@TARGET_MPICH2_CPPFLAGS@)
mpiotf_mpich2_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@TARGET_MPICH2_LDFLAGS@)
mpiotf_mpich2_rt_offline_la_LIBADD = $(call rt_offline_mpich2_LIBADD_generate,@TARGET_MPICH2_LIBS@)
mpiotf_mpich2_rt_offline_la_DEPENDENCIES = libvt-mpich2-mpi-a
mpiotf_mpich2_rt_offline_la_SOURCES = $(rt_offline_sources)
endif

libmpiotf_mpich2_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@TARGET_MPICH2_CPPFLAGS@)
libmpiotf_mpich2_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@TARGET_MPICH2_LDFLAGS@)
libmpiotf_mpich2_offline_la_LIBADD = $(call lib_offline_mpich2_LIBADD_generate,@TARGET_MPICH2_LIBS@)
libmpiotf_mpich2_offline_la_DEPENDENCIES = libvt-mpich2-mpi-a
libmpiotf_mpich2_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpiotf_mpich2_offline_la_SOURCES =
endif
endif


if HAVE_MPT
mpiotf_rt_mpt_la_CFLAGS = -DSGI_MPT $(call rt_CFLAGS_generate,@MPT_CPPFLAGS@)
mpiotf_rt_mpt_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MPT_LDFLAGS@)
mpiotf_rt_mpt_la_LIBADD = $(call rt_mpt_LIBADD_generate,@MPT_LIBS@)
mpiotf_rt_mpt_la_DEPENDENCIES = libvt-mpt-mpi-a
mpiotf_rt_mpt_la_SOURCES = $(rt_sources)

if HAVE_LIBMONITOR
mpiotf_mpt_rt_offline_la_CFLAGS = -DSGI_MPT $(call rt_offline_CFLAGS_generate,@MPT_CPPFLAGS@)
mpiotf_mpt_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@MPT_LDFLAGS@)
mpiotf_mpt_rt_offline_la_LIBADD = $(call rt_offline_mpt_LIBADD_generate,@MPT_LIBS@)
mpiotf_mpt_rt_offline_la_DEPENDENCIES = libvt-mpt-mpi-a
mpiotf_mpt_rt_offline_la_SOURCES = $(rt_offline_sources)

libmpiotf_mpt_offline_la_CFLAGS = -DSGI_MPT -static $(call lib_offline_CFLAGS_generate,@MPT_CPPFLAGS@)
libmpiotf_mpt_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MPT_LDFLAGS@)
libmpiotf_mpt_offline_la_LIBADD = $(call lib_offline_mpt_LIBADD_generate,@MPT_LIBS@)
libmpiotf_mpt_offline_la_DEPENDENCIES = libvt-mpt-mpi-a
libmpiotf_mpt_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpiotf_mpt_offline_la_SOURCES =
endif
if HAVE_TARGET_LIBMONITOR
libmpiotf_mpt_offline_la_CFLAGS = -DSGI_MPT -static $(call lib_offline_CFLAGS_generate,@MPT_CPPFLAGS@)
libmpiotf_mpt_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MPT_LDFLAGS@)
libmpiotf_mpt_offline_la_LIBADD = $(call lib_offline_mpt_LIBADD_generate,@MPT_LIBS@)
libmpiotf_mpt_offline_la_DEPENDENCIES = libvt-mpt-mpi-a
libmpiotf_mpt_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpiotf_mpt_offline_la_SOURCES =
endif
endif

if HAVE_MVAPICH
mpiotf_rt_mvapich_la_CFLAGS = $(call rt_CFLAGS_generate,@MVAPICH_CPPFLAGS@)
mpiotf_rt_mvapich_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MVAPICH_LDFLAGS@)
mpiotf_rt_mvapich_la_LIBADD = $(call rt_mvapich_LIBADD_generate,@MVAPICH_LIBS@)
mpiotf_rt_mvapich_la_DEPENDENCIES = libvt-mvapich-mpi-a
mpiotf_rt_mvapich_la_SOURCES = $(rt_sources)

if HAVE_LIBMONITOR
mpiotf_mvapich_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@MVAPICH_CPPFLAGS@)
mpiotf_mvapich_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@MVAPICH_LDFLAGS@)
mpiotf_mvapich_rt_offline_la_LIBADD = $(call rt_offline_mvapich_LIBADD_generate,@MVAPICH_LIBS@)
mpiotf_mvapich_rt_offline_la_DEPENDENCIES = libvt-mvapich-mpi-a
mpiotf_mvapich_rt_offline_la_SOURCES = $(rt_offline_sources)

libmpiotf_mvapich_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@MVAPICH_CPPFLAGS@)
libmpiotf_mvapich_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MVAPICH_LDFLAGS@)
libmpiotf_mvapich_offline_la_LIBADD = $(call lib_offline_mvapich_LIBADD_generate,@MVAPICH_LIBS@)
libmpiotf_mvapich_offline_la_DEPENDENCIES = libvt-mvapich-mpi-a
libmpiotf_mvapich_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpiotf_mvapich_offline_la_SOURCES =
endif
if HAVE_TARGET_LIBMONITOR
libmpiotf_mvapich_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@MVAPICH_CPPFLAGS@)
libmpiotf_mvapich_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MVAPICH_LDFLAGS@)
libmpiotf_mvapich_offline_la_LIBADD = $(call lib_offline_mvapich_LIBADD_generate,@MVAPICH_LIBS@)
libmpiotf_mvapich_offline_la_DEPENDENCIES = libvt-mvapich-mpi-a
libmpiotf_mvapich_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpiotf_mvapich_offline_la_SOURCES =
endif
endif

if HAVE_MVAPICH2
mpiotf_rt_mvapich2_la_CFLAGS = $(call rt_CFLAGS_generate,@MVAPICH2_CPPFLAGS@)
mpiotf_rt_mvapich2_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@MVAPICH2_LDFLAGS@)
mpiotf_rt_mvapich2_la_LIBADD = $(call rt_mvapich2_LIBADD_generate,@MVAPICH2_LIBS@)
mpiotf_rt_mvapich2_la_DEPENDENCIES = libvt-mvapich2-mpi-a
mpiotf_rt_mvapich2_la_SOURCES = $(rt_sources)

if HAVE_LIBMONITOR
mpiotf_mvapich2_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@MVAPICH2_CPPFLAGS@)
mpiotf_mvapich2_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@MVAPICH2_LDFLAGS@)
mpiotf_mvapich2_rt_offline_la_LIBADD = $(call rt_offline_mvapich2_LIBADD_generate,@MVAPICH2_LIBS@)
mpiotf_mvapich2_rt_offline_la_DEPENDENCIES = libvt-mvapich2-mpi-a
mpiotf_mvapich2_rt_offline_la_SOURCES = $(rt_offline_sources)

libmpiotf_mvapich2_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@MVAPICH2_CPPFLAGS@)
libmpiotf_mvapich2_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MVAPICH2_LDFLAGS@)
libmpiotf_mvapich2_offline_la_LIBADD = $(call lib_offline_mvapich2_LIBADD_generate,@MVAPICH2_LIBS@)
libmpiotf_mvapich2_offline_la_DEPENDENCIES = libvt-mvapich2-mpi-a
libmpiotf_mvapich2_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpiotf_mvapich2_offline_la_SOURCES =
endif

if HAVE_TARGET_LIBMONITOR
libmpiotf_mvapich2_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@MVAPICH2_CPPFLAGS@)
libmpiotf_mvapich2_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@MVAPICH2_LDFLAGS@)
libmpiotf_mvapich2_offline_la_LIBADD = $(call lib_offline_mvapich2_LIBADD_generate,@MVAPICH2_LIBS@)
libmpiotf_mvapich2_offline_la_DEPENDENCIES = libvt-mvapich2-mpi-a
libmpiotf_mvapich2_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpiotf_mvapich2_offline_la_SOURCES =
endif

endif

if HAVE_OPENMPI
mpiotf_rt_openmpi_la_CFLAGS = $(call rt_CFLAGS_generate,@OPENMPI_CPPFLAGS@)
mpiotf_rt_openmpi_la_LDFLAGS = -module $(call rt_LDFLAGS_generate,@OPENMPI_LDFLAGS@)
mpiotf_rt_openmpi_la_LIBADD = $(call rt_openmpi_LIBADD_generate,@OPENMPI_LIBS@)
mpiotf_rt_openmpi_la_DEPENDENCIES = libvt-openmpi-mpi-a
mpiotf_rt_openmpi_la_SOURCES = $(rt_sources)

if HAVE_LIBMONITOR
mpiotf_openmpi_rt_offline_la_CFLAGS = $(call rt_offline_CFLAGS_generate,@OPENMPI_CPPFLAGS@)
mpiotf_openmpi_rt_offline_la_LDFLAGS = -module $(call rt_offline_LDFLAGS_generate,@OPENMPI_LDFLAGS@)
mpiotf_openmpi_rt_offline_la_LIBADD = $(call rt_offline_openmpi_LIBADD_generate,@OPENMPI_LIBS@)
mpiotf_openmpi_rt_offline_la_DEPENDENCIES = libvt-openmpi-mpi-a
mpiotf_openmpi_rt_offline_la_SOURCES = $(rt_offline_sources)

libmpiotf_openmpi_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@OPENMPI_CPPFLAGS@)
libmpiotf_openmpi_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@OPENMPI_LDFLAGS@)
libmpiotf_openmpi_offline_la_LIBADD = $(call lib_offline_openmpi_LIBADD_generate,@OPENMPI_LIBS@)
libmpiotf_openmpi_offline_la_DEPENDENCIES = libvt-openmpi-mpi-a
libmpiotf_openmpi_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpiotf_openmpi_offline_la_SOURCES =
endif
if HAVE_TARGET_LIBMONITOR
libmpiotf_openmpi_offline_la_CFLAGS = -static $(call lib_offline_CFLAGS_generate,@OPENMPI_CPPFLAGS@)
libmpiotf_openmpi_offline_la_LDFLAGS = $(call lib_offline_LDFLAGS_generate,@OPENMPI_LDFLAGS@)
libmpiotf_openmpi_offline_la_LIBADD = $(call lib_offline_openmpi_LIBADD_generate,@OPENMPI_LIBS@)
libmpiotf_openmpi_offline_la_DEPENDENCIES = libvt-openmpi-mpi-a
libmpiotf_openmpi_offline_la_SOURCES = $(lib_offline_sources)
nodist_libmpiotf_openmpi_offline_la_SOURCES =
endif

endif

endif


CLEANFILES = $(BUILT_SOURCES) 
if HAVE_LAM
CLEANFILES += libvt.mpi.lam.a
endif
if HAVE_LAMPI
CLEANFILES += libvt.mpi.lampi.a
endif
if HAVE_MPICH
CLEANFILES += libvt.mpi.mpich.a
endif
if HAVE_MPICH2
CLEANFILES += libvt.mpi.mpich2.a
endif
if HAVE_TARGET_MPICH2
CLEANFILES += libvt.mpi.mpich2.a
endif
if HAVE_MPT
CLEANFILES += libvt.mpi.mpt.a
endif
if HAVE_MVAPICH
CLEANFILES += libvt.mpi.mvapich.a
endif
if HAVE_MVAPICH2
CLEANFILES += libvt.mpi.mvapich2.a
endif
if HAVE_OPENMPI
CLEANFILES += libvt.mpi.openmpi.a
endif
# endif for BUILD_INSTRUMENTOR
endif

SUFFIXES = .x 
blobs.h blobs.c: blobs.x 
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

EXTRA_DIST = vt_openss.h

