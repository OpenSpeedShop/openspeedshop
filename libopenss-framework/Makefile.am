################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2010 Krell Institute All Rights Reserved.
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
endif

DIST_SUBDIRS = dpcl dyninst mrnet offline

lib_LTLIBRARIES = libopenss-framework.la

libopenss_framework_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-runtime \
	-I$(top_srcdir)/libopenss-runtime/offline \
	$(LTDLINCL) \
	@ARRAYSVCS_CPPFLAGS@ \
	@BINUTILS_CPPFLAGS@ \
	@SQLITE_CPPFLAGS@

libopenss_framework_la_LDFLAGS = \
	@ARRAYSVCS_LDFLAGS@ \
	@BINUTILS_LDFLAGS@ \
	@SQLITE_LDFLAGS@ \
	-export-dynamic -version-info 1:0:0

if BUILD_INSTRUMENTOR
libopenss_framework_la_LIBADD = \
	@INSTRUMENTOR@/libopenss-framework-@INSTRUMENTOR@.la \
	$(LIBLTDL) \
	@ARRAYSVCS_LIBS@ \
	@BINUTILS_LIBS@ \
	@SQLITE_LIBS@ \
	-lpthread -lrt
else
libopenss_framework_la_LIBADD = \
	$(LIBLTDL) \
	@ARRAYSVCS_LIBS@ \
	@BINUTILS_LIBS@ \
	@SQLITE_LIBS@ \
	-lpthread -lrt
endif

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
	Metadata.hxx \
	NonCopyable.hxx \
	Path.hxx Path.cxx \
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
	TotallyOrdered.hxx

INSTRUMENTOR_SOURCES = \
	$(top_srcdir)/libopenss-runtime/offline/offline.c \
	BFDSymbols.hxx BFDSymbols.cxx \
	CallSite.hxx CallSite.cxx \
	Instrumentor.hxx \
	Job.hxx \
	OutputCallback.hxx \
	OfflineExperiment.hxx OfflineExperiment.cxx \
	PCBuffer.hxx PCBuffer.cxx

if BUILD_INSTRUMENTOR
libopenss_framework_la_SOURCES = \
	$(COMMON_SOURCES) $(INSTRUMENTOR_SOURCES)
else
libopenss_framework_la_SOURCES = \
	$(COMMON_SOURCES)
endif

EXTRA_DIST = OpenSSPath.cxx PapiAPI.h PapiAPI.cxx

doc:
	$(DOXYGEN)
