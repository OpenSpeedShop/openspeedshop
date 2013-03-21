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

lib_LTLIBRARIES = libopenss-guichart.la

libopenss_guichart_la_CXXFLAGS = \
	$(LTDLINCL) \
	$(QTLIB_CPPFLAGS) \
	-I/usr/include \
	-I. \
	-I$(top_srcdir)/plugin \
	-I$(top_srcdir)/libopenss-guibase

libopenss_guichart_la_LDFLAGS = \
	$(QTLIB_LDFLAGS) \
	-export-dynamic -version-info 0:0:0

libopenss_guichart_la_LIBADD = \
	$(LIBLTDL) \
	$(QTLIB_LIBS) \
	$(top_srcdir)/libopenss-guibase/libopenss-guibase.la

moc_canvasview.cxx: canvasview.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_canvasview.cxx canvasview.hxx

moc_chartform.cxx: chartform.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_chartform.cxx chartform.hxx

moc_optionsform.cxx: optionsform.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_optionsform.cxx optionsform.hxx

moc_setdataform.cxx: setdataform.hxx
	-$(QTLIB_HOME)/bin/moc -o moc_setdataform.cxx setdataform.hxx

libopenss_guichart_la_SOURCES = \
	canvasellipse.hxx        canvastext.hxx  chartform.hxx  optionsform.hxx \
	canvasrectangle.hxx \
	canvasshadowellipse.hxx  canvasview.hxx  element.hxx    setdataform.hxx \
	canvasview.cxx        element.cxx          \
	chartform_canvas.cxx  optionsform.cxx \
	chartform.cxx         setdataform.cxx \
	chartform_files.cxx

nodist_libopenss_guichart_la_SOURCES = \
	moc_setdataform.cxx \
	moc_canvasview.cxx \
	moc_chartform.cxx \
	moc_optionsform.cxx

# need to include images file in tarball
EXTRA_DIST = images/*.sk  images/*.xpm


CLEANFILES=moc_canvasview.cxx moc_chartform.cxx moc_optionsform.cxx moc_setdataform.cxx
