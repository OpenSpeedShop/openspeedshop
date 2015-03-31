################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2007-2014 Krell Institute  All Rights Reserved.
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

DIST_SUBDIRS = rapidxml-1.13

BUILT_SOURCES = \
	SS_Parse_Lex.cxx \
	SS_Parse_Yacc.cxx \
	SS_Parse_Yacc.h \
	SS_Parse_Yacc.hxx \
	cbtf-offline.py \
	offline.py \
	mrnet.py

AM_YFLAGS = -d
#YFLAGS = -d

programfilesdir = $(pkglibdir)
dist_programfiles_SCRIPTS = \
	init.py cbtf-offline.py offline.py mrnet.py

lib_LTLIBRARIES = libopenss-cli.la


libopenss_cli_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-guibase \
	-I$(top_srcdir)/libopenss-message \
	-I$(top_srcdir)/libopenss-framework \
	-I$(top_srcdir)/libopenss-queries \
	-I$(top_srcdir)/libopenss-guiobjects \
	-DLIBRARY_DIR=\"$(libdir)\" \
	-DPLUGIN_DIR=\"$(pkglibdir)\" \
	$(LTDLINCL) \
	@PYTHON_CPPFLAGS@

libopenss_cli_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-message \
	-L$(top_srcdir)/libopenss-framework \
	-L$(top_srcdir)/libopenss-queries \
	@PYTHON_LDFLAGS@ \
	@OPENMP_LDFLAGS@ \
	-export-dynamic -version-info 0:0:0

libopenss_cli_la_LIBADD = \
	-lopenss-message \
	-lopenss-framework \
	-lopenss-queries \
	$(LIBLTDL) \
	@PYTHON_LIBS@ \
	@PYTHON_EXTRA_LIBS@ \
	@OPENMP_LIBS@ \
	-lpthread -lutil

if BUILD_PTGF_DIRECT

library_includedir = $(includedir)/openspeedshop
library_include_HEADERS = Direct.hxx

lib_LTLIBRARIES += libopenss-cli-direct.la

libopenss_cli_direct_la_CXXFLAGS = \
	@PYTHON_CPPFLAGS@ \
	-I$(top_srcdir)/libopenss-message \
	-I$(top_srcdir)/libopenss-framework \
	-I$(top_srcdir)/libopenss-queries \
        -I$(top_srcdir)/libopenss-runtime \
        $(LTDLINCL) \
        -fpermissive -fexceptions -fPIC \
        @SYMTABAPI_CPPFLAGS@

libopenss_cli_direct_la_LDFLAGS = \
	-L$(top_srcdir)/libopenss-framework \
	-L$(top_srcdir)/libopenss-cli \
        -export-dynamic -version-info 1:0:0

libopenss_cli_direct_la_LIBADD = \
        $(LIBLTDL) \
	-lopenss-framework \
	-lopenss-cli \
        -lpthread -lrt

libopenss_cli_direct_la_SOURCES = \
	ServerException.hxx Direct.hxx Direct.cxx \
	OpenSpeedShopCLI.hxx OpenSpeedShopCLI.cxx 

endif

if BUILD_PTGF_SERVER

lib_LTLIBRARIES += libopenss-cli-server.la

libopenss_cli_server_la_CXXFLAGS = \
	@PYTHON_CPPFLAGS@ \
	-I$(top_srcdir)/libopenss-message \
	-I$(top_srcdir)/libopenss-framework \
	-I$(top_srcdir)/libopenss-queries \
        -I$(top_srcdir)/libopenss-runtime \
        $(LTDLINCL) \
        -fpermissive -fexceptions -fPIC \
        @SYMTABAPI_CPPFLAGS@

libopenss_cli_server_la_LDFLAGS = \
        -export-dynamic -version-info 1:0:0

libopenss_cli_server_la_LIBADD = \
        $(LIBLTDL) \
        -lpthread -lrt

libopenss_cli_server_la_SOURCES = \
	Socket.hxx Socket.cxx \
	SocketServer.hxx SocketServer.cxx \
	FileSystem.hxx FileSystem.cxx \
	OpenSpeedShopCLI.hxx OpenSpeedShopCLI.cxx 

endif

CLI_TIMING_SOURCES = \
	SS_Timings.hxx SS_Timings.cxx 

CLI_GENERAL_SOURCES = \
	ArgClass.hxx \
	Clip.hxx \
	Commander.hxx Commander.cxx \
	CommandObject.hxx CommandObject.cxx \
	Load_Messages.cxx help_cmd_msg.dxx \
	PY_Input.cxx help_general_msg.dxx \
	SS_Cmd_Control.hxx SS_Cmd_Control.cxx \
	SS_Cmd_Execution.hxx SS_Cmd_Execution.cxx \
	SS_savedViewInfo.hxx \
	SS_CommandResult.hxx SS_CommandResult.cxx \
	SS_Compare.cxx \
	SS_Configure.hxx SS_Configure.cxx \
	SS_Exp.hxx SS_Exp.cxx \
	SS_Input_Manager.hxx help_grammar_msg.dxx \
	SS_Output.hxx \
	SS_Parse_Lex.lxx help_topic_msg.dxx \
	SS_Parse_Param.hxx SS_Parse_Param.cxx \
	SS_Parse_Interval.hxx SS_Parse_Interval.cxx \
	SS_Parse_Range.hxx SS_Parse_Range.cxx \
	SS_Parse_Result.hxx SS_Parse_Result.cxx \
	SS_Parse_Target.hxx SS_Parse_Target.cxx \
	SS_Parse_Yacc.yxx \
	SS_ThreadInfo.hxx SS_ThreadInfo.cxx \
	SS_View.hxx SS_View.cxx \
	SS_View_init.cxx \
	SS_View_detail.hxx SS_View_detail.txx \
	SS_View_getmetrics.txx \
	SS_View_multi.cxx \
	SS_View_output.cxx \
	SS_View_stats.cxx \
	SS_View_util.cxx \
	SS_Watcher.hxx SS_Watcher.cxx \
	SS_Start.cxx \
	SS_Settings.hxx SS_Settings.cxx \
		SS_Parse_Expr.hxx help_cmd_msg.dxx  help_general_msg.dxx  \
		help_scripting_msg.dxx help_grammar_msg.dxx  help_topic_msg.dxx \
	        SS_View_bythread_help.hxx SS_View_bythread_recognize.hxx SS_View_detail.hxx  SS_View.hxx \
	        SS_View_bythread_locations.hxx SS_View_bythread_setmetrics.hxx SS_View_Expr.hxx \
                SS_View_metric_expressions.hxx \
	Start_Modes.cxx

libopenss_cli_la_SOURCES = \
	$(CLI_GENERAL_SOURCES)

if BUILD_CLI_TIMING
libopenss_cli_la_SOURCES += \
	$(CLI_TIMING_SOURCES)
endif

LFILES = SS_Parse_Lex.lxx
YFILES = SS_Parse_Yacc.yxx

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

SS_Parse_Yacc.h: $(YFILES) SS_Parse_Yacc.hxx
	cp SS_Parse_Yacc.hxx SS_Parse_Yacc.h

SS_Parse_Lex.cxx: $(LFILES) SS_Parse_Yacc.hxx
	-rm -f SS_Parse_Lex.cxx
	-$(LEX) $(LFILES);
	-mv lex.yy.c SS_Parse_Lex.cxx

EXTRA_DIST =


CLEANFILES = $(BUILT_SOURCES)

doc:
	$(DOXYGEN)

if HAVE_RUNTIME_DIR
ossrun_location=$(RUNTIME_DIR)
else
ossrun_location=$(prefix)
endif

if BUILD_TARGETED

monitor_libdir=$(TARGET_LIBMONITOR_LIBDIR)
monitor_dir=$(TARGET_LIBMONITOR_DIR)
mpich2_dir=$(TARGET_MPICH2_DIR)

else

mpich2_dir=$(MPICH2_DIR)
monitor_dir=$(LIBMONITOR_DIR)
monitor_libdir=$(LIBMONITOR_LIBDIR)

# endif for else of BUILD_TARGETED
endif

cbtf-offline.py: cbtf-offline.py.in
	@rm -f cbtf-offline.py
	@sed \
		-e 's,@cbtfinstalldir@,$(CBTF_INSTALLDIR),' \
		-e 's,@cbtflibdir@,$(CBTF_LIBDIR),' \
		-e 's,@cbtfruncmd@,$(CBTF_TOOLS_CBTFRUN),' \
		-e 's,@cbtfcollectors@,$(CBTF_TOOLS_COLLECTORS),' \
		-e 's,@cbtfcomponents@,$(CBTF_TOOLS_COMPONENTS),' \
		-e 's,@cbtfxmldir@,$(CBTF_TOOLS_XML),' \
		-e 's,@osslibdir@,$(pkglibdir),' \
		-e 's,@ossdefaultmpi@,$(DEFAULT_MPI_IMPL),' \
		-e 's,@libmonitordir@,$(LIBMONITOR_DIR),' \
		$(srcdir)/cbtf-offline.py.in > cbtf-offline.py
	@chmod 755 cbtf-offline.py

offline.py: offline.py.in
	@rm -f offline.py
	@sed \
		-e 's,@target_os@,$(TARGET_OS),' \
		-e 's,@oss_installation_dir@,$(ossrun_location),' \
		-e 's,@machine_dependent_libdir@,$(abi_libdir_name),' \
		-e 's,@osslibdir@,$(pkglibdir),' \
		-e 's,@ossdefaultmpi@,$(DEFAULT_MPI_IMPL),' \
		-e 's,@libmonitordir@,$(monitor_dir),' \
		-e 's,@libmonitorlibdir@,$(monitor_libdir),' \
                -e 's,@alt_machine_dependent_libdir@,$(alt_abi_libdir_name),' \
                -e 's,@mpichdir@,$(MPICH_DIR),' \
                -e 's,@mpich2dir@,$(mpich2_dir),' \
                -e 's,@mvapichdir@,$(MVAPICH_DIR),' \
                -e 's,@mvapich2dir@,$(MVAPICH2_DIR),' \
                -e 's,@mptdir@,$(MPT_DIR),' \
                -e 's,@openmpidir@,$(OPENMPI_DIR),' \
		$(srcdir)/offline.py.in > offline.py
	@chmod 755 offline.py

mrnet.py: mrnet.py.in
	@rm -f mrnet.py
	@cat \
		$(srcdir)/mrnet.py.in > mrnet.py
	@chmod 755 mrnet.py

if BUILD_INSTRUMENTOR
# PYopenss.so
install-exec-hook: libopenss-cli.la
	@rm -f $(pkglibdir)/PYopenss.so
	$(LN_S) $(DESTDIR)$(libdir)/libopenss-cli.so $(pkglibdir)/PYopenss.so
	chmod 755 $(pkglibdir)/PYopenss.so

uninstall-hook:
	@rm -f $(pkglibdir)/PYopenss.so 
endif

EXTRA_DIST += offline.py.in mrnet.py.in cbtf-offline.py.in

