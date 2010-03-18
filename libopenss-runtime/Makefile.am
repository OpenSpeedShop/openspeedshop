################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2010 Krell Institute All Rights Reserved.
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

if BUILD_INSTRUMENTOR
SUBDIRS = @INSTRUMENTOR@

if HAVE_LIBMONITOR
SUBDIRS += offline
endif

if HAVE_TARGET_LIBMONITOR
SUBDIRS += offline
endif

else

SUBDIRS =

endif

DIST_SUBDIRS = dpcl dyninst offline mrnet


# Note: Some versions of GCC (e.g. 3.3.3) appear to contain an optimization bug
#       that causes accesses of thread-local storage (TLS) to fail with SIGSEGV.
#       The exact nature of the bug has yet to be identified, but it results in
#       incorrect addresses being calculated for TLS data structures, and occurs
#       under both "-O1" and "-O2" on affected systems. For now it is safest to
#       turn off ALL optimizations for code that uses TLS.

CFLAGS = -g -O0

BUILT_SOURCES = \
	OpenSS_DataHeader.h OpenSS_DataHeader.c

pkglib_LTLIBRARIES =


if BUILD_INSTRUMENTOR
if !BUILD_OFFLINE
pkglib_LTLIBRARIES += libopenss-runtime.la
endif

if HAVE_LIBMONITOR
pkglib_LTLIBRARIES += libopenss-runtime-offline.la
endif

if HAVE_TARGET_LIBMONITOR
pkglib_LTLIBRARIES += libopenss-runtime-offline.la
endif

if !BUILD_OFFLINE
libopenss_runtime_la_CFLAGS = \
	@LIBUNWIND_CPPFLAGS@ \
	@BINUTILS_CPPFLAGS@ \
	@PAPI_CPPFLAGS@

libopenss_runtime_la_LDFLAGS = \
	@LIBUNWIND_LDFLAGS@ \
	@BINUTILS_LDFLAGS@ \
	@PAPI_LDFLAGS@ \
	-static

libopenss_runtime_la_LIBADD = \
	@INSTRUMENTOR@/libopenss-runtime-@INSTRUMENTOR@.la \
	@LIBUNWIND_LIBS@ \
	@BINUTILS_LIBS@ \
	@PAPI_LIBS@ \
	-lpthread -lrt
endif

if HAVE_LIBMONITOR
libopenss_runtime_offline_la_CFLAGS = \
	-DOPENSS_OFFLINE \
	@LIBUNWIND_CPPFLAGS@ \
	@BINUTILS_CPPFLAGS@ \
	@PAPI_CPPFLAGS@

libopenss_runtime_offline_la_LDFLAGS = \
	@LIBUNWIND_LDFLAGS@ \
	@BINUTILS_LDFLAGS@ \
	@PAPI_LDFLAGS@ \
	-static

libopenss_runtime_offline_la_LIBADD = \
	$(top_srcdir)/libopenss-runtime/offline/libopenss-runtime-monitor.la \
	@LIBUNWIND_LIBS@ \
	@BINUTILS_LIBS@ \
	@PAPI_LIBS@ \
	-lpthread -lrt
endif

if HAVE_TARGET_LIBMONITOR
libopenss_runtime_offline_la_CFLAGS = \
	-DOPENSS_OFFLINE \
	@TARGET_LIBUNWIND_CPPFLAGS@ \
	@TARGET_BINUTILS_CPPFLAGS@ \
	@TARGET_PAPI_CPPFLAGS@

libopenss_runtime_offline_la_LDFLAGS = \
	@TARGET_LIBUNWIND_LDFLAGS@ \
	@TARGET_BINUTILS_LDFLAGS@ \
	@TARGET_PAPI_LDFLAGS@ \
	-static

libopenss_runtime_offline_la_LIBADD = \
	$(top_srcdir)/libopenss-runtime/offline/libopenss-runtime-monitor.la \
	@TARGET_LIBUNWIND_LIBS@ \
	@TARGET_BINUTILS_LIBS@ \
	@TARGET_PAPI_LIBS@ \
	-lpthread -lrt
endif
endif

COMMON_SOURCES = \
	Assert.h \
	OpenSS_DataHeader.x $(BUILT_SOURCES)

INSTRUMENTOR_SOURCES = \
	OpenSS_DecodeParameters.c \
	OpenSS_EncodeParameters.c \
	OpenSS_GetAddressOfFunction.c \
	OpenSS_GetExecutablePath.c \
	OpenSS_GetPCFromContext.c \
	OpenSS_InitializeDataHeader.c \
	OpenSS_UpdatePCData.c \
	OpenSS_GetTime.c \
	OpenSS_Send.c \
	OpenSS_SendToFile.c \
	OpenSS_Timer.c \
	OpenSS_FPEHandler.c \
	OpenSS_SetPCInContext.c \
	OpenSS_Path_From_Pid.c \
	RuntimeAPI.h

if BUILD_INSTRUMENTOR
libopenss_runtime_la_SOURCES = $(COMMON_SOURCES) $(INSTRUMENTOR_SOURCES)
else
libopenss_runtime_la_SOURCES = $(COMMON_SOURCES)
endif

if BUILD_INSTRUMENTOR

if HAVE_LIBMONITOR
libopenss_runtime_offline_la_SOURCES = $(COMMON_SOURCES) $(INSTRUMENTOR_SOURCES)
endif #HAVE_LIBMONITOR

if HAVE_TARGET_LIBMONITOR
libopenss_runtime_offline_la_SOURCES = $(COMMON_SOURCES) $(INSTRUMENTOR_SOURCES)
endif #HAVE_TARGET_LIBMONITOR

if HAVE_LIBUNWIND
libopenss_runtime_la_SOURCES += \
	OpenSS_GetStackTraceFromContext.c
if HAVE_LIBMONITOR
libopenss_runtime_offline_la_SOURCES += \
	OpenSS_GetStackTraceFromContext.c
endif	#HAVE_LIBMONITOR
endif	#HAVE_LIBUNWIND

if HAVE_TARGET_LIBUNWIND
libopenss_runtime_la_SOURCES += \
	OpenSS_GetStackTraceFromContext.c
if HAVE_TARGET_LIBMONITOR
libopenss_runtime_offline_la_SOURCES += \
	OpenSS_GetStackTraceFromContext.c
endif	#HAVE_TARGET_LIBMONITOR
endif	#HAVE_TARGET_LIBUNWIND

if HAVE_PAPI
libopenss_runtime_la_SOURCES += \
	OpenSS_Overflow.c \
	PapiAPI.h PapiAPI.c

if HAVE_LIBMONITOR
libopenss_runtime_offline_la_SOURCES += \
	OpenSS_Overflow.c \
	PapiAPI.h PapiAPI.c
endif   #HAVE_LIBMONITOR
endif   #HAVE_PAPI

if HAVE_TARGET_PAPI
libopenss_runtime_la_SOURCES += \
	OpenSS_Overflow.c \
	PapiAPI.h PapiAPI.c
if HAVE_TARGET_LIBMONITOR
libopenss_runtime_offline_la_SOURCES += \
	OpenSS_Overflow.c \
	PapiAPI.h PapiAPI.c
endif   #HAVE_TARGET_LIBMONITOR
endif   #HAVE_TARGET_PAPI

if HAVE_BINUTILS
libopenss_runtime_la_SOURCES += \
	OpenSS_GetInstrLength.c
if HAVE_LIBMONITOR
libopenss_runtime_offline_la_SOURCES += \
	OpenSS_GetInstrLength.c
endif  #HAVE_LIBMONITOR
endif  #HAVE_BINUTILS

if HAVE_TARGET_BINUTILS
libopenss_runtime_la_SOURCES += \
	OpenSS_GetInstrLength.c
if HAVE_TARGET_LIBMONITOR
libopenss_runtime_offline_la_SOURCES += \
	OpenSS_GetInstrLength.c
endif #HAVE_TARGET_LIBMONITOR
endif #HAVE_TARGET_BINUTILS

if USE_EXPLICIT_TLS
libopenss_runtime_la_SOURCES += \
	OpenSS_TLS.c

if HAVE_LIBMONITOR
libopenss_runtime_offline_la_SOURCES += \
	OpenSS_TLS.c
endif #HAVE_LIBMONITOR

if HAVE_TARGET_LIBMONITOR
libopenss_runtime_offline_la_SOURCES += \
	OpenSS_TLS.c
endif #HAVE_TARGET_LIBMONITOR

endif #USE_EXPLICIT_TLS

endif

SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES)

OpenSS_DataHeader.h OpenSS_DataHeader.c : OpenSS_DataHeader.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

all-local:
	$(DOXYGEN)
doc:
	$(DOXYGEN)
