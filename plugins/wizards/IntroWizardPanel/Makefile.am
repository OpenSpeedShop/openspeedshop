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

pkglib_LTLIBRARIES = IntroWizardPanel.la

IntroWizardPanel_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-gui \
	-I$(top_srcdir)/libopenss-guibase \
	-I$(top_srcdir)/libopenss-guiobjects \
	-I$(top_srcdir)/libopenss-guiplugin \
	@QTLIB_CPPFLAGS@

IntroWizardPanel_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-guibase \
	-L$(top_srcdir)/libopenss-guiobjects \
	@QTLIB_LDFLAGS@ \
	-export-dynamic -export-all-symbols -module -avoid-version

IntroWizardPanel_la_LIBADD = \
	-lopenss-guibase \
	-lopenss-guiobjects \
	@QTLIB_LIBS@

IntroWizardPanel_la_SOURCES = \
	IntroWizardPanel.hxx IntroWizardPanel.cxx moc_IntroWizardPanel.cxx \
	local_plugin_info.hxx

moc_IntroWizardPanel.cxx: IntroWizardPanel.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_IntroWizardPanel.cxx IntroWizardPanel.hxx

CLEANFILES = \
	moc_IntroWizardPanel.cxx
