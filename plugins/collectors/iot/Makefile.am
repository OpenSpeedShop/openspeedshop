################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2007 William Hachfeld. All Rights Reserved.
# Copyright (c) 2008 The Krell Institute. All Rights Reserved.
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

pkglib_LTLIBRARIES += iot.la

if !BUILD_OFFLINE
pkglib_LTLIBRARIES += iot-rt.la
endif

if HAVE_LIBMONITOR
OFFLINE_SOURCES = \
	iot_offline.c iot_offline.h

pkglib_LTLIBRARIES += iot-rt-offline.la libiot-offline.la
endif

iot_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-framework

iot_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-framework \
	-no-undefined -module -avoid-version

iot_la_LIBADD = \
	-lopenss-framework

iot_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	IOTCollector.hxx IOTDetail.hxx IOTCollector.cxx

if !BUILD_OFFLINE
if BUILD_MRNET
iot_rt_la_CFLAGS = \
	-DOPENSS_USE_FILEIO -I$(top_srcdir)/libopenss-runtime/fileio  \
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
	@LIBUNWIND_LIBS@ \
	-lpthread -lrt

iot_rt_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	wrappers.c \
	runtime.h runtime.c
endif

if HAVE_LIBMONITOR
iot_rt_offline_la_CFLAGS = \
	-DOPENSS_OFFLINE -DOPENSS_USE_FILEIO \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/fileio \
	-I$(top_srcdir)/libopenss-runtime/offline \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@

iot_rt_offline_la_LDFLAGS = \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	-L$(top_srcdir)/libopenss-runtime \
	-no-undefined -module -avoid-version

iot_rt_offline_la_LIBADD = \
	@LIBUNWIND_LIBS@ \
	@LIBMONITOR_LIBS@ \
	$(top_srcdir)/libopenss-runtime/libopenss-runtime-offline.la \
	-lpthread -lrt

iot_rt_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	wrappers.c \
	runtime.h runtime.c

libiot_offline_la_CFLAGS = \
	-static -DOPENSS_OFFLINE -DOPENSS_USE_FILEIO \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/fileio \
	-I$(top_srcdir)/libopenss-runtime/offline \
	@LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@

libiot_offline_la_LDFLAGS = \
	-static -L$(top_srcdir)/libopenss-runtime \
	@LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined -module -avoid-version

libiot_offline_la_LIBADD = \
	@LIBUNWIND_LIBS@ \
	@LIBMONITOR_LIBS@ \
	$(top_srcdir)/libopenss-runtime/libopenss-runtime-offline.la \
	-lpthread -lrt

libiot_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	wrappers.c \
	runtime.h runtime.c
endif

SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES)

blobs.h blobs.c : blobs.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<
