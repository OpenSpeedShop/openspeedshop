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

pkglib_LTLIBRARIES += usertime.la

if !BUILD_OFFLINE
pkglib_LTLIBRARIES += usertime-rt.la
endif

if HAVE_LIBMONITOR
OFFLINE_SOURCES = \
	usertime_offline.c usertime_offline.h

pkglib_LTLIBRARIES += usertime-rt-offline.la libusertime-offline.la
endif

usertime_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-framework

usertime_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-framework \
	-no-undefined -module -avoid-version

usertime_la_LIBADD = \
	-lopenss-framework

usertime_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	UserTimeCollector.hxx UserTimeCollector.cxx \
	UserTimeDetail.hxx

if !BUILD_OFFLINE
if BUILD_MRNET
usertime_rt_la_CFLAGS = \
        -DOPENSS_USE_FILEIO -I$(top_srcdir)/libopenss-runtime/fileio  \
        -I$(top_srcdir)/libopenss-runtime \
	@LIBUNWIND_CPPFLAGS@
else
usertime_rt_la_CFLAGS = \
	-I$(top_srcdir)/libopenss-runtime \
	@LIBUNWIND_CPPFLAGS@
endif

usertime_rt_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined -module -avoid-version

usertime_rt_la_LIBADD = \
	-lopenss-runtime \
	@LIBUNWIND_LIBS@ \
	-lpthread -lrt

usertime_rt_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	runtime.c
endif

if HAVE_LIBMONITOR
usertime_rt_offline_la_CFLAGS = \
        @BINUTILS_CPPFLAGS@ \
        @LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@ \
	-DOPENSS_OFFLINE -DOPENSS_USE_FILEIO \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/fileio \
	-I$(top_srcdir)/libopenss-runtime/offline

usertime_rt_offline_la_LDFLAGS = \
        @BINUTILS_LDFLAGS@ \
        @LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	-L$(top_srcdir)/libopenss-runtime \
	-no-undefined -module -avoid-version

usertime_rt_offline_la_LIBADD = \
        @BINUTILS_LIBS@ \
        @LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	$(top_srcdir)/libopenss-runtime/libopenss-runtime-offline.la \
	-lpthread -lrt

usertime_rt_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	runtime.c

libusertime_offline_la_CFLAGS = \
	-static -DOPENSS_OFFLINE -DOPENSS_USE_FILEIO \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/fileio \
	-I$(top_srcdir)/libopenss-runtime/offline \
        @BINUTILS_CPPFLAGS@ \
        @LIBMONITOR_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@

libusertime_offline_la_LDFLAGS = \
	-static -L$(top_srcdir)/libopenss-runtime \
        @BINUTILS_LDFLAGS@ \
        @LIBMONITOR_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined

libusertime_offline_la_LIBADD = \
        @BINUTILS_LIBS@ \
        @LIBMONITOR_LIBS@ \
	@LIBUNWIND_LIBS@ \
	$(top_srcdir)/libopenss-runtime/libopenss-runtime-offline.la

libusertime_offline_la_SOURCES = \
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
