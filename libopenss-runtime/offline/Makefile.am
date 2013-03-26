################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2008 William Hachfeld. All Rights Reserved.
# Copyright (c) 2006-2013 Krell Institute. All Rights Reserved.
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

noinst_LTLIBRARIES = libopenss-runtime-monitor.la

EXTRA_DIST = monlibs.h

BUILT_SOURCES = \
	offline.h offline.c


if HAVE_LIBMONITOR
libopenss_runtime_monitor_la_CFLAGS = \
	$(LIBMONITOR_CPPFLAGS) \
	-I$(top_srcdir)/libopenss-runtime
endif

if HAVE_TARGET_LIBMONITOR
libopenss_runtime_monitor_la_CFLAGS = \
	$(TARGET_LIBMONITOR_CPPFLAGS) \
	-I$(top_srcdir)/libopenss-runtime
endif

libopenss_runtime_monitor_la_SOURCES = \
	offline.x $(BUILT_SOURCES) \
	offline_monitor.c \
	openss_send.c \
	OpenSS_GetDLInfo.c \
	OpenSS_InitializeParameters.c \
	OpenSS_Monitor.h \
	OpenSS_Offline.h


SUFFIXES = .x
CLEANFILES = $(BUILT_SOURCES)

offline.h offline.c : offline.x
	rm -f  $(patsubst %.x, %.h, $<) $(patsubst %.x, %.c, $<)
	$(RPCGEN) -h -o $(patsubst %.x, %.h, $<) $<
	$(RPCGEN) -c -o $(patsubst %.x, %.c, $<) $<

install-exec-hook:
	test -d "${pkglibdir}" || mkdir -p "${pkglibdir}"
	ar x .libs/libopenss-runtime-monitor.a libopenss_runtime_monitor_la-offline_monitor.o
	cp libopenss_runtime_monitor_la-offline_monitor.o ${pkglibdir}/offline_monitor.o
uninstall-hook:
	rm -f ${pkglibdir}/offline_monitor.o
