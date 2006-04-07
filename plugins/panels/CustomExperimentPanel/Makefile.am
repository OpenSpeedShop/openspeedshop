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

lib_LTLIBRARIES = libopenss-CustomExperimentPanel.la
pkglib_LTLIBRARIES = CustomExperimentPanel.la

CustomExperimentPanel_la_CXXFLAGS = \
	-c -LANG:std -fpic \
    -I/usr/include \
	$(QTLIB_CPPFLAGS) \
	-I. \
	-I$(top_srcdir)/libopenss-framework \
    -I$(top_srcdir)/libopenss-queries \
    -I$(top_srcdir)/libopenss-cli \
    -I$(top_srcdir)/libopenss-message \
    -I$(top_srcdir)/libopenss-gui \
    -I$(top_srcdir)/libopenss-guiimages \
    -I$(top_srcdir)/libopenss-guiplugin \
    -I$(top_srcdir)/libopenss-guiobjects \
    -I$(top_srcdir)/libopenss-guidialogs \
    -I$(top_srcdir)/plugins/panels/ManageProcessesPanel \
    -I$(top_srcdir)/plugins/panels/SourcePanel \
    -I$(top_srcdir)/plugins/panels/CustomExperimentPanel \
    -I$(top_srcdir)/libopenss-guibase \
	-I@PYTHON_CPPFLAGS@

CustomExperimentPanel_la_LDFLAGS = \
	-fpic \
	--export-dynamic \
	--export-all-symbols \
	-module \
	-avoid-version \
	-L$(top_srcdir)/libopenss-framework \
	-L$(top_srcdir)/libopenss-guidialogs \
	$(QTLIB_LIBS) $(QTLIB_LDFLAGS) \
	-ldl

CustomExperimentPanel_la_LIBADD = $(LIBLTDL) -lopenss-framework -lopenss-guidialogs

moc_CustomExperimentPanel.cxx: CustomExperimentPanel.hxx
	-moc -o moc_CustomExperimentPanel.cxx CustomExperimentPanel.hxx

CustomExperimentPanel_la_SOURCES = \
	CustomExperimentPanel.hxx CustomExperimentPanel.cxx moc_CustomExperimentPanel.cxx \
	local_plugin_info.hxx


libopenss_CustomExperimentPanel_la_CXXFLAGS = \
	-c -LANG:std -fpic \
    -I/usr/include \
	$(QTLIB_CPPFLAGS) \
	$(PYTHON_INCLUDES) \
	-I. \
	-I$(top_srcdir)/libopenss-framework \
    -I$(top_srcdir)/libopenss-queries \
    -I$(top_srcdir)/libopenss-cli \
    -I$(top_srcdir)/libopenss-message \
    -I$(top_srcdir)/libopenss-gui \
    -I$(top_srcdir)/libopenss-guiimages \
    -I$(top_srcdir)/libopenss-guiplugin \
    -I$(top_srcdir)/libopenss-guiobjects \
    -I$(top_srcdir)/libopenss-guidialogs \
    -I$(top_srcdir)/plugins/panels/ManageProcessesPanel \
    -I$(top_srcdir)/plugins/panels/SourcePanel \
    -I$(top_srcdir)/plugins/panels/CustomExperimentPanel \
    -I$(top_srcdir)/libopenss-guibase

libopenss_CustomExperimentPanel_la_LDFLAGS = \
	-fpic \
	--export-dynamic \
	--export-all-symbols \
	-module \
	-avoid-version \
	-L$(top_srcdir)/libopenss-framework \
	-L$(top_srcdir)/libopenss-guidialogs \
	$(QTLIB_LIBS) $(QTLIB_LDFLAGS) \
	-ldl

libopenss_CustomExperimentPanel_la_LIBADD = $(LIBLTDL) -lopenss-framework -lopenss-guidialogs

moc_CustomExperimentPanel.cxx: CustomExperimentPanel.hxx
	-moc -o moc_CustomExperimentPanel.cxx CustomExperimentPanel.hxx

libopenss_CustomExperimentPanel_la_SOURCES = \
	CustomExperimentPanel.hxx CustomExperimentPanel.cxx moc_CustomExperimentPanel.cxx \
	local_plugin_info.hxx

CLEANFILES=moc_CustomExperimentPanel.cxx
