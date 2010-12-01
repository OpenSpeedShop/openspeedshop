################################################################################
# Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 59 Temple
# Place, Suite 330, Boston, MA  02111-1307  USA
################################################################################

SUBDIRS	= watcher
bin_PROGRAMS = openssd

openssd_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-framework \
	-I$(top_srcdir)/libopenss-framework/mrnet/common \
	-I$(top_srcdir)/libopenss-framework/mrnet/openssd/watcher \
	@DYNINST_CPPFLAGS@ \
	@BINUTILS_CPPFLAGS@ \
	@LIBDWARF_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

openssd_LDFLAGS = \
	-L$(top_srcdir)/libopenss-framework/mrnet/common \
	-L$(top_srcdir)/libopenss-framework/mrnet/openssd/watcher \
	@DYNINST_LDFLAGS@ \
	@BINUTILS_IBERTY_LDFLAGS@ \
	@LIBDWARF_LDFLAGS@ \
	@MRNET_LDFLAGS@

openssd_LDADD = \
	-lopenss-framework-mrnet-common \
	-lopenss-watcher \
	@DYNINST_LIBS@ \
	@LIBDWARF_LIBS@ \
	@MRNET_LIBS@ \
	-lpthread -lrt @BINUTILS_IBERTY_LIB@

openssd_SOURCES = \
	$(top_srcdir)/libopenss-framework/AddressBitmap.cxx \
	$(top_srcdir)/libopenss-framework/Blob.cxx \
	$(top_srcdir)/libopenss-framework/Path.cxx \
	Backend.hxx Backend.cxx \
	Callbacks.hxx Callbacks.cxx \
	Collector.hxx \
	Dyninst.hxx Dyninst.cxx \
	ExecuteAtEntryOrExitEntry.hxx ExecuteAtEntryOrExitEntry.cxx \
	ExecuteInPlaceOfEntry.hxx ExecuteInPlaceOfEntry.cxx \
	ExecuteNowEntry.hxx ExecuteNowEntry.cxx \
	Experiment.hxx \
	ExperimentGroup.hxx \
	FileName.hxx \
	InstrumentationEntry.hxx \
	InstrumentationTable.hxx InstrumentationTable.cxx \
	openssd.cxx \
	Senders.hxx Senders.cxx \
	SentFilesTable.hxx SentFilesTable.cxx \
	StdStreamPipes.hxx \
	StopAtEntryOrExitEntry.hxx StopAtEntryOrExitEntry.cxx \
	SymbolTable.hxx SymbolTable.cxx \
	ThreadName.hxx \
	ThreadNameGroup.hxx \
	ThreadTable.hxx ThreadTable.cxx
