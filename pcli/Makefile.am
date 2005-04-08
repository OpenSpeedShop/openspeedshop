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

pkglib_LTLIBRARIES = libopenss-cli.la

libopenss_cli_la_CXXFLAGS =  \
	-c -LANG:std -fpic \
	-I. \
        $(PYTHON_CPPFLAGS) \
	-I$(top_srcdir)/libopenss-framework \
    -DPLUGIN_PATH=\"$(pkglibdir)\" \
	-I/usr/include -I. -I../objects -I../base

libopenss_cli_la_LDFLAGS = \
    	-fpic \
	--export-dynamic \
	-module \
	-avoid-version \
	-L$(top_srcdir)/libopenss-framework \
        $(PYTHON_LDFLAGS) \
	$(LIBLTDL) \
	-lpthread -lutil -LANG:std

libopenss_cli_la_LIBADD = $(LIBLTDL) -lopenss-framework

LEXWOFF =
YFLAGS = -d
LFILES = SS_Parse_Lex.l
YFILES = SS_Parse_Yacc.y

pcli_SOURCES =

#test_SOURCES = \
#	$(LFILES) $(YFILES)
	
EXTRA_DIST = $(LFILES) $(YFILES) myparse.py

pcli_includes = ArgClass.hxx Clip.hxx Commander.hxx CommandObject.hxx Experiment.hxx SS_Cmd_Execution.hxx SS_Input_Manager.hxx SS_Parse_Param.hxx SS_Parse_Range.hxx SS_Parse_Result.hxx SS_Parse_Target.hxx SS_View.hxx

libopenss_cli_la_SOURCES = \
    	help.cxx \
   	Start.cxx \
	Start_Modes.cxx \
	Commander.cxx \
	CommandObject.cxx \
	SS_Cmd_Execution.cxx \
	SS_View.cxx \
	SS_View_exp.cxx \
	SS_View_pcfunc.cxx \
	SS_View_params.cxx \
	SS_Parse_Param.cxx \
	SS_Parse_Range.cxx \
	SS_Parse_Result.cxx \
	SS_Parse_Target.cxx \
	Experiment.cxx \
	PY_Input.cxx \
	$(pcli_includes)

nodist_libopenss_cli_la_SOURCES = \
	SS_Parse_Yacc.cxx \
	SS_Parse_Lex.cxx 

SS_Parse_Yacc.cxx: $(YFILES) SS_Parse_Result.hxx
	-rm -f SS_Parse_Yacc.cxx
	-$(YACC) $(YFLAGS) $(YFILES);
	-mv y.tab.c SS_Parse_Yacc.cxx

y.tab.h: $(YFILES)
	-rm -f SS_Parse_Yacc.cxx
	-$(YACC) $(YFLAGS) $(YFILES);
	-mv y.tab.c SS_Parse_Yacc.cxx

SS_Parse_Lex.cxx: $(LFILES) y.tab.h
	-rm -f SS_Parse_Lex.cxx
	-$(LEX) $(LFILES);
	-mv lex.yy.c SS_Parse_Lex.cxx

clobber:
	-rm \
	*.lo \
	*.la \
	y.tab.h \
	SS_Parse_Lex.cxx \
	SS_Parse_Yacc.cxx

bin_PROGRAMS = pcli

pcli:
	-rm -f ${prefix}/lib/openspeedshop/myparse.py;
	-cp myparse.py ${prefix}/lib/openspeedshop;

doc:
	-doxygen Doxyfile

CLEANFILES = \
    	lex.yy.cxx \
	y.tab.cxx \
	y.tab.h

