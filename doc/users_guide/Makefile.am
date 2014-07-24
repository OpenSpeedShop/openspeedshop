###############################################################################
# Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2014 Krell Institute. All Rights Reserved.
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
SUBDIRS = OpenSpeedShop_BuildInstallGuide_files OpenSpeedShop_User_Manual_files

DIST_SUBDIRS = OpenSpeedShop_BuildInstallGuide_files OpenSpeedShop_User_Manual_files

EXTRA_DIST = BuildAndInstallGuide.html \
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
docdir3  = ${prefix}/share/doc/packages/OpenSpeedShop
docdir4  = ${prefix}/share/doc/packages/OpenSpeedShop/users_guide/OpenSpeedShop_User_Manual_files
docdir5  = ${prefix}/share/doc/packages/OpenSpeedShop/users_guide/OpenSpeedShop_BuildInstallGuide_files

install-reference: index.html
	@$(NORMAL_INSTALL)
	${mkinstalldirs} ${DESTDIR}${docdir}
	${mkinstalldirs} ${DESTDIR}${docdir4}
	${mkinstalldirs} ${DESTDIR}${docdir5}
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
	if test -f OpenSpeedShop_User_Manual_files/filelist.xml ; then \
		for file in OpenSpeedShop_User_Manual_files/*.htm ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir4}/$$basefile; \
		done ; \
		for file in OpenSpeedShop_User_Manual_files/*.png ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir4}/$$basefile; \
		done ; \
		for file in OpenSpeedShop_User_Manual_files/*.xml ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir4}/$$basefile; \
		done ; \
		for file in OpenSpeedShop_User_Manual_files/*.jpg ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir4}/$$basefile; \
		done ; \
	fi
	if test -f OpenSpeedShop_BuildInstallGuide_files/filelist.xml ; then \
		for file in OpenSpeedShop_BuildInstallGuide_files/*.htm ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir5}/$$basefile; \
		done ; \
		for file in OpenSpeedShop_BuildInstallGuide_files/*.xml ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir5}/$$basefile; \
		done ; \
	fi

uninstall-reference: index.html
	@$(NORMAL_UNINSTALL)
	@dir='${<D}'; \
	echo "dir =@dir"
	for file in $$dir/* ; do \
		basefile=`echo $$file | sed -e 's,^.*/,,'`; \
		rm -f ${DESTDIR}${docdir}/$$basefile; \
		rm -f ${DESTDIR}${docdir4}/$$basefile; \
		rm -f ${DESTDIR}${docdir5}/$$basefile; \
	done
	-rm -rf ${DESTDIR}${docdir}/
	-rm -rf ${DESTDIR}${docdir3}/
	-rm -rf ${DESTDIR}${docdir4}/
	-rm -rf ${DESTDIR}${docdir5}/

all-local: index.html

install-data-am: install-reference

uninstall: uninstall-reference
