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

pkglib_LTLIBRARIES = pcSamplePanel.la

# NOTE 1: The -I../SourcePanel is only here for #define DEMO 1
# NOTE 2: The -I../TopPanel is only here for #define DEMO 1
pcSamplePanel_la_CXXFLAGS = \
	-c -LANG:std -fpic \
    -I/usr/include \
    -I$(QTDIR)/include \
    -I. \
	-I../../libopenss-framework \
    -I../../pcli \
    -I../../gui \
    -I../../images \
    -I../../plugin \
    -I../../objects \
    -I../SourcePanel \
    -I../TopPanel \
    -I../../base

pcSamplePanel_la_LDFLAGS = \
	-fpic \
	--export-dynamic \
	--export-all-symbols \
	-module \
	-avoid-version \
	-L../../libopenss-framework \
	-L$(QTDIR)/lib \
	-lqt \
	-ldl

pcSamplePanel_la_LIBADD = $(LIBLTDL) -lopenss-framework \
	../../base/.libs/libopenss_Base.so \
	../../objects/.libs/ossObjects.so \
	../../pcli/.libs/posscli.so \
	../../gui/.libs/ossgui.so

moc_pcSamplePanel.cxx: pcSamplePanel.hxx
	-moc -o moc_pcSamplePanel.cxx pcSamplePanel.hxx

pcSamplePanel_la_SOURCES = \
	pcSamplePanel.hxx pcSamplePanel.cxx moc_pcSamplePanel.cxx \
	plugin_entry_point.hxx Panel.hxx \
	PanelContainer.hxx local_plugin_info.hxx debug.hxx

CLEANFILES=moc_pcSamplePanel.cxx
