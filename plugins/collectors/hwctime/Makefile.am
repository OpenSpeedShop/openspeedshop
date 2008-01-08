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

pkglib_LTLIBRARIES = hwctime.la hwctime-rt.la

if HAVE_LIBMONITOR
OFFLINE_SOURCES = \
	hwctime_offline.c hwctime_offline.h

pkglib_LTLIBRARIES += hwctime-rt-offline.la libhwctime-offline.la
endif

hwctime_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-framework \
	@PAPI_CPPFLAGS@

hwctime_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-framework \
	-no-undefined -module -avoid-version

hwctime_la_LIBADD = \
	-lopenss-framework

hwctime_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	HWTimeCollector.hxx HWTimeCollector.cxx \
	HWTimeDetail.hxx

hwctime_rt_la_CFLAGS = \
	-I$(top_srcdir)/libopenss-runtime \
	@LIBUNWIND_CPPFLAGS@ \
	@PAPI_CPPFLAGS@

hwctime_rt_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
	@LIBUNWIND_LDFLAGS@ \
	@PAPI_LDFLAGS@ \
	-no-undefined -module -avoid-version

hwctime_rt_la_LIBADD = \
	-lopenss-runtime \
	@LIBUNWIND_LIBS@ \
	@PAPI_LIBS@ \
	-lpthread -lrt

hwctime_rt_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	runtime.c

if HAVE_LIBMONITOR
hwctime_rt_offline_la_CFLAGS = \
        @LIBMONITOR_CPPFLAGS@ \
	-DOPENSS_OFFLINE -DOPENSS_USE_FILEIO \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/fileio \
	-I$(top_srcdir)/libopenss-runtime/offline \
	@LIBUNWIND_CPPFLAGS@ \
	@PAPI_CPPFLAGS@

hwctime_rt_offline_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
        @LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@PAPI_LDFLAGS@ \
	-no-undefined -module -avoid-version

hwctime_rt_offline_la_LIBADD = \
	-lopenss-runtime-offline \
        @LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@PAPI_LIBS@ \
	-lpthread -lrt

hwctime_rt_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	runtime.c

libhwctime_offline_la_CFLAGS = \
        @LIBMONITOR_CPPFLAGS@ \
	-DOPENSS_OFFLINE -DOPENSS_USE_FILEIO \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/fileio \
	-I$(top_srcdir)/libopenss-runtime/offline \
	@LIBUNWIND_CPPFLAGS@ \
	@PAPI_CPPFLAGS@

libhwctime_offline_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
        @LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	@PAPI_LDFLAGS@ \
	-static -no-undefined -module -avoid-version

libhwctime_offline_la_LIBADD = \
	-lopenss-runtime-offline \
        @LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	@PAPI_LIBS@ \
	-lpthread -lrt

libhwctime_offline_la_SOURCES = \
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
