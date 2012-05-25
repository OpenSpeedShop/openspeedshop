################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2012 Krell Institute All Rights Reserved.
# Copyright (c) 2008 William Hachfeld. All Rights Reserved.
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

BUILT_SOURCES = \
	blobs.h blobs.c

pkglib_LTLIBRARIES =

if !BUILD_RUNTIME_ONLY
if !HAVE_TARGET_LIBMONITOR
pkglib_LTLIBRARIES += pcsamp.la
endif
endif

if BUILD_INSTRUMENTOR
if !BUILD_OFFLINE
if !BUILD_CBTF
pkglib_LTLIBRARIES += pcsamp-rt.la
endif
endif
endif

if BUILD_INSTRUMENTOR
if HAVE_LIBMONITOR
if !HAVE_TARGET_LIBMONITOR
if !BUILD_CBTF
OFFLINE_SOURCES = \
	offline.c

pkglib_LTLIBRARIES += pcsamp-rt-offline.la libpcsamp-offline.la
endif
endif
endif

if HAVE_TARGET_LIBMONITOR
if !BUILD_CBTF
OFFLINE_SOURCES = \
	offline.c

pkglib_LTLIBRARIES += libpcsamp-offline.la
if HAVE_TARGET_SHARED
pkglib_LTLIBRARIES += pcsamp-rt-offline.la
endif
endif
endif

endif

if !BUILD_RUNTIME_ONLY
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

endif

if BUILD_INSTRUMENTOR
if !BUILD_CBTF
if !BUILD_OFFLINE
if BUILD_MRNET
pcsamp_rt_la_CFLAGS = \
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
endif

if HAVE_LIBMONITOR
if !HAVE_TARGET_LIBMONITOR
if !BUILD_CBTF
pcsamp_rt_offline_la_CFLAGS = \
	-DOPENSS_OFFLINE \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline

pcsamp_rt_offline_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
	-L$(top_srcdir)/libopenss-runtime/offline \
	-no-undefined -module -avoid-version

pcsamp_rt_offline_la_LIBADD = \
	-lopenss-runtime-offline -lopenss-runtime-monitor \
	-lpthread -lrt

pcsamp_rt_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	runtime.c

libpcsamp_offline_la_CFLAGS = \
	-static \
	-DOPENSS_OFFLINE \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline

libpcsamp_offline_la_LDFLAGS = \
	-static \
	-L$(top_srcdir)/libopenss-runtime \
	-L$(top_srcdir)/libopenss-runtime/offline \
	-no-undefined

libpcsamp_offline_la_LIBADD = \
	-lopenss-runtime-offline -lopenss-runtime-monitor \
	-lpthread -lrt

libpcsamp_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	runtime.c
endif
endif
endif

if HAVE_TARGET_LIBMONITOR

if !BUILD_CBTF
if HAVE_TARGET_SHARED
pcsamp_rt_offline_la_CFLAGS = \
        -DOPENSS_OFFLINE \
        -I$(top_srcdir)/libopenss-runtime \
        -I$(top_srcdir)/libopenss-runtime/offline

pcsamp_rt_offline_la_LDFLAGS = \
        -L$(top_srcdir)/libopenss-runtime \
        -L$(top_srcdir)/libopenss-runtime/offline \
        -no-undefined -module -avoid-version

pcsamp_rt_offline_la_LIBADD = \
        -lopenss-runtime-offline -lopenss-runtime-monitor \
        -lpthread -lrt

pcsamp_rt_offline_la_SOURCES = \
        blobs.x $(BUILT_SOURCES) \
        $(OFFLINE_SOURCES) \
        runtime.c

endif

libpcsamp_offline_la_CFLAGS = \
	-static \
	-DOPENSS_OFFLINE \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline

libpcsamp_offline_la_LDFLAGS = \
	-static \
	-L$(top_srcdir)/libopenss-runtime \
	-L$(top_srcdir)/libopenss-runtime/offline \
	-no-undefined

libpcsamp_offline_la_LIBADD = \
	-lopenss-runtime-offline -lopenss-runtime-monitor \
	-lpthread -lrt

libpcsamp_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	runtime.c
endif
endif
endif



SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES)

blobs.h blobs.c : blobs.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<
