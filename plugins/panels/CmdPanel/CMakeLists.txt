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

set(CmdPanel_SOURCES
	CmdPanel.hxx
	CmdPanel.cxx
	CPTextEdit.hxx
	CPTextEdit.cxx
	local_plugin_info.hxx
	KeyEventFilter.cxx
	moc_CmdPanel.cxx
	moc_CPTextEdit.cxx
    )

add_library(CmdPanel MODULE
	${CmdPanel_SOURCES}
    )

add_definitions(
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_CPTextEdit.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_CPTextEdit.cxx ${CMAKE_CURRENT_SOURCE_DIR}/CPTextEdit.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/CPTextEdit.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/CPTextEdit.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_CmdPanel.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_CmdPanel.cxx ${CMAKE_CURRENT_SOURCE_DIR}/CmdPanel.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/CmdPanel.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/CmdPanel.hxx
    )

target_include_directories(CmdPanel PUBLIC
    ${QT_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/libopenss-cli
    ${PROJECT_SOURCE_DIR}/libopenss-framework
    ${PROJECT_SOURCE_DIR}/libopenss-message
    ${PROJECT_SOURCE_DIR}/libopenss-queries
    ${PROJECT_SOURCE_DIR}/libopenss-gui
    ${PROJECT_SOURCE_DIR}/libopenss-guibase
    ${PROJECT_SOURCE_DIR}/libopenss-guiobjects
    ${PROJECT_SOURCE_DIR}/libopenss-guiplugin
    )

target_link_libraries(CmdPanel
    openss-guibase
    openss-cli
    openss-framework
    openss-queries
    openss-message
    openss-guiobjects
    openss-guiplugin
    ${PYTHON_LIBRARIES}
    ${QT_LIBRARIES}
    ${CMAKE_DL_LIBS}
    )


set_target_properties(CmdPanel PROPERTIES PREFIX "")

install(TARGETS CmdPanel LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )

