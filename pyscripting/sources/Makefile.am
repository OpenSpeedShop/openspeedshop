################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2006-2012 Krell Institute  All Rights Reserved.
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 2.1 of the License, or (at your option)
# any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
################################################################################

BUILT_SOURCES = \
	__init__.py \
	 openss.py \
	 openss_classes.py \
	 openss_commands.py \
	 openss_parse.py

programfilesdir = $(pkglibdir)

dist_programfiles_SCRIPTS = \
	__init__.py  openss.py openss_classes.py  openss_commands.py  openss_parse.py

doc:	__init__.py openss.py openss_classes.py openss_commands.py openss_parse.py
	ln -sf $(top_srcdir)/libopenss-cli/.libs/libopenss-cli.so.0.0.0 PYopenss.so;

__init__.py: __init__.py.in
	@rm -f __init__.py
	@cat \
		$(srcdir)/__init__.py.in > __init__.py
	@chmod 755 __init__.py

openss.py: openss.py.in
	@rm -f openss.py
	@cat \
		$(srcdir)/openss.py.in > openss.py
	@chmod 755 openss.py

openss_classes.py: openss_classes.py.in
	@rm -f openss_classes.py
	@cat \
		$(srcdir)/openss_classes.py.in > openss_classes.py
	@chmod 755 openss_classes.py

openss_commands.py: openss_commands.py.in
	@rm -f openss_commands.py
	@cat \
		$(srcdir)/openss_commands.py.in > openss_commands.py
	@chmod 755 openss_commands.py

openss_parse.py: openss_parse.py.in
	@rm -f openss_parse.py
	@cat \
		$(srcdir)/openss_parse.py.in > openss_parse.py
	@chmod 755 openss_parse.py

clean:
	rm -rf *.py *.pyc PYopenss.so

EXTRA_DIST = __init__.py.in openss.py.in openss_classes.py.in openss_commands.py.in openss_parse.py.in
