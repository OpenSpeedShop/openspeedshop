################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2014 Krell Institute All Rights Reserved.
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

# CBTF needs:
# CBTF_KRELL_MESSAGES_PERFDATA_LIBS
# -I CBTF_KRELL_CPPFLAGS
#  DataHeader.h
#

if BUILD_INSTRUMENTOR
SUBDIRS = @INSTRUMENTOR@
else
SUBDIRS = @INSTRUMENTOR@
endif

DIST_SUBDIRS = cbtf dpcl dyninst mrnet offline

#lib_LTLIBRARIES = libopenss-framework.la libopenss-framework-binutils.la
lib_LTLIBRARIES = libopenss-framework-binutils.la

if OPENSS_USE_SYMTABAPI
lib_LTLIBRARIES += libopenss-framework-symtabapi.la
endif

lib_LTLIBRARIES += libopenss-framework.la

libopenss_framework_binutils_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	$(LTDLINCL) \
	@BINUTILS_CPPFLAGS@

libopenss_framework_binutils_la_LDFLAGS = \
	@BINUTILS_LDFLAGS@ \
	-export-dynamic -version-info 1:0:0

libopenss_framework_binutils_la_LIBADD = \
	$(LIBLTDL) \
	@BINUTILS_BFD_LIB@ \
	-lpthread -lrt

libopenss_framework_symtabapi_la_CXXFLAGS = \
	@BOOST_CPPFLAGS@ \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	$(LTDLINCL) \
	@SYMTABAPI_CPPFLAGS@

libopenss_framework_symtabapi_la_LDFLAGS = \
	@SYMTABAPI_LDFLAGS@ \
	-export-dynamic -version-info 1:0:0

libopenss_framework_symtabapi_la_LIBADD = \
	$(LIBLTDL) \
	@SYMTABAPI_LIBS@ \
	-lpthread -lrt

if HAVE_DYNINST
libopenss_framework_symtabapi_la_CXXFLAGS += @DYNINST_CPPFLAGS@
libopenss_framework_symtabapi_la_LDFLAGS += @DYNINST_LDFLAGS@
libopenss_framework_symtabapi_la_LIBADD += @DYNINST_LIBS@
endif

#
# OPENSS_USE_SYMTABAPI means use the symtabAPI interface to 
# resolve symbols, instead of the BFD interface from bintuils
#
if OPENSS_USE_SYMTABAPI
libopenss_framework_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	$(LTDLINCL) \
	@ARRAYSVCS_CPPFLAGS@ \
	@SQLITE_CPPFLAGS@


libopenss_framework_la_LDFLAGS = \
	@ARRAYSVCS_LDFLAGS@ \
	@SQLITE_LDFLAGS@ \
	-export-dynamic -version-info 1:0:0

if BUILD_CBTF
libopenss_framework_la_CXXFLAGS += \
	@CBTF_KRELL_CPPFLAGS@
libopenss_framework_la_LDFLAGS += \
	@CBTF_KRELL_LDFLAGS@
endif

#
# We are not using the symtabAPI interface to 
# resolve symbols, so don't compile/link using the symtabAPI interface
else

libopenss_framework_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	$(LTDLINCL) \
	@ARRAYSVCS_CPPFLAGS@ \
	@BINUTILS_CPPFLAGS@ \
	@SQLITE_CPPFLAGS@

libopenss_framework_la_LDFLAGS = \
	@ARRAYSVCS_LDFLAGS@ \
	@SQLITE_LDFLAGS@ \
	-export-dynamic -version-info 1:0:0

if BUILD_CBTF
libopenss_framework_la_CXXFLAGS += \
	@CBTF_KRELL_CPPFLAGS@
libopenss_framework_la_LDFLAGS += \
	@CBTF_KRELL_LDFLAGS@
endif

# end of the else clause for the non-symtabAPI section
endif


#if BUILD_INSTRUMENTOR

if OPENSS_USE_SYMTABAPI
libopenss_framework_la_LIBADD = \
	@INSTRUMENTOR@/libopenss-framework-@INSTRUMENTOR@.la \
	$(LIBLTDL) \
	@ARRAYSVCS_LIBS@ \
	@SQLITE_LIBS@ \
	-lpthread -lrt @BINUTILS_IBERTY_LDFLAGS@ @BINUTILS_IBERTY_LIB@
else
libopenss_framework_la_LIBADD = \
	@INSTRUMENTOR@/libopenss-framework-@INSTRUMENTOR@.la \
	$(LIBLTDL) \
	@ARRAYSVCS_LIBS@ \
	@SQLITE_LIBS@ \
	-lpthread -lrt @BINUTILS_IBERTY_LDFLAGS@ @BINUTILS_IBERTY_LIB@
endif

if BUILD_CBTF
libopenss_framework_la_LDFLAGS += \
	-L$(top_srcdir)/libopenss-framework/.libs

libopenss_framework_la_LIBADD += \
	-lopenss-framework-symtabapi \
	@CBTF_KRELL_MESSAGES_PERFDATA_LIBS@ \
	@CBTF_KRELL_MESSAGES_BASE_LIBS@
endif

# else for BUILD_INSTRUMENTOR
#else

#if OPENSS_USE_SYMTABAPI
#libopenss_framework_la_LIBADD = \
#	$(LIBLTDL) \
#	@ARRAYSVCS_LIBS@ \
#	@SQLITE_LIBS@ \
#	-lpthread -lrt 

#if BUILD_CBTF
#libopenss_framework_la_LDFLAGS += \
#	-L$(top_srcdir)/libopenss-framework/.libs
#
#libopenss_framework_la_LIBADD += \
#	-lopenss-framework-symtabapi \
#	@CBTF_KRELL_MESSAGES_PERFDATA_LIBS@
#endif
#
#else
#
#libopenss_framework_la_LIBADD = \
#	$(LIBLTDL) \
#	@ARRAYSVCS_LIBS@ \
#	@SQLITE_LIBS@ \
#	-lpthread -lrt @BINUTILS_IBERTY_LDFLAGS@ @BINUTILS_IBERTY_LIB@
#
#if BUILD_CBTF
#libopenss_framework_la_LIBADD += \
#	@CBTF_KRELL_MESSAGES_PERFDATA_LIBS@
#endif
#
## endif for not using symtabAPI else clause
#endif

# endif for not build instrumentor else clause
#endif

COMMON_SOURCES = \
	$(top_srcdir)/libopenss-runtime/OpenSS_DataHeader.c \
	Address.hxx \
	AddressBitmap.hxx AddressBitmap.cxx \
	AddressRange.hxx \
	AddressSpace.hxx AddressSpace.cxx \
	Assert.hxx \
	Blob.hxx Blob.cxx \
	Collector.hxx Collector.cxx \
	CollectorAPI.hxx \
	CollectorGroup.hxx CollectorGroup.cxx \
	CollectorImpl.hxx CollectorImpl.cxx \
	CollectorPluginTable.hxx CollectorPluginTable.cxx \
	Database.hxx Database.cxx \
	DataBucket.hxx \
	DataCache.hxx DataCache.cxx \
	DataQueues.hxx DataQueues.cxx \
	Entry.hxx Entry.cxx \
	EntrySpy.hxx \
	Exception.hxx Exception.cxx \
	Experiment.hxx Experiment.cxx \
	Extent.hxx \
	ExtentGroup.hxx ExtentGroup.cxx \
	ExtentTable.hxx \
	Function.hxx Function.cxx \
	FunctionCache.hxx FunctionCache.cxx \
	Guard.hxx \
	Interval.hxx \
	LinkedObject.hxx LinkedObject.cxx \
	Lockable.hxx \
	Loop.hxx Loop.cxx \
	LoopCache.hxx LoopCache.cxx \
	Metadata.hxx \
	NonCopyable.hxx \
	Path.hxx Path.cxx \
	PCBuffer.hxx PCBuffer.cxx \
	SmartPtr.hxx \
	StackTrace.hxx \
	Statement.hxx Statement.cxx \
	StatementCache.hxx StatementCache.cxx \
	SymbolTable.hxx SymbolTable.cxx \
	Thread.hxx Thread.cxx \
	ThreadGroup.hxx ThreadGroup.cxx \
	ThreadName.hxx ThreadName.cxx \
	Time.hxx \
	TimeInterval.hxx \
	ToolAPI.hxx \
	OfflineParameters.hxx \
	TotallyOrdered.hxx

INSTRUMENTOR_SOURCES = \
	Instrumentor.hxx \
	Job.hxx \
	OutputCallback.hxx

OPENSSBINUTILS_SOURCES = \
	BFDSymbols.hxx BFDSymbols.cxx

OPENSSSYMTABAPI_SOURCES = \
	SymtabAPISymbols.hxx SymtabAPISymbols.cxx

if !BUILD_CBTF
OPENSSBINUTILS_SOURCES += \
    $(top_srcdir)/libopenss-runtime/offline/offline.c \
    OfflineExperiment.hxx OfflineExperiment.cxx
OPENSSSYMTABAPI_SOURCES += \
    $(top_srcdir)/libopenss-runtime/offline/offline.c \
    OfflineExperiment.hxx OfflineExperiment.cxx
endif

if HAVE_DYNINST
OPENSSSYMTABAPI_SOURCES += DyninstSymbols.hxx DyninstSymbols.cxx
endif

#if BUILD_INSTRUMENTOR
libopenss_framework_la_SOURCES = \
	$(COMMON_SOURCES) $(INSTRUMENTOR_SOURCES)
libopenss_framework_binutils_la_SOURCES = \
	$(OPENSSBINUTILS_SOURCES)
libopenss_framework_symtabapi_la_SOURCES = \
	$(OPENSSSYMTABAPI_SOURCES)
#else
#libopenss_framework_la_SOURCES = \
#	$(COMMON_SOURCES)
#libopenss_framework_binutils_la_SOURCES = \
#	$(OPENSSBINUTILS_SOURCES)
#libopenss_framework_symtabapi_la_SOURCES = \
#	$(OPENSSSYMTABAPI_SOURCES)
#endif

EXTRA_DIST = OpenSSPath.cxx PapiAPI.h PapiAPI.cxx

doc:
	$(DOXYGEN)
