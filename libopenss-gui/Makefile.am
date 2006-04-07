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

lib_LTLIBRARIES = libopenss-gui.la

libopenss_gui_la_CXXFLAGS =  \
	-g \
	-I/usr/include \
	$(QTLIB_CPPFLAGS) \
	@PYTHON_CPPFLAGS@ \
	-I. \
	-I$(top_srcdir)/libopenss-framework \
	-I$(top_srcdir)/libopenss-queries \
	-I$(top_srcdir)/libopenss-cli \
	-I$(top_srcdir)/libopenss-message \
	-I$(top_srcdir)/libopenss-guiplugin \
	-I$(top_srcdir)/libopenss-guiimages \
	-I$(top_srcdir)/libopenss-guidialogs \
	-I$(top_srcdir)/plugins/panels/DebugPanel \
	-I$(top_srcdir)/libopenss-guiobjects \
	-I$(top_srcdir)/libopenss-guibase

libopenss_gui_la_LDFLAGS = \
	-export-dynamic -version-info 0:0:0

libopenss_gui_la_LIBADD = $(LIBLTDL) $(QTLIB_LIBS) $(QTLIB_LDFLAGS) -lqassistantclient -L/usr/X11R6/lib -lXext -lX11 -lm -lpthread -L$(top_srcdir)/libopenss-guibase -lopenss-guibase -L$(top_srcdir)/libopenss-guidialogs -lopenss-guidialogs -L$(top_srcdir)/libopenss-guiplugin -lopenss-guiplugin -L$(top_srcdir)/libopenss-guiobjects -lopenss-guiobjects

moc_CLIInterface.cxx: CLIInterface.hxx
	-moc -o moc_CLIInterface.cxx CLIInterface.hxx

moc_openspeedshop.cxx: openspeedshop.hxx
	-moc -o moc_openspeedshop.cxx openspeedshop.hxx

moc_preferencesdialog.cxx: preferencesdialog.hxx
	-moc -o moc_preferencesdialog.cxx preferencesdialog.hxx

moc_AboutClass.cxx: AboutClass.hxx
	-moc -o moc_AboutClass.cxx AboutClass.hxx

libopenss_gui_la_SOURCES = \
    gui.cxx \
    AboutClass.cxx AboutClass.hxx moc_AboutClass.cxx \
	CLIInterface.hxx CLIInterface.cxx moc_CLIInterface.cxx \
    openspeedshop.hxx openspeedshop.cxx moc_openspeedshop.cxx \
    qmake_image_collection.cxx openspeedshop.ui.hxx \
    preferencesdialog.hxx preferencesdialog.cxx moc_preferencesdialog.cxx \
	preferencesdialog.ui.hxx

CLEANFILES=moc_openspeedshop.cxx moc_preferencesdialog.cxx moc_CLIInterface.cxx

doc:
	$(DOXYGEN)
