################################################################################
# Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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

noinst_PROGRAMS = mutator mutatee

AM_CXXFLAGS	= \
	-g -gdwarf-2 -O0

AM_CPPFLAGS	= \
	-g -gdwarf-2 -O0

mutator_CXXFLAGS = \
	-DUSE_STL_VECTOR -DTEST_CREATE_PROCESS \
	-I$(DYNINST_DIR)/include/dyninst 

mutator_LDFLAGS = \
	$(DYNINST_LDFLAGS) \
	$(DYNINST_LIBS) \
	-lparseAPI \
	$(LIBDWARF_LDFLAGS) \
	$(LIBDWARF_LIBS) \
	-lelf -liberty -ldl

mutator_LIBS = \
	$(DYNINST_LDFLAGS) \
	$(DYNINST_LIBS) \
	-liberty -ldl

mutator_SOURCES = \
	mutator.cxx

mutatee_CXXFLAGS = \
	-g -gdwarf-2

mutatee_LDFLAGS = \
	-ldl

mutatee_SOURCES = \
	mutatee.cxx

EXTRA_DIST      =       runall-dyninst runone-dyninst fpe.tst fpe.tst.expected Makefile.am

