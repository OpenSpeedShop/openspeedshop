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

pkglib_LTLIBRARIES = ComparePanel.la

ComparePanel_la_CXXFLAGS = \
    -I/usr/include \
     $(QTLIB_CPPFLAGS) \
	@PYTHON_CPPFLAGS@ \
    -I. \
    -I$(top_srcdir) \
    -I$(top_srcdir)/libopenss-framework \
    -I$(top_srcdir)/libopenss-gui \
	-I$(top_srcdir)/libopenss-queries \
	-I$(top_srcdir)/libopenss-cli \
	-I$(top_srcdir)/libopenss-message \
	-I$(top_srcdir)/libopenss-guiobjects \
	-I$(top_srcdir)/libopenss-guiimages \
	-I$(top_srcdir)/libopenss-guidialogs \
    -I$(top_srcdir)/libopenss-guiplugin \
    -I$(top_srcdir)/libopenss-guibase \
    -I$(top_srcdir)/plugins/panels/ManageProcessesPanel

ComparePanel_la_LDFLAGS = --export-dynamic --export-all-symbols -module -avoid-version

ComparePanel_la_LIBADD = $(QTLIB_LIBS) $(QTLIB_LDFLAGS)

moc_ComparePanel.cxx: ComparePanel.hxx
	-moc -o moc_ComparePanel.cxx ComparePanel.hxx

moc_CompareClass.cxx: CompareClass.hxx
	-moc -o moc_CompareClass.cxx CompareClass.hxx

moc_CompareSet.cxx: CompareSet.hxx
	-moc -o moc_CompareSet.cxx CompareSet.hxx

moc_ColumnSet.cxx: ColumnSet.hxx
	-moc -o moc_ColumnSet.cxx ColumnSet.hxx

moc_CompareProcessesDialog.cxx: CompareProcessesDialog.hxx
	-moc -o moc_CompareProcessesDialog.cxx CompareProcessesDialog.hxx

moc_MPListView.cxx: $(top_srcdir)/plugins/panels/ManageProcessesPanel/MPListView.hxx
	-moc -o moc_MPListView.cxx $(top_srcdir)/plugins/panels/ManageProcessesPanel/MPListView.hxx


ComparePanel_la_SOURCES = \
	ComparePanel.hxx ComparePanel.cxx moc_ComparePanel.cxx \
	CompareClass.hxx CompareClass.cxx moc_CompareClass.cxx \
	CompareSet.hxx CompareSet.cxx moc_CompareSet.cxx \
	ColumnSet.hxx ColumnSet.cxx moc_ColumnSet.cxx \
	CompareProcessesDialog.hxx CompareProcessesDialog.cxx moc_CompareProcessesDialog.cxx \
	$(top_srcdir)/plugins/panels/ManageProcessesPanel/MPListViewItem.hxx $(top_srcdir)/plugins/panels/ManageProcessesPanel/MPListViewItem.cxx \
	$(top_srcdir)/plugins/panels/ManageProcessesPanel/MPListView.hxx $(top_srcdir)/plugins/panels/ManageProcessesPanel/MPListView.cxx moc_MPListView.cxx \
	local_plugin_info.hxx preference_plugin_info.hxx

CLEANFILES=moc_ComparePanel.cxx moc_CompareClass.cxx moc_CompareSet.cxx moc_ColumnSet.cxx moc_MPListView.cxx  moc_CompareProcessesDialog.cxx
