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

pkglib_LTLIBRARIES = ossBase.la

ossBase_la_CXXFLAGS = \
    -I../objects \
    -I/usr/include \
    -I$(QTDIR)/include \
    -I. \
    -I../plugin \
    -I../images \
    -I../gui \
    -I../base


ossBase_la_LDFLAGS = --export-dynamic -module -avoid-version

ossBase_la_LIBADD = $(LIBLTDL)

moc_TopWidget.cxx: TopWidget.hxx
	-moc -o moc_TopWidget.cxx TopWidget.hxx

moc_TabWidget.cxx: TabWidget.hxx
	-moc -o moc_TabWidget.cxx TabWidget.hxx

moc_TabBarWidget.cxx: TabBarWidget.hxx
	-moc -o moc_TabBarWidget.cxx TabBarWidget.hxx

moc_Frame.cxx: Frame.hxx
	-moc -o moc_Frame.cxx Frame.hxx

moc_PanelContainer.cxx: PanelContainer.hxx
	-moc -o moc_PanelContainer.cxx PanelContainer.hxx

moc_Panel.cxx: Panel.hxx
	-moc -o moc_Panel.cxx Panel.hxx

ossBase_la_SOURCES = \
	pc_plugin.cxx \
	DragNDropPanel.hxx DragNDropPanel.cxx \
	TopWidget.hxx TopWidget.cxx moc_TopWidget.cxx \
	TabWidget.hxx TabWidget.cxx moc_TabWidget.cxx \
	TabBarWidget.hxx TabBarWidget.cxx moc_TabBarWidget.cxx \
	WhatsThis.hxx WhatsThis.cxx  \
	Frame.hxx Frame.cxx moc_Frame.cxx \
	Panel.hxx Panel.cxx moc_Panel.cxx \
	PanelContainer.hxx PanelContainer.cxx moc_PanelContainer.cxx \
	debug.hxx

doc:
	-doxygen Doxyfile
