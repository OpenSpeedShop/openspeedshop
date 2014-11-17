################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2013 Krell Institute All Rights Reserved.
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
ACLOCAL_AMFLAGS = -I libltdl/m4

BASE_SUBDIRS =

LTDL_SUBDIRS = libltdl

MESSAGE_SUBDIRS = libopenss-message

RT_SUBDIRS = libopenss-runtime

FW_SUBDIRS = libopenss-framework libopenss-queries

CLI_SUBDIRS = libopenss-cli

GUI_SUBDIRS = \
	libopenss-guibase \
	libopenss-guiobjects \
	libopenss-guichart \
	libopenss-guidialogs \
	libopenss-guiexprinfo \
	libopenss-guiplugin \
	libopenss-gui

PLUGINS_SUBDIRS = plugins

COLLECTOR_SUBDIRS = plugins/collectors

TARGET_SHARED_PLUGINS_SUBDIRS = \
                        plugins/collectors  

CBTF_SUBDIRS = \
	ossdumpsym

CUDA_SUBDIRS = \
	libopenss-queries-cuda \
	ossdumpcuda

CLIENT_SUBDIRS = \
	openss

MISC_SUBDIRS = \
	pyscripting \
	doc \
	man \
	sanity \
	usability

TARGET_SHARED_MISC_SUBDIRS = \
	pyscripting \
	sanity 

if BUILD_CBTF
if HAVE_CBTF_MESSAGES_CUDA
FW_SUBDIRS += libopenss-queries-cuda
CLIENT_SUBDIRS += $(CUDA_SUBDIRS) 
CLIENT_SUBDIRS += $(CBTF_SUBDIRS) 
endif
endif

if HAVE_QTLIB

if BUILD_RUNTIME_ONLY
if HAVE_TARGET_SHARED
BASE_SUBDIRS += \
	$(RT_SUBDIRS) \
	$(TARGET_SHARED_PLUGINS_SUBDIRS) \
	$(CLIENT_SUBDIRS)  
else
BASE_SUBDIRS += \
	$(RT_SUBDIRS) \
	$(COLLECTOR_SUBDIRS) \
	$(CLIENT_SUBDIRS)
endif
else
BASE_SUBDIRS += \
	$(LTDL_SUBDIRS) \
	$(MESSAGE_SUBDIRS) \
	$(RT_SUBDIRS) \
	$(FW_SUBDIRS) \
	$(CLI_SUBDIRS) \
	$(GUI_SUBDIRS) \
	$(PLUGINS_SUBDIRS) \
	$(CLIENT_SUBDIRS) \
	$(MISC_SUBDIRS)
endif

else
if BUILD_RUNTIME_ONLY
if HAVE_TARGET_SHARED
BASE_SUBDIRS += \
	$(RT_SUBDIRS) \
	$(TARGET_SHARED_PLUGINS_SUBDIRS) \
	$(CLIENT_SUBDIRS) \
	$(TARGET_SHARED_MISC_SUBDIRS)
else
BASE_SUBDIRS += \
	$(RT_SUBDIRS) \
	$(COLLECTOR_SUBDIRS) \
	$(CLIENT_SUBDIRS)
endif
else
BASE_SUBDIRS += \
	$(LTDL_SUBDIRS) \
	$(MESSAGE_SUBDIRS) \
	$(RT_SUBDIRS) \
	$(FW_SUBDIRS) \
	$(CLI_SUBDIRS) \
	$(PLUGINS_SUBDIRS) \
	$(CLIENT_SUBDIRS) \
	$(MISC_SUBDIRS)
endif

endif

SUBDIRS = \
	$(BASE_SUBDIRS)

CHECK_SUBDIRS = \
	test

check: $(CHECK_SUBDIRS)
	cd $(CHECK_SUBDIRS); $(MAKE) clean; $(MAKE) check


DOC_SUBDIRS = \
	libopenss-runtime \
	libopenss-framework \
	libopenss-queries \
	libopenss-cli \
	libopenss-message \
	plugins/collectors \
	libopenss-gui

doc: $(DOC_SUBDIRS) 
	for i in $(DOC_SUBDIRS); do (cd $$i; $(MAKE) doc); done

DIST_SUBDIRS = \
	$(LTDL_SUBDIRS) \
	$(MESSAGE_SUBDIRS) \
	$(RT_SUBDIRS) \
	$(FW_SUBDIRS) \
	$(CLI_SUBDIRS) \
	$(GUI_SUBDIRS) \
	$(PLUGINS_SUBDIRS) \
	$(CLIENT_SUBDIRS) \
	$(CBTF_SUBDIRS) \
	$(CUDA_SUBDIRS) \
	$(MISC_SUBDIRS)

# Need the images files and the tests files in tarball.
EXTRA_DIST = libopenss-guiimages/*.xpm libopenss-guiimages/*.bmp mc bootstrap do_build bootstrap_rpm README-mpi

# The following "framework" target is simply a short-hand for the framework
# developers (i.e. WDH) to build just those components necessary for working
# with the framework library.

FRAMEWORK_SUBDIRS = \
	$(RT_SUBDIRS) \
	$(FW_SUBDIRS)

framework:
	for i in $(FRAMEWORK_SUBDIRS); do (cd $$i; $(MAKE)); done

# The following "cli" and "cli-install" targets are simply a short-hand for the
# CLI developers to build (and install in the later case) just those components
# necessary for working with the CLI.

CLIONLY_SUBDIRS = \
	$(MESSAGE_SUBDIRS) \
	$(RT_SUBDIRS) \
	$(FW_SUBDIRS) \
	$(CLI_SUBDIRS) \
	openss \
	plugins/collectors \
	plugins/views \
	pyscripting 

cli:
	for i in $(CLIONLY_SUBDIRS); do (cd $$i; $(MAKE)); done

cli-install:
	for i in $(CLIONLY_SUBDIRS); do (cd $$i; $(MAKE) install); done

TARGET_STATIC_SUBDIRS = \
	libopenss-runtime \
	plugins/collectors 

target-static:
	for i in $(TARGET_STATIC_SUBDIRS); do (cd $$i; $(MAKE)); done

target-static-install:
	for i in $(TARGET_STATIC_SUBDIRS); do (cd $$i; $(MAKE) install); done

target-static-uninstall:
	for i in $(TARGET_STATIC_SUBDIRS); do (cd $$i; $(MAKE) uninstall); done

target_static-clean:
	for i in $(TARGET_STATIC_SUBDIRS); do (cd $$i; $(MAKE) clean); done

# The following "offline" target is simply a short-hand for the offline only users
# and developers to build just those components necessary for working
# with the offline collectors.

OFFLINE_SUBDIRS = \
	libopenss-message \
	libopenss-runtime \
	libopenss-framework \
	libopenss-queries \
	libopenss-cli \
	openss \
	plugins/collectors \
	plugins/views \
	pyscripting 

offline:
	for i in $(OFFLINE_SUBDIRS); do (cd $$i; $(MAKE)); done

offline-install:
	for i in $(OFFLINE_SUBDIRS); do (cd $$i; $(MAKE) install); done

offline-uninstall:
	for i in $(OFFLINE_SUBDIRS); do (cd $$i; $(MAKE) uninstall); done

offline-clean:
	for i in $(OFFLINE_SUBDIRS); do (cd $$i; $(MAKE) clean); done

OFFLINE_STATIC_SUBDIRS = \
	libopenss-runtime \
	plugins/collectors

offlinestatic:
	for i in $(OFFLINE_STATIC_SUBDIRS); do (cd $$i; $(MAKE)); done

offlinestatic-install:
	for i in $(OFFLINE_STATIC_SUBDIRS); do (cd $$i; $(MAKE) install); done

offlinestatic-uninstall:
	for i in $(OFFLINE_STATIC_SUBDIRS); do (cd $$i; $(MAKE) uninstall); done

offlinestatic-clean:
	for i in $(OFFLINE_STATIC_SUBDIRS); do (cd $$i; $(MAKE) clean); done
