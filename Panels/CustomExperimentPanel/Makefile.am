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

pkglib_LTLIBRARIES = CustomExperimentPanel.la

CustomExperimentPanel_la_CXXFLAGS = \
	-c -LANG:std -fpic \
    -I/usr/include \
	$(QTLIB_CPPFLAGS) \
	-I. \
	-I$(top_srcdir)/libopenss-framework \
    -I$(top_srcdir)/pcli \
    -I$(top_srcdir)/message \
    -I$(top_srcdir)/gui \
    -I$(top_srcdir)/images \
    -I$(top_srcdir)/plugin \
    -I$(top_srcdir)/objects \
    -I$(top_srcdir)/dialogs \
    -I$(top_srcdir)/Panels/ManageProcessesPanel \
    -I$(top_srcdir)/Panels/SourcePanel \
    -I$(top_srcdir)/base

CustomExperimentPanel_la_LDFLAGS = \
	-fpic \
	--export-dynamic \
	--export-all-symbols \
	-module \
	-avoid-version \
	-L$(top_srcdir)/libopenss-framework \
	-L$(top_srcdir)/dialogs \
	$(QTLIB_LIBS) $(QTLIB_LDFLAGS) \
	-ldl

CustomExperimentPanel_la_LIBADD = $(LIBLTDL) -lopenss-framework -lopenss-Dialogs

moc_CustomExperimentPanel.cxx: CustomExperimentPanel.hxx
	-moc -o moc_CustomExperimentPanel.cxx CustomExperimentPanel.hxx

CustomExperimentPanel_la_SOURCES = \
	CustomExperimentPanel.hxx CustomExperimentPanel.cxx moc_CustomExperimentPanel.cxx \
	local_plugin_info.hxx

CLEANFILES=moc_CustomExperimentPanel.cxx
