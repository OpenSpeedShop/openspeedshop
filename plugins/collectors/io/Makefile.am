################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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

IO_BUILT_SOURCES = \
	blobs.h blobs.c

BUILT_SOURCES = $(IO_BUILT_SOURCES)

pkglib_LTLIBRARIES = io.la io-rt.la

### IO

io_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-framework

io_la_LDFLAGS = \
	-no-undefined -module -avoid-version

io_la_SOURCES = \
	blobs.x $(IO_BUILT_SOURCES) \
	IOCollector.hxx IODetail.hxx IOCollector.cxx

io_rt_la_CFLAGS = \
	-I$(top_srcdir)/libopenss-runtime \
	@LIBUNWIND_CPPFLAGS@

io_rt_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-runtime \
	@LIBUNWIND_LDFLAGS@ \
	-no-undefined -module -avoid-version

io_rt_la_LIBADD = \
	-lopenss-runtime \
	@LIBUNWIND_LIBS@ \
	-lpthread -lrt

io_rt_la_SOURCES = \
	blobs.x $(IO_BUILT_SOURCES) \
	wrappers.c \
	runtime.h runtime.c

SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES)
EXTRA_DIST = blobs.x

blobs.h blobs.c : blobs.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<
