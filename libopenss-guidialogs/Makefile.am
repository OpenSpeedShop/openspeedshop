################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006, 2007 Krell Institute All Rights Reserved.
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

lib_LTLIBRARIES = libopenss-guidialogs.la

libopenss_guidialogs_la_CXXFLAGS = \
	@PYTHON_CPPFLAGS@ \
	$(QTLIB_CPPFLAGS) \
	-I/usr/include \
	-I. \
	-I$(top_srcdir) \
	-I$(top_srcdir)/libopenss-framework \
	-I$(top_srcdir)/libopenss-gui \
	-I$(top_srcdir)/libopenss-queries \
	-I$(top_srcdir)/libopenss-cli \
	-I$(top_srcdir)/libopenss-message \
	-I$(top_srcdir)/libopenss-guiimages \
	-I$(top_srcdir)/libopenss-guibase \
	-I$(top_srcdir)/libopenss-guiplugin \
	-I$(top_srcdir)/libopenss-guiobjects \
	-I$(top_srcdir)/libopenss-guichart

libopenss_guidialogs_la_LDFLAGS = \
	$(QTLIB_LDFLAGS) \
	-L../libopenss-guiobjects \
	-export-dynamic -version-info 0:0:0

libopenss_guidialogs_la_LIBADD = $(LIBLTDL) \
	$(QTLIB_LIBS) \
	$(top_srcdir)/libopenss-cli/libopenss-cli.la \
	$(top_srcdir)/libopenss-guichart/libopenss-guichart.la \
	$(top_srcdir)/libopenss-guiobjects/libopenss-guiobjects.la

moc_AttachArgDialog.cxx: AttachArgDialog.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_AttachArgDialog.cxx AttachArgDialog.hxx

moc_AttachProcessDialog.cxx: AttachProcessDialog.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_AttachProcessDialog.cxx AttachProcessDialog.hxx

moc_AttachMPProcessDialog.cxx: AttachMPProcessDialog.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_AttachMPProcessDialog.cxx AttachMPProcessDialog.hxx

moc_AttachProgramDialog.cxx: AttachProgramDialog.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_AttachProgramDialog.cxx AttachProgramDialog.hxx

moc_AttachMPProgramDialog.cxx: AttachMPProgramDialog.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_AttachMPProgramDialog.cxx AttachMPProgramDialog.hxx

moc_ManageProcessesDialog.cxx: ManageProcessesDialog.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_ManageProcessesDialog.cxx ManageProcessesDialog.hxx

moc_SelectExperimentDialog.cxx: SelectExperimentDialog.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_SelectExperimentDialog.cxx SelectExperimentDialog.hxx

moc_SaveAsExperimentDialog.cxx: SaveAsExperimentDialog.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_SaveAsExperimentDialog.cxx SaveAsExperimentDialog.hxx

moc_GenericProgressDialog.cxx: GenericProgressDialog.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_GenericProgressDialog.cxx GenericProgressDialog.hxx

moc_SelectTimeSegmentDialog.cxx: SelectTimeSegmentDialog.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_SelectTimeSegmentDialog.cxx SelectTimeSegmentDialog.hxx

libopenss_guidialogs_la_SOURCES = \
	AttachArgDialog.hxx AttachArgDialog.cxx \
	AttachProcessDialog.hxx AttachProcessDialog.cxx \
	AttachMPProcessDialog.hxx AttachMPProcessDialog.cxx \
	AttachProgramDialog.hxx AttachProgramDialog.cxx \
	AttachMPProgramDialog.hxx AttachMPProgramDialog.cxx \
	ManageProcessesDialog.hxx ManageProcessesDialog.cxx \
	PanelListViewItem.hxx PanelListViewItem.cxx \
	GenericProgressDialog.hxx GenericProgressDialog.cxx \
	SelectTimeSegmentDialog.hxx SelectTimeSegmentDialog.cxx \
	SelectExperimentDialog.hxx SelectExperimentDialog.cxx \
	SaveAsExperimentDialog.hxx SaveAsExperimentDialog.cxx \
	AboutDialog.hxx AboutDialog.cxx

nodist_libopenss_guidialogs_la_SOURCES = \
	moc_AttachArgDialog.cxx \
	moc_AttachProcessDialog.cxx \
	moc_AttachMPProcessDialog.cxx \
	moc_AttachProgramDialog.cxx \
	moc_AttachMPProgramDialog.cxx \
	moc_ManageProcessesDialog.cxx \
	moc_GenericProgressDialog.cxx \
	moc_SelectTimeSegmentDialog.cxx \
	moc_SelectExperimentDialog.cxx \
	moc_SaveAsExperimentDialog.cxx

CLEANFILES=moc_AttachArgDialog.cxx moc_AttachProcessDialog.cxx moc_AttachMPProcessDialog.cxx moc_AttachProgramDialog.cxx moc_AttachMPProgramDialog.cxx moc_SelectExperimentDialog.cxx moc_GenericProgressDialog.cxx moc_ManageCollectorsDialog.cxx 
