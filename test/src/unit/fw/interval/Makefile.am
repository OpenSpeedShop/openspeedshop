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
	interval1 \
	interval2 \
	interval3 \
	interval4 \
	interval5 \
	interval6 \
	interval7

interval_CXXFLAGS =  \
	-I. \
	-I$(top_srcdir)/libopenss-framework \
	-I/usr/include -I.

interval1_CXXFLAGS = \
	$($(interval_CXXFLAGS))

interval1_SOURCES = \
	interval1.cxx

interval2_CXXFLAGS = \
	$($(interval_CXXFLAGS))

interval2_SOURCES = \
	interval2.cxx

interval3_CXXFLAGS = \
	$(interval_CXXFLAGS)

interval3_SOURCES = \
	interval3.cxx

interval4_CXXFLAGS = \
        $(interval_CXXFLAGS)

interval4_SOURCES = \
        interval4.cxx

interval5_CXXFLAGS = \
        $(interval_CXXFLAGS)

interval5_SOURCES = \
        interval5.cxx

interval6_CXXFLAGS = \
        $(interval_CXXFLAGS)

interval6_SOURCES = \
        interval6.cxx

interval7_CXXFLAGS = \
        $(interval_CXXFLAGS)

interval7_SOURCES = \
        interval7.cxx

TESTS = $(check_PROGRAMS)

dist_interval_sources	= \
	interval1.cxx  interval4.cxx  interval7.cxx  \
	interval2.cxx  interval5.cxx   \
	interval3.cxx  interval6.cxx  
	
EXTRA_DIST	= \
	test_config runall test_list rununit

