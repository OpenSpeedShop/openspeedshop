################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2013 Krell Institute All Rights Reserved.
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


#EXTRA_DIST = CompareInfo.hxx

lib_LTLIBRARIES = libopenss-guiobjects.la

libopenss_guiobjects_la_CXXFLAGS = \
	@PYTHON_CPPFLAGS@ \
	$(QTLIB_CPPFLAGS) \
	-I/usr/include \
	-I. \
	-I$(top_srcdir) \
	-I$(top_srcdir)/libopenss-guiimages \
	-I$(top_srcdir)/libopenss-guibase \
	-I$(top_srcdir)/libopenss-framework \
	-I$(top_srcdir)/libopenss-queries \
	-I$(top_srcdir)/libopenss-cli \
	-I$(top_srcdir)/libopenss-message \
	-I$(top_srcdir)/libopenss-gui \
	-I$(top_srcdir)/libopenss-guiplugin \
	-I$(LTDLINCL)

libopenss_guiobjects_la_LDFLAGS = \
	$(QTLIB_LDFLAGS) \
	-export-dynamic -version-info 0:0:0

libopenss_guiobjects_la_LIBADD = \
	$(LIBLTDL) \
	$(QTLIB_LIBS) \
	$(top_srcdir)/libopenss-cli/libopenss-cli.la \
	$(top_srcdir)/libopenss-framework/libopenss-framework.la \
	$(top_srcdir)/libopenss-guibase/libopenss-guibase.la
	

moc_ProcessControlObject.cxx: ProcessControlObject.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_ProcessControlObject.cxx ProcessControlObject.hxx

moc_InfoEventFilter.cxx: InfoEventFilter.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_InfoEventFilter.cxx InfoEventFilter.hxx

libopenss_guiobjects_la_SOURCES = \
	HighlightObject.hxx \
	ProcessEntryClass.hxx \
	CollectorEntryClass.hxx \
	CompareInfo.hxx \
	MessageObject.hxx MessageObject.cxx \
	SourceObject.hxx SourceObject.cxx \
	UpdateObject.hxx UpdateObject.cxx \
	ExpIDInUseObject.hxx ExpIDInUseObject.cxx \
	FocusObject.hxx FocusObject.cxx \
	FocusCompareObject.hxx FocusCompareObject.cxx \
	ArgumentObject.hxx ArgumentObject.cxx \
	ClosingDownObject.hxx ClosingDownObject.cxx \
	PrepareToRerunObject.hxx PrepareToRerunObject.cxx \
	PreferencesChangedObject.hxx PreferencesChangedObject.cxx \
	LoadAttachObject.hxx LoadAttachObject.cxx \
	SaveAsObject.hxx SaveAsObject.cxx \
	ControlObject.hxx ControlObject.cxx \
	ProcessListObject.hxx ProcessListObject.cxx \
	CollectorListObject.hxx CollectorListObject.cxx \
	OutputObject.hxx OutputObject.cxx \
	AnimatedQPushButton.hxx AnimatedQPushButton.cxx \
	LocalToolbar.hxx LocalToolbar.cxx \
	InfoEventFilter.hxx InfoEventFilter.cxx \
	ProcessControlObject.hxx ProcessControlObject.cxx \
	DescriptionClassObject.hxx DescriptionClassObject.cxx \
	CollectorParameterEntryClass.hxx \
	CollectorMetricEntryClass.hxx

nodist_libopenss_guiobjects_la_SOURCES = \
	moc_ProcessControlObject.cxx \
	moc_InfoEventFilter.cxx

CLEANFILES=$(nodist_libopenss_guiobjects_la_SOURCES)
