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

SUBDIRS = sources 

DIST_SUBDIRS = sources test openss

doc:
if HAVE_EPYDOC
# Generate expdoc documentation
# epydoc -o doc -t __init__.py -n openss __init__.py openss_*.py 
	rm -rf doc
	ln -sf $(OPENSS_PREFIX)/$(abi_libdir_name)/libopenss-cli.so.0.0.0 PYopenss.so; 
	epydoc -o doc  -n openss sources/openss*.py sources/__init__.py
# Preprocess out epytext and copy to package directory
	sed -f epytext.sed < sources/__init__.py > openss/__init__.py
	sed -f epytext.sed < sources/openss_classes.py > openss/openss_classes.py
	sed -f epytext.sed < sources/openss_commands.py > openss/openss_commands.py
	sed -f epytext.sed < sources/openss_parse.py > openss/openss_parse.py
endif

clean:
	rm -rf doc PYopenss.so

EXTRA_DIST = \
	doit.csh command_status.doc README test.py epytext.sed
