################################################################################
# Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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

if !BUILD_OFFLINE
pkglib_LTLIBRARIES += fpe.la fpe-rt.la

fpe_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-framework

fpe_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-framework \
	-no-undefined -module -avoid-version

fpe_la_LIBADD = \
	-lopenss-framework

fpe_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	FPECollector.hxx FPECollector.cxx \
	FPEDetail.hxx

fpe_rt_la_CFLAGS = \
	-I$(top_srcdir)/libopenss-runtime \
	@BINUTILS_CPPFLAGS@ \
	@LIBUNWIND_CPPFLAGS@

fpe_rt_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
	@BINUTILS_LDFLAGS@ \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined -module -avoid-version

fpe_rt_la_LIBADD = \
	-lopenss-runtime \
	@BINUTILS_LIBS@ \
	@LIBUNWIND_LIBS@ \
	-lpthread -lrt -lm

fpe_rt_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	runtime.c
endif

SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES)

blobs.h blobs.c : blobs.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<
