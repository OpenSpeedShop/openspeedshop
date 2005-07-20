################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 59 Temple
# Place, Suite 330, Boston, MA  02111-1307  USA
################################################################################

SUBDIRS = \
	libltdl \
	message \
	libopenss-framework/runtime \
	libopenss-framework \
	libopenss-queries \
	pcli \
	base \
	objects \
	dialogs \
	expr_info \
	plugin \
	chart \
	gui \
	openss \
	plugins \
	Panels \
	examples \
	test \
	usability

TEST_SUBDIRS =	\
	test/ \
	test/test_source/ \
	test/test_source/unit/ \
	test/test_source/unit/fw \
	test/test_source/unit/fw/interval \
	test/test_source/unit/pcli \
	test/test_source/unit/pcli/parse_result \
	test/test_source/unit/pcli/parse_target \
	test/test_source/dpcl \
	test/test_source/dpcl/one_shot \
	test/test_source/dpcl/probe_module \
	test/test_source/dpcl/point_probe \
	test/test_source/dpcl/start_terminate \
	test/test_source/dpcl/module_addr \
	test/test_source/dpcl/stmt_info \
	test/test_source/dpcl/phase \
	test/test_scripts \
	test/executables/mutatee/ \
	test/executables/mutatee/c_version \
	test/executables/mutatee/cplus_version \
	test/executables/mutatee/f77_version \
	test/executables/Eon/ 

tests: $(TEST_SUBDIRS)

DOC_SUBDIRS = \
	libopenss-framework/runtime \
	libopenss-framework \
	libopenss-queries \
	pcli \
	gui

doc: $(DOC_SUBDIRS) 
	for i in $(DOC_SUBDIRS); do (cd $$i; $(MAKE) doc); done

DIST_SUBDIRS = $(SUBDIRS)
# Need the images files and the tests files in tarball.
EXTRA_DIST = images/* 

# The following "framework" target is simply a short-hand for the framework
# developers (i.e. WDH) to build just those components necessary for working
# on the framework library.

FRAMEWORK_SUBDIRS = \
	libltdl \
	libopenss-framework/runtime \
	libopenss-framework \
	libopenss-queries \
	plugins/collectors \
	examples/libopenss-framework/tool

framework:
	for i in $(FRAMEWORK_SUBDIRS); do (cd $$i; $(MAKE)); done
