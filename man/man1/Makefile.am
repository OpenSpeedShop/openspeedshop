###############################################################################
# Copyright (c) 2009-2010 Krell Institute. All Rights Reserved.
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
	openss.1\
	oss.1\
	osscompare.1\
	osshwc.1\
	osshwcsamp.1\
	osshwctime.1\
	ossio.1\
	ossiot.1\
	ossmpi.1\
	ossmpiotf.1\
	ossmpit.1\
	osspcsamp.1\
	ossusertime.1

package = @PACKAGE@
packver = @PACKAGE@-@VERSION@
mandir  = ${prefix}/share/man/man1

install-reference: all
	@$(NORMAL_INSTALL)
	${mkinstalldirs} ${DESTDIR}${mandir}
	if test -f openss.1 ; then \
		for file in *.1 ; do \
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
	-rm -rf ${DESTDIR}${mandir}/

all-local: openss.1

install-data-am: install-reference

uninstall: uninstall-reference
