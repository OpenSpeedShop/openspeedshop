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

pkglib_LTLIBRARIES = ftcli.la

ftcli_la_CXXFLAGS =  -I/usr/include -I. -I../objects -I../base

ftcli_la_LDFLAGS = --export-dynamic -module -avoid-version

ftcli_la_LIBADD = $(LIBLTDL)

ftcli_la_SOURCES = cli.cxx ../objects/OutputObject.cxx





bin_PROGRAMS = cli

cli_la_CXXFLAGS =  -I/usr/include -I. -I../objects -I../base

LDFLAGS = -lc -ldl

cli_SOURCES =  main.cxx cli.cxx ../objects/OutputObject.cxx

cli:
	-g++ $(ftcli_la_CXXFLAGS) -fpic -c cli.cxx;
	-g++ $(ftcli_la_CXXFLAGS) -fpic -c main.cxx;
	-g++ $(ftcli_la_CXXFLAGS) -fpic -c ../objects/OutputObject.cxx;
	-g++ -dynamic -fpic -o cli main.o cli.o OutputObject.o -lc -ldl;
