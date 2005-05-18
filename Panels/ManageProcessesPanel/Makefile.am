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

pkglib_LTLIBRARIES = ManageProcessesPanel.la

ManageProcessesPanel_la_CXXFLAGS = \
    -I/usr/include \
    -I$(QTDIR)/include \
    -I. \
    -I$(top_srcdir) \
    -I$(top_srcdir)/libopenss-framework \
    -I$(top_srcdir)/gui \
	-I$(top_srcdir)/pcli \
	-I$(top_srcdir)/objects \
	-I$(top_srcdir)/dialogs \
    -I$(top_srcdir)/plugin \
    -I$(top_srcdir)/base

ManageProcessesPanel_la_LDFLAGS = --export-dynamic --export-all-symbols -module -avoid-version

ManageProcessesPanel_la_LIBADD = -L$(QTDIR)/lib -lqt

moc_ManageProcessesPanel.cxx: ManageProcessesPanel.hxx
	-moc -o moc_ManageProcessesPanel.cxx ManageProcessesPanel.hxx

moc_ManageCollectorsClass.cxx: ManageCollectorsClass.hxx
	-moc -o moc_ManageCollectorsClass.cxx ManageCollectorsClass.hxx

ManageProcessesPanel_la_SOURCES = \
	ManageProcessesPanel.hxx ManageProcessesPanel.cxx moc_ManageProcessesPanel.cxx \
	ManageCollectorsClass.hxx ManageCollectorsClass.cxx moc_ManageCollectorsClass.cxx \
	local_plugin_info.hxx

CLEANFILES=moc_ManageProcessesPanel.cxx
