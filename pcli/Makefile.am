################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 2.1 of the License, or (at your option)
# any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
################################################################################

pkglib_LTLIBRARIES = posscli.la

posscli_la_CXXFLAGS =  \
	-c -LANG:std -fpic \
	-I. \
	-I../libopenss-framework \
	-I/perftools/ROOT/include/python2.3 \
	-I/usr/include -I. -I../objects -I../base

posscli_la_LDFLAGS = \
    	-fpic \
	--export-dynamic \
	-module \
	-avoid-version \
	-L../libopenss-framework \
	-Wl,/perftools/ROOT/lib/libpython2.3.so \
	-lpthread -lutil -LANG:std -ldl

posscli_la_LIBADD = $(LIBLTDL) -lopenss-framework

LEXWOFF =
YFLAGS = -d
LFILES = test.l
YFILES = test.y

CLEANFILES = \
    	lex.yy.cxx \
	y.tab.cxx \
	y.tab.h

test_SOURCES = \
	$(LFILES) $(YFILES)
	
posscli_la_SOURCES = \
    	Start.cxx \
	Start_Modes.cxx \
	Commander.cxx \
	CommandObject.cxx \
	SS_Cmd_Execution.cxx \
	SS_Parse_Range.cxx \
	SS_Parse_Result.cxx \
	SS_Parse_Target.cxx \
	Experiment.cxx \
	support.cxx \
	help.cxx \
	y.tab.cxx \
	lex.yy.cxx \
	PY_Input.cxx

y.tab.cxx:
	-yacc $(YFLAGS) $(YFILES);
	-mv y.tab.c y.tab.cxx

lex.yy.cxx: y.tab.cxx
	-lex $(LFILES);
	-mv lex.yy.c lex.yy.cxx

clobber:
	-rm *.lo *.la lex.yy.cxx y.tab.cxx y.tab.h

bin_PROGRAMS = pcli

pcli:
	-rm -f $(OPENSPEEDSHOP_PLUGIN_PATH)/myparse.py;
	-cp myparse.py $(OPENSPEEDSHOP_PLUGIN_PATH);
	-rm -f $(OPENSPEEDSHOP_PLUGIN_PATH)/../../bin/openss;
	-cp ../openss $(OPENSPEEDSHOP_PLUGIN_PATH)/../../bin;

doc:
	-doxygen Doxyfile
