#!/usr/bin/python

##***************************************************************************##
##              Open|SpeedShop Plugin Structure Creation Script              ##
##                                   V 0.5                                   ##
##***************************************************************************##
#*****************************************************************************#
#               Copyright (2007) Los Alamos National Laboratory               #
#                Author: Samuel K. Gutierrez <samuel@lanl.gov>                #
#*****************************************************************************#

#*****************************************************************************#
#      Creates an Open|SpeedShop plugin skeleton with all the necessary       #
#        files/directories to begin Open|SpeedShop plugin development         #
#*****************************************************************************#

#*****************************************************************************#
#                                    Usage                                    #
#*****************************************************************************#
# usage: createPluginStructure.py [-n|--name=]NAME [-o|--output=]DIR          # 
# [-t|--topsrcdir=]DIR [--force]                                              #
#                                                                             #
# options:                                                                    #
#    -h, --help              show this help message and exit                  #
#    -nNAME, --name=NAME     specifies plugin name [Required]                 #
#    -oDIR, --output=DIR     specifies output destination [Required]          #
#    -tDIR, --topsrcdir=DIR                                                   #
#                            location of OpenSpeedShop's top source directory #
#                            [Required]                                       #
#    --force                 force overwrite [Optional]                       #
#*****************************************************************************#

#Note: The OSS Top Source Directory is used for autoconfiguration purposes.
#If not set correctly, modification to the automake and autoconfig 
#files will be needed.  The addition of Open|SpeedShop's acinclude.m4 and 
#ac_pkg_mpi.m4 to your plugin source root directory will also be needed.

import os
import sys
import string
import datetime
from optparse import OptionParser

oss_top_src_dir = None
plugin_root = None
raw_plugin_name = None
plugin_name = None
pi_root = None

## get_valid_user_input(output_string)
# @param output_string: Prompt String
# @return: String ('y' or 'n') 
def get_valid_user_input(output_string):
	user_input = raw_input(output_string)
	while user_input != 'y' and user_input != 'n':
			user_input = raw_input(output_string)
	if user_input == 'y':
		return 'y'
	else:
		return 'n'

## make_dirs(force)
# @param force: Boolean
def make_dirs(force) :
	if get_valid_user_input('create: ' + plugin_root + plugin_name + 
						    ' ?\n' '[y or n] : ') == 'y' :
		if os.path.exists(plugin_root + plugin_name) and force:
			try:
				if get_valid_user_input(plugin_root + plugin_name + 
							         ' already exists. \noverwrite: \n' +
									   plugin_root + plugin_name + 
									   ' ?\n[y or n] : ') == 'y':
					os.system('rm -rf ' + plugin_root + plugin_name)
				else:
					print 'no changes made to: ' + plugin_root + plugin_name
					sys.exit()				
			except:
				exc_info = sys.exc_info()
				print exc_info[1]
				sys.exit()
		print 'creating directories...'		
		try :
			os.system('mkdir -p ' + pi_root)
			os.mkdir(pi_root + '/plugins')
			os.mkdir(pi_root + '/plugins/collector')
			os.mkdir(pi_root + '/plugins/collector/' + 
					 plugin_name)
			os.mkdir(pi_root + '/plugins/view')
			os.mkdir(pi_root + '/plugins/view/' + plugin_name)
			os.mkdir(pi_root + '/plugins/panel')
			os.mkdir(pi_root + '/plugins/panel/' + 
					 raw_plugin_name + 'Panel')
			os.mkdir(pi_root + '/plugins/wizard')
			os.mkdir(pi_root + '/plugins/wizard/' + 
					 raw_plugin_name + 'WizardPanel')
			print 'creating directories...done'
		except :
			exc_info = sys.exc_info()
			print exc_info[1]
			print 'creating directories...error'
			sys.exit()
	else : 
		print 'no files written to: ' + plugin_root + plugin_name
		sys.exit()

## add_make_files()
def add_make_files() :
	print 'creating make files...'
	try :
		file = open(pi_root + '/Makefile.am', 'w')
		file.write(
					'SUBDIRS = \\\n' +
					'\tplugins\n\n' +
					'DIST_SUBDIRS = \\\n' +
					'\t$(SUBDIRS)\n\n' +
					'CLI_SUBDIRS = \\\n' +
					'\tplugins/collector \\\n' +
					'\tplugins/view \\\n' +
					'\tplugins/panel \\\n' +
					'\tplugins/wizard\n\n' +
					'cli:\n' +
					'\tfor i in $(CLI_SUBDIRS); do ' + 
					'(cd $$i; $(MAKE)); done\n\n' +
					'cli-install:\n' +
					'\tfor i in $(CLI_SUBDIRS); do ' + 
					'(cd $$i; $(MAKE) install); done'
					)
		file.close()

		file = open(pi_root + '/plugins/Makefile.am', 'w')
		file.write(
					'SUBDIRS = collector \\\n' + 
					'\t\t  view \\\n' +
					'\t\t  panel \\\n' +
					'\t\t  wizard \n'
					)
		file.close()

		file = open(pi_root + '/plugins/collector/Makefile.am', 'w')
		file.write('SUBDIRS = ' + plugin_name)
		file.close()
		
		file = open(pi_root + '/plugins/collector/' + plugin_name + 
				    '/Makefile.am', 'w')
		file.write(
					'# Note: See note in ' + 
					'\"libopenss-runtime/Makefile.am\" for why \"-O0\" ' + 
					'is used here.\n\n' +
					'CFLAGS = -g -O0\n' +
					'oss_top_srcdir = ' + oss_top_src_dir +'\n\n' +
					'BUILT_SOURCES = \\\n' +
					'\tblobs.h blobs.c\n\n' +
					'pkglib_LTLIBRARIES = ' + plugin_name + '.la ' + 
					plugin_name + '-rt.la\n' +
					plugin_name + '_la_CXXFLAGS = \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-framework\n\n' +
					plugin_name + '_la_LDFLAGS = \\\n' +
					'\t-no-undefined -module -avoid-version\n\n' +
					plugin_name + '_la_SOURCES = \\\n' +
					'\tblobs.x $(BUILT_SOURCES) \\\n' +
					'\t' + raw_plugin_name + 'Collector.hxx ' +
					raw_plugin_name + 'Collector.cxx\n\n' +
					plugin_name + '_rt_la_CFLAGS = \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-runtime \\\n' +
					'\t@LIBUNWIND_CPPFLAGS@\n\n' +
					plugin_name + '_rt_la_LDFLAGS = \\\n' +
					'\t-L$(oss_top_srcdir)/libopenss-runtime \\\n' +
					'\t@LIBUNWIND_LDFLAGS@ \\\n' +
					'\t-no-undefined -module -avoid-version\n\n' +
					plugin_name + '_rt_la_LIBADD = \\\n' +
					'\t-lopenss-runtime \\\n' +
					'\t@LIBUNWIND_LIBS@ \\\n' +
					'\t-lpthread -lrt\n\n' +
					plugin_name + '_rt_la_SOURCES = \\\n' +
					'\tblobs.x $(BUILT_SOURCES) \\\n' +
					'\twrappers.c \\\n' +
					'\truntime.h runtime.c\n\n' +
					'SUFFIXES = .x\n' +
					'CLEANFILES = $(BUILT_SOURCES)\n\n' +
					'blobs.h blobs.c : blobs.x\n' +
					'\trm -f  $(patsubst %.x, %.h, $<) ' + 
					'$(patsubst %.x, %.c, $<)\n' +
					'\t$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<\n' +
					'\t$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<\n'
					)
		file.close()

		file = open(pi_root + '/plugins/view/Makefile.am', 'w')
		file.write('SUBDIRS = ' + plugin_name)
		file.close()
				
		file = open(pi_root + '/plugins/view/' + plugin_name + 
				    '/Makefile.am', 'w')
		file.write(
					'pkglib_LTLIBRARIES = ' + plugin_name + '_view.la\n' +
					'oss_top_srcdir = ' + oss_top_src_dir + '\n' +
					'top_collector_srcdir = ../..\n\n' +
					'BUILT_SOURCES =\n\n' +
					plugin_name + '_view_la_CXXFLAGS = \\\n' +
					'\t@PYTHON_CPPFLAGS@ \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-queries \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-framework \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-cli \\\n' +
					'\t-I$(top_collector_srcdir)/collector/' + plugin_name + 
					' \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-message\n\n' +
					plugin_name + '_view_la_LDFLAGS = \\\n' +
					'\t-no-undefined -module -avoid-version\n\n' +
					plugin_name + '_view_la_SOURCES = \\\n' +
					'\t' + plugin_name + '_view.cxx\n\n' +
					'SUFFIXES = .x .cxx\n' +
					'CLEANFILES = $(BUILT_SOURCES)'
					)		
		file.close()
	
		file = open(pi_root + '/plugins/panel/Makefile.am', 'w')
		file.write('SUBDIRS = ' + raw_plugin_name + 'Panel')
		file.close()
		
		file = open(pi_root + '/plugins/panel/' + raw_plugin_name + 
				    'Panel/Makefile.am', 'w')
		file.write(
					'pkglib_LTLIBRARIES = ' + raw_plugin_name + 'Panel.la\n' +
					'oss_top_srcdir = ' + oss_top_src_dir + '\n\n' +
					raw_plugin_name + 'Panel_la_CXXFLAGS = \\\n' +
					'\t-c -LANG:std -fpic \\\n' +
					'\t-I/usr/include \\\n' +
					'\t$(QTLIB_CPPFLAGS) \\\n' +
					'\t@PYTHON_CPPFLAGS@ \\\n' +
					'\t-I. \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-framework \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-queries \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-cli \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-message \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-gui \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-guiimages \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-guiplugin \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-guiobjects \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-guidialogs \\\n' +
					'\t-I$(oss_top_srcdir)/plugins/panels/' + 
					'ManageProcessesPanel \\\n' +
					'\t-I$(oss_top_srcdir)/plugins/panels/SourcePanel \\\n' +
					'\t-I$(oss_top_srcdir)/plugins/panels/' + 
					'CustomExperimentPanel \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-guibase\n\n' +
					raw_plugin_name + 'Panel_la_LDFLAGS = \\\n' +
					'\t-fpic \\\n' +
					'\t--export-dynamic \\\n' +
					'\t--export-all-symbols \\\n' +
					'\t-module \\\n' +
					'\t-avoid-version \\\n' +
					'\t-L$(oss_top_srcdir)/libopenss-framework \\\n' +
					'\t-L$(oss_top_srcdir)/libopenss-guidialogs \\\n' +
					'\t-L$(oss_top_srcdir)/plugins/panels/' + 
					'CustomExperimentPanel \\\n' +
					'\t$(QTLIB_LIBS) $(QTLIB_LDFLAGS) \\\n' +
					'\t-ldl\n\n' +                            
					raw_plugin_name + 'Panel_la_LIBADD = ' + 
					'$(LIBLTDL) -lopenss-framework ' +
					'-lopenss-guidialogs -lopenss-CustomExperimentPanel\n\n' +
					raw_plugin_name + 'Panel_la_SOURCES = \\\n' +
					'\t' + raw_plugin_name + 'Panel.hxx ' + raw_plugin_name + 
					'Panel.cxx \\\n' +
					'\tlocal_plugin_info.hxx'
					)
		file.close()
				
		file = open(pi_root + '/plugins/wizard/Makefile.am', 'w')
		file.write('SUBDIRS = ' + raw_plugin_name + 'WizardPanel')
		file.close()
				
		file = open(pi_root + '/plugins/wizard/' + raw_plugin_name + 
				    'WizardPanel/Makefile.am', 'w')
		file.write(
					'pkglib_LTLIBRARIES = ' + raw_plugin_name + 
					'WizardPanel.la\n' +
					'oss_top_srcdir = ' + oss_top_src_dir + '\n\n' +
					raw_plugin_name + 'WizardPanel_la_CXXFLAGS = \\\n' +
					'\t-I/usr/include \\\n' +
					'\t$(QTLIB_CPPFLAGS) \\\n' +
					'\t@PYTHON_CPPFLAGS@ \\\n' +
					'\t-I. \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-gui \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-guiimages \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-guiobjects \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-framework \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-queries \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-cli \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-message \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-guidialogs \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-guiplugin \\\n' +
					'\t-I$(oss_top_srcdir)/libopenss-guibase\n\n' +
					raw_plugin_name + 'WizardPanel_la_LDFLAGS = ' + 
					'--export-dynamic \\\n' +
					'\t--export-all-symbols \\\n' +
					'\t-module -avoid-version \\\n' +
					'\t-L$(oss_top_srcdir)/libopenss-framework\n\n' +
					raw_plugin_name + 'WizardPanel_la_LIBADD = ' +
					'$(QTLIB_LIBS) $(QTLIB_LDFLAGS) -lopenss-framework\n\n' +
					'moc_' + raw_plugin_name + 'WizardPanel.cxx: ' + 
					raw_plugin_name + 'WizardPanel.hxx\n' +
					'\t-moc -o moc_' + raw_plugin_name + 'WizardPanel.cxx ' + 
					raw_plugin_name + 'WizardPanel.hxx\n\n' +                         
					raw_plugin_name + 'WizardPanel_la_SOURCES = \\\n' +
					'\t' + raw_plugin_name + 'WizardPanel.hxx ' + 
					raw_plugin_name + 'WizardPanel.cxx \\\n' +
					'\tmoc_' + raw_plugin_name + 'WizardPanel.cxx ' +
					raw_plugin_name + 'Description.hxx \\\n' +
					'\tlocal_plugin_info.hxx\n\n' +
					'CLEANFILES=moc_' + raw_plugin_name + 'WizardPanel.cxx'                       
					)
		file.close()
		print 'creating make files...done'
	except:
		exc_info = sys.exc_info()
		print exc_info[1]
		print 'creating make files...error'
		sys.exit()

## add_ac_files()		
def add_ac_files():
	if (os.path.isfile(oss_top_src_dir + os.sep + 'acinclude.m4') and 
	os.path.isfile(oss_top_src_dir + os.sep + 'ac_pkg_mpi.m4')) :
		try:
			os.system('cp ' + oss_top_src_dir + os.sep + 'acinclude.m4 ' + 
					  pi_root)
			os.system('cp ' + oss_top_src_dir + os.sep + 'ac_pkg_mpi.m4 ' + 
					  pi_root)
		except:
			exc_info = sys.exc_info()
			print ('It seems as if the provided Open|SpeedShop ' + 
				   'top source directory path was invalid or inaccessible. ' + 
				   'Make certain the path is correct ' + 
				   'and that you have sufficient privileges.')
			sys.exit()
	else:
		print ('It seems as if the provided Open|SpeedShop ' + 
				   'top source directory path was invalid. ' +  
				   'Make certain the path is correct ' + 
				   'and that you have sufficient privileges.')
		sys.exit()
	print 'creating autoconfiguration files...'
	try:	
		file = open(pi_root + '/configure.ac', 'w')
		file.write(
				   
					'AC_INIT(' + raw_plugin_name + ', 0.1)\n' +
					'AC_COPYRIGHT([[Copyright (c) ' + 
					str(datetime.date.today().year) +
					' All Rights Reserved.]])\n' + 
					'AM_INIT_AUTOMAKE\n\n' +
					'#' * 80 + '\n' +
					'# Setup Configuration Header File\n' + 
					'#' * 80 + '\n\n' +
					'AM_CONFIG_HEADER([config.h])\n\n' +
					'AH_TOP([\n' + 
					'/' + '*' * 79 + '\n' +  
					'** Copyright (c) ' + str(datetime.date.today().year) + 
					'. All Rights Reserved.\n' + 
					'**\n' + 
					'** This library is free software; you can ' + 
					'redistribute it and/or modify it under\n' + 
					'** the terms of the GNU Lesser General ' + 
					'Public License as published by the Free\n' + 
					'** Software Foundation; either version ' + 
					'2.1 of the License, or (at your option)\n' + 
					'** any later version.\n' + 
					'**\n' + 
					'** This library is distributed in the hope that it ' + 
					'will be useful, but WITHOUT\n' + 
					'** ANY WARRANTY; without even the implied ' + 
					'warranty of MERCHANTABILITY or FITNESS\n' +
					'** FOR A PARTICULAR PURPOSE.  See the GNU Lesser ' + 
					'General Public License for more\n' + 
					'** details.\n' +
					'**\n' +
					'** You should have received a copy of the ' + 
					'GNU Lesser General Public License\n' + 
					'** along with this library; if not, write ' + 
					'to the Free Software Foundation, Inc.,\n' + 
					'** 59 Temple Place, Suite 330, ' + 
					'Boston, MA  02111-1307  USA\n' + 
					'*' * 79 + '/\n' +
					'])\n\n' + 
					'#' * 80 + '\n' +  
					'# Check for Standard Build Tools\n' + 
					'#' * 80 + '\n\n' + 
					'AC_GNU_SOURCE\n' +
					'AC_PROG_CC\n' +
					'AC_PROG_CXX\n\n' +
					'AC_DISABLE_STATIC\n' +
					'AC_LIBLTDL_INSTALLABLE\n' +
					'AC_SUBST(LTDLINCL)\n' +
					'AC_SUBST(LIBLTDL)\n' +
					'AC_LIBTOOL_DLOPEN\n' +
					'AC_PROG_LIBTOOL\n' +
					'AC_CONFIG_SUBDIRS(libltdl)\n\n' +
					'AC_PATH_PROG([DOXYGEN], doxygen)\n' +
					'AC_PROG_INSTALL\n' +
					'AC_PROG_LEX\n' +
					'AC_PATH_PROG([RPCGEN], rpcgen)\n' +
					'AC_PROG_YACC\n\n' + 
					'#' * 80 + '\n' +
					'# Handle X86-64 Special Case\n' + 
					'#' * 80 + '\n\n' +					
					'case "$host" in\n' +
					'\tx86_64-*-linux*)\n' +
        			'''\t\tif test x"$libdir" == ''' + 
        			'''x'${exec_prefix}/lib'; then\n''' + 
            		'''\t\t\tlibdir='${exec_prefix}/lib64'\n''' + 
              		'\t\tfi\n' +
                	'\t\tabi_libdir="lib64"\n' +
                 	'\t\talt_abi_libdir="lib"\n' +
                 	'\t\tLDFLAGS="-L/usr/lib64 $LDFLAGS"\n' +
					'\t\t;;\n' +
					'\t*)\n' +
        			'\t\tabi_libdir="lib"\n' +
           			'\t\talt_abi_libdir="lib64"\n' +
              		'\t\tLDFLAGS="-L/usr/lib -DLIB_DIR=lib $LDFLAGS"\n' +
                	'\t\t;;\n' +
					'esac\n' +
					'topdir_execprefix=${exec_prefix}\n' +
					'AC_SUBST(topdir_execprefix)\n\n' +					
					'abi_libdir_name=${abi_libdir}\n' +
					'AC_SUBST(abi_libdir_name)\n\n' + 
					'#' * 80 + '\n' +	
					'# Check for POSIX Threads (ISO/IEC 9945-1:1996)\n' + 
					'#' * 80 + '\n\n' +
					'AC_CHECK_HEADER(pthread.h, ' + 
					'[ AC_CHECK_LIB(pthread, pthread_create, [\n' + 
    				'\tAC_DEFINE(HAVE_POSIX_THREADS, 1, ' + 
    				'[Define to 1 if you have POSIX threads.])\n' + 
    				'], [\n' + 
    				'\tAC_MSG_FAILURE([cannot locate POSIX ' + 
    				'thread library and/or headers.])\n' + 
    				']) ])\n\n' + 
    				'#' * 80 + '\n' +
					'# Check for Additional Packages\n' + 
					'#' * 80 + '\n\n' +
					'AC_PKG_ARRAYSVCS()\n' +
					'AC_PKG_BINUTILS()\n' +
					'AC_PKG_MPI()\n' +
					'AC_PKG_LIBUNWIND()\n' +
					'AC_PKG_OPENMP()\n' +
					'AC_PKG_OTF()\n' +
					'AC_PKG_VT()\n' +
					'AC_PKG_PAPI()\n' +
					'AC_PKG_SQLITE()\n\n' +					
					'''AC_PYTHON_DEVEL([>= '2.3'])\n''' +
					'AC_PKG_QTLIB()\n\n' + 
					'#' * 80 + '\n' +
					'# Check for Dynamic Instrumentation Package\n' + 
					'#' * 80 + '\n\n' +
					'AC_PKG_DYNINST()\n\n' +
					'#' * 80 + '\n' +
					'# Check for Instrumentor\n' + 
					'#' * 80 + '\n\n' +
					'AC_ARG_WITH(instrumentor,\n' + 
            		'\tAC_HELP_STRING([--with-instrumentor=TYPE],\n' + 
                    '\t\t[instrumentor (dpcl, dyninst) @<:@dpcl@:>@]),\n' + 
                    '\tINSTRUMENTOR=$withval, INSTRUMENTOR="dpcl")\n\n' + 
                    'case "$INSTRUMENTOR" in\n' + 
                    '\tdpcl)\n' + 
                    '\t\tAC_PKG_DPCL()\n' + 
                    '\t\t;;\n\n' +
                    '\tdyninst)\n' + 
                    '\t\tAC_PKG_DYNINST()\n' + 
                    '\t\t;;\n' + 
                    '\t*)\n' + 
                    '\t\tAC_MSG_ERROR([invalid ' + 
                    'instrumentor "$INSTRUMENTOR".])\n' + 
                    '\t\t;;\n' + 
                    'esac\n\n' + 
                    'AC_SUBST(INSTRUMENTOR)\n\n' + 
                    '#' * 80 + '\n' +
                    '# Generate Output Files\n' +
                     '#' * 80 + '\n\n' + 
                     'AC_CONFIG_FILES(\n' + 
                     '\tMakefile\n' +
					'\tplugins/Makefile\n' +
					'\tplugins/collector/Makefile\n' +
					'\tplugins/collector/' + plugin_name + '/Makefile\n' +
					'\tplugins/view/Makefile\n' +
					'\tplugins/view/' + plugin_name + '/Makefile\n' +
					'\tplugins/panel/Makefile\n' +
					'\tplugins/panel/' + raw_plugin_name + 'Panel/Makefile\n' +
					'\tplugins/wizard/Makefile\n' +
					'\tplugins/wizard/' + raw_plugin_name + 
					'WizardPanel/Makefile\n' +
					')\n\n' +
					'AC_OUTPUT'
					)                                
		file.close()
		print 'creating autoconfiguration files...done'

	except :
		exc_info = sys.exc_info()
		print exc_info[1]
		print 'creating autoconfiguration files...error'
		sys.exit()

## add_collector_files()
def add_collector_files() :
	collector_root = pi_root + '/plugins/collector/' + plugin_name + '/'
	coll_class_name = raw_plugin_name + 'Collector'
	print 'creating collector plugin files...'
	try :
		file = open(collector_root + raw_plugin_name + 'Collector.hxx', 'w')
		file.write(
					'#ifndef _' + coll_class_name + '_\n' +
					'#define _' + coll_class_name + '_\n\n' +
					'#ifdef HAVE_CONFIG_H\n' +
					'#include \"config.h\"\n' +
					'#endif\n\n' +
					'#include \"CollectorAPI.hxx\"\n\n\n\n' +
					'namespace OpenSpeedShop { namespace Framework {\n\n' +
					'\t\tclass ' + coll_class_name + ' :\n' +
					'\tpublic CollectorImpl\n' +
					'\t\t{\n\n' +
					'\t\tpublic:\n\n' +
					'\t' + coll_class_name + '();\n\n' +
					'\tvirtual Blob getDefaultParameterValues() const;\n' +
					'\tvirtual void getParameterValue(const std::string&,\n' +
					'                                 const Blob&, void*) ' + 
					'const;\n\n' +
					'\tvirtual void setParameterValue(const std::string&,\n' +
					'                                 const void*, Blob&) ' + 
					'const;\n\n' +
					'\tvirtual void startCollecting(const Collector&, ' + 
					'const ThreadGroup&) const;\n' +
					'\tvirtual void stopCollecting(const Collector&, ' + 
					'const ThreadGroup&) const;\n\n' +
							'\tvirtual void getMetricValues(const ' + 
							'std::string&,\n' +
					'                                 const Collector&, ' + 
					'const Thread&,\n' +
					'                                 const Extent&, ' + 
					'const Blob&,\n' +
					'                                 const ExtentGroup&, ' + 
					'void*) const;\n\n' +
					'\t\t};\n\n' +
					'} }\n\n' +
					'#endif\n'
				)
		file.close()
		
		file = open(collector_root + raw_plugin_name + 'Collector.cxx', 'w')
		file.write(
					'#include \"' + coll_class_name + '.hxx\"\n' +
					'#include \"blobs.h\"\n\n' +
					'using namespace OpenSpeedShop::Framework;\n\n' +
					'extern "C" CollectorImpl* ' + plugin_name + 
					'_LTX_CollectorFactory()\n{\n' +
					'\treturn new ' + coll_class_name + '();\n}\n\n' +
					coll_class_name + '::' + coll_class_name + '() :\n' +
					'\tCollectorImpl(\"' + plugin_name + 
					'\", \"Short Plugin Description\", ' + 
					'\"Long Plugin Description\")\n{\n}\n\n' +
					'Blob ' + coll_class_name + 
					'::getDefaultParameterValues() const\n{\n}\n\n' +
					'void ' + coll_class_name + 
					'::getParameterValue(const std::string& parameter,\n' +
					'                                 const Blob& data, ' + 
					'void* ptr) const\n{\n}\n\n' +
					'void ' + coll_class_name + '::setParameterValue' + 
					'(const std::string& parameter,\n' +
					'                                 const void* ptr, ' + 
					'Blob& data) const\n{\n}\n\n' +
					'void ' + coll_class_name + 
					'::startCollecting(const Collector& collector,\n' +
					'                                const ThreadGroup& thread) ' + 
					'const\n{\n}\n\n' +
					'void ' + coll_class_name + 
					'::stopCollecting(const Collector& collector,\n' +
					'                                 const ThreadGroup& thread) ' + 
					'const\n{\n' +
					'\t// Execute ' + plugin_name + 
					'_stop_tracing() in the thread\n' +
					'\texecuteNow(collector, thread, \"' + 
					plugin_name + '-rt: ' +
					plugin_name + '_stop_tracing\", Blob());\n\n' +
					'\t// Remove all instrumentation associated ' + 
					'with this collector/thread pairing\n' +
					'\tuninstrument(collector, thread);\n}\n\n' +
					'void ' + coll_class_name + 
					'::getMetricValues(const std::string& metric,\n' +
					'                                 const Collector& ' + 
					'collector,\n' +
					'                                 const Thread& thread,\n' +
					'                                 const Extent& extent,\n' +
					'                                 const Blob& blob,\n' +
					'                                 const ExtentGroup& ' + 
					'subextents,\n' +
					'                                 void* ptr) ' + 
					'const\n{\n}\n\n'
					)
		file.close()
		os.system('touch ' + collector_root  + os.sep + 'blobs.x')
		os.system('touch ' + collector_root  + os.sep + 'runtime.c')
		print 'creating collector plugin files...done'
	
	except :
		exc_info = sys.exc_info()
		print exc_info[1]
		print 'creating collector plugin files...error'
		sys.exit()

## add_panel_files()
def add_panel_files() :
	panel_root = pi_root + '/plugins/panel/' + raw_plugin_name + 'Panel/'
	panel_class_name = raw_plugin_name + 'Panel'
	print 'creating panel plugin files...'
	try :
		file = open(panel_root + raw_plugin_name + 'Panel.hxx', 'w')
		file.write(
				'#ifndef ' + panel_class_name + '_H_H\n' +
				'#define ' + panel_class_name + '_H_H\n' +
				'#include "CustomExperimentPanel.hxx"           ' + 
				'// Do not remove\n' +
				'#include "Panel.hxx"           // Do not remove\n\n' +
				'class PanelContainer;   // Do not remove\n\n' +
				'class ArgumentObject;\n\n\n' +				
				'#undef PANEL_CLASS_NAME\n' +
				'#define PANEL_CLASS_NAME ' + panel_class_name + '\n\n' +
				'class ' + panel_class_name + 
				' : public CustomExperimentPanel\n{\n' +
				'\tpublic:\n' +
				'\t\t' + panel_class_name + 
				'(PanelContainer *pc, const char *n, ArgumentObject *ao);\n\n' +
				'\t\t~' + panel_class_name + 
				'();\n};\n#endif\n'
				)
		file.close()
		
		file = open(panel_root + raw_plugin_name + 'Panel.cxx', 'w')
		file.write(
				'#include \"' + panel_class_name + '.h\"\n' +
				'#include \"PanelContainer.hxx\"   // Do not remove\n' +
				'#include \"plugin_entry_point.hxx\"   // Do not remove\n\n' +
				panel_class_name + '::' + panel_class_name + 
				'(PanelContainer *pc, const char *n, ArgumentObject *ao) : ' +
				'CustomExperimentPanel(pc, n, ao, (const char *)\"' + 
				plugin_name + '\")\n{\n}\n\n' +
				'//! Destroys the object and frees any allocated resources\n' +
				'/*! The only thing that needs to be cleaned ' + 
				'up is the baseWidgetFrame.\n' +
				'*/\n' +
				panel_class_name + '::~' + panel_class_name + '()\n{\n}\n'
				)
		file.close()
		
		file = open(panel_root + 'local_plugin_info.hxx', 'w')
		file.write(
					'  plugin_entry->grouping = \"TOPLEVEL\";\n' +
					'  plugin_entry->show_immediate = 0;\n' +
					'  plugin_entry->plugin_description = ' + 
					'\"PLUGIN_DESCRIPTION\";\n' +
					'  plugin_entry->plugin_short_description = ' + 
					'\"Add Short Plugin Description\";\n' +
					'  plugin_entry->plugin_name =  \"' + 
					panel_class_name + '.so\";\n' +
					'  plugin_entry->plugin_location = ' + 
					'\"$OPENSS_PLUGIN_PATH\";\n' +
					'  plugin_entry->plugin_entry_point = \"panel_init\";\n' +
					'  plugin_entry->menu_heading =  \"&File\";\n' +
					'  plugin_entry->sub_menu_heading =  \"&Experiments\";\n' +
					'  plugin_entry->menu_label =  \"' + 
					raw_plugin_name + '\";\n' +
					'  plugin_entry->menu_accel =  \"Alt+Q\";\n' +
					'  plugin_entry->panel_type =  \"' + 
					raw_plugin_name + '\";\n'
					)
		file.close()
		print 'creating panel plugin files...done'
		
	except :
		exc_info = sys.exc_info()
		print exc_info[1]
		print 'creating panel plugin files...error'
		sys.exit()
							
## add_wizard_files()
def add_wizard_files() :
	wizard_root = (pi_root + '/plugins/wizard/' + raw_plugin_name + 
				   'WizardPanel/')
	wizard_class_name = raw_plugin_name + 'WizardPanel'
	print 'creating wizard plugin files...'
	try :
		file = open(wizard_root + raw_plugin_name + 'WizardPanel.hxx', 'w')
		file.write(
					'//Please refer to the OpenSpeedShop ' + 
					'wizard source code located\n' +
					'//in the <oss_top_src_dir>/plugins/wizards directory\n\n' +
					'#ifndef ' + string.upper(wizard_class_name) + '_H\n' +
					'#define ' + string.upper(wizard_class_name) + '_H\n' +
					'#include "Panel.hxx"           // Do not remove\n\n' +
					'class PanelContainer;   // Do not remove\n\n' +
					'#define PANEL_CLASS_NAME ' + wizard_class_name + '\n\n' +
					'class ' + wizard_class_name + 
					' : public Panel\n{\n\tQ_OBJECT\n' +
					'public:\n\t' +
					wizard_class_name + '();\n\t' +
					wizard_class_name + '(PanelContainer *pc, ' + 
					'const char *n, ArgumentObject *ao);\n' +
					'\t~' + wizard_class_name + '();\n' +
					'};\n#endif\n'
					)
		file.close()
		
		file = open(wizard_root + raw_plugin_name + 'WizardPanel.cxx', 'w')
		file.write(
					'//Please refer to the OpenSpeedShop ' + 
					'wizard source code located\n' +
					'//in the <oss_top_src_dir>/plugins/wizards directory\n\n' +
					'#include \"' + wizard_class_name + '.hxx\"\n' +
					'#include \"PanelContainer.hxx\"   // Do not remove\n' +
					'#include \"plugin_entry_point.hxx\"   ' + 
					'// Do not remove\n\n' +
					'using namespace OpenSpeedShop::Framework;\n\n' +
					wizard_class_name + '::' + wizard_class_name + 
					'(PanelContainer *pc, const char *n, ArgumentObject *ao) ' + 
					': Panel(pc, n)\n{\n}\n\n' +
					wizard_class_name + '::~' + wizard_class_name + 
					'()\n{\n}\n\n'
					)
		file.close()
		
		file = open(wizard_root + raw_plugin_name + 'Description.hxx', 'w')
		file.write(
					'#define vIODescription \"Add Long ' + 
					'Plugin Description Here\"\n' +
					'#define eIODescription \"Add Short ' + 
					'Plugin Description Here\"\n'
					)
		file.close()
		
		file = open(wizard_root + 'local_plugin_info.hxx', 'w')
		file.write(
					'  plugin_entry->grouping = \"TOPLEVEL\";\n' +
					'  plugin_entry->show_immediate = 0;\n' +
					'  plugin_entry->plugin_description = ' + 
					'\"PLUGIN_DESCRIPTION\";\n' +
					'  plugin_entry->plugin_short_description = ' + 
					'\"Add Short Plugin Description\";\n' +
					'  plugin_entry->plugin_name =  \"' + wizard_class_name + 
					'.so\";\n' +
					'  plugin_entry->plugin_location = ' + 
					'\"$OPENSS_PLUGIN_PATH\";\n' +
					'  plugin_entry->plugin_entry_point = \"panel_init\";\n' +
					'  plugin_entry->menu_heading =  \"&File\";\n' +
					'  plugin_entry->sub_menu_heading =  \"&Wizards\";\n' +
					'  plugin_entry->menu_label =  \"' + raw_plugin_name + 
					' Wizard\";\n' +
					'  plugin_entry->menu_accel =  \"Alt+Z\";\n' +
					'  plugin_entry->panel_type =  \"' + raw_plugin_name + 
					' Wizard\";\n'
					)
		file.close()
		print 'creating wizard plugin files...done'
		
	except :
		exc_info = sys.exc_info()
		print exc_info[1]
		print 'creating wizard plugin files...error'
		sys.exit()

## add_view_files()
def add_view_files() :
	view_root = pi_root + '/plugins/view/' + plugin_name + '/'
	print 'creating view plugin files...'	
	try :
		file = open(view_root + plugin_name + '_view.cxx', 'w')
		file.write(
					'//Please refer to the OpenSpeedShop view ' + 
					'source code located\n' +
					'//in the <oss_top_src_dir>/plugins/views directory\n\n'
					)
		file.close()
		print 'creating view plugin files...done'
	except :
		exc_info = sys.exc_info()
		print exc_info[1]
		print 'creating view plugin files...error'
		sys.exit()

def add_bootstrap():
	print 'creating bootstrap script...'
	try:
		file = open(pi_root + os.sep + 'bootstrap', 'w')
		file.write(
					'#! /bin/bash\n' + 
					'echo running bootstrap\n' + 
				  	'aclocal\n' +
				  	'libtoolize --force --ltdl --copy\n' +
				  	'autoheader\n' +
				  	'automake --foreign --add-missing --copy\n' +
				  	'autoconf\n' +
					'echo done running bootstrap\n'				   					   
				   )
		file.close()
		os.system('chmod +x ' + pi_root + os.sep + 'bootstrap')
		print 'creating bootstrap script...done'
	except:
		exc_info = sys.exc_info()
		print exc_info[1]
		print 'creating bootstrap script...error'
		sys.exit()
## format_dir_string(dir_string)
# @param dir_string: Directory String
# @return: String
def format_dir_string(dir_string):
	if dir_string[-1] != os.sep:
		return dir_string + os.sep
	else:
		return dir_string

## main()	
def main() :
	global oss_top_src_dir
	global plugin_root
	global raw_plugin_name
	global plugin_name
	global pi_root
	
	parser = OptionParser('createPluginStructure.py ' + 
						  ' [-n|--name=]NAME [-o|--output=]' + 
						  'DIR [-t|--topsrcdir=]DIR [--force]')	
		
	parser.add_option('-n', '--name', action='store', type='string',
					  dest='name', help='specifies plugin name [Required]', 
					  metavar='NAME') 
		
	parser.add_option('-o', '--output', action='store', type='string', 
					  dest='output', 
					  help='specifies output destination [Required]', 
					  metavar='DIR')
		
	parser.add_option('-t', '--topsrcdir', action='store', type='string', 
					  dest='topsrcdir', 
					  help='location of OpenSpeedShop\'s ' + 
					  'top source directory [Required]', 
					  metavar='DIR')
		
	parser.add_option('--force', action='store_true', dest='force', 
					  help='force overwrite [Optional]')
		
	parser.set_defaults(force = False)
	
	(options,args) = parser.parse_args()
	
	if (options.name == None or options.output == None or 
	    options.topsrcdir == None):
		print 'too few arguments'
		print parser.usage
		print ('try createPluginStructure.py --help for more information')
		sys.exit()
	else:
		if options.topsrcdir[-1] == os.sep :
			oss_top_src_dir = options.topsrcdir[:-1]
		else:
			oss_top_src_dir = options.topsrcdir						
		plugin_root = format_dir_string(options.output)
		raw_plugin_name = options.name
		plugin_name = string.lower(raw_plugin_name)
		pi_root = plugin_root + plugin_name
	try:	
		make_dirs(options.force)
		add_make_files()
		add_ac_files()
		add_collector_files()
		add_view_files()
		add_panel_files()
		add_wizard_files()
		add_bootstrap()		
	except :
		exc_info = sys.exc_info()
		print exc_info[1]
		sys.exit()

if __name__ == '__main__' :
	main()
	print 'done...'
