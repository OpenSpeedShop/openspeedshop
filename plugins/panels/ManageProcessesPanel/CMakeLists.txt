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

set(ManageProcessesPanel_SOURCES
	ManageCollectorsClass.hxx
	ManageCollectorsClass.cxx
	moc_ManageCollectorsClass.cxx
	ManageProcessesPanel.hxx
	ManageProcessesPanel.cxx
	moc_ManageProcessesPanel.cxx
	MPListViewItem.hxx
	MPListViewItem.cxx
	MPListView.hxx
	MPListView.cxx
	moc_MPListView.cxx
	local_plugin_info.hxx
	preference_plugin_info.hxx
    )

add_library(ManageProcessesPanel MODULE
	${ManageProcessesPanel_SOURCES}
    )

add_definitions(
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_ManageCollectorsClass.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_ManageCollectorsClass.cxx ${CMAKE_CURRENT_SOURCE_DIR}/ManageCollectorsClass.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/ManageCollectorsClass.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/ManageCollectorsClass.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_MPListView.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_MPListView.cxx ${CMAKE_CURRENT_SOURCE_DIR}/MPListView.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/MPListView.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/MPListView.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_ManageProcessesPanel.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_ManageProcessesPanel.cxx ${CMAKE_CURRENT_SOURCE_DIR}/ManageProcessesPanel.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/ManageProcessesPanel.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/ManageProcessesPanel.hxx
    )

target_include_directories(ManageProcessesPanel PUBLIC
    ${QT_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/libopenss-cli
    ${PROJECT_SOURCE_DIR}/libopenss-framework
    ${PROJECT_SOURCE_DIR}/libopenss-gui
    ${PROJECT_SOURCE_DIR}/libopenss-guibase
    ${PROJECT_SOURCE_DIR}/libopenss-guidialogs
    ${PROJECT_SOURCE_DIR}/libopenss-guiobjects
    ${PROJECT_SOURCE_DIR}/libopenss-guiplugin
    ${PROJECT_SOURCE_DIR}/libopenss-message
    ${PROJECT_SOURCE_DIR}/libopenss-queries
    ${PROJECT_SOURCE_DIR}/plugins/panels/CustomizeStatsPanel
    )

target_link_libraries(ManageProcessesPanel
    openss-framework
    openss-guibase
    openss-gui
    ${QT_LIBRARIES}
    ${CMAKE_DL_LIBS}
    )


set_target_properties(ManageProcessesPanel PROPERTIES PREFIX "")

install(TARGETS ManageProcessesPanel LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )

