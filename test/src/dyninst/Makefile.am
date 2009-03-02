###############################################################################
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
#
# directories that will be built
SUBDIRS = create findPoint fpe exitpt

#if HAVE_F77
#SUBDIRS += createf77
#endif

#
# directories that will be packaged into tar.gz.
# these can be a subset of the directories in SUBDIRS.
#
DIST_SUBDIRS = $(SUBDIRS) createf77

check_SCRIPTS_common = runall-dyninst

check_SCRIPTS = $(check_SCRIPTS_common)

TESTS = $(check_SCRIPTS)

EXTRA_DIST = runall-dyninst Makefile.am NEWS README AUTHORS INSTALL COPYING ChangeLog

