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

pkglib_LTLIBRARIES = libopenss_Chart.la

libopenss_Chart_la_CXXFLAGS = \
	-I/usr/include \
	-I$(QTDIR)/include \
	-I. \
	-I../plugin \
	-I../base

libopenss_Chart_la_LDFLAGS = --export-dynamic -module -avoid-version

libopenss_Chart_la_LIBADD = $(LIBLTDL)

moc_canvasview.cxx: canvasview.hxx
	-moc -o moc_canvasview.cxx canvasview.hxx

moc_chartform.cxx: chartform.hxx
	-moc -o moc_chartform.cxx chartform.hxx

moc_optionsform.cxx: optionsform.hxx
	-moc -o moc_optionsform.cxx optionsform.hxx

moc_setdataform.cxx: setdataform.hxx
	-moc -o moc_setdataform.cxx setdataform.hxx

libopenss_Chart_la_SOURCES = \
	canvasellipse.hxx        canvastext.hxx  chartform.hxx  optionsform.hxx \
	canvasshadowellipse.hxx  canvasview.hxx  element.hxx    setdataform.hxx \
	canvasview.cxx        element.cxx          moc_setdataform.cxx \
	chartform_canvas.cxx  moc_canvasview.cxx   optionsform.cxx \
	chartform.cxx         moc_chartform.cxx    setdataform.cxx \
	chartform_files.cxx   moc_optionsform.cxx


CLEANFILES=moc_canvasview.cxx moc_chartform.cxx moc_optionsform.cxx moc_setdataform.cxx
