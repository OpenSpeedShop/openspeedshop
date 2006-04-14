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

check_PROGRAMS = \
        addbitmap1 \
        addbitmap2 \
        blob1

utility_CXXFLAGS =  \
	-I. \
	-Ii$(top_srcdir)/libopenss-framework \
	-I/usr/include -I.

addbitmap1_CXXFLAGS = \
	$(utility_CXXFLAGS)

addbitmap1_SOURCES = \
	addbitmap1.cxx

addbitmap2_CXXFLAGS = \
	$(utility_CXXFLAGS)

addbitmap2_SOURCES = \
	addbitmap2.cxx

blob1_CXXFLAGS = \
	$(utility_CXXFLAGS)

blob1_SOURCES = \
	blob1.cxx

TESTS = $(check_PROGRAMS)

dist_utility_sources = \
	addbitmap1.cxx  blob1.cxx  addbitmap2.cxx 

EXTRA_DIST	= \
	rununit test_list runall test_config
