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

noinst_PROGRAMS = mutator probe_module

mutator_CXXFLAGS = \
	 $(DPCL_CPPFLAGS) -fPIC

mutator_LDFLAGS = \
	-L$(ROOT)/lib \
	$(DPCL_LDFLAGS) \
	$(DPCL_LIBS) -lelf

mutator_LIBS = \
	$(DPCL_LDFLAGS) \
	$(DPCL_LIBS) -lelf

mutator_SOURCES = \
	mutator.C

probe_module_CFLAGS = \
	 $(DPCL_CPPFLAGS) -fPIC

probe_module_LDFLAGS = \
	$(DPCL_LDFLAGS) \
	$(DPCL_LIBS) -shared -o probe_module.so 

probe_module_LIBS = \
	$(DPCL_LDFLAGS) \
	$(DPCL_LIBS) 

probe_module_SOURCES = \
	probe_module.c

EXTRA_DIST = \
        README \
        phase.tst \
        phase.tst.expected \
        runone-dpcl \
        runall-dpcl

