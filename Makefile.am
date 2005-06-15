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

TEST_SUBDIRS =	test/test_source/unit/fw \
		test/test_source/unit/fw/interval \
		test/test_source/unit/fw/database \
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

DIST_TEST_SUBDIRS = test/test_source/unit/fw/* \
		test/test_source/unit/fw/interval/* \
		test/test_source/unit/fw/database/* \
		test/test_source/unit/pcli/* \
		test/test_source/unit/pcli/parse_result/* \
		test/test_source/unit/pcli/parse_target/* \
		test/test_source/dpcl/* \
		test/test_source/dpcl/one_shot/* \
		test/test_source/dpcl/probe_module/* \
		test/test_source/dpcl/point_probe/* \
		test/test_source/dpcl/start_terminate/* \
		test/test_source/dpcl/module_addr/* \
		test/test_source/dpcl/stmt_info/* \
		test/test_source/dpcl/phase/* \
		test/test_scripts/* \
		test/executables/mutatee/* \
		test/executables/mutatee/c_version/* \
		test/executables/mutatee/cplus_version/* \
		test/executables/mutatee/f77_version/* \
		test/executables/Eon/* 

SUBDIRS = . \
	libltdl \
	message \
	libopenss-framework \
	plugins \
	openss \
	pcli \
	examples \
	base \
	objects \
	dialogs \
	expr_info \
	plugin \
	chart \
	gui \
	Panels \
	usability

DIST_SUBDIRS = $(SUBDIRS)
# need the images files and the tests files in tarball.
EXTRA_DIST = images/* $(DIST_TEST_SUBDIRS)

tests: $(TEST_SUBDIRS)

DOC_SUBDIRS = \
	libopenss-framework \
	Panels pcli base chart gui plugin objects dialogs plugins/collectors

doc: $(DOC_SUBDIRS) 
	$(DOXYGEN) Doxyfile
