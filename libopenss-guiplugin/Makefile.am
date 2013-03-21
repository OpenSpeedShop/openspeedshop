################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2012 Krell Institute. All Rights Reserved.
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
	$(QTLIB_CPPFLAGS) \
	-I. \
	-I/usr/include \
	-I$(top_srcdir) \
	-I$(top_srcdir)/libopenss-guibase \
	-I$(top_srcdir)/libopenss-gui

libopenss_guiplugin_la_LDFLAGS = \
	$(QTLIB_LDFLAGS) \
	-export-dynamic -version-info 0:0:0

libopenss_guiplugin_la_LIBADD = \
	$(LIBLTDL) \
        $(top_srcdir)/libopenss-guibase/libopenss-guibase.la \
	$(QTLIB_LIBS)

moc_SlotInfo.cxx: SlotInfo.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_SlotInfo.cxx SlotInfo.hxx

libopenss_guiplugin_la_SOURCES = \
    ../libopenss-gui/openspeedshop.hxx \
	plugin_handler.hxx plugin_handler.cxx \
	PluginInfo.hxx PluginInfo.cxx \
	SlotInfo.hxx SlotInfo.cxx \
	plugin_entry_point.hxx

nodist_libopenss_guiplugin_la_SOURCES = \
	moc_SlotInfo.cxx


CLEANFILES=moc_SlotInfo.cxx
