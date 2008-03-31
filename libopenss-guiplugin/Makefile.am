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

lib_LTLIBRARIES = libopenss-guiplugin.la

libopenss_guiplugin_la_CXXFLAGS =  \
	$(LTDLINCL) \
	-I/usr/include \
	-I$(QTDIR)/include \
	-I. \
	-I$(top_srcdir) \
	-I$(top_srcdir)/libopenss-guibase \
	-I$(top_srcdir)/libopenss-gui

libopenss_guiplugin_la_LDFLAGS = \
	-export-dynamic -version-info 0:0:0

libopenss_guiplugin_la_LIBADD = $(LIBLTDL)

moc_SlotInfo.cxx: SlotInfo.hxx
	-$(QTDIR)/bin/moc -o moc_SlotInfo.cxx SlotInfo.hxx

libopenss_guiplugin_la_SOURCES = \
    ../libopenss-gui/openspeedshop.hxx \
	plugin_handler.hxx plugin_handler.cxx \
	PluginInfo.hxx PluginInfo.cxx \
	SlotInfo.hxx SlotInfo.cxx moc_SlotInfo.cxx \
	plugin_entry_point.hxx


CLEANFILES=moc_SlotInfo.cxx
