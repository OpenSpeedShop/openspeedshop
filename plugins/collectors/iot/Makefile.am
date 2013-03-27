################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2007 William Hachfeld. All Rights Reserved.
# Copyright (c) 2006-2013 The Krell Institute. All Rights Reserved.
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

#EXTRA_DIST = IOTTraceableFunctions.h

BUILT_SOURCES = \
	blobs.h blobs.c

pkglib_LTLIBRARIES =

if !BUILD_RUNTIME_ONLY
if !HAVE_TARGET_LIBMONITOR
pkglib_LTLIBRARIES += iot.la
endif
endif

if BUILD_INSTRUMENTOR
if !BUILD_CBTF
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += iot-rt.la
endif

if HAVE_LIBMONITOR
OFFLINE_SOURCES = \
	offline.c

pkglib_LTLIBRARIES += iot-rt-offline.la libiot-offline.la
endif

if HAVE_TARGET_LIBMONITOR
OFFLINE_SOURCES = \
	offline.c

if HAVE_TARGET_SHARED
pkglib_LTLIBRARIES += iot-rt-offline.la
endif

pkglib_LTLIBRARIES += libiot-offline.la
endif
endif
endif

if !BUILD_RUNTIME_ONLY
iot_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-framework

iot_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-framework \
	-no-undefined -module -avoid-version

iot_la_LIBADD = \
	-lopenss-framework

iot_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	IOTTraceableFunctions.h \
	IOTCollector.hxx IOTDetail.hxx IOTCollector.cxx
endif

if BUILD_INSTRUMENTOR
if !BUILD_CBTF

if !BUILD_OFFLINE
if BUILD_MRNET
iot_rt_la_CFLAGS = \
	-I$(top_srcdir)/libopenss-runtime \
	@LIBUNWIND_CPPFLAGS@
else
iot_rt_la_CFLAGS = \
	-I$(top_srcdir)/libopenss-runtime \
	@LIBUNWIND_CPPFLAGS@
endif

iot_rt_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined -module -avoid-version

iot_rt_la_LIBADD = \
	-lopenss-runtime \
	-lopenss-runtime-unwind \
	@LIBUNWIND_LIBS@ \
	-lpthread -lrt

iot_rt_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	wrappers.c \
	runtime.h runtime.c
endif

if HAVE_LIBMONITOR
iot_rt_offline_la_CFLAGS = \
	-DOPENSS_OFFLINE \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	@LIBUNWIND_CPPFLAGS@

iot_rt_offline_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
	-L$(top_srcdir)/libopenss-runtime/offline \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined -module -avoid-version

iot_rt_offline_la_LIBADD = \
	-lopenss-runtime-offline -lopenss-runtime-monitor \
	-lopenss-runtime-unwind \
	@LIBUNWIND_LIBS@ \
	-lpthread -lrt

iot_rt_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	wrappers.c \
	runtime.h runtime.c

libiot_offline_la_CFLAGS = \
	-static \
	-DOPENSS_OFFLINE -DOPENSS_STATIC \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	@LIBUNWIND_CPPFLAGS@

libiot_offline_la_LDFLAGS = \
	-static \
	-L$(top_srcdir)/libopenss-runtime \
	-L$(top_srcdir)/libopenss-runtime/offline \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined -module -avoid-version

libiot_offline_la_LIBADD = \
	$(top_srcdir)/libopenss-runtime/libopenss-runtime-offline.la \
	$(top_srcdir)/libopenss-runtime/offline/libopenss-runtime-monitor.la \
	-lopenss-runtime-unwind \
	@LIBUNWIND_LIBS@ \
	-lpthread -lrt

libiot_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	wrappers.c \
	runtime.h runtime.c
endif

if HAVE_TARGET_LIBMONITOR

if HAVE_TARGET_SHARED
iot_rt_offline_la_CFLAGS = \
	-DOPENSS_OFFLINE \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	@TARGET_LIBUNWIND_CPPFLAGS@

iot_rt_offline_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
	-L$(top_srcdir)/libopenss-runtime/offline \
	@TARGET_LIBUNWIND_LDFLAGS@ \
	-no-undefined -module -avoid-version

iot_rt_offline_la_LIBADD = \
	-lopenss-runtime-offline -lopenss-runtime-monitor \
	-lopenss-runtime-unwind \
	@TARGET_LIBUNWIND_LIBS@ \
	-lpthread -lrt

iot_rt_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	wrappers.c \
	runtime.h runtime.c

endif

libiot_offline_la_CFLAGS = \
	-static \
	-DOPENSS_OFFLINE -DOPENSS_STATIC \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	@TARGET_LIBUNWIND_CPPFLAGS@

libiot_offline_la_LDFLAGS = \
	-static \
	-L$(top_srcdir)/libopenss-runtime \
	-L$(top_srcdir)/libopenss-runtime/offline \
	@TARGET_LIBUNWIND_LDFLAGS@ \
	-no-undefined -module -avoid-version

libiot_offline_la_LIBADD = \
	$(top_srcdir)/libopenss-runtime/libopenss-runtime-offline.la \
	$(top_srcdir)/libopenss-runtime/offline/libopenss-runtime-monitor.la \
	-lopenss-runtime-unwind \
	@TARGET_LIBUNWIND_LIBS@ \
	-lpthread -lrt

libiot_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	wrappers.c \
	runtime.h runtime.c
endif

endif

endif

SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES)

blobs.h blobs.c : blobs.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<
