################################################################################# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
noinst_PROGRAMS = eut_diag

CFLAGS  = \
         -g -gdwarf-2

CXXFLAGS        = \
         -g -gdwarf-2

CPPFLAGS        = \
         -pg -g -gdwarf-2

eut_diag_CXXFLAGS = \
         $(DPCL_CPPFLAGS)

eut_diag_LDFLAGS = \
        -L$(ROOT)/lib \
        $(DPCL_LDFLAGS) \
        $(DPCL_LIBS) -lelf -lunwind

eut_diag_LIBS = \
        $(DPCL_LDFLAGS) \
        $(DPCL_LIBS) -lelf -lunwind

eut_diag_SOURCES = \
        eut_diag.cxx

EXTRA_DIST = \
        README \
        eut_diag.tst \
        eut_diag.tst.expected \
        runone-dpcl \
        runall-dpcl

