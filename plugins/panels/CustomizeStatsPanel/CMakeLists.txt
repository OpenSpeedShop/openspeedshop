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

set(CustomizeStatsPanel_SOURCES
	ColumnSet.hxx
	ColumnSet.cxx
	moc_ColumnSet.cxx
	CompareProcessesDialog.hxx
	CompareProcessesDialog.cxx
	moc_CompareProcessesDialog.cxx
	CompareSet.hxx
	CompareSet.cxx
	moc_CompareSet.cxx
	CustomizeClass.hxx
	CustomizeClass.cxx
	moc_CustomizeClass.cxx
	CustomizeStatsPanel.hxx
	CustomizeStatsPanel.cxx
	moc_CustomizeStatsPanel.cxx
	${PROJECT_SOURCE_DIR}/plugins/panels/ManageProcessesPanel/MPListViewItem.hxx
	${PROJECT_SOURCE_DIR}/plugins/panels/ManageProcessesPanel/MPListViewItem.cxx
	${PROJECT_SOURCE_DIR}/plugins/panels/ManageProcessesPanel/MPListView.hxx
	${PROJECT_SOURCE_DIR}/plugins/panels/ManageProcessesPanel/MPListView.cxx
	moc_MPListView.cxx
	local_plugin_info.hxx
    )

add_library(CustomizeStatsPanel MODULE
	${CustomizeStatsPanel_SOURCES}
    )

add_definitions(
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )


add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_MPListView.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_MPListView.cxx ${PROJECT_SOURCE_DIR}/plugins/panels/ManageProcessesPanel/MPListView.hxx
    DEPENDS
        ${PROJECT_SOURCE_DIR}/plugins/panels/ManageProcessesPanel/MPListView.cxx
        ${PROJECT_SOURCE_DIR}/plugins/panels/ManageProcessesPanel/MPListView.hxx
    )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_CustomizeStatsPanel.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_CustomizeStatsPanel.cxx ${CMAKE_CURRENT_SOURCE_DIR}/CustomizeStatsPanel.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/CustomizeStatsPanel.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/CustomizeStatsPanel.hxx
    )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_CustomizeClass.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_CustomizeClass.cxx ${CMAKE_CURRENT_SOURCE_DIR}/CustomizeClass.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/CustomizeClass.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/CustomizeClass.hxx
    )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_ColumnSet.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_ColumnSet.cxx ${CMAKE_CURRENT_SOURCE_DIR}/ColumnSet.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/ColumnSet.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/ColumnSet.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_CompareProcessesDialog.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_CompareProcessesDialog.cxx ${CMAKE_CURRENT_SOURCE_DIR}/CompareProcessesDialog.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/CompareProcessesDialog.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/CompareProcessesDialog.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_CompareSet.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_CompareSet.cxx ${CMAKE_CURRENT_SOURCE_DIR}/CompareSet.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/CompareSet.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/CompareSet.hxx
    )

target_include_directories(CustomizeStatsPanel PUBLIC
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
    ${PROJECT_SOURCE_DIR}/libopenss-guiobjects
    ${PROJECT_SOURCE_DIR}/libopenss-guiplugin
    ${PROJECT_SOURCE_DIR}/libopenss-message
    ${PROJECT_SOURCE_DIR}/libopenss-queries
    ${PROJECT_SOURCE_DIR}/plugins/panels/ManageProcessesPanel
    )

target_link_libraries(CustomizeStatsPanel
    openss-framework
    openss-guibase
    openss-guiobjects
    openss-gui
    ${QT_LIBRARIES}
    ${CMAKE_DL_LIBS}
    )


set_target_properties(CustomizeStatsPanel PROPERTIES PREFIX "")

install(TARGETS CustomizeStatsPanel LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )

