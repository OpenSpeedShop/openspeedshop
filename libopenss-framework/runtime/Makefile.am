################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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

SUBDIRS = @INSTRUMENTOR@

noinst_LTLIBRARIES = libopenss-framework-runtime.la

libopenss_framework_runtime_la_LIBADD = \
	@INSTRUMENTOR@/libopenss-framework-runtime-@INSTRUMENTOR@.la

libopenss_framework_runtime_la_SOURCES = \
	OpenSS_DataHeader.h OpenSS_DataHeader.c OpenSS_DataHeader.x \
	OpenSS_DecodeParameters.c \
	OpenSS_Send.c \
	RuntimeAPI.h

SUFFIXES = .x
CLEANFILES = OpenSS_DataHeader.h OpenSS_DataHeader.c

OpenSS_DataHeader.h OpenSS_DataHeader.c : OpenSS_DataHeader.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<
