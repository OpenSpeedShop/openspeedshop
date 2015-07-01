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

set(GUIPLUGIN_SOURCES
        ${PROJECT_SOURCE_DIR}/libopenss-gui/openspeedshop.hxx
	plugin_handler.hxx
        plugin_handler.cxx
	PluginInfo.hxx
        PluginInfo.cxx
	SlotInfo.hxx
        SlotInfo.cxx
	plugin_entry_point.hxx
	moc_SlotInfo.cxx
    )

add_library(openss-guiplugin SHARED
	${GUIPLUGIN_SOURCES}
    )

add_definitions(
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_SlotInfo.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_SlotInfo.cxx ${CMAKE_CURRENT_SOURCE_DIR}/SlotInfo.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/SlotInfo.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/SlotInfo.hxx
    )

target_include_directories(openss-guiplugin PUBLIC
    ${QT_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/libopenss-guibase
    ${PROJECT_SOURCE_DIR}/libopenss-gui
    ${LTDL_INCLUDE_DIR}
    )

target_link_libraries(openss-guiplugin
    openss-guibase
    ${QT_LIBRARIES}
    ${LTDL_SHARED_LIBRARY}
    ${CMAKE_DL_LIBS}
    )

set_target_properties(openss-guiplugin PROPERTIES VERSION 1.1.0)

install(TARGETS openss-guiplugin LIBRARY DESTINATION lib${LIB_SUFFIX}
    )
