################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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

pkglib_LTLIBRARIES = ManageProcessesPanel.la

ManageProcessesPanel_la_CXXFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/libopenss-cli \
	-I$(top_srcdir)/libopenss-framework \
	-I$(top_srcdir)/libopenss-gui \
	-I$(top_srcdir)/libopenss-guibase \
	-I$(top_srcdir)/libopenss-guidialogs \
	-I$(top_srcdir)/libopenss-guiobjects \
	-I$(top_srcdir)/libopenss-guiplugin \
	-I$(top_srcdir)/libopenss-message \
	-I$(top_srcdir)/libopenss-queries \
	-I$(top_srcdir)/plugins/panels/CustomizeStatsPanel \
	@PYTHON_CPPFLAGS@ \
	@QTLIB_CPPFLAGS@

ManageProcessesPanel_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-framework \
	-L$(top_srcdir)/libopenss-gui \
	-L$(top_srcdir)/libopenss-guibase \
	@QTLIB_LDFLAGS@ \
	-export-dynamic -module -avoid-version

ManageProcessesPanel_la_LIBADD = \
	-lopenss-framework \
	-lopenss-gui \
	-lopenss-guibase \
	@QTLIB_LIBS@

ManageProcessesPanel_la_SOURCES = \
	ManageCollectorsClass.hxx ManageCollectorsClass.cxx \
		moc_ManageCollectorsClass.cxx \
	ManageProcessesPanel.hxx ManageProcessesPanel.cxx \
		moc_ManageProcessesPanel.cxx \
	MPListViewItem.hxx MPListViewItem.cxx \
	MPListView.hxx MPListView.cxx moc_MPListView.cxx \
	local_plugin_info.hxx \
	preference_plugin_info.hxx

moc_ManageCollectorsClass.cxx: ManageCollectorsClass.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_ManageCollectorsClass.cxx ManageCollectorsClass.hxx

moc_ManageProcessesPanel.cxx: ManageProcessesPanel.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_ManageProcessesPanel.cxx ManageProcessesPanel.hxx

moc_MPListView.cxx: MPListView.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_MPListView.cxx MPListView.hxx

CLEANFILES = \
	moc_ManageCollectorsPanel.cxx \
	moc_ManageProcessesPanel.cxx \
	moc_MPListView.cxx
