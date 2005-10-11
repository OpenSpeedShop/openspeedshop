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

pkglib_LTLIBRARIES = SourcePanel.la

SourcePanel_la_CXXFLAGS = \
	-I$(top_srcdir)/objects \
    -I/usr/include \
    $(QTLIB_CPPFLAGS) \
    -I. \
    -I$(top_srcdir)/pcli \
    -I$(top_srcdir)/libopenss-message \
    -I$(top_srcdir)/gui \
    -I$(top_srcdir)/plugin \
    -I$(top_srcdir)/libopenss-framework \
    -I$(top_srcdir)/libopenss-guibase

SourcePanel_la_LDFLAGS = --export-dynamic --export-all-symbols -module -avoid-version

SourcePanel_la_LIBADD = $(QTLIB_LIBS) $(QTLIB_LDFLAGS)

moc_SPTextEdit.cxx: SPTextEdit.hxx
	-moc -o moc_SPTextEdit.cxx SPTextEdit.hxx

moc_SourcePanel.cxx: SourcePanel.hxx
	-moc -o moc_SourcePanel.cxx SourcePanel.hxx

moc_SPCanvasView.cxx: SPCanvasView.hxx
	-moc -o moc_SPCanvasView.cxx SPCanvasView.hxx

moc_SPCanvasForm.cxx: SPCanvasForm.hxx
	-moc -o moc_SPCanvasForm.cxx SPCanvasForm.hxx

SourcePanel_la_SOURCES = \
    SPCanvasForm.hxx SPCanvasForm.cxx moc_SPCanvasForm.cxx \
    SPCanvasView.hxx SPCanvasView.cxx moc_SPCanvasView.cxx \
    SPTextEdit.hxx SPTextEdit.cxx moc_SPTextEdit.cxx \
	SourcePanel.hxx SourcePanel.cxx moc_SourcePanel.cxx \
	local_plugin_info.hxx preference_plugin_info.hxx

CLEANFILES=moc_SPTextEdit.cxx moc_SourcePanel.cxx moc_SPCanvasView.cxx moc_SPCanvasForm.cxx
