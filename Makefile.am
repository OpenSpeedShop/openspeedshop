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
	libltdl \
	libopenss-message \
	libopenss-runtime \
	libopenss-framework \
	libopenss-queries \
	libopenss-cli \
	libopenss-guibase \
	libopenss-guiobjects \
	libopenss-guidialogs \
	expr_info \
	libopenss-guiplugin \
	libopenss-guichart \
	libopenss-gui \
	openss \
	plugins \
	examples \
	test \
	usability


DOC_SUBDIRS = \
	libopenss-runtime \
	libopenss-framework \
	libopenss-queries \
	libopenss-cli \
	libopenss-gui

doc: $(DOC_SUBDIRS) 
	for i in $(DOC_SUBDIRS); do (cd $$i; $(MAKE) doc); done

DIST_SUBDIRS = $(SUBDIRS) \
		test
# Need the images files and the tests files in tarball.
EXTRA_DIST = libopenss-guiimages/* 

# The following "framework" target is simply a short-hand for the framework
# developers (i.e. WDH) to build just those components necessary for working
# on the framework library.

FRAMEWORK_SUBDIRS = \
	libltdl \
	libopenss-runtime \
	libopenss-framework \
	libopenss-queries \
	examples/libopenss-framework

framework:
	for i in $(FRAMEWORK_SUBDIRS); do (cd $$i; $(MAKE)); done
