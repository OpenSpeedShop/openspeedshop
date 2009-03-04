################################################################################
# Copyright (c) 2009 Krell Institute All Rights Reserved.
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

noinst_PROGRAMS = helloworld

MPI_IMPL = @DEFAULT_MPI_IMPL@

MPICC	= $($(MPI_IMPL)_CC)
CC	= $(MPICC)

CFLAGS = \
	$($(MPI_IMPL)_CPPFLAGS) \
	-g -gdwarf-2 -O1 

helloworld_LDFLAGS = \
	-lm $($(MPI_IMPL)_LDFLAGS) $($(MPI_IMPL)_LIBS)

helloworld_LDADD = \
	-ldl

helloworld_SOURCES = \
	helloworld.c

EXTRA_DIST = helloworld.c Makefile.am
