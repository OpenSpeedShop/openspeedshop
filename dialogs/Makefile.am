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

lib_LTLIBRARIES = libopenss-Dialogs.la

libopenss_Dialogs_la_CXXFLAGS = \
	-I/usr/include \
	-I$(QTDIR)/include \
	-I. \
	-I$(top_srcdir) \
	-I$(top_srcdir)/libopenss-framework \
	-I$(top_srcdir)/gui \
	-I$(top_srcdir)/pcli \
	-I$(top_srcdir)/images \
	-I$(top_srcdir)/base \
	-I$(top_srcdir)/plugin \
    -I$(top_srcdir)/objects \
	-I$(QTDIR)

libopenss_Dialogs_la_LDFLAGS = --export-dynamic -avoid-version

libopenss_Dialogs_la_LIBADD = $(LIBLTDL)  -L../objects -lopenss-Objects

moc_AttachProcessDialog.cxx: AttachProcessDialog.hxx
	-moc -o moc_AttachProcessDialog.cxx AttachProcessDialog.hxx

moc_ManageCollectorsDialog.cxx: ManageCollectorsDialog.hxx
	-moc -o moc_ManageCollectorsDialog.cxx ManageCollectorsDialog.hxx

moc_SelectExperimentDialog.cxx: SelectExperimentDialog.hxx
	-moc -o moc_SelectExperimentDialog.cxx SelectExperimentDialog.hxx

moc_GenericProgressDialog.cxx: GenericProgressDialog.hxx
	-moc -o moc_GenericProgressDialog.cxx GenericProgressDialog.hxx

libopenss_Dialogs_la_SOURCES = \
	AttachProcessDialog.hxx AttachProcessDialog.cxx moc_AttachProcessDialog.cxx \
	ManageCollectorsDialog.hxx ManageCollectorsDialog.cxx moc_ManageCollectorsDialog.cxx \
	PanelListViewItem.hxx PanelListViewItem.cxx \
	GenericProgressDialog.hxx GenericProgressDialog.cxx moc_GenericProgressDialog.cxx \
	SelectExperimentDialog.hxx SelectExperimentDialog.cxx moc_SelectExperimentDialog.cxx

CLEANFILES=moc_AttachProcessDialog.cxx moc_SelectExperimentDialog.cxx moc_GenericProgressDialog.cxx moc_ManageCollectorsDialog.cxx
