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

pkglib_LTLIBRARIES = pcStatsPanel.la

pcStatsPanel_la_CXXFLAGS = \
    -I/usr/include \
    -I$(QTDIR)/include \
    -I. \
    -I../../objects \
    -I../../libopenss-framework \
    -I../StatsPanel \
    -I../../pcli \
    -I../../gui \
    -I../../expr_info \
    -I../../plugin \
    -I../../base

pcStatsPanel_la_LDFLAGS = --export-dynamic --export-all-symbols -module -avoid-version

pcStatsPanel_la_LIBADD = -L$(QTDIR)/lib -lqt -L../../libopenss-framework -lopenss-framework -L../StatsPanel -lopenss-StatsPanel

moc_pcStatsPanel.cxx: pcStatsPanel.hxx
	-moc -o moc_pcStatsPanel.cxx pcStatsPanel.hxx

pcStatsPanel_la_SOURCES = \
	pcStatsPanel.hxx pcStatsPanel.cxx moc_pcStatsPanel.cxx \
	local_plugin_info.hxx preference_plugin_info.hxx

CLEANFILES=moc_pcStatsPanel.cxx
