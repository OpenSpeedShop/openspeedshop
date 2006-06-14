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

lib_LTLIBRARIES = libopenss-guidialogs.la

libopenss_guidialogs_la_CXXFLAGS = \
	@PYTHON_CPPFLAGS@ \
	-I/usr/include \
	-I$(QTDIR)/include \
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
    -I$(top_srcdir)/libopenss-guichart \
	-I$(QTDIR)

libopenss_guidialogs_la_LDFLAGS = \
	-export-dynamic -version-info 0:0:0

libopenss_guidialogs_la_LIBADD = $(LIBLTDL)  -L../libopenss-guiobjects -lopenss-guiobjects

moc_AttachProcessDialog.cxx: AttachProcessDialog.hxx
	-moc -o moc_AttachProcessDialog.cxx AttachProcessDialog.hxx

moc_AttachProgramDialog.cxx: AttachProgramDialog.hxx
	-moc -o moc_AttachProgramDialog.cxx AttachProgramDialog.hxx

moc_ManageProcessesDialog.cxx: ManageProcessesDialog.hxx
	-moc -o moc_ManageProcessesDialog.cxx ManageProcessesDialog.hxx

moc_SelectExperimentDialog.cxx: SelectExperimentDialog.hxx
	-moc -o moc_SelectExperimentDialog.cxx SelectExperimentDialog.hxx

moc_SaveAsExperimentDialog.cxx: SaveAsExperimentDialog.hxx
	-moc -o moc_SaveAsExperimentDialog.cxx SaveAsExperimentDialog.hxx

moc_GenericProgressDialog.cxx: GenericProgressDialog.hxx
	-moc -o moc_GenericProgressDialog.cxx GenericProgressDialog.hxx

moc_SelectTimeSegmentDialog.cxx: SelectTimeSegmentDialog.hxx
	-moc -o moc_SelectTimeSegmentDialog.cxx SelectTimeSegmentDialog.hxx

libopenss_guidialogs_la_SOURCES = \
	AttachProcessDialog.hxx AttachProcessDialog.cxx moc_AttachProcessDialog.cxx \
	AttachProgramDialog.hxx AttachProgramDialog.cxx moc_AttachProgramDialog.cxx \
	ManageProcessesDialog.hxx ManageProcessesDialog.cxx moc_ManageProcessesDialog.cxx \
	PanelListViewItem.hxx PanelListViewItem.cxx \
	GenericProgressDialog.hxx GenericProgressDialog.cxx moc_GenericProgressDialog.cxx \
	SelectTimeSegmentDialog.hxx SelectTimeSegmentDialog.cxx moc_SelectTimeSegmentDialog.cxx \
	SelectExperimentDialog.hxx SelectExperimentDialog.cxx moc_SelectExperimentDialog.cxx \
	SaveAsExperimentDialog.hxx SaveAsExperimentDialog.cxx moc_SaveAsExperimentDialog.cxx \
	AboutDialog.hxx AboutDialog.cxx

CLEANFILES=moc_AttachProcessDialog.cxx moc_AttachProgramDialog.cxx moc_SelectExperimentDialog.cxx moc_GenericProgressDialog.cxx moc_ManageCollectorsDialog.cxx 
