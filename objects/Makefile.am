################################################################################
# License Applicability.  Except to the extent portions of this file are
# made subject to an alternative license as permitted in the SGI Free
# Software License B, Version 1.1 (the "License"), the contents of this
# file are subject only to the provisions of the License. You may not use
# this file except in compliance with the License. You may obtain a copy
# of the License at Silicon Graphics, Inc., attn: Legal Services, 1500
# Crittenden Lane, Moutain View, CA 94043, or at:
# 
#      http://oss.sgi.com/projects/FreeB
# 
# Note that, as provided in the License, the Software is distributed on
# on "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
# DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
# CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
# PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
# 
# Original Code.  The Original Code is: Open/Speedshop, Version 1.0, released
# <MONTH>, <DAY>, 2004, developed by Silicon Graphics, Inc. The Original
# Code is Copyright (c) 2004 Silicon Graphics, Inc. Copyright in any
# portions created by third parties is as indicated elsewhere herein.
# All Rights Reserved.
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
	LoadAttachObject.hxx LoadAttachObject.cxx \
	SaveAsObject.hxx SaveAsObject.cxx \
	ControlObject.hxx ControlObject.cxx \
	ProcessListObject.hxx ProcessListObject.cxx \
	OutputObject.hxx OutputObject.cxx \
	AnimatedQPushButton.hxx AnimatedQPushButton.cxx moc_AnimatedQPushButton.cxx \
	InfoEventFilter.hxx InfoEventFilter.cxx moc_InfoEventFilter.cxx \
	ProcessControlObject.hxx ProcessControlObject.cxx moc_ProcessControlObject.cxx


clean:
	-rm moc_*
