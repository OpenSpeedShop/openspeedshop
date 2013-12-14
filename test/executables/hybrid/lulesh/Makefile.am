################################################################################
# Copyright (c) 2013 Krell Institute All Rights Reserved.
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

noinst_PROGRAMS = lulesh

MPI_IMPL = @DEFAULT_MPI_IMPL@

MPICC	= $($(MPI_IMPL)_CC)
CC	= $(MPICC)

CXXFLAGS = $($(MPI_IMPL)_CPPFLAGS) -g -gdwarf-2 -O2 -fopenmp -DUSE_MPI=1

lulesh_LDFLAGS = \
	-lm  $($(MPI_IMPL)_LDFLAGS) $($(MPI_IMPL)_LIBS) -lmpi_cxx


lulesh_LIBS = \
	-ldl


lulesh_SOURCES = \
        lulesh.cc \
        lulesh-comm.cc \
        lulesh-viz.cc \
        lulesh-util.cc \
        lulesh-init.cc

EXTRA_DIST = lulesh.cc lulesh-comm.cc lulesh-viz.cc lulesh-util.cc lulesh-init.cc Makefile.am

