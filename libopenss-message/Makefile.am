################################################################################
# Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
# Copyright (c) 2007-2010 Krell Institute  All Rights Reserved.
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

lib_LTLIBRARIES = libopenss-message.la

libopenss_message_la_CXXFLAGS = \
	-I$(top_srcdir)/libopenss-guibase \
	-I$(top_srcdir)/libopenss-framework \
	-I$(top_srcdir)/libopenss-guiobjects \
	-DPLUGIN_PATH=\"$(pkglibdir)\" \
	$(LTDLINCL) \
	$(PYTHON_CPPFLAGS)

libopenss_message_la_LDFLAGS = \
	-export-dynamic -version-info 0:0:0

libopenss_message_la_LIBADD = \
	$(LIBLTDL) \
	-lutil

libopenss_message_la_SOURCES = \
	SS_Message_Element.cxx \
	SS_Message_Element.hxx \
	SS_Message_Czar.cxx SS_Message_Czar.hxx 

doc:
	-doxygen Doxyfile

