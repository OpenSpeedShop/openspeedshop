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

pkglib_LTLIBRARIES = pcSamplePanel.la

# NOTE 1: The -I../SourcePanel is only here for #define DEMO 1
# NOTE 2: The -I../TopPanel is only here for #define DEMO 1
pcSamplePanel_la_CXXFLAGS = \
    -I/usr/include \
    -I$(QTDIR)/include \
    -I. \
    -I../../gui \
    -I../../images \
    -I../../plugin \
    -I../../objects \
    -I../SourcePanel \
    -I../TopPanel \
    -I../../base

pcSamplePanel_la_LDFLAGS = --export-dynamic --export-all-symbols -module -avoid-version -Wl,../../base/.libs/ossBase.so -Wl,../../objects/.libs/ossObjects.so

pcSamplePanel_la_LIBADD = -L$(QTDIR)/lib -lqt

moc_pcSamplePanel.cxx: pcSamplePanel.hxx
	-moc -o moc_pcSamplePanel.cxx pcSamplePanel.hxx

pcSamplePanel_la_SOURCES = \
	pcSamplePanel.hxx pcSamplePanel.cxx moc_pcSamplePanel.cxx \
	plugin_entry_point.hxx Panel.hxx \
	PanelContainer.hxx local_plugin_info.hxx debug.hxx

clean:
	-rm moc_*
