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

pkglib_LTLIBRARIES = CmdPanel.la

CmdPanel_la_CXXFLAGS = \
    -I/usr/include \
    -I$(QTDIR)/include \
    -I. \
	-I../../objects \
	-I../../libopenss-framework \
	-I../../pcli \
    -I../../gui \
    -I../../plugin \
    -I../../base

CmdPanel_la_LDFLAGS = --export-dynamic --export-all-symbols -module -avoid-version 

CmdPanel_la_LIBADD =  -L$(QTDIR)/lib -lqt ../../base/.libs/ossBase.so ../../objects/.libs/ossObjects.so ../../pcli/.libs/posscli.so

moc_CmdPanel.cxx: CmdPanel.hxx
	-moc -o moc_CmdPanel.cxx CmdPanel.hxx

CmdPanel_la_SOURCES = \
	CmdPanel.h CmdPanel.cxx moc_CmdPanel.cxx \
	KeyEventFilter.hxx KeyEventFilter.cxx \
	PanelContainer.h local_plugin_info.h debug.h \
	plugin_entry_point.h Panel.h

CLEANFILES=moc_CmdPanel.cxx moc_SPTextEdit.cxx
