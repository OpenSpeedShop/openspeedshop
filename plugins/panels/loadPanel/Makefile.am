################################################################################
# Copyright (c) 2007 Krell Institute  All Rights Reserved.
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

pkglib_LTLIBRARIES = loadPanel.la

loadPanel_la_CXXFLAGS = \
	$(QTLIB_CPPFLAGS) \
	-I. \
	-I/usr/include \
	-I$(top_srcdir)/libopenss-gui \
	-I$(top_srcdir)/libopenss-guichart \
	-I$(top_srcdir)/libopenss-guiobjects \
	-I$(top_srcdir)/libopenss-guiplugin \
	-I$(top_srcdir)/libopenss-guibase

loadPanel_la_LDFLAGS = \
	$(QTLIB_LDFLAGS) \
	--export-dynamic --export-all-symbols -module -avoid-version

loadPanel_la_LIBADD = \
	$(QTLIB_LIBS) \
	$(top_srcdir)/libopenss-gui/libopenss-gui.la \
	$(top_srcdir)/libopenss-guibase/libopenss-guibase.la \
	$(top_srcdir)/libopenss-guiobjects/libopenss-guiobjects.la

moc_loadPanel.cxx: loadPanel.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_loadPanel.cxx loadPanel.hxx

loadPanel_la_SOURCES = \
	loadPanel.hxx loadPanel.cxx moc_loadPanel.cxx \
	local_plugin_info.hxx

CLEANFILES=moc_loadPanel.cxx
