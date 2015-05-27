################################################################################
# Copyright (c) 2012-2013 The Krell Institute. All Rights Reserved.
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

pkglib_LTLIBRARIES =

if BUILD_CBTF
pkglib_LTLIBRARIES += pthreads.la
endif


if BUILD_CBTF
pthreads_la_CXXFLAGS = \
	@CBTF_KRELL_CPPFLAGS@ \
	-I$(top_srcdir)/libopenss-framework

pthreads_la_LDFLAGS = \
	@CBTF_KRELL_LDFLAGS@ \
	-L$(top_srcdir)/libopenss-framework \
	-no-undefined -module -avoid-version

pthreads_la_LIBADD = \
	@CBTF_KRELL_MESSAGES_LIBS@ \
	-lopenss-framework

pthreads_la_SOURCES = \
	PthreadsCollector.hxx PthreadsDetail.hxx PthreadsCollector.cxx PthreadsTraceableFunctions.h
endif
