################################################################################
# Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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

noinst_LTLIBRARIES = libopenss-framework-mrnet.la

libopenss_framework_mrnet_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-framework \
	@MRNET_CPPFLAGS@

libopenss_framework_mrnet_la_LDFLAGS = \
	@MRNET_LDFLAGS@

libopenss_framework_mrnet_la_LIBADD = \
	@MRNET_LIBS@ \
	-lpthread

libopenss_framework_mrnet_la_SOURCES = \
	Instrumentor.cxx
