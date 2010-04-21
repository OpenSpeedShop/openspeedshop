###############################################################################
# Copyright (c) 2010 Krell Institute. All Rights Reserved.
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

# directories that will be built

SUBDIRS = openss sources 

DIST_SUBDIRS = sources test openss

docdir  = ${prefix}/share/doc/packages/OpenSpeedShop/pyscriptingAPI
docdir_private  = ${prefix}/share/doc/packages/OpenSpeedShop/pyscriptingAPI/private
docdir_public  = ${prefix}/share/doc/packages/OpenSpeedShop/pyscriptingAPI/public

doc:
if HAVE_EPYDOC
# Generate expdoc documentation
# epydoc -o doc -t __init__.py -n openss __init__.py openss_*.py 
	rm -rf doc
	ln -sf $(top_srcdir)/libopenss-cli/.libs/libopenss-cli.so.0.0.0 PYopenss.so; 
# Preprocess out epytext and copy to package directory
	sed -f epytext.sed < sources/__init__.py > openss/__init__.py
	sed -f epytext.sed < sources/openss_classes.py > openss/openss_classes.py
	sed -f epytext.sed < sources/openss_commands.py > openss/openss_commands.py
	sed -f epytext.sed < sources/openss_parse.py > openss/openss_parse.py
	epydoc -o doc  -n openss sources/openss*.py sources/__init__.py

package = @PACKAGE@
packver = @PACKAGE@-@VERSION@

	epydoc -o doc  -n openss sources/openss*.py sources/__init__.py

	@$(NORMAL_INSTALL)
	${mkinstalldirs} ${DESTDIR}${docdir}
	${mkinstalldirs} ${DESTDIR}${docdir_private}
	${mkinstalldirs} ${DESTDIR}${docdir_public}
	if test -f doc/index.html ; then \
	        for file in doc/*.css ; do \
	                basefile=`echo $$file | sed -e 's,^.*/,,'`; \
	                ${INSTALL_DATA} $$file ${DESTDIR}${docdir}/$$basefile; \
	        done ; \
	        for file in doc/*.html ; do \
	                basefile=`echo $$file | sed -e 's,^.*/,,'`; \
	                ${INSTALL_DATA} $$file ${DESTDIR}${docdir}/$$basefile; \
	        done ; \
	        if ! test -f doc/private/index.html ; then \
	          for file in doc/*.png ; do \
	                basefile=`echo $$file | sed -e 's,^.*/,,'`; \
	                ${INSTALL_DATA} $$file ${DESTDIR}${docdir}/$$basefile; \
	          done ; \
	          for file in doc/*.js ; do \
	                basefile=`echo $$file | sed -e 's,^.*/,,'`; \
	                ${INSTALL_DATA} $$file ${DESTDIR}${docdir}/$$basefile; \
	          done ; \
                fi ; \
	        if test -f doc/private/index.html ; then \
	          for file in doc/private/*.html ; do \
	                basefile=`echo $$file | sed -e 's,^.*/,,'`; \
	                ${INSTALL_DATA} $$file ${DESTDIR}${docdir_private}/$$basefile; \
	          done ; \
	          for file in doc/private/*.css ; do \
	                basefile=`echo $$file | sed -e 's,^.*/,,'`; \
	                ${INSTALL_DATA} $$file ${DESTDIR}${docdir_private}/$$basefile; \
	          done ; \
                fi ; \
	        if test -f doc/public/index.html ; then \
	          for file in doc/public/*.html ; do \
	                basefile=`echo $$file | sed -e 's,^.*/,,'`; \
	                ${INSTALL_DATA} $$file ${DESTDIR}${docdir_public}/$$basefile; \
	          done ; \
	          for file in doc/public/*.css ; do \
	                basefile=`echo $$file | sed -e 's,^.*/,,'`; \
	                ${INSTALL_DATA} $$file ${DESTDIR}${docdir_public}/$$basefile; \
	          done ; \
	        fi ; \
	fi

uninstall-local: 
	@$(NORMAL_UNINSTALL)
	@dir='${<D}'; \
	echo "dir =@dir"
	for file in $$dir/* ; do \
	        basefile=`echo $$file | sed -e 's,^.*/,,'`; \
	        rm -f ${DESTDIR}${docdir}/$$basefile; \
	done
	-rm -rf ${DESTDIR}${docdir}/


uninstall: uninstall-local

endif

clean:
	rm -rf doc PYopenss.so

EXTRA_DIST = \
	doit.csh command_status.doc README test.py epytext.sed
