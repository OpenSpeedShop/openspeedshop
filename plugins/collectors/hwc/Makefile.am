################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2007 William Hachfeld. All Rights Reserved.
# Copyright (c) 2006, 2007 Krell Institute. All Rights Reserved.
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

pkglib_LTLIBRARIES += hwc.la

if !BUILD_OFFLINE
pkglib_LTLIBRARIES += hwc-rt.la
endif

if HAVE_LIBMONITOR
OFFLINE_SOURCES = \
	hwc_offline.c hwc_offline.h

pkglib_LTLIBRARIES += hwc-rt-offline.la libhwc-offline.la
endif

hwc_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-framework \
	-I$(top_srcdir)/libopenss-message \
	@PAPI_CPPFLAGS@

hwc_la_LDFLAGS = \
	@PAPI_LDFLAGS@ \
	-L$(top_srcdir)/libopenss-message \
	-L$(top_srcdir)/libopenss-framework \
	-no-undefined -module -avoid-version

hwc_la_LIBADD = \
	@PAPI_LIBS@ \
	-lopenss-message \
	-lopenss-framework

hwc_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	HWCCollector.hxx HWCCollector.cxx

if !BUILD_OFFLINE
if BUILD_MRNET
hwc_rt_la_CFLAGS = \
	-DOPENSS_USE_FILEIO -I$(top_srcdir)/libopenss-runtime/fileio  \
	-I$(top_srcdir)/libopenss-runtime \
	@PAPI_CPPFLAGS@
else
hwc_rt_la_CFLAGS = \
	-I$(top_srcdir)/libopenss-runtime \
	@PAPI_CPPFLAGS@
endif

hwc_rt_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
	@PAPI_LDFLAGS@ \
	-no-undefined -module -avoid-version

hwc_rt_la_LIBADD = \
	-lopenss-runtime \
	@PAPI_LIBS@ \
	-lpthread -lrt

hwc_rt_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	runtime.c
endif

if HAVE_LIBMONITOR
hwc_rt_offline_la_CFLAGS = \
	-DOPENSS_OFFLINE -DOPENSS_USE_FILEIO \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/fileio \
	-I$(top_srcdir)/libopenss-runtime/offline \
        @LIBMONITOR_CPPFLAGS@ \
	@PAPI_CPPFLAGS@

hwc_rt_offline_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
        @LIBMONITOR_LDFLAGS@ \
	@PAPI_LDFLAGS@ \
	-no-undefined -module -avoid-version

hwc_rt_offline_la_LIBADD = \
        @LIBMONITOR_LIBS@ \
	@PAPI_LIBS@ \
	$(top_srcdir)/libopenss-runtime/libopenss-runtime-offline.la \
	-lpthread -lrt

hwc_rt_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	runtime.c

libhwc_offline_la_CFLAGS = \
        -static -DOPENSS_OFFLINE -DOPENSS_USE_FILEIO \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/fileio \
	-I$(top_srcdir)/libopenss-runtime/offline \
	@LIBMONITOR_CPPFLAGS@ \
	@PAPI_CPPFLAGS@

libhwc_offline_la_LDFLAGS = \
	-static -L$(top_srcdir)/libopenss-runtime \
	@PAPI_LDFLAGS@ \
        @LIBMONITOR_LDFLAGS@ \
	-no-undefined

libhwc_offline_la_LIBADD = \
	@PAPI_LIBS@ \
        @LIBMONITOR_LIBS@ \
	$(top_srcdir)/libopenss-runtime/libopenss-runtime-offline.la \
	-lpthread -lrt

libhwc_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	runtime.c
endif

SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES)

blobs.h blobs.c : blobs.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<
