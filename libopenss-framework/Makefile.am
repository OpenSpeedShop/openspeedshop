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

SUBDIRS = @INSTRUMENTOR@ runtime
DIST_SUBDIRS = dpcl dyninst runtime

lib_LTLIBRARIES = libopenss-framework.la

libopenss_framework_la_CXXFLAGS = \
	-DPLUGIN_PATH=\"$(pkglibdir)\" \
	$(LTDLINCL)

libopenss_framework_la_LDFLAGS = \
	-export-dynamic -version-info 0:0:0

libopenss_framework_la_LIBADD = \
	@INSTRUMENTOR@/libopenss-framework-@INSTRUMENTOR@.la \
	$(LIBLTDL) \
	-lsqlite3 -lpthread -lrt

libopenss_framework_la_SOURCES = \
	runtime/OpenSS_DataHeader.c \
	Address.hxx \
	AddressRange.hxx \
	Assert.hxx \
	Blob.hxx Blob.cxx \
	CallSite.hxx CallSite.cxx \
	Collector.hxx Collector.cxx \
	CollectorAPI.hxx \
	CollectorGroup.hxx CollectorGroup.cxx \
	CollectorImpl.hxx CollectorImpl.cxx \
	Database.hxx Database.cxx \
	Entry.hxx Entry.cxx \
	EntrySpy.hxx \
	Experiment.hxx Experiment.cxx \
	ExperimentTable.hxx ExperimentTable.cxx \
	Function.hxx Function.cxx \
	Guard.hxx \
	Instrumentor.hxx \
	Interval.hxx \
	LinkedObject.hxx LinkedObject.cxx \
	Lockable.hxx \
	Metadata.hxx \
	NonCopyable.hxx \
	Optional.hxx \
	Path.hxx \
	Queries.hxx Queries.cxx \
	SmartPtr.hxx \
	Statement.hxx Statement.cxx \
	Thread.hxx Thread.cxx \
	ThreadGroup.hxx ThreadGroup.cxx \
	Time.hxx \
	TimeInterval.hxx \
	ToolAPI.hxx \
	TotallyOrdered.hxx

doc: $(SUBDIRS)
	-doxygen Doxyfile
