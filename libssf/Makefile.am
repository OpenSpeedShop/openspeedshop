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

lib_LTLIBRARIES = libssf.la

libssf_la_CXXFLAGS = $(DPCL_CPPFLAGS) $(LTDLINCL) \
	-DCOLLECTOR_PATH=\"$(pkglibdir)\"

libssf_la_LDFLAGS = $(DPCL_LDFLAGS) -export-dynamic -version-info 0:0:0

libssf_la_LIBADD = $(DPCL_LIBS) $(LIBLTDL) -lpthread -lrt

libssf_la_SOURCES = \
	Address.hxx \
	AddressRange.hxx \
	AddressSpace.hxx \
	Application.hxx Application.cxx \
	Assert.hxx \
	CallSite.hxx CallSite.cxx \
	CallSiteEntry.hxx \
	Collector.hxx Collector.cxx \
	CollectorAPI.hxx \
	CollectorGroup.hxx CollectorGroup.cxx \
	Function.hxx Function.cxx \
	FunctionEntry.hxx \
	Guard.hxx \
	Interval.hxx \
	LinkedObject.hxx LinkedObject.cxx \
	LinkedObjectEntry.hxx \
	Lockable.hxx \
	MainLoop.hxx MainLoop.cxx \
	Metadata.hxx \
	Metric.hxx \
	NonCopyable.hxx \
	Optional.hxx \
	Parameter.hxx \
	Path.hxx \
	PluginTable.hxx PluginTable.cxx \
	Process.hxx Process.cxx \
	ProcessTable.hxx \
	Statement.hxx Statement.cxx \
	StatementEntry.hxx \
	StringTable.hxx StringTable.cxx \
	SymbolTable.hxx SymbolTable.cxx \
	Thread.hxx Thread.cxx \
	ThreadGroup.hxx ThreadGroup.cxx \
	Time.hxx \
	TimerInterval.hxx \
	TotallyOrdered.hxx \
	TypedMetric.hxx \
	TypedParameter.hxx
