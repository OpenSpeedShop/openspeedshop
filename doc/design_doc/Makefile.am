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
#SUBDIRS = CLI Framework Runtime Collectors GUI Message Queries

DIST_SUBDIRS = CLI Framework Runtime Collectors GUI Message Queries

EXTRA_DIST = GUI_overview.html \
	Design.png \
	index.html \
	Makefile.am \
	OpenSpeedShop_Logo.jpg 


package = @PACKAGE@
packver = @PACKAGE@-@VERSION@
docdir1  = ${prefix}/share/doc/packages/OpenSpeedShop/design_doc
docdir2  = ${prefix}/share/doc/packages/OpenSpeedShop/design_doc/CLI
docdir3  = ${prefix}/share/doc/packages/OpenSpeedShop/design_doc/Framework
docdir4  = ${prefix}/share/doc/packages/OpenSpeedShop/design_doc/Runtime
docdir5  = ${prefix}/share/doc/packages/OpenSpeedShop/design_doc/Collectors
docdir6  = ${prefix}/share/doc/packages/OpenSpeedShop/design_doc/GUI
docdir7  = ${prefix}/share/doc/packages/OpenSpeedShop/design_doc/Message
docdir8  = ${prefix}/share/doc/packages/OpenSpeedShop/design_doc/Queries

CLEANDIRS = CLI/html  Collectors/html  Framework/html  GUI/html \
	     Message/html  Queries/html  Runtime/html

install-reference: index.html
	@$(NORMAL_INSTALL)
	${mkinstalldirs} ${DESTDIR}${docdir1}
	${mkinstalldirs} ${DESTDIR}${docdir2}
	${mkinstalldirs} ${DESTDIR}${docdir3}
	${mkinstalldirs} ${DESTDIR}${docdir4}
	${mkinstalldirs} ${DESTDIR}${docdir5}
	${mkinstalldirs} ${DESTDIR}${docdir6}
	${mkinstalldirs} ${DESTDIR}${docdir7}
	${mkinstalldirs} ${DESTDIR}${docdir8}
	if test -f index.html ; then \
		for file in *.html ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir1}/$$basefile; \
		done ; \
		for file in *.png ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir1}/$$basefile; \
		done ; \
		${INSTALL_DATA} OpenSpeedShop_Logo.jpg ${DESTDIR}${docdir1} ;\
		${INSTALL_DATA} Design.png ${DESTDIR}${docdir1} ;\
	fi
	if test -f CLI/html/index.html ; then \
		for file in CLI/html/*.html ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir2}/$$basefile; \
		done ; \
		for file in CLI/html/*.png ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir2}/$$basefile; \
		done ; \
		for file in CLI/html/*.css ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir2}/$$basefile; \
		done ; \
	        if test -f CLI/html/*.gif ; then \
	  	   for file in CLI/html/*.gif ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir2}/$$basefile; \
		   done ; \
               fi \
	fi
	if test -f Framework/html/index.html ; then \
		for file in Framework/html/*.html ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir3}/$$basefile; \
		done ; \
		for file in Framework/html/*.png ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir3}/$$basefile; \
		done ; \
		for file in Framework/html/*.css ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir3}/$$basefile; \
		done ; \
	        if test -f Framework/html/*.gif ; then \
		   for file in Framework/html/*.gif ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir3}/$$basefile; \
		   done ; \
               fi \
	fi
	if test -f Runtime/html/index.html ; then \
		for file in Runtime/html/*.html ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir4}/$$basefile; \
		done ; \
		for file in Runtime/html/*.png ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir4}/$$basefile; \
		done ; \
		for file in Runtime/html/*.css ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir4}/$$basefile; \
		done ; \
	        if test -f Runtime/html/*.gif ; then \
		   for file in Runtime/html/*.gif ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir4}/$$basefile; \
		   done ; \
               fi \
	fi
	if test -f Collectors/html/index.html ; then \
		for file in Collectors/html/*.html ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir5}/$$basefile; \
		done ; \
		for file in Collectors/html/*.png ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir5}/$$basefile; \
		done ; \
		for file in Collectors/html/*.css ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir5}/$$basefile; \
		done ; \
	        if test -f Collectors/html/*.gif ; then \
		   for file in Collectors/html/*.gif ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir5}/$$basefile; \
		   done ; \
               fi \
	fi
	if test -f GUI/html/index.html ; then \
		for file in GUI/html/*.html ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir6}/$$basefile; \
		done ; \
		for file in GUI/html/*.png ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir6}/$$basefile; \
		done ; \
		for file in GUI/html/*.css ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir6}/$$basefile; \
		done ; \
	        if test -f GUI/html/*.gif ; then \
		   for file in GUI/html/*.gif ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir6}/$$basefile; \
		   done ; \
               fi \
	fi
	if test -f Message/html/index.html ; then \
		for file in Message/html/*.html ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir7}/$$basefile; \
		done ; \
		for file in Message/html/*.png ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir7}/$$basefile; \
		done ; \
		for file in Message/html/*.css ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir7}/$$basefile; \
		done ; \
	        if test -f Message/html/*.gif ; then \
		   for file in Message/html/*.gif ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir7}/$$basefile; \
		   done ; \
               fi \
	fi
	if test -f Queries/html/index.html ; then \
		for file in Queries/html/*.html ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir8}/$$basefile; \
		done ; \
		for file in Queries/html/*.png ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir8}/$$basefile; \
		done ; \
		for file in Queries/html/*.css ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir8}/$$basefile; \
		done ; \
	        if test -f Queries/html/*.gif ; then \
		   for file in Queries/html/*.gif ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir8}/$$basefile; \
		   done ; \
               fi \
	fi

uninstall-reference: index.html
	@$(NORMAL_UNINSTALL)
	@dir='${<D}'; \
	echo "dir =@dir"
	for file in $$dir/* ; do \
		basefile=`echo $$file | sed -e 's,^.*/,,'`; \
		rm -f ${DESTDIR}${docdir}/$$basefile; \
		rm -f ${DESTDIR}${docdir2}/$$basefile; \
	done
	-rm -rf ${DESTDIR}${docdir}/
	-rm -rf ${DESTDIR}${docdir2}/
	-rm -rf ${DESTDIR}${docdir3}/
	-rm -rf ${DESTDIR}${docdir4}/
	-rm -rf ${DESTDIR}${docdir5}/
	-rm -rf ${DESTDIR}${docdir6}/
	-rm -rf ${DESTDIR}${docdir7}/
	-rm -rf ${DESTDIR}${docdir8}/

all-local: index.html

install-data-am: install-reference

uninstall: uninstall-reference

clean-local: clean-local-check
.PHONY: clean-local-check
clean-local-check:
	-rm -rf ${CLEANDIRS}
