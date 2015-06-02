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

set(DebugPanel_SOURCES
	DebugPanel.hxx
	DebugPanel.cxx
	local_plugin_info.hxx
	moc_DebugPanel.cxx
    )

add_library(DebugPanel MODULE
	${DebugPanel_SOURCES}
    )

add_definitions(
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_DebugPanel.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_DebugPanel.cxx ${CMAKE_CURRENT_SOURCE_DIR}/DebugPanel.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/DebugPanel.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/DebugPanel.hxx
    )

target_include_directories(DebugPanel PUBLIC
    ${QT_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/libopenss-gui
    ${PROJECT_SOURCE_DIR}/libopenss-guibase
    ${PROJECT_SOURCE_DIR}/libopenss-guiplugin
    )

target_link_libraries(DebugPanel
    openss-guibase
    ${QT_LIBRARIES}
    ${CMAKE_DL_LIBS}
    )


set_target_properties(DebugPanel PROPERTIES PREFIX "")

install(TARGETS DebugPanel LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )

