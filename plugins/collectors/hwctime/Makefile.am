################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2007 William Hachfeld. All Rights Reserved.
# Copyright (c) 2006-2010 Krell Institute. All Rights Reserved.
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

pkglib_LTLIBRARIES =

if !BUILD_RUNTIME_ONLY
if !HAVE_TARGET_LIBMONITOR
pkglib_LTLIBRARIES += hwctime.la
endif
endif

if HAVE_PAPI
if BUILD_INSTRUMENTOR
if !BUILD_CBTF
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += hwctime-rt.la
endif
endif
endif
# endif for HAVE_PAPI
endif

if BUILD_INSTRUMENTOR
if !BUILD_CBTF
if HAVE_PAPI
if HAVE_LIBMONITOR
OFFLINE_SOURCES = \
	offline.c

pkglib_LTLIBRARIES += hwctime-rt-offline.la libhwctime-offline.la
endif
# endif for HAVE_PAPI
endif

if HAVE_TARGET_PAPI
if HAVE_TARGET_LIBMONITOR
OFFLINE_SOURCES = \
	offline.c

if HAVE_TARGET_SHARED
if HAVE_TARGET_PAPI_SHARED
pkglib_LTLIBRARIES += hwctime-rt-offline.la
endif
endif

pkglib_LTLIBRARIES += libhwctime-offline.la
endif
# endif for HAVE_TARGET_PAPI
endif
endif
endif



if !BUILD_RUNTIME_ONLY
if !HAVE_TARGET_LIBMONITOR
hwctime_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-framework
else
hwctime_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-framework
endif

hwctime_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-framework \
	-no-undefined -module -avoid-version

hwctime_la_LIBADD = \
	-lopenss-framework

hwctime_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	HWTimeCollector.hxx HWTimeCollector.cxx \
	HWTimeDetail.hxx
endif

if BUILD_INSTRUMENTOR
if !BUILD_CBTF
if HAVE_PAPI

if !BUILD_OFFLINE
if BUILD_MRNET
hwctime_rt_la_CFLAGS = \
	-DUSE_FASTTRACE \
	-I$(top_srcdir)/libopenss-runtime \
	@LIBUNWIND_CPPFLAGS@ \
	@PAPI_CPPFLAGS@
else
hwctime_rt_la_CFLAGS = \
	-DUSE_FASTTRACE \
	-I$(top_srcdir)/libopenss-runtime \
	@LIBUNWIND_CPPFLAGS@ \
	@PAPI_CPPFLAGS@
endif

hwctime_rt_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
	@LIBUNWIND_LDFLAGS@ \
	@PAPI_LDFLAGS@ \
	-no-undefined -module -avoid-version

hwctime_rt_la_LIBADD = \
	-lopenss-runtime \
	-lopenss-runtime-papi \
	-lopenss-runtime-unwind \
	@LIBUNWIND_LIBS@ \
	@PAPI_LIBS@ \
	-lpthread -lrt

hwctime_rt_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	runtime.c
endif

if HAVE_LIBMONITOR
hwctime_rt_offline_la_CFLAGS = \
	-DOPENSS_OFFLINE \
	-DUSE_FASTTRACE \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	@LIBUNWIND_CPPFLAGS@ \
	@PAPI_CPPFLAGS@

hwctime_rt_offline_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
	-L$(top_srcdir)/libopenss-runtime/offline \
	@LIBUNWIND_LDFLAGS@ \
	@PAPI_LDFLAGS@ \
	-no-undefined -module -avoid-version

hwctime_rt_offline_la_LIBADD = \
	-lopenss-runtime-offline -lopenss-runtime-monitor \
	-lopenss-runtime-papi \
	-lopenss-runtime-unwind \
	@LIBUNWIND_LIBS@ \
	@PAPI_LIBS@ \
	-lpthread -lrt

hwctime_rt_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	runtime.c

libhwctime_offline_la_CFLAGS = \
	-static \
	-DOPENSS_OFFLINE \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	@LIBUNWIND_CPPFLAGS@ \
	@PAPI_CPPFLAGS@

libhwctime_offline_la_LDFLAGS = \
	-static \
	-L$(top_srcdir)/libopenss-runtime \
	-L$(top_srcdir)/libopenss-runtime/offline \
	@LIBUNWIND_LDFLAGS@ \
	@PAPI_LDFLAGS@ \
	-no-undefined

libhwctime_offline_la_LIBADD = \
	-lopenss-runtime-offline -lopenss-runtime-monitor \
	-lopenss-runtime-papi \
	-lopenss-runtime-unwind \
	@LIBUNWIND_LIBS@ \
	@PAPI_LIBS@ \
	-lpthread -lrt

libhwctime_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	runtime.c
endif
# endif for HAVE_PAPI
endif

if HAVE_TARGET_PAPI

if HAVE_TARGET_LIBMONITOR

if HAVE_TARGET_SHARED
if HAVE_TARGET_PAPI_SHARED

hwctime_rt_offline_la_CFLAGS = \
	-DOPENSS_OFFLINE \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	@TARGET_LIBUNWIND_CPPFLAGS@ \
	@TARGET_PAPI_CPPFLAGS@

hwctime_rt_offline_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
	-L$(top_srcdir)/libopenss-runtime/offline \
	@TARGET_LIBUNWIND_LDFLAGS@ \
	@TARGET_PAPI_LDFLAGS@ \
	-no-undefined -module -avoid-version

hwctime_rt_offline_la_LIBADD = \
	-lopenss-runtime-offline -lopenss-runtime-monitor \
	-lopenss-runtime-papi \
	-lopenss-runtime-unwind \
	@TARGET_LIBUNWIND_LIBS@ \
	@TARGET_PAPI_LIBS@ \
	-lpthread -lrt

hwctime_rt_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	runtime.c

endif
endif

libhwctime_offline_la_CFLAGS = \
	-static \
	-DOPENSS_OFFLINE \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	@TARGET_LIBUNWIND_CPPFLAGS@ \
	@TARGET_PAPI_CPPFLAGS@

libhwctime_offline_la_LDFLAGS = \
	-static \
	-L$(top_srcdir)/libopenss-runtime \
	-L$(top_srcdir)/libopenss-runtime/offline \
	@TARGET_LIBUNWIND_LDFLAGS@ \
	@TARGET_PAPI_LDFLAGS@ \
	-no-undefined

libhwctime_offline_la_LIBADD = \
	-lopenss-runtime-offline -lopenss-runtime-monitor \
	-lopenss-runtime-papi \
	-lopenss-runtime-unwind \
	@TARGET_LIBUNWIND_LIBS@ \
	@TARGET_PAPI_LIBS@ \
	-lpthread -lrt

libhwctime_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	runtime.c
endif
# endif for HAVE_TARGET_PAPI
endif
endif
endif

SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES)

blobs.h blobs.c : blobs.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<
