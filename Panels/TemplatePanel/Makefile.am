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

pkglib_LTLIBRARIES = TemplatePanel.la

TemplatePanel_la_CXXFLAGS = \
    -I/usr/include \
    -I$(QTDIR)/include \
    -I. \
    -I../../gui \
    -I../../plugin \
    -I../../base

TemplatePanel_la_LDFLAGS = --export-dynamic --export-all-symbols -module -avoid-version

TemplatePanel_la_LIBADD = -L$(QTDIR)/lib -lqt -lopenss-Base

moc_TemplatePanel.cxx: TemplatePanel.hxx
	-moc -o moc_TemplatePanel.cxx TemplatePanel.hxx

TemplatePanel_la_SOURCES = \
	TemplatePanel.hxx TemplatePanel.cxx moc_TemplatePanel.cxx \
	plugin_entry_point.hxx Panel.hxx \
	PanelContainer.hxx local_plugin_info.hxx debug.hxx
