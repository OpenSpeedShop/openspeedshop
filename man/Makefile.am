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
SUBDIRS = man1 man3

DIST_SUBDIRS = man1 man3

package = @PACKAGE@
packver = @PACKAGE@-@VERSION@
mandir  = ${prefix}/share/doc/man

install-reference: all
	@$(NORMAL_INSTALL)
	${mkinstalldirs} ${DESTDIR}${mandir}

uninstall-reference: 
	@$(NORMAL_UNINSTALL)
	rm -rf ${DESTDIR}${mandir}

install-data-am: install-reference

uninstall: uninstall-reference





