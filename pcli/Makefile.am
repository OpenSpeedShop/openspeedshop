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

pkglib_LTLIBRARIES = posscli.la

posscli_la_CXXFLAGS =  \
	-c -LANG:std -fpic \
	-I. \
	-I../libssf \
	-I/perftools/ROOT/include/python2.3 \
	-I/usr/include -I. -I../objects -I../base

posscli_la_LDFLAGS = \
    	-fpic \
	--export-dynamic \
	-module \
	-avoid-version \
	-Wl,/perftools/ROOT/lib/libpython2.3.so \
	-lpthread -lutil -LANG:std -ldl

posscli_la_LIBADD = $(LIBLTDL)

posscli_la_SOURCES = \
    	Start.cxx \
	Start_Modes.cxx \
	Commander.cxx \
	Experiment.cxx \
	support.cxx \
	help.cxx \
	lex.yy.cxx \
	y.tab.cxx \
	PY_Input.cxx




bin_PROGRAMS = pcli

pcli:
	-rm -f $(OPENSPEEDSHOP_PLUGIN_PATH)/myparse.py;
	-cp myparse.py $(OPENSPEEDSHOP_PLUGIN_PATH);
	-rm -f $(OPENSPEEDSHOP_PLUGIN_PATH)/../../bin/openss;
	-cp ../openss $(OPENSPEEDSHOP_PLUGIN_PATH)/../../bin;

doc:
	-doxygen Doxyfile
