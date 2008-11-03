################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006,2007 Krell Institute All Rights Reserved.
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

pkglib_LTLIBRARIES += pcsamp.la

if !BUILD_OFFLINE
pkglib_LTLIBRARIES += pcsamp-rt.la
endif

if HAVE_LIBMONITOR
OFFLINE_SOURCES = \
	pcsamp_offline.c pcsamp_offline.h

pkglib_LTLIBRARIES += pcsamp-rt-offline.la libpcsamp-offline.la
endif

pcsamp_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-framework

pcsamp_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-framework \
	-no-undefined -module -avoid-version

pcsamp_la_LIBADD = \
	-lopenss-framework

pcsamp_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	PCSampCollector.hxx PCSampCollector.cxx

if !BUILD_OFFLINE
if BUILD_MRNET
pcsamp_rt_la_CFLAGS = \
	-DOPENSS_USE_FILEIO -I$(top_srcdir)/libopenss-runtime/fileio  \
	-I$(top_srcdir)/libopenss-runtime
else
pcsamp_rt_la_CFLAGS = \
	-I$(top_srcdir)/libopenss-runtime
endif

pcsamp_rt_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
	-no-undefined -module -avoid-version

pcsamp_rt_la_LIBADD = \
	-lopenss-runtime \
	-lpthread -lrt

pcsamp_rt_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	runtime.c
endif

if HAVE_LIBMONITOR
pcsamp_rt_offline_la_CFLAGS = \
        @BINUTILS_CPPFLAGS@ \
	@LIBMONITOR_CPPFLAGS@ \
	-DOPENSS_OFFLINE -DOPENSS_USE_FILEIO \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/fileio \
	-I$(top_srcdir)/libopenss-runtime/offline

pcsamp_rt_offline_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
        @BINUTILS_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	-no-undefined -module -avoid-version

pcsamp_rt_offline_la_LIBADD = \
        @BINUTILS_LIBS@ \
	@LIBMONITOR_LIBS@ \
	$(top_srcdir)/libopenss-runtime/libopenss-runtime-offline.la

pcsamp_rt_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	runtime.c

libpcsamp_offline_la_CFLAGS = \
	-static -DOPENSS_OFFLINE -DOPENSS_USE_FILEIO \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/fileio \
	-I$(top_srcdir)/libopenss-runtime/offline \
        @BINUTILS_CPPFLAGS@ \
        @LIBMONITOR_CPPFLAGS@

libpcsamp_offline_la_LDFLAGS = \
	-static -L$(top_srcdir)/libopenss-runtime \
        @BINUTILS_LDFLAGS@ \
	@LIBMONITOR_LDFLAGS@ \
	-no-undefined

libpcsamp_offline_la_LIBADD = \
        @BINUTILS_LIBS@ \
	@LIBMONITOR_LIBS@ \
	$(top_srcdir)/libopenss-runtime/libopenss-runtime-offline.la

libpcsamp_offline_la_SOURCES = \
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
