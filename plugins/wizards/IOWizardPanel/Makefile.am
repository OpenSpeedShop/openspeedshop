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

pkglib_LTLIBRARIES = IOWizardPanel.la

IOWizardPanel_la_CXXFLAGS = \
    -I/usr/include \
	$(QTLIB_CPPFLAGS) \
	$(PYTHON_INCLUDES) \
    -I. \
    -I$(top_srcdir)/libopenss-gui \
    -I$(top_srcdir)/libopenss-guiimages \
    -I$(top_srcdir)/libopenss-guiobjects \
    -I$(top_srcdir)/libopenss-framework \
    -I$(top_srcdir)/libopenss-queries \
    -I$(top_srcdir)/libopenss-cli \
    -I$(top_srcdir)/libopenss-message \
    -I$(top_srcdir)/libopenss-guidialogs \
    -I$(top_srcdir)/libopenss-guiplugin \
    -I$(top_srcdir)/libopenss-guibase

IOWizardPanel_la_LDFLAGS = --export-dynamic \
	--export-all-symbols \
	-module -avoid-version \
	-L$(top_srcdir)/libopenss-framework

IOWizardPanel_la_LIBADD = $(QTLIB_LIBS) $(QTLIB_LDFLAGS) -lopenss-framework

moc_IOWizardPanel.cxx: IOWizardPanel.hxx
	-moc -o moc_IOWizardPanel.cxx IOWizardPanel.hxx

IOWizardPanel_la_SOURCES = \
	IOWizardPanel.hxx IOWizardPanel.cxx \
    moc_IOWizardPanel.cxx IODescription.hxx \
	local_plugin_info.hxx

CLEANFILES=moc_IOWizardPanel.cxx
