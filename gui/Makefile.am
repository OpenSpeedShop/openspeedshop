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

pkglib_LTLIBRARIES = ossgui.la

ossgui_la_CXXFLAGS =  \
	-g \
	-I/usr/include \
	-I$(QTDIR)/include \
	-I. \
	-I../libopenss-framework \
	-I../pcli \
	-I../plugin \
	-I../images \
	-I../dialogs \
	-I../objects \
	-I../base

ossgui_la_LDFLAGS = --export-dynamic --export-all-symbols -module -avoid-version -Wl,../plugin/.libs/libopenss_Plugin.so -Wl,../base/.libs/libopenss_Base.so -Wl,../objects/.libs/ossObjects.so -Wl,../dialogs/.libs/ossDialogs.so -Wl,../pcli/.libs/posscli.so

ossgui_la_LIBADD = -L$(QTDIR)/lib -lqt -lqassistantclient -L/usr/X11R6/lib -lXext -lX11 -lm -lpthread

moc_CLIInterface.cxx: CLIInterface.hxx
	-moc -o moc_CLIInterface.cxx CLIInterface.hxx

moc_openspeedshop.cxx: openspeedshop.hxx
	-moc -o moc_openspeedshop.cxx openspeedshop.hxx

moc_preferencesdialog.cxx: preferencesdialog.hxx
	-moc -o moc_preferencesdialog.cxx preferencesdialog.hxx

ossgui_la_SOURCES = \
	gui.hxx gui.cxx \
	CLIInterface.hxx CLIInterface.cxx moc_CLIInterface.cxx \
    openspeedshop.hxx openspeedshop.cxx moc_openspeedshop.cxx \
    qmake_image_collection.cxx openspeedshop.ui.hxx \
    preferencesdialog.hxx preferencesdialog.cxx moc_preferencesdialog.cxx \
	Panel.hxx PanelContainer.hxx debug.hxx preferencesdialog.ui.hxx

doc:
	-doxygen Doxyfile

CLEANFILES=moc_openspeedshop.cxx moc_preferencesdialog.cxx moc_CLIInterface.cxx
