################################################################################
# Copyright (c) 2004 Silicon Graphics, Inc. All Rights Reserved.
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

lib_LTLIBRARIES = libssf.la

libssf_la_CXXFLAGS = $(LTDLINCL) -DCOLLECTOR_PATH=\"$(pkglibdir)\"

libssf_la_LDFLAGS = -export-dynamic -version-info 0:0:0

libssf_la_LIBADD = $(LIBLTDL) -lsqlite3 -lpthread -lrt \
	@INSTRUMENTOR@/libssf-@INSTRUMENTOR@.la

libssf_la_SOURCES = \
	Address.hxx \
	AddressRange.hxx \
	Assert.hxx \
	CallSite.hxx CallSite.cxx \
	Collector.hxx Collector.cxx \
	CollectorAPI.hxx \
	CollectorGroup.hxx CollectorGroup.cxx \
	CollectorImpl.hxx CollectorImpl.cxx \
	Database.hxx Database.cxx \
	Experiment.hxx Experiment.cxx \
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
	SmartPtr.hxx \
	Statement.hxx Statement.cxx \
	Thread.hxx Thread.cxx \
	ThreadGroup.hxx ThreadGroup.cxx \
	ThreadSpy.hxx \
	Time.hxx \
	TimeInterval.hxx \
	ToolAPI.hxx \
	TotallyOrdered.hxx
