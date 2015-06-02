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

set(CompareExperimentsPanel_SOURCES
	CompareExperimentsPanel.hxx
	CompareExperimentsPanel.cxx
	local_plugin_info.hxx
	moc_CompareExperimentsPanel.cxx
    )

add_library(CompareExperimentsPanel MODULE
	${CompareExperimentsPanel_SOURCES}
    )

add_definitions(
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_CompareExperimentsPanel.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_CompareExperimentsPanel.cxx ${CMAKE_CURRENT_SOURCE_DIR}/CompareExperimentsPanel.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/CompareExperimentsPanel.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/CompareExperimentsPanel.hxx
    )

target_include_directories(CompareExperimentsPanel PUBLIC
    ${QT_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/libopenss-cli
    ${PROJECT_SOURCE_DIR}/libopenss-framework
    ${PROJECT_SOURCE_DIR}/libopenss-gui
    ${PROJECT_SOURCE_DIR}/libopenss-guibase
    ${PROJECT_SOURCE_DIR}/libopenss-guidialogs
    ${PROJECT_SOURCE_DIR}/libopenss-guiimages
    ${PROJECT_SOURCE_DIR}/libopenss-guiplugin
    ${PROJECT_SOURCE_DIR}/libopenss-guiobjects
    ${PROJECT_SOURCE_DIR}/libopenss-message
    ${PROJECT_SOURCE_DIR}/libopenss-queries
    ${PROJECT_SOURCE_DIR}/plugins/panels/CustomExperimentPanel
    ${PROJECT_SOURCE_DIR}/plugins/panels/ManageProcessPanel
    ${PROJECT_SOURCE_DIR}/plugins/panels/SourcePanel
    )

target_link_libraries(CompareExperimentsPanel
    openss-guibase
    openss-CustomExperimentPanel
    ${QT_LIBRARIES}
    ${CMAKE_DL_LIBS}
    )


set_target_properties(CompareExperimentsPanel PROPERTIES PREFIX "")

install(TARGETS CompareExperimentsPanel LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )

