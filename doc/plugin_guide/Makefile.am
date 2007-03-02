###############################################################################
# Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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

EXTRA_DIST	=  \
	index.html \
	OpenSpeedShop_Logo.jpg \
	ToolClientOverView.png \
	ToolOverView.dia \
	ToolOverView.png \
	ToolServerOverView5.dia \
	ToolServerOverView5.png \
	ToolServerOverView6.png \
	ToolServerOverView.png \
	ToolStartCollectingView.dia \
	ToolStartCollectingView.png

package = @PACKAGE@
packver = @PACKAGE@-@VERSION@
docdir  = ${prefix}/share/doc/packages/OpenSpeedShop/plugin_guide
docdir3  = ${prefix}/share/doc/packages/OpenSpeedShop

install-reference: index.html
	@$(NORMAL_INSTALL)
	${mkinstalldirs} ${DESTDIR}${docdir}
	if test -f index.html ; then \
		for file in *.html ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir}/$$basefile; \
		done ; \
		for file in *.dia ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir}/$$basefile; \
		done ; \
		for file in *.png ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${docdir}/$$basefile; \
		done ; \
		${INSTALL_DATA} OpenSpeedShop_Logo.jpg ${DESTDIR}${docdir} ;\
	fi

uninstall-reference: index.html
	@$(NORMAL_UNINSTALL)
	@dir='${<D}'; \
	echo "dir =@dir"
	for file in $$dir/* ; do \
		basefile=`echo $$file | sed -e 's,^.*/,,'`; \
		rm -f ${DESTDIR}${docdir}/$$basefile; \
	done
	-rm -rf ${DESTDIR}${docdir}/

all-local: index.html

install-data-am: install-reference

uninstall: uninstall-reference
