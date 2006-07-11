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

# Note: Some versions of GCC (e.g. 3.3.3) appear to contain an optimization bug
#       that causes accesses of thread-local storage (TLS) to fail with SIGSEGV.
#       The exact nature of the bug has yet to be identified, but it results in
#       incorrect addresses being calculated for TLS data structures, and occurs
#       under both "-O1" and "-O2" on affected systems. For now it is safest to
#       turn off ALL optimizations for code that uses TLS.

CFLAGS = -g -O0

BUILT_SOURCES = \
	OpenSS_DataHeader.h OpenSS_DataHeader.c

noinst_LTLIBRARIES = libopenss-runtime.la

libopenss_runtime_la_CFLAGS = \
	@LIBUNWIND_CPPFLAGS@ \
	@BINUTILS_CPPFLAGS@ \
	@PAPI_CPPFLAGS@

libopenss_runtime_la_LDFLAGS = \
	@LIBUNWIND_LDFLAGS@ \
	@BINUTILS_LDFLAGS@ \
	@PAPI_LDFLAGS@

libopenss_runtime_la_LIBADD = \
	@INSTRUMENTOR@/libopenss-runtime-@INSTRUMENTOR@.la \
	@LIBUNWIND_LIBS@ \
	@BINUTILS_LIBS@ \
	@PAPI_LIBS@ \
	-lpthread -lrt

libopenss_runtime_la_SOURCES = \
	Assert.h \
	OpenSS_DataHeader.x $(BUILT_SOURCES) \
	OpenSS_DecodeParameters.c \
	OpenSS_GetAddressOfFunction.c \
	OpenSS_GetPCFromContext.c \
	OpenSS_UpdatePCData.c \
	OpenSS_GetTime.c \
	OpenSS_Send.c \
	OpenSS_Timer.c \
	OpenSS_FPEHandler.c \
	OpenSS_SetPCInContext.c \
	RuntimeAPI.h

if HAVE_LIBUNWIND
libopenss_runtime_la_SOURCES += \
	OpenSS_GetStackTraceFromContext.c

endif	

if HAVE_PAPI
libopenss_runtime_la_SOURCES += \
	OpenSS_Overflow.c \
	PapiAPI.h PapiAPI.c
endif

if HAVE_BINUTILS
libopenss_runtime_la_SOURCES += \
	OpenSS_GetInstrLength.c
endif

SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES)

OpenSS_DataHeader.h OpenSS_DataHeader.c : OpenSS_DataHeader.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

doc:
	$(DOXYGEN)
