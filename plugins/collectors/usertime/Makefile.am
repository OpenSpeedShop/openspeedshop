################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2007 William Hachfeld. All Rights Reserved.
# Copyright (c) 2006-2014 Krell Institute. All Rights Reserved.
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
pkglib_LTLIBRARIES += usertime.la
endif
endif

if BUILD_INSTRUMENTOR
if !BUILD_OFFLINE
if !BUILD_CBTF
pkglib_LTLIBRARIES += usertime-rt.la
endif
endif
endif

if BUILD_INSTRUMENTOR
if HAVE_LIBMONITOR
if !BUILD_CBTF
OFFLINE_SOURCES = \
	offline.c

pkglib_LTLIBRARIES += usertime-rt-offline.la libusertime-offline.la
# endif (below) for !BUILD_CBT
endif
# endif (below) for if HAVE_LIBMONITOR
endif

if HAVE_TARGET_LIBMONITOR
if !BUILD_CBTF
OFFLINE_SOURCES = \
	offline.c

pkglib_LTLIBRARIES += libusertime-offline.la
if HAVE_TARGET_SHARED
pkglib_LTLIBRARIES += usertime-rt-offline.la
endif

# endif (below) for !BUILD_CBT
endif
# endif (below) for if HAVE_TARGET_LIBMONITOR
endif

# endif (below) for if BUILD_INSTRUMENTOR
endif

if !BUILD_RUNTIME_ONLY
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
#endif (below) for if !BUILD_RUNTIME_ONLY
endif

if BUILD_INSTRUMENTOR
if !BUILD_CBTF
if !BUILD_OFFLINE

if BUILD_MRNET
usertime_rt_la_CFLAGS = \
	-DUSE_FASTTRACE \
        -I$(top_srcdir)/libopenss-runtime \
	@LIBUNWIND_CPPFLAGS@
else
usertime_rt_la_CFLAGS = \
	-DUSE_FASTTRACE \
	-I$(top_srcdir)/libopenss-runtime \
	@LIBUNWIND_CPPFLAGS@
endif

usertime_rt_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined -module -avoid-version

usertime_rt_la_LIBADD = \
	-lopenss-runtime \
	-lopenss-runtime-unwind \
	@LIBUNWIND_LIBS@ \
	-lpthread -lrt

usertime_rt_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	runtime.c
#endif (below) for if !BUILD_OFFLINE
endif
#endif (below) for if !BUILD_CBTF
endif

if HAVE_LIBMONITOR
usertime_rt_offline_la_CFLAGS = \
	-DOPENSS_OFFLINE \
	-DUSE_FASTTRACE \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	@LIBUNWIND_CPPFLAGS@

usertime_rt_offline_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
	-L$(top_srcdir)/libopenss-runtime/offline \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined -module -avoid-version

usertime_rt_offline_la_LIBADD = \
	-lopenss-runtime-offline -lopenss-runtime-monitor \
	-lopenss-runtime-unwind \
	@LIBUNWIND_LIBS@ \
	-lpthread -lrt

usertime_rt_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	runtime.c

libusertime_offline_la_CFLAGS = \
	-static \
	-DOPENSS_OFFLINE \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	@LIBUNWIND_CPPFLAGS@

libusertime_offline_la_LDFLAGS = \
	-static \
	-L$(top_srcdir)/libopenss-runtime \
	-L$(top_srcdir)/libopenss-runtime/offline \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined

libusertime_offline_la_LIBADD = \
	-lopenss-runtime-offline -lopenss-runtime-monitor \
	-lopenss-runtime-unwind \
	@LIBUNWIND_LIBS@

libusertime_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	runtime.c
endif
endif

if HAVE_TARGET_LIBMONITOR

if !BUILD_CBTF
if HAVE_TARGET_SHARED

usertime_rt_offline_la_CFLAGS = \
	-DOPENSS_OFFLINE \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	@TARGET_LIBUNWIND_CPPFLAGS@

usertime_rt_offline_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
	-L$(top_srcdir)/libopenss-runtime/offline \
	@TARGET_LIBUNWIND_LDFLAGS@ \
	-no-undefined -module -avoid-version

usertime_rt_offline_la_LIBADD = \
	-lopenss-runtime-offline -lopenss-runtime-monitor \
	-lopenss-runtime-unwind \
	@TARGET_LIBUNWIND_LIBS@ \
	-lpthread -lrt

usertime_rt_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	runtime.c

endif

libusertime_offline_la_CFLAGS = \
	-static \
	-DOPENSS_OFFLINE \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	@TARGET_LIBUNWIND_CPPFLAGS@

libusertime_offline_la_LDFLAGS = \
	-static \
	-L$(top_srcdir)/libopenss-runtime \
	-L$(top_srcdir)/libopenss-runtime/offline \
	@TARGET_LIBUNWIND_LDFLAGS@ \
	-no-undefined

libusertime_offline_la_LIBADD = \
	-lopenss-runtime-offline -lopenss-runtime-monitor \
	-lopenss-runtime-unwind \
	@TARGET_LIBUNWIND_LIBS@

libusertime_offline_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	$(OFFLINE_SOURCES) \
	runtime.c
endif
endif

SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES)

blobs.h blobs.c : blobs.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<
