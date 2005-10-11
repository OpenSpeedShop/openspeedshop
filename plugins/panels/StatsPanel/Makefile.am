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

pkglib_LTLIBRARIES = StatsPanel.la

StatsPanel_la_CXXFLAGS = \
    -I/usr/include \
	$(QTLIB_CPPFLAGS) \
    -I. \
    -I$(top_srcdir)/objects \
    -I$(top_srcdir)/libopenss-framework \
    -I$(top_srcdir)/libopenss-queries \
    -I$(top_srcdir)/pcli \
    -I$(top_srcdir)/libopenss-message \
    -I$(top_srcdir)/plugins/panels/ManageProcessesPanel \
    -I$(top_srcdir)/libopenss-guichart \
    -I$(top_srcdir)/gui \
    -I$(top_srcdir)/expr_info \
    -I$(top_srcdir)/plugin \
    -I$(top_srcdir)/dialogs \
    -I$(top_srcdir)/libopenss-guibase

StatsPanel_la_LDFLAGS = --export-dynamic --export-all-symbols -module -avoid-version

StatsPanel_la_LIBADD = $(QTLIB_LIBS) $(QTLIB_LDFLAGS) -L$(top_srcdir)/libopenss-framework -L$(top_srcdir)/libopenss-queries -lopenss-framework -lopenss-queries -L$(top_srcdir)/plugins/panels/StatsPanel -L$(top_srcdir)/guichart -lopenss-guichart

moc_StatsPanel.cxx: StatsPanel.hxx
	-moc -o moc_StatsPanel.cxx StatsPanel.hxx

moc_SPChartForm.cxx: SPChartForm.hxx
	-moc -o moc_SPChartForm.cxx SPChartForm.hxx


StatsPanel_la_SOURCES = \
	StatsPanel.hxx StatsPanel.cxx moc_StatsPanel.cxx \
	SPListViewItem.hxx SPListViewItem.cxx \
	SPListView.hxx SPListView.cxx \
	SPChartForm.hxx SPChartForm.cxx moc_SPChartForm.cxx \
	local_plugin_info.hxx preference_plugin_info.hxx

CLEANFILES=moc_StatsPanel.cxx moc_SPChartForm.cxx
