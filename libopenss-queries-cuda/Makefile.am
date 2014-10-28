################################################################################
# Copyright (c) 2014 Argo Navis Technologies. All Rights Reserved.
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

lib_LTLIBRARIES =
noinst_PROGRAMS =

if BUILD_CBTF
if HAVE_CBTF_MESSAGES_CUDA

lib_LTLIBRARIES += libopenss-queries-cuda.la
noinst_PROGRAMS += test

libopenss_queries_cuda_la_CXXFLAGS = \
	@BOOST_CPPFLAGS@ \
	@MESSAGES_CPPFLAGS@ \
	@MESSAGES_CUDA_CPPFLAGS@ \
	-I$(top_srcdir)/libopenss-framework \
	$(LTDLINCL)

libopenss_queries_cuda_la_LDFLAGS = \
	@MESSAGES_LDFLAGS@ \
	@MESSAGES_CUDA_LDFLAGS@ \
	-L$(top_srcdir)/libopenss-framework \
	-export-dynamic -version-info 0:0:0

libopenss_queries_cuda_la_LIBADD = \
	@MESSAGES_LIBS@ \
	@MESSAGES_CUDA_LIBS@ \
	-lopenss-framework \
	$(LIBLTDL)

libopenss_queries_cuda_la_SOURCES = \
	CUDAExecXferBalance.hxx \
	CUDAQueries.hxx CUDAQueries.cxx

test_CXXFLAGS =\
	@BOOST_CPPFLAGS@ \
	-I$(top_srcdir)/libopenss-framework \
	-I$(top_srcdir)/libopenss-queries-cuda

test_LDFLAGS = \
	-L$(top_srcdir)/libopenss-framework \
	-L$(top_srcdir)/libopenss-queries-cuda

test_LDADD = -lopenss-framework -lopenss-queries-cuda

test_SOURCES = test.cxx

endif
endif
