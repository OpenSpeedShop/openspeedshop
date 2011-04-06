###############################################################################
# Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2010 Krell Institute. All Rights Reserved.
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

# directories that will be affected
SUBDIRS = TroubleShootingGuide

DIST_SUBDIRS = TroubleShootingGuide BuildInstallGuide_files BuildTutorial usersGuide-1.9c_files

EXTRA_DIST = BuildingInstallingDPCL.html \
	BuildInstallGuideTutorial.html \
	OpenSpeedShopTargetUsage.html \
	QuickStartGuide.html \
	index.html \
	Makefile.am \
	ossscript.html \
	OfflineEnvironmentVariables.html \
	OpenSpeedShop_Logo.jpg \
	rpmQuery.asc 


package = @PACKAGE@
packver = @PACKAGE@-@VERSION@
#docdir  = ${prefix}/share/doc/@PACKAGE@-@VERSION@/users_guide
#docdir2  = ${prefix}/share/doc/@PACKAGE@-@VERSION@/users_guide/TroubleShootingGuide
#docdir3  = ${prefix}/share/doc/@PACKAGE@-@VERSION@
docdir  = ${prefix}/share/doc/packages/OpenSpeedShop/users_guide
docdir2  = ${prefix}/share/doc/packages/OpenSpeedShop/users_guide/TroubleShootingGuide
docdir3  = ${prefix}/share/doc/packages/OpenSpeedShop
docdir4  = ${prefix}/share/doc/packages/OpenSpeedShop/users_guide/usersGuide-1.9c_files

install-reference: index.html
	@$(NORMAL_INSTALL)
	${mkinstalldirs} ${DESTDIR}${docdir}
	${mkinstalldirs} ${DESTDIR}${docdir2}
	if test -f index.html ; then \
		for file in *.html ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir}/$$basefile; \
		done ; \
		for file in *.png ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir}/$$basefile; \
		done ; \
		${INSTALL_DATA} OpenSpeedShop_Logo.jpg ${DESTDIR}${docdir} ;\
	fi
	if test -f TroubleShootingGuide/index.html ; then \
		for file in TroubleShootingGuide/*.html ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir2}/$$basefile; \
		done ; \
		for file in TroubleShootingGuide/*.png ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir2}/$$basefile; \
		done ; \
		${INSTALL_DATA} TroubleShootingGuide/doxygen.css ${DESTDIR}${docdir2} ;\
	fi
	if test -f usersGuide-1.9c_files/index.html ; then \
		for file in usersGuide-1.9c_files/*.htm ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir4}/$$basefile; \
		done ; \
		for file in usersGuide-1.9c_files/*.html ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir4}/$$basefile; \
		done ; \
		for file in usersGuide-1.9c_files/*.gif ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir4}/$$basefile; \
		done ; \
		for file in usersGuide-1.9c_files/*.png ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir4}/$$basefile; \
		done ; \
		for file in usersGuide-1.9c_files/*.thmx ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir4}/$$basefile; \
		done ; \
		for file in usersGuide-1.9c_files/*.xml ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir4}/$$basefile; \
		done ; \
		${INSTALL_DATA} usersGuide-1.9c_files/doxygen.css ${DESTDIR}${docdir4} ;\
	fi

uninstall-reference: index.html
	@$(NORMAL_UNINSTALL)
	@dir='${<D}'; \
	echo "dir =@dir"
	for file in $$dir/* ; do \
		basefile=`echo $$file | sed -e 's,^.*/,,'`; \
		rm -f ${DESTDIR}${docdir}/$$basefile; \
		rm -f ${DESTDIR}${docdir2}/$$basefile; \
		rm -f ${DESTDIR}${docdir4}/$$basefile; \
	done
	-rm -rf ${DESTDIR}${docdir}/
	-rm -rf ${DESTDIR}${docdir2}/
	-rm -rf ${DESTDIR}${docdir3}/

all-local: index.html

install-data-am: install-reference

uninstall: uninstall-reference
