################################################################################
# Copyright (c) 2014-2015 Krell Institute. All Rights Reserved.
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

include(CheckIncludeFile)

set(GUICHART_SOURCES
	canvasellipse.hxx
        canvastext.hxx
        chartform.hxx
        optionsform.hxx
	canvasrectangle.hxx
	canvasshadowellipse.hxx
        canvasview.hxx
        element.hxx
        setdataform.hxx
	canvasview.cxx
        element.cxx
	chartform_canvas.cxx
        optionsform.cxx
	chartform.cxx
        setdataform.cxx
	chartform_files.cxx
	moc_setdataform.cxx
	moc_canvasview.cxx
	moc_chartform.cxx
	moc_optionsform.cxx
    )

add_library(openss-guichart SHARED
	${GUICHART_SOURCES}
    )

add_definitions(
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_canvasview.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_canvasview.cxx ${CMAKE_CURRENT_SOURCE_DIR}/canvasview.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/canvasview.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/canvasview.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_chartform.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_chartform.cxx ${CMAKE_CURRENT_SOURCE_DIR}/chartform.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/chartform.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/chartform.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_optionsform.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_optionsform.cxx ${CMAKE_CURRENT_SOURCE_DIR}/optionsform.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/optionsform.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/optionsform.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_setdataform.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_setdataform.cxx ${CMAKE_CURRENT_SOURCE_DIR}/setdataform.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/setdataform.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/setdataform.hxx
    )

target_include_directories(openss-guichart PUBLIC
    ${QT_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/plugin
    ${PROJECT_SOURCE_DIR}/libopenss-guibase
    )

target_link_libraries(openss-guichart
    ${QT_LIBRARIES}
    ${CMAKE_DL_LIBS}
    -export-dynamic 
    )

set_target_properties(openss-guichart PROPERTIES VERSION 1.1.0)

install(TARGETS openss-guichart LIBRARY DESTINATION lib${LIB_SUFFIX}
    )

