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

BUILT_SOURCES = SS_Preparse.py
AM_YFLAGS = -d
YFLAGS = -d

dist_pkglib_SCRIPTS = SS_Preparse.py

lib_LTLIBRARIES = libopenss-cli.la

libopenss_cli_la_CXXFLAGS = \
	-I$(top_srcdir)/base \
	-I$(top_srcdir)/libopenss-framework \
	-I$(top_srcdir)/objects \
	$(LTDLINCL) \
	$(PYTHON_CPPFLAGS)

libopenss_cli_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-framework \
	$(PYTHON_LDFLAGS) \
	-export-dynamic -version-info 0:0:0

libopenss_cli_la_LIBADD = \
	-lopenss-framework \
	$(LIBLTDL) \
	$(PYTHON_LIBS) \
	-lpthread -lutil

libopenss_cli_la_SOURCES = \
	ArgClass.hxx \
	Clip.hxx \
	Commander.hxx Commander.cxx \
	CommandObject.hxx CommandObject.cxx \
	Experiment.hxx Experiment.cxx \
	help.cxx \
	PY_Input.cxx \
	SS_Cmd_Execution.hxx SS_Cmd_Execution.cxx \
	SS_Input_Manager.hxx \
	SS_Parse_Lex.lxx \
	SS_Parse_Param.hxx SS_Parse_Param.cxx \
	SS_Parse_Range.hxx SS_Parse_Range.cxx \
	SS_Parse_Result.hxx SS_Parse_Result.cxx \
	SS_Parse_Target.hxx SS_Parse_Target.cxx \
	SS_Parse_Yacc.yxx \
	SS_View.hxx SS_View.cxx \
	SS_View_util.cxx \
	SS_View_stats.cxx \
	SS_View_exp.cxx \
	SS_View_params.cxx \
	SS_View_pcfunc.cxx \
	Start.cxx \
	Start_Modes.cxx

LFILES = SS_Parse_Lex.lxx
YFILES = SS_Parse_Yacc.yxx

doc:
	-doxygen Doxyfile
SS_Parse_Yacc.cxx: $(YFILES) SS_Parse_Result.hxx
	-rm -f SS_Parse_Yacc.cxx
	-$(YACC) $(AM_YFLAGS) $(YFILES);
	-mv y.tab.c SS_Parse_Yacc.cxx
	-cp y.tab.h SS_Parse_Yacc.h
	-mv y.tab.h SS_Parse_Yacc.hxx

SS_Parse_Yacc.hxx: $(YFILES)
	-rm -f SS_Parse_Yacc.cxx
	-$(YACC) $(AM_YFLAGS) $(YFILES);
	-mv y.tab.c SS_Parse_Yacc.cxx
	-mv y.tab.h SS_Parse_Yacc.hxx

SS_Parse_Lex.cxx: $(LFILES) SS_Parse_Yacc.hxx
	-rm -f SS_Parse_Lex.cxx
	-$(LEX) $(LFILES);
	-mv lex.yy.c SS_Parse_Lex.cxx

