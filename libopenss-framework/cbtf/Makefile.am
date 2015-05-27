################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2015 Krell Institute All Rights Reserved.
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

noinst_LTLIBRARIES = libopenss-framework-cbtf.la

libopenss_framework_cbtf_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-framework \
	@BOOST_CPPFLAGS@ \
	@CBTF_CPPFLAGS@ \
	@CBTF_XML_CPPFLAGS@ \
	@CBTF_MRNET_CPPFLAGS@ \
	@CBTF_KRELL_CPPFLAGS@ \
	@MRNET_CPPFLAGS@

libopenss_framework_cbtf_la_LDFLAGS = \
	@BOOST_LDFLAGS@ \
	@CBTF_LDFLAGS@ \
	@CBTF_MRNET_LDFLAGS@ \
	@CBTF_XML_LDFLAGS@ \
	@CBTF_KRELL_LDFLAGS@ \
	@MRNET_LDFLAGS@

libopenss_framework_cbtf_la_LIBADD = \
	@BOOST_SYSTEM_LIB@ \
	@BOOST_THREAD_LIB@ \
	@CBTF_LIBS@ \
	@CBTF_MRNET_LIBS@\
	@CBTF_XML_LIBS@\
	@CBTF_KRELL_CORE_LIBS@ \
	@CBTF_KRELL_CORE_MRNET_LIBS@ \
	@CBTF_KRELL_MESSAGES_LIBS@ \
	@MRNET_LIBS@

libopenss_framework_cbtf_la_SOURCES = \
	Callbacks.cxx Callbacks.hxx \
	FEThread.cxx FEThread.hxx \
	Utility.cxx Utility.hxx \
	Frontend.cxx Frontend.hxx \
	Instrumentor.cxx \
	ThreadTable.cxx \
	ThreadTable.hxx
