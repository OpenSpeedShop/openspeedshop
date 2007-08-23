################################################################################
# Copyright (c) 2007 Krell Institute. All Rights Reserved.
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

pkglib_LTLIBRARIES = mpiotf_view.la

mpiotf_view_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-cli \
	-I$(top_srcdir)/libopenss-framework \
	-I$(top_srcdir)/libopenss-message \
	-I$(top_srcdir)/libopenss-queries \
	-I$(top_srcdir)/plugins/collectors/mpiotf \
	@PYTHON_CPPFLAGS@

mpiotf_view_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-cli \
	-no-undefined -module -avoid-version

mpiotf_view_la_LIBADD = \
	-lopenss-cli

mpiotf_view_la_SOURCES = \
	mpiotf_view.cxx
