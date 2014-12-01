################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2008 William Hachfeld. All Rights Reserved.
# Copyright (c) 2006-2012 Krell Institute All Rights Reserved.
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

SUBDIRS =
BUILT_SOURCES =
AM_CFLAGS =


if BUILD_INSTRUMENTOR
if !BUILD_CBTF
SUBDIRS += @INSTRUMENTOR@
endif

if HAVE_LIBMONITOR
SUBDIRS += offline
endif

if HAVE_TARGET_LIBMONITOR
SUBDIRS += offline
endif

else

SUBDIRS +=

endif

DIST_SUBDIRS = cbtf dpcl dyninst offline mrnet fileio

BUILT_SOURCES += \
	OpenSS_DataHeader.h OpenSS_DataHeader.c

if !BUILD_CBTF
library_includedir = $(includedir)/openspeedshop
library_include_HEADERS = RuntimeAPI.h

# Note: Some versions of GCC (e.g. 3.3.3) appear to contain an optimization bug
#       that causes accesses of thread-local storage (TLS) to fail with SIGSEGV.
#       The exact nature of the bug has yet to be identified, but it results in
#       incorrect addresses being calculated for TLS data structures, and occurs
#       under both "-O1" and "-O2" on affected systems. For now it is safest to
#       turn off ALL optimizations for code that uses TLS.

AM_CFLAGS += -g -O0


lib_LTLIBRARIES =


if BUILD_INSTRUMENTOR
if !BUILD_OFFLINE
lib_LTLIBRARIES += libopenss-runtime.la
endif

if HAVE_LIBMONITOR
lib_LTLIBRARIES += libopenss-runtime-offline.la
endif

if HAVE_TARGET_LIBMONITOR
lib_LTLIBRARIES += libopenss-runtime-offline.la
endif

if HAVE_PAPI
if !HAVE_TARGET_PAPI
lib_LTLIBRARIES += libopenss-runtime-papi.la
library_include_HEADERS += PapiAPI.h
endif
endif

if HAVE_TARGET_PAPI
lib_LTLIBRARIES += libopenss-runtime-papi.la
library_include_HEADERS += PapiAPI.h
endif

if HAVE_BINUTILS 
if !HAVE_TARGET_BINUTILS
lib_LTLIBRARIES += libopenss-runtime-binutils.la
library_include_HEADERS += BinutilsAPI.h
endif
endif

if HAVE_TARGET_BINUTILS
lib_LTLIBRARIES += libopenss-runtime-binutils.la
library_include_HEADERS += BinutilsAPI.h
endif

if HAVE_LIBUNWIND
if !HAVE_TARGET_LIBUNWIND
lib_LTLIBRARIES += libopenss-runtime-unwind.la
library_include_HEADERS += UnwindAPI.h
endif
endif

if HAVE_TARGET_LIBUNWIND
lib_LTLIBRARIES += libopenss-runtime-unwind.la
library_include_HEADERS += UnwindAPI.h
endif

if !BUILD_OFFLINE
libopenss_runtime_la_CFLAGS =


libopenss_runtime_la_LDFLAGS = \
	-static

libopenss_runtime_la_LIBADD = \
	@INSTRUMENTOR@/libopenss-runtime-@INSTRUMENTOR@.la \
	-lpthread -lrt
endif

if HAVE_LIBMONITOR
libopenss_runtime_offline_la_CFLAGS = \
	-DOPENSS_OFFLINE

libopenss_runtime_offline_la_LDFLAGS = \
	-static 

libopenss_runtime_offline_la_LIBADD = \
	$(top_srcdir)/libopenss-runtime/offline/libopenss-runtime-monitor.la \
	-lpthread -lrt
endif

if HAVE_TARGET_LIBMONITOR
if HAVE_TARGET_PERSONALITY
libopenss_runtime_offline_la_CFLAGS = \
	-DOPENSS_OFFLINE \
	@TARGET_PERSONALITY_CPPFLAGS@ \
	@TARGET_LIBUNWIND_CPPFLAGS@
else
libopenss_runtime_offline_la_CFLAGS = \
	-DOPENSS_OFFLINE \
	@TARGET_LIBUNWIND_CPPFLAGS@
endif

libopenss_runtime_offline_la_LDFLAGS = \
	-static \
	@TARGET_LIBUNWIND_LDFLAGS@

libopenss_runtime_offline_la_LIBADD = \
 	$(top_srcdir)/libopenss-runtime/offline/libopenss-runtime-monitor.la \
	@TARGET_LIBUNWIND_LIBS@ \
	-lpthread -lrt
endif

if HAVE_PAPI
libopenss_runtime_papi_la_CFLAGS = \
	@PAPI_CPPFLAGS@

libopenss_runtime_papi_la_LDFLAGS = \
	-static \
	@PAPI_LDFLAGS@

libopenss_runtime_papi_la_LIBADD = \
	@PAPI_LIBS@
endif

if HAVE_TARGET_PAPI
libopenss_runtime_papi_la_CFLAGS = \
	@TARGET_PAPI_CPPFLAGS@

libopenss_runtime_papi_la_LDFLAGS = \
	-static \
	@TARGET_PAPI_LDFLAGS@

libopenss_runtime_papi_la_LIBADD = \
	@TARGET_PAPI_LIBS@
endif

if HAVE_BINUTILS
libopenss_runtime_binutils_la_CFLAGS = \
	@BINUTILS_CPPFLAGS@

libopenss_runtime_binutils_la_LDFLAGS = \
	-static \
	@BINUTILS_LDFLAGS@

libopenss_runtime_binutils_la_LIBADD = \
	@BINUTILS_OPCODES_LIB@
endif

if HAVE_TARGET_BINUTILS
libopenss_runtime_binutils_la_CFLAGS = \
	@TARGET_BINUTILS_CPPFLAGS@

libopenss_runtime_binutils_la_LDFLAGS = \
	-static \
	@TARGET_BINUTILS_LDFLAGS@

libopenss_runtime_binutils_la_LIBADD = \
	@TARGET_BINUTILS_LIBS@
endif

if HAVE_LIBUNWIND
libopenss_runtime_unwind_la_CFLAGS = \
	@LIBUNWIND_CPPFLAGS@

libopenss_runtime_unwind_la_LDFLAGS = \
	-static \
	@LIBUNWIND_LDFLAGS@

libopenss_runtime_unwind_la_LIBADD = \
	@LIBUNWIND_LIBS@
endif

if HAVE_TARGET_LIBUNWIND
libopenss_runtime_unwind_la_CFLAGS = \
	@TARGET_LIBUNWIND_CPPFLAGS@

libopenss_runtime_unwind_la_LDFLAGS = \
	-static \
	@TARGET_LIBUNWIND_LDFLAGS@

libopenss_runtime_unwind_la_LIBADD = \
	@TARGET_LIBUNWIND_LIBS@
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
	OpenSS_UpdateHWCPCData.c \
	OpenSS_GetTime.c \
	OpenSS_Send.c \
	OpenSS_SendToFile.c \
	OpenSS_Timer.c \
	OpenSS_FPEHandler.c \
	OpenSS_SetPCInContext.c \
	OpenSS_Path_From_Pid.c \
	RuntimeAPI.h

if BUILD_TARGETED_BGQ
INSTRUMENTOR_SOURCES += OpenSS_BGQNodeName.c
endif

if BUILD_TARGETED_BGP
INSTRUMENTOR_SOURCES += OpenSS_BGPNodeName.c
endif

if BUILD_TARGETED_BGL
INSTRUMENTOR_SOURCES += OpenSS_BGLNodeName.c
endif

if HAVE_PAPI
libopenss_runtime_papi_la_SOURCES = \
	OpenSS_Overflow.c \
	PapiAPI.h PapiAPI.c
endif
if HAVE_TARGET_PAPI
libopenss_runtime_papi_la_SOURCES = \
	OpenSS_Overflow.c \
	PapiAPI.h PapiAPI.c
endif

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
libopenss_runtime_unwind_la_SOURCES = \
	OpenSS_GetStackTraceFromContext.c
endif	#HAVE_LIBUNWIND

if HAVE_TARGET_LIBUNWIND
libopenss_runtime_unwind_la_SOURCES = \
	OpenSS_GetStackTraceFromContext.c
endif	#HAVE_TARGET_LIBUNWIND

if HAVE_BINUTILS
libopenss_runtime_binutils_la_SOURCES = \
	BinutilsAPI.h \
	OpenSS_GetInstrLength.c
endif  #HAVE_BINUTILS

if HAVE_TARGET_BINUTILS
libopenss_runtime_binutils_la_SOURCES = \
	BinutilsAPI.h \
	OpenSS_GetInstrLength.c
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
endif


SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES)

OpenSS_DataHeader.h OpenSS_DataHeader.c : OpenSS_DataHeader.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<
if !BUILD_CBTF
endif

doc:
	$(DOXYGEN)

BUILT_SOURCES += OpenSS_Papi_Events.h
if HAVE_PAPI

OpenSS_Papi_Events.h:
	@rm -rf OpenSS_Papi_Events.h
	@echo "static const char* OpenSS_Papi_ALLEvents[] = {" > OpenSS_Papi_Events.h
	@$(PAPI_DIR)/bin/papi_avail -a | grep ^PAPI_ |  awk '{ printf("\""); printf($$1);  printf(":"); for(i=4;i<=NF;i++) {printf("%s ",$$i) } ; printf("\",\n") }' >> OpenSS_Papi_Events.h
	@echo "NULL" >> OpenSS_Papi_Events.h
	@echo "};" >> OpenSS_Papi_Events.h
	@echo "static const char* OpenSS_Papi_NonDerivedEvents[] = {"  >> OpenSS_Papi_Events.h
	@$(PAPI_DIR)/bin/papi_avail -a | grep ^PAPI_ | grep No |  awk '{ printf("\""); printf($$1);  printf(":"); for(i=4;i<=NF;i++) {printf("%s ",$$i) } ; printf("\",\n") }' >> OpenSS_Papi_Events.h
	@echo "NULL" >> OpenSS_Papi_Events.h
	@echo "};" >> OpenSS_Papi_Events.h
else
OpenSS_Papi_Events.h:
	@rm -rf OpenSS_Papi_Events.h
	@echo "static const char* OpenSS_Papi_ALLEvents[] = {" > OpenSS_Papi_Events.h
	@echo "NULL" >> OpenSS_Papi_Events.h
	@echo "};" >> OpenSS_Papi_Events.h
	@echo "static const char* OpenSS_Papi_NonDerivedEvents[] = {"  >> OpenSS_Papi_Events.h
	@echo "NULL" >> OpenSS_Papi_Events.h
	@echo "};" >> OpenSS_Papi_Events.h
endif
