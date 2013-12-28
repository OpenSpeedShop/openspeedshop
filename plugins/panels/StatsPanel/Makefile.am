################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2007 William Hachfeld. All Rights Reserved.
# Copyright (c) 2006-2014 Krell Institute All Rights Reserved.
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

pkglib_LTLIBRARIES = StatsPanel.la

StatsPanel_la_CXXFLAGS = \
	@PYTHON_CPPFLAGS@ \
	@QTLIB_CPPFLAGS@ \
	-I$(top_srcdir)/libopenss-cli \
	-I$(top_srcdir)/libopenss-framework \
	-I$(top_srcdir)/libopenss-gui \
	-I$(top_srcdir)/libopenss-guibase \
	-I$(top_srcdir)/libopenss-guichart \
	-I$(top_srcdir)/libopenss-guidialogs \
	-I$(top_srcdir)/libopenss-guiexprinfo \
	-I$(top_srcdir)/libopenss-guiimages \
	-I$(top_srcdir)/libopenss-guiobjects \
	-I$(top_srcdir)/libopenss-guiplugin \
	-I$(top_srcdir)/libopenss-message \
	-I$(top_srcdir)/libopenss-queries \
	-I$(top_srcdir)/plugins/panels/CustomExperimentPanel \
	-I$(top_srcdir)/plugins/panels/ManageProcessesPanel

StatsPanel_la_LDFLAGS = \
	@QTLIB_LDFLAGS@ \
	-L$(top_srcdir)/libopenss-gui \
	-L$(top_srcdir)/libopenss-guibase \
	-L$(top_srcdir)/libopenss-guichart \
	-L$(top_srcdir)/libopenss-guidialogs \
	-L$(top_srcdir)/plugins/panels/CustomExperimentPanel \
	-export-dynamic -export-all-symbols -module -avoid-version

StatsPanel_la_LIBADD = \
	@QTLIB_LIBS@ \
	$(top_srcdir)/libopenss-gui/libopenss-gui.la \
	$(top_srcdir)/libopenss-guibase/libopenss-guibase.la \
	$(top_srcdir)/libopenss-guichart/libopenss-guichart.la \
	$(top_srcdir)/plugins/panels/CustomExperimentPanel/libopenss-CustomExperimentPanel.la \
	$(top_srcdir)/libopenss-guidialogs/libopenss-guidialogs.la

StatsPanel_la_SOURCES = \
	SPChartForm.hxx SPChartForm.cxx moc_SPChartForm.cxx \
	SPCheckBox.hxx SPCheckBox.cxx moc_SPCheckBox.cxx \
	SPListViewItem.hxx SPListViewItem.cxx \
	SPListView.hxx SPListView.cxx \
	ChooseExperimentDialog.hxx ChooseExperimentDialog.cxx \
	moc_ChooseExperimentDialog.cxx  \
	OptionalViewsDialog.hxx OptionalViewsDialog.cxx \
	moc_OptionalViewsDialog.cxx  \
	SourcePanelAnnotationDialog.hxx SourcePanelAnnotationDialog.cxx \
	moc_SourcePanelAnnotationDialog.cxx  \
	StatsPanel.hxx StatsPanel.cxx moc_StatsPanel.cxx \
	SPTextEdit.hxx SPTextEdit.cxx moc_SPTextEdit.cxx \
	local_plugin_info.hxx \
	preference_plugin_info.hxx

moc_SPChartForm.cxx: SPChartForm.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_SPChartForm.cxx SPChartForm.hxx

moc_SPCheckBox.cxx: SPCheckBox.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_SPCheckBox.cxx SPCheckBox.hxx

moc_StatsPanel.cxx: StatsPanel.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_StatsPanel.cxx StatsPanel.hxx

moc_OptionalViewsDialog.cxx: OptionalViewsDialog.hxx OptionalViewsDialog.cxx
	-$(QTLIB_HOME)/bin/moc -o moc_OptionalViewsDialog.cxx OptionalViewsDialog.hxx

moc_SourcePanelAnnotationDialog.cxx: SourcePanelAnnotationDialog.hxx SourcePanelAnnotationDialog.cxx
	-$(QTLIB_HOME)/bin/moc -o moc_SourcePanelAnnotationDialog.cxx SourcePanelAnnotationDialog.hxx

moc_ChooseExperimentDialog.cxx: ChooseExperimentDialog.hxx ChooseExperimentDialog.cxx
	-$(QTLIB_HOME)/bin/moc -o moc_ChooseExperimentDialog.cxx ChooseExperimentDialog.hxx

moc_SPTextEdit.cxx: SPTextEdit.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_SPTextEdit.cxx SPTextEdit.hxx

CLEANFILES = \
	moc_SPChartForm.cxx \
	moc_SPCheckBox.cxx \
	moc_StatsPanel.cxx \
	moc_OptionalViewsDialog.cxx \
	moc_SourcePanelAnnotationDialog.cxx \
	moc_ChooseExperimentDialog.cxx \
	moc_SPTextEdit.cxx
