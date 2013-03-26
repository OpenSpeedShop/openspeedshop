###############################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2013 Krell Institute. All Rights Reserved.
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

# directories that will be built

SUBDIRS =

if BUILD_OFFLINE
SUBDIRS += check_setup
else

if BUILD_INSTRUMENTOR
SUBDIRS += check_openss
endif

if BUILD_DPCL
SUBDIRS += check_dpcl
endif

SUBDIRS += check_setup
endif

DIST_SUBDIRS = check_dpcl check_openss check_setup check_dyninst

EXTRA_DIST = \
	README runit
