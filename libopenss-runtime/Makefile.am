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

SUBDIRS = @INSTRUMENTOR@
DIST_SUBDIRS = dpcl dyninst

noinst_LTLIBRARIES = libopenss-runtime.la

libopenss_runtime_la_CFLAGS = \
	@PAPI_CPPFLAGS@

libopenss_runtime_la_LDFLAGS = \
	@PAPI_LDFLAGS@

libopenss_runtime_la_LIBADD = \
	@INSTRUMENTOR@/libopenss-runtime-@INSTRUMENTOR@.la \
	@PAPI_LIBS@ \
	-lpthread -lrt

libopenss_runtime_la_SOURCES = \
	Assert.h \
	OpenSS_DataHeader.h OpenSS_DataHeader.c OpenSS_DataHeader.x \
	OpenSS_DecodeParameters.c \
	OpenSS_GetPCFromContext.c \
	OpenSS_GetTime.c \
	OpenSS_Send.c \
	OpenSS_Timer.c \
	OpenSS_Overflow.c \
	OpenSS_PapiUtil.c \
	PapiAPI.h \
	RuntimeAPI.h

SUFFIXES = .x
CLEANFILES = OpenSS_DataHeader.h OpenSS_DataHeader.c

OpenSS_DataHeader.h OpenSS_DataHeader.c : OpenSS_DataHeader.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

doc:
	$(DOXYGEN)
