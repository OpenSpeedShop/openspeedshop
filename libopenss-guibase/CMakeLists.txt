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

set(GUIBASE_SOURCES
	pc_plugin.cxx
	DragNDropPanel.hxx DragNDropPanel.cxx
	TopWidget.hxx TopWidget.cxx
	TabWidget.hxx TabWidget.cxx
	TabBarWidget.hxx TabBarWidget.cxx
	WhatsThis.hxx WhatsThis.cxx
	Frame.hxx Frame.cxx
	Panel.hxx Panel.cxx
	PanelContainer.hxx PanelContainer.cxx
	debug.hxx
	moc_TopWidget.cxx
	moc_TabWidget.cxx
	moc_TabBarWidget.cxx
	moc_Frame.cxx
	moc_Panel.cxx
	moc_PanelContainer.cxx
    )

add_library(openss-guibase SHARED
	${GUIBASE_SOURCES}
    )

add_definitions(
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_TopWidget.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_TopWidget.cxx ${CMAKE_CURRENT_SOURCE_DIR}/TopWidget.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/TopWidget.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/TopWidget.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_TabWidget.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_TabWidget.cxx ${CMAKE_CURRENT_SOURCE_DIR}/TabWidget.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/TabWidget.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/TabWidget.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_TabBarWidget.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_TabBarWidget.cxx ${CMAKE_CURRENT_SOURCE_DIR}/TabBarWidget.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/TabBarWidget.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/TabBarWidget.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_Frame.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_Frame.cxx ${CMAKE_CURRENT_SOURCE_DIR}/Frame.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/Frame.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/Frame.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_PanelContainer.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_PanelContainer.cxx ${CMAKE_CURRENT_SOURCE_DIR}/PanelContainer.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/PanelContainer.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/PanelContainer.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_Panel.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_Panel.cxx ${CMAKE_CURRENT_SOURCE_DIR}/Panel.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/Panel.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/Panel.hxx
    )

target_include_directories(openss-guibase PUBLIC
    ${QT_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/libopenss-guiobjects
    ${PROJECT_SOURCE_DIR}/libopenss-guiplugin
    ${PROJECT_SOURCE_DIR}/libopenss-guiimages
    ${PROJECT_SOURCE_DIR}/libopenss-gui
    ${PROJECT_SOURCE_DIR}/libopenss-guibase
    )

target_link_libraries(openss-guibase
    ${QT_LIBRARIES}
    ${CMAKE_DL_LIBS}
    -export-dynamic 
    )

set_target_properties(openss-guibase PROPERTIES VERSION 1.1.0)

install(TARGETS openss-guibase LIBRARY DESTINATION lib${LIB_SUFFIX}
    )

