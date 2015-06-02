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

set(SourcePanel_SOURCES
	SourcePanel.hxx
	SourcePanel.cxx
	SPCanvasForm.hxx
	SPCanvasForm.cxx
	SPCanvasView.hxx
	SPCanvasView.cxx
	SPTextEdit.hxx
	SPTextEdit.cxx
	local_plugin_info.hxx
	preference_plugin_info.hxx
	moc_SourcePanel.cxx
	moc_SPCanvasForm.cxx
	moc_SPCanvasView.cxx
	moc_SPTextEdit.cxx
    )

add_library(SourcePanel MODULE
	${SourcePanel_SOURCES}
    )

add_definitions(
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_SPCanvasView.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_SPCanvasView.cxx ${CMAKE_CURRENT_SOURCE_DIR}/SPCanvasView.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/SPCanvasView.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/SPCanvasView.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_SPTextEdit.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_SPTextEdit.cxx ${CMAKE_CURRENT_SOURCE_DIR}/SPTextEdit.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/SPTextEdit.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/SPTextEdit.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_SPCanvasForm.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_SPCanvasForm.cxx ${CMAKE_CURRENT_SOURCE_DIR}/SPCanvasForm.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/SPCanvasForm.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/SPCanvasForm.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_SourcePanel.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_SourcePanel.cxx ${CMAKE_CURRENT_SOURCE_DIR}/SourcePanel.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/SourcePanel.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/SourcePanel.hxx
    )

target_include_directories(SourcePanel PUBLIC
    ${QT_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/libopenss-cli
    ${PROJECT_SOURCE_DIR}/libopenss-framework
    ${PROJECT_SOURCE_DIR}/libopenss-gui
    ${PROJECT_SOURCE_DIR}/libopenss-guibase
    ${PROJECT_SOURCE_DIR}/libopenss-guiimages
    ${PROJECT_SOURCE_DIR}/libopenss-guiobjects
    ${PROJECT_SOURCE_DIR}/libopenss-guiplugin
    ${PROJECT_SOURCE_DIR}/libopenss-message
    ${PROJECT_SOURCE_DIR}/libopenss-queries
    )

target_link_libraries(SourcePanel
    openss-framework
    openss-guibase
    openss-guiobjects
    openss-cli
    ${QT_LIBRARIES}
    ${CMAKE_DL_LIBS}
    )


set_target_properties(SourcePanel PROPERTIES PREFIX "")

install(TARGETS SourcePanel LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )
