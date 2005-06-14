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

database_la_CXXFLAGS =  \
	-I. \
	-I../../../../../libopenss-framework \
	-I/usr/include -I.

database_la_LDFLAGS = \
	-L/perftools/ROOT/lib \
	-Wl,../../../../../libopenss-framework/.libs/libopenss-framework.so \
	-LANG:std -lsqlite3

database_la_LIBADD = $(LIBLTDL)

filelist = \
	addbitmap1 \
	addbitmap2 \
	blob1 

all: $(filelist)

addbitmap1: addbitmap1.cxx
	-g++ -o addbitmap1 $(database_la_CXXFLAGS) $(database_la_LDFLAGS) addbitmap1.cxx

addbitmap2: addbitmap2.cxx
	-g++ -o addbitmap2 $(database_la_CXXFLAGS) $(database_la_LDFLAGS) addbitmap2.cxx

blob1: blob1.cxx
	-g++ -o blob1 $(database_la_CXXFLAGS) $(database_la_LDFLAGS) blob1.cxx

clean:
	rm $(filelist)
