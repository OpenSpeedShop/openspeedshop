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


pkglib_LTLIBRARIES = ossObjects.la

ossObjects_la_CXXFLAGS = \
	-I/usr/include \
	-I$(QTDIR)/include \
	-I. \
	-I.. \
	-I../images \
	-I../base \
	-I../plugin \
	-I$(QTDIR)

ossObjects_la_LDFLAGS = --export-dynamic -module -avoid-version

ossObjects_la_LIBADD = $(LIBLTDL)

moc_AnimatedQPushButton.cxx: AnimatedQPushButton.hxx
	-moc -o moc_AnimatedQPushButton.cxx AnimatedQPushButton.hxx

moc_ProcessControlObject.cxx: ProcessControlObject.hxx
	-moc -o moc_ProcessControlObject.cxx ProcessControlObject.hxx

moc_InfoEventFilter.cxx: InfoEventFilter.hxx
	-moc -o moc_InfoEventFilter.cxx InfoEventFilter.hxx

ossObjects_la_SOURCES = \
	HighlightObject.hxx \
	ProcessEntryClass.hxx \
	MessageObject.hxx MessageObject.cxx \
	SourceObject.hxx SourceObject.cxx \
	PreferencesChangedObject.hxx PreferencesChangedObject.cxx \
	LoadAttachObject.hxx LoadAttachObject.cxx \
	SaveAsObject.hxx SaveAsObject.cxx \
	ControlObject.hxx ControlObject.cxx \
	ProcessListObject.hxx ProcessListObject.cxx \
	OutputObject.hxx OutputObject.cxx \
	AnimatedQPushButton.hxx AnimatedQPushButton.cxx moc_AnimatedQPushButton.cxx \
	InfoEventFilter.hxx InfoEventFilter.cxx moc_InfoEventFilter.cxx \
	ProcessControlObject.hxx ProcessControlObject.cxx moc_ProcessControlObject.cxx


CLEANFILES=moc_AnimatedQPushButton.cxx moc_ProcessControlObject.cxx moc_InfoEventFilter.cxx
