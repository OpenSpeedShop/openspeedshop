################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2013 Krell Institute  All Rights Reserved.
# Copyright (c) 2014 Argo Navis Technologies. All Rights Reserved.
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
else
if !BUILD_INSTRUMENTOR
SUBDIRS += usertime io iot
endif
if BUILD_CBTF
SUBDIRS += usertime io iot
endif
endif

if HAVE_TARGET_LIBMONITOR
if HAVE_TARGET_LIBUNWIND
SUBDIRS += usertime io iot
endif
endif

#if HAVE_PAPI
SUBDIRS += hwc hwcsamp
if HAVE_LIBUNWIND
SUBDIRS += hwctime
else
if !BUILD_INSTRUMENTOR
SUBDIRS += hwctime
endif
endif
#endif

if HAVE_TARGET_LIBMONITOR
if HAVE_TARGET_PAPI
SUBDIRS += hwc hwcsamp
if HAVE_TARGET_LIBUNWIND
SUBDIRS += hwctime
endif
endif
endif

if HAVE_MPI
if HAVE_LIBUNWIND
SUBDIRS += mpi mpit
else
if !BUILD_INSTRUMENTOR
SUBDIRS += mpi mpit
endif
if BUILD_CBTF
SUBDIRS += mpi mpit
endif
endif
endif

if HAVE_MPI
if HAVE_LIBUNWIND
if HAVE_OTF
if HAVE_VT
SUBDIRS += mpiotf
endif
endif
endif
endif

if HAVE_TARGET_LIBMONITOR
if HAVE_TARGET_MPI
if HAVE_TARGET_LIBUNWIND
SUBDIRS += mpi mpit
if HAVE_TARGET_OTF
if HAVE_TARGET_VT
SUBDIRS += mpiotf
endif
endif
endif
endif
endif

if HAVE_BINUTILS
if HAVE_LIBUNWIND
SUBDIRS += fpe
else
if !BUILD_INSTRUMENTOR
SUBDIRS += fpe
endif
endif
endif

if HAVE_TARGET_LIBMONITOR
if HAVE_TARGET_BINUTILS
if HAVE_TARGET_LIBUNWIND
SUBDIRS += fpe
endif
endif
endif

if BUILD_CBTF
# SUBDIRS += mem pthreads iop mpip
# For cbtf instrumentor, we do not build collector runtimes.
# BUT, in spite of the configuration values for things like papi,unwind, etc,
# we DO need to build all the collectors to get the metric routines.
# ie. the cbtf instrumentor version should still be able to view a database
# and this code is not dependent on the runtimes.  The individual data blobs.h 
# for all collectors originally in OSS are a match to the cbtf xdr defines
# so we use the existing original xdr blobs definitions.
# For the new collectors, iop,mpip,mem,pthreads, the messages are defined for
# xdr by cbtf and installed by cbtf-krell/messages. So there is a dependency on
# the cbtf messages being installed...
SUBDIRS += pcsamp usertime fpe hwc hwcsamp hwctime io iot iop mem mpi mpit mpiotf mpip pthreads
if HAVE_CBTF_MESSAGES_CUDA
SUBDIRS += cuda
endif
endif


DIST_SUBDIRS = \
	pcsamp usertime io iot iop hwc hwcsamp hwctime mpi mpip mpit mpiotf fpe mem pthreads cuda

doc:
	$(DOXYGEN)
