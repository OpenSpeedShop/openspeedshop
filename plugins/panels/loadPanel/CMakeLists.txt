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

set(loadPanel_SOURCES
	loadPanel.hxx
	loadPanel.cxx
	local_plugin_info.hxx
	moc_loadPanel.cxx
    )

add_library(loadPanel MODULE
	${loadPanel_SOURCES}
    )

add_definitions(
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_loadPanel.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_loadPanel.cxx ${CMAKE_CURRENT_SOURCE_DIR}/loadPanel.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/loadPanel.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/loadPanel.hxx
    )

target_include_directories(loadPanel PUBLIC
    ${QT_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/libopenss-gui
    ${PROJECT_SOURCE_DIR}/libopenss-guibase
    ${PROJECT_SOURCE_DIR}/libopenss-guichart
    ${PROJECT_SOURCE_DIR}/libopenss-guiimages
    ${PROJECT_SOURCE_DIR}/libopenss-guiplugin
    ${PROJECT_SOURCE_DIR}/libopenss-guiobjects
    )

target_link_libraries(loadPanel
    openss-gui
    openss-guibase
    openss-guiobjects
    ${QT_LIBRARIES}
    ${CMAKE_DL_LIBS}
    )


set_target_properties(loadPanel PROPERTIES PREFIX "")

install(TARGETS loadPanel LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )

