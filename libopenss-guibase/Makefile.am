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

lib_LTLIBRARIES = libopenss-guibase.la

libopenss_guibase_la_CXXFLAGS = \
	$(LTDLINCL) \
	$(QTLIB_CPPFLAGS) \
	-I. \
	-I/usr/include \
	-I$(top_srcdir)/libopenss-guiobjects \
	-I$(top_srcdir)/libopenss-guiplugin \
	-I$(top_srcdir)/libopenss-guiimages \
	-I$(top_srcdir)/libopenss-gui \
	-I$(top_srcdir)/libopenss-guibase

libopenss_guibase_la_LDFLAGS = \
	$(QTLIB_LDFLAGS) \
	-export-dynamic -version-info 0:0:0

libopenss_guibase_la_LIBADD = \
	$(LIBLTDL) \
	$(QTLIB_LIBS)


moc_TopWidget.cxx: TopWidget.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_TopWidget.cxx TopWidget.hxx

moc_TabWidget.cxx: TabWidget.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_TabWidget.cxx TabWidget.hxx

moc_TabBarWidget.cxx: TabBarWidget.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_TabBarWidget.cxx TabBarWidget.hxx

moc_Frame.cxx: Frame.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_Frame.cxx Frame.hxx

moc_PanelContainer.cxx: PanelContainer.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_PanelContainer.cxx PanelContainer.hxx

moc_Panel.cxx: Panel.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_Panel.cxx Panel.hxx

libopenss_guibase_la_SOURCES = \
	pc_plugin.cxx \
	DragNDropPanel.hxx DragNDropPanel.cxx \
	TopWidget.hxx TopWidget.cxx \
	TabWidget.hxx TabWidget.cxx \
	TabBarWidget.hxx TabBarWidget.cxx \
	WhatsThis.hxx WhatsThis.cxx  \
	Frame.hxx Frame.cxx \
	Panel.hxx Panel.cxx \
	PanelContainer.hxx PanelContainer.cxx \
	debug.hxx

nodist_libopenss_guibase_la_SOURCES = \
	moc_TopWidget.cxx \
	moc_TabWidget.cxx \
	moc_TabBarWidget.cxx \
	moc_Frame.cxx \
	moc_Panel.cxx \
	moc_PanelContainer.cxx


CLEANFILES=moc_TopWidget.cxx moc_TabWidget.cxx moc_TabBarWidget.cxx moc_Frame.cxx moc_PanelContainer.cxx moc_Panel.cxx
