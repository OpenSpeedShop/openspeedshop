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

pkglib_LTLIBRARIES = StatsPanel.la libopenss-StatsPanel.la

StatsPanel_la_CXXFLAGS = \
    -I/usr/include \
    -I$(QTDIR)/include \
    -I. \
    -I../../objects \
    -I../../pcli \
    -I../../libopenss-framework \
    -I../../gui \
    -I../../expr_info \
    -I../../plugin \
    -I../../base

libopenss_StatsPanel_la_CXXFLAGS = \
    -I/usr/include \
    -I$(QTDIR)/include \
    -I. \
    -I../../objects \
    -I../../pcli \
    -I../../libopenss-framework \
    -I../../gui \
    -I../../expr_info \
    -I../../plugin \
    -I../../base

StatsPanel_la_LDFLAGS = --export-dynamic --export-all-symbols -module -avoid-version

StatsPanel_la_LIBADD = -L$(QTDIR)/lib -lqt ../../libopenss-framework/.libs/libopenss-framework.so 

libopenss_StatsPanel_la_LDFLAGS = --export-dynamic --export-all-symbols -module -avoid-version

libopenss_StatsPanel_la_LIBADD = -L$(QTDIR)/lib -lqt ../../libopenss-framework/.libs/libopenss-framework.so 

moc_StatsPanel.cxx: StatsPanel.hxx
	-moc -o moc_StatsPanel.cxx StatsPanel.hxx

moc_StatsPanelBase.cxx: StatsPanelBase.hxx
	-moc -o moc_StatsPanelBase.cxx StatsPanelBase.hxx

moc_SPListView.cxx: SPListView.hxx
	-moc -o moc_SPListView.cxx SPListView.hxx

StatsPanel_la_SOURCES = \
	StatsPanel.hxx StatsPanel.cxx moc_StatsPanel.cxx \
	StatsPanelBase.hxx StatsPanelBase.cxx moc_StatsPanelBase.cxx \
	SPListViewItem.hxx SPListViewItem.cxx \
	SPListView.hxx SPListView.cxx moc_SPListView.cxx \
	plugin_entry_point.hxx Panel.hxx \
	PanelContainer.hxx local_plugin_info.hxx debug.hxx

libopenss_StatsPanel_la_SOURCES = \
	StatsPanel.hxx StatsPanel.cxx moc_StatsPanel.cxx \
	StatsPanelBase.hxx StatsPanelBase.cxx moc_StatsPanelBase.cxx \
	SPListViewItem.hxx SPListViewItem.cxx \
	SPListView.hxx SPListView.cxx moc_SPListView.cxx \
	plugin_entry_point.hxx Panel.hxx \
	PanelContainer.hxx local_plugin_info.hxx debug.hxx

CLEANFILES=moc_SPListView.cxx moc_StatsPanel.cxx
