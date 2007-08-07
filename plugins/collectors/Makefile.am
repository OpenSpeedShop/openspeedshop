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
	pcsamp

if HAVE_LIBUNWIND
SUBDIRS += usertime io iot
endif

if HAVE_PAPI
SUBDIRS += hwc
if HAVE_LIBUNWIND
SUBDIRS += hwctime
endif
endif

if HAVE_MPI
if HAVE_LIBUNWIND
SUBDIRS += mpi mpit
if HAVE_OTF
if HAVE_VT
SUBDIRS += mpiotf
endif
endif
endif
endif

if HAVE_BINUTILS
if HAVE_LIBUNWIND
SUBDIRS += fpe
endif
endif

DIST_SUBDIRS = \
	pcsamp

if HAVE_LIBUNWIND
DIST_SUBDIRS += usertime io iot
endif

if HAVE_PAPI
DIST_SUBDIRS += hwc
if HAVE_LIBUNWIND
DIST_SUBDIRS += hwctime
endif
endif

if HAVE_MPI
if HAVE_LIBUNWIND
DIST_SUBDIRS += mpi mpit
if HAVE_OTF
if HAVE_VT
DIST_SUBDIRS += mpiotf
endif
endif
endif
endif

if HAVE_BINUTILS
if HAVE_LIBUNWIND
DIST_SUBDIRS += fpe
endif
endif


doc:
	$(DOXYGEN)
