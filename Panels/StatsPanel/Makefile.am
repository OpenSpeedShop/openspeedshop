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
    -I$(QTDIR)/include \
    -I. \
    -I$(top_srcdir)/objects \
    -I$(top_srcdir)/libopenss-framework \
    -I$(top_srcdir)/pcli \
    -I$(top_srcdir)/chart \
    -I$(top_srcdir)/gui \
    -I$(top_srcdir)/expr_info \
    -I$(top_srcdir)/plugin \
	-I$(top_srcdir)/dialogs \
    -I$(top_srcdir)/base

StatsPanel_la_LDFLAGS = --export-dynamic --export-all-symbols -module -avoid-version

StatsPanel_la_LIBADD = -L$(QTDIR)/lib -lqt -L../../libopenss-framework -lopenss-framework -L$(top_srcdir)/Panels/StatsPanel -L$(top_srcdir)/chart -lopenss-Chart

moc_StatsPanel.cxx: StatsPanel.hxx
	-moc -o moc_StatsPanel.cxx StatsPanel.hxx

StatsPanel_la_SOURCES = \
	StatsPanel.hxx StatsPanel.cxx moc_StatsPanel.cxx \
	SPListViewItem.hxx SPListViewItem.cxx \
	SPListView.hxx SPListView.cxx \
	SPChartForm.hxx SPChartForm.cxx \
	local_plugin_info.hxx preference_plugin_info.hxx

CLEANFILES=moc_StatsPanel.cxx moc_StatsPanel.cxx
