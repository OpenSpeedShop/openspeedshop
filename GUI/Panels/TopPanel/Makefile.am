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

pkglib_LTLIBRARIES = TopPanel.la

TopPanel_la_CXXFLAGS = \
	-I/usr/include \
	-I$(QTDIR)/include \
	-I. \
	-I../../objects \
	-I../../expr_info \
	-I../../chart \
	-I../../gui \
	-I../../plugin \
	-I../../base

TopPanel_la_LDFLAGS = --export-dynamic --export-all-symbols -module -avoid-version -Wl,../../base/.libs/ftBase.so -Wl,../../expr_info/.libs/ftExprInfo.so -Wl,../../objects/.libs/ftObjects.so -Wl,../../chart/.libs/ftChart.so

TopPanel_la_LIBADD = -L$(QTDIR)/lib -lqt

moc_TopPanel.cxx: TopPanel.hxx
	-moc -o moc_TopPanel.cxx TopPanel.hxx

TopPanel_la_SOURCES = \
	TopPanel.hxx TopPanel.cxx moc_TopPanel.cxx \
	PanelContainer.hxx local_plugin_info.hxx debug.hxx \
	plugin_entry_point.hxx Panel.hxx \
	MyQTextEdit.hxx MyQTextEdit.cxx \
	MyChartForm.hxx MyChartForm.cxx
