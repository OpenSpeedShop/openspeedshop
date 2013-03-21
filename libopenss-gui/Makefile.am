################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2007 William Hachfeld. All Rights Reserved.
# Copyright (c) 2007-2010 Krell Institute  All Rights Reserved.
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

lib_LTLIBRARIES = libopenss-gui.la

libopenss_gui_la_CXXFLAGS = \
	$(LTDLINCL) \
	@PYTHON_CPPFLAGS@ \
	@QTLIB_CPPFLAGS@ \
	-I$(top_srcdir)/libopenss-cli \
	-I$(top_srcdir)/libopenss-framework \
	-I$(top_srcdir)/libopenss-guibase \
	-I$(top_srcdir)/libopenss-guidialogs \
	-I$(top_srcdir)/libopenss-guiimages \
	-I$(top_srcdir)/libopenss-guiobjects \
	-I$(top_srcdir)/libopenss-guiplugin \
	-I$(top_srcdir)/libopenss-message \
	-I$(top_srcdir)/libopenss-queries \
	-I$(top_srcdir)/plugins/panels/DebugPanel

libopenss_gui_la_LDFLAGS = \
	@QTLIB_LDFLAGS@ \
	-L$(top_srcdir)/libopenss-cli \
	-L$(top_srcdir)/libopenss-guibase \
	-L$(top_srcdir)/libopenss-guidialogs \
	-L$(top_srcdir)/libopenss-guiobjects \
	-L$(top_srcdir)/libopenss-guiplugin \
	-export-dynamic -version-info 0:0:0

libopenss_gui_la_LIBADD = \
	$(LIBLTDL) \
	@QTLIB_LIBS@ \
	$(top_srcdir)/libopenss-cli/libopenss-cli.la \
	$(top_srcdir)/libopenss-guibase/libopenss-guibase.la \
	$(top_srcdir)/libopenss-guidialogs/libopenss-guidialogs.la \
	$(top_srcdir)/libopenss-guiobjects/libopenss-guiobjects.la \
	$(top_srcdir)/libopenss-guiplugin/libopenss-guiplugin.la \
	-lqassistantclient -lm -lpthread

libopenss_gui_la_SOURCES = \
	AboutClass.cxx AboutClass.hxx \
	CLIInterface.hxx CLIInterface.cxx \
	gui.cxx \
	openspeedshop.hxx openspeedshop.cxx \
		openspeedshop.ui.hxx \
	preferencesdialog.hxx preferencesdialog.cxx \
		preferencesdialog.ui.hxx \
	qmake_image_collection.cxx

nodist_libopenss_gui_la_SOURCES = \
	moc_AboutClass.cxx \
	moc_CLIInterface.cxx \
	moc_openspeedshop.cxx \
	moc_preferencesdialog.cxx

moc_AboutClass.cxx: AboutClass.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_AboutClass.cxx AboutClass.hxx

moc_CLIInterface.cxx: CLIInterface.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_CLIInterface.cxx CLIInterface.hxx

moc_openspeedshop.cxx: openspeedshop.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_openspeedshop.cxx openspeedshop.hxx

moc_preferencesdialog.cxx: preferencesdialog.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_preferencesdialog.cxx preferencesdialog.hxx

CLEANFILES = \
	moc_AboutClass.cxx \
	moc_CLIInterface.cxx \
	moc_openspeedshop.cxx \
	moc_preferencesdialog.cxx

doc:
	$(DOXYGEN)
