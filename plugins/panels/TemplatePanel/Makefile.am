################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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
	-I$(top_srcdir)/libopenss-gui \
	-I$(top_srcdir)/libopenss-guibase
	-I$(top_srcdir)/libopenss-guichart \
	-I$(top_srcdir)/libopenss-guiobjects \
	-I$(top_srcdir)/libopenss-guiplugin \
	@QTLIB_CPPFLAGS@

TemplatePanel_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-guibase \
	@QTLIB_LDFLAGS@ \
	-export-dynamic -export-all-symbols -module -avoid-version

TemplatePanel_la_LIBADD = \
	-lopenss-guibase \
	@QTLIB_LIBS@

TemplatePanel_la_SOURCES = \
	TemplatePanel.hxx TemplatePanel.cxx moc_TemplatePanel.cxx \
	local_plugin_info.hxx

moc_TemplatePanel.cxx: TemplatePanel.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_TemplatePanel.cxx TemplatePanel.hxx

CLEANFILES = \
	moc_TemplatePanel.cxx
