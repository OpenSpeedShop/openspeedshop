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
	-I../../../libopenss-framework \
	-I/usr/include -I.

database_la_LDFLAGS = \
	-L/perftools/ROOT/lib \
	-LANG:std -lsqlite3

database_la_LIBADD = $(LIBLTDL)

all: interval1 interval2 interval3 interval4 interval5 interval6 interval7

interval1: interval1.cxx
	-g++ -o interval1 $(database_la_CXXFLAGS) $(database_la_LDFLAGS) interval1.cxx

interval2: interval2.cxx
	-g++ -o interval2 $(database_la_CXXFLAGS) $(database_la_LDFLAGS) interval2.cxx

interval3: interval3.cxx
	-g++ -o interval3 $(database_la_CXXFLAGS) $(database_la_LDFLAGS) interval3.cxx

interval4: interval4.cxx
	-g++ -o interval4 $(database_la_CXXFLAGS) $(database_la_LDFLAGS) interval4.cxx

interval5: interval5.cxx
	-g++ -o interval5 $(database_la_CXXFLAGS) $(database_la_LDFLAGS) interval5.cxx

interval6: interval6.cxx
	-g++ -o interval6 $(database_la_CXXFLAGS) $(database_la_LDFLAGS) interval6.cxx

interval7: interval7.cxx
	-g++ -o interval7 $(database_la_CXXFLAGS) $(database_la_LDFLAGS) interval7.cxx

