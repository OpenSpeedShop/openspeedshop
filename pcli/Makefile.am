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

bin_PROGRAMS = pcli

pcli_la_CXXFLAGS = \
	-c -LANG:std \
	-I. \
	-I../libssf \
	-I/usr/local/include/python2.3

LDFLAGS = /usr/local/lib/python2.3/config/libpython2.3.a -lpthread -lmpi -lutil -LANG:std

pcli:
	if test -f /usr/local/lib/python2.3/config/libpython2.3.a ; then \
		g++ $(pcli_la_CXXFLAGS) -fpic -c Start.cxx; \
		g++ $(pcli_la_CXXFLAGS) -fpic -c Start_Modes.cxx; \
		g++ $(pcli_la_CXXFLAGS) -fpic -c Commander.cxx; \
		g++ $(pcli_la_CXXFLAGS) -fpic -c Experiment.cxx; \
		g++ $(pcli_la_CXXFLAGS) -fpic -c SS_Util.cxx; \
		g++ -dynamic -fpic -o pcli Start.o Start_Modes.o Commander.o Experiment.o SS_Util.o $(LDFLAGS); \
	 fi;

