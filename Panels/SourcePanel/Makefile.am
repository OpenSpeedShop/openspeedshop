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
# <MONTH>, <DAY>, 2005, developed by Silicon Graphics, Inc. The Original
# Code is Copyright (c) 2005 Silicon Graphics, Inc. Copyright in any
# portions created by third parties is as indicated elsewhere herein.
# All Rights Reserved.
################################################################################

pkglib_LTLIBRARIES = SourcePanel.la

SourcePanel_la_CXXFLAGS = \
	-I../../objects \
    -I/usr/include \
    -I$(QTDIR)/include \
    -I. \
    -I../../gui \
    -I../../plugin \
    -I../../base

SourcePanel_la_LDFLAGS = --export-dynamic --export-all-symbols -module -avoid-version -Wl,../../base/.libs/ossBase.so

SourcePanel_la_LIBADD = ../../objects/.libs/ossObjects.so -L$(QTDIR)/lib -lqt

moc_SPTextEdit.cxx: SPTextEdit.hxx
	-moc -o moc_SPTextEdit.cxx SPTextEdit.hxx

moc_SourcePanel.cxx: SourcePanel.hxx
	-moc -o moc_SourcePanel.cxx SourcePanel.hxx

moc_SPCanvasView.cxx: SPCanvasView.hxx
	-moc -o moc_SPCanvasView.cxx SPCanvasView.hxx

moc_SPCanvasForm.cxx: SPCanvasForm.hxx
	-moc -o moc_SPCanvasForm.cxx SPCanvasForm.hxx

SourcePanel_la_SOURCES = \
    SPCanvasForm.hxx SPCanvasForm.cxx moc_SPCanvasForm.cxx \
    SPCanvasView.hxx SPCanvasView.cxx moc_SPCanvasView.cxx \
    SPTextEdit.hxx SPTextEdit.cxx moc_SPTextEdit.cxx \
	SourcePanel.hxx SourcePanel.cxx moc_SourcePanel.cxx \
	PanelContainer.hxx local_plugin_info.hxx debug.hxx \
	plugin_entry_point.hxx Panel.hxx HighlightObject.hxx

CLEANFILES=moc_SPTextEdit.cxx moc_SourcePanel.cxx moc_SPCanvasView.cxx moc_SPCanvasForm.cxx
