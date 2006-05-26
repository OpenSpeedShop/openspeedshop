################################################################################
# Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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

pkglib_LTLIBRARIES = fpe.la fpe-rt.la

fpe_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-framework

fpe_la_LDFLAGS = \
	-no-undefined -module -avoid-version

fpe_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	FPECollector.hxx FPECollector.cxx \
	FPEDetail.hxx

fpe_rt_la_CFLAGS = \
	-I$(top_srcdir)/libopenss-runtime \
	@LIBUNWIND_CPPFLAGS@

fpe_rt_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined -module -avoid-version

fpe_rt_la_LIBADD = \
	-lopenss-runtime \
	@LIBUNWIND_LIBS@ \
	-lpthread -lrt

fpe_rt_la_SOURCES = \
	blobs.x $(BUILT_SOURCES) \
	runtime.c

SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES)

blobs.h blobs.c : blobs.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<