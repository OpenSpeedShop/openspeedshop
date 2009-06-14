###############################################################################
# Copyright (c) 2009 Krell Institute. All Rights Reserved.
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
	OpenSpeedShop.3

package = @PACKAGE@
packver = @PACKAGE@-@VERSION@
mandir  = ${prefix}/share/man/man3

install-reference: all
	@$(NORMAL_INSTALL)
	${mkinstalldirs} ${DESTDIR}${mandir}
	if test -f OpenSpeedShop.3 ; then \
		for file in *.3 ; do \
			basefile=`echo $$file | sed -e 's,^.*/,,'`; \
			${INSTALL_DATA} $$file ${DESTDIR}${mandir}/$$basefile; \
		done ; \
	fi

uninstall-reference: openss.1
	@$(NORMAL_UNINSTALL)
	@dir='${<D}'; \
	echo "dir =@dir"
	for file in $$dir/* ; do \
		basefile=`echo $$file | sed -e 's,^.*/,,'`; \
		rm -f ${DESTDIR}${mandir}/$$basefile; \
	done
	rm -rf ${DESTDIR}${mandir}/

all-local: OpenSpeedShop.3

install-data-am: install-reference

uninstall: uninstall-reference
