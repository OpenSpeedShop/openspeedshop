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
# Original Code.  The Original Code is: FutureTool, Version 1.0, released
# <MONTH>, <DAY>, 2004, developed by Silicon Graphics, Inc. The Original
# Code is Copyright (c) 2004 Silicon Graphics, Inc. Copyright in any
# portions created by third parties is as indicated elsewhere herein.
# All Rights Reserved.
################################################################################

pkglib_LTLIBRARIES = ftgui.la

ftgui_la_CXXFLAGS =  \
	-g \
	-I/usr/include \
	-I$(QTDIR)/include \
	-I. \
	-I../plugin \
	-I../images \
	-I../base

ftgui_la_LDFLAGS = --export-dynamic --export-all-symbols -module -avoid-version -Wl,../plugin/.libs/ftPlugin.so -Wl,../base/.libs/ftBase.so

ftgui_la_LIBADD = -L$(QTDIR)/lib -lqt -lqassistantclient -L/usr/X11R6/lib -lXext -lX11 -lm

moc_performanceleader.cxx: performanceleader.hxx
	-moc -o moc_performanceleader.cxx performanceleader.hxx

ftgui_la_SOURCES = \
	gui.hxx gui.cxx \
    performanceleader.hxx performanceleader.cxx moc_performanceleader.cxx qmake_image_collection.cxx performanceleader.ui.hxx \
	Panel.hxx PanelContainer.hxx debug.hxx
