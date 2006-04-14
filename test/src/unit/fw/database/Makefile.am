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

filelist = \
	dbtest1 \
	dbtest2 \
	dbtest3 \
	dbtest4 \
	dbtest5 

all: $(filelist)

dbtest1: dbtest1.cxx
	-g++ -o dbtest1 $(database_la_CXXFLAGS) $(database_la_LDFLAGS) dbtest1.cxx

dbtest2: dbtest2.cxx
	-g++ -o dbtest2 $(database_la_CXXFLAGS) $(database_la_LDFLAGS) dbtest2.cxx

dbtest3: dbtest3.cxx
	-g++ -o dbtest3 $(database_la_CXXFLAGS) $(database_la_LDFLAGS) dbtest3.cxx

dbtest4: dbtest4.cxx
	-g++ -o dbtest4 $(database_la_CXXFLAGS) $(database_la_LDFLAGS) dbtest4.cxx

dbtest5: dbtest5.cxx
	-g++ -o dbtest5 $(database_la_CXXFLAGS) $(database_la_LDFLAGS) dbtest5.cxx

clean:
	rm $(filelist)
