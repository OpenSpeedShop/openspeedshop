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

set(StatsPanel_SOURCES
	SPChartForm.hxx
	SPChartForm.cxx
	moc_SPChartForm.cxx
	SPCheckBox.hxx
	SPCheckBox.cxx
	moc_SPCheckBox.cxx
	SPListViewItem.hxx
	SPListViewItem.cxx
	SPListView.hxx
	SPListView.cxx
	ChooseExperimentDialog.hxx
	ChooseExperimentDialog.cxx
	moc_ChooseExperimentDialog.cxx
	OptionalViewsDialog.hxx
	OptionalViewsDialog.cxx
	moc_OptionalViewsDialog.cxx
	SourcePanelAnnotationDialog.hxx
	SourcePanelAnnotationDialog.cxx
	moc_SourcePanelAnnotationDialog.cxx
	StatsPanel.hxx
	StatsPanel.cxx
	moc_StatsPanel.cxx
	SPTextEdit.hxx
	SPTextEdit.cxx
	moc_SPTextEdit.cxx
	local_plugin_info.hxx
	preference_plugin_info.hxx
    )

add_library(StatsPanel MODULE
	${StatsPanel_SOURCES}
    )

add_definitions(
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_SPChartForm.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_SPChartForm.cxx ${CMAKE_CURRENT_SOURCE_DIR}/SPChartForm.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/SPChartForm.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/SPChartForm.hxx
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
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_SPCheckBox.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_SPCheckBox.cxx ${CMAKE_CURRENT_SOURCE_DIR}/SPCheckBox.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/SPCheckBox.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/SPCheckBox.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_StatsPanel.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_StatsPanel.cxx ${CMAKE_CURRENT_SOURCE_DIR}/StatsPanel.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/StatsPanel.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/StatsPanel.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_OptionalViewsDialog.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_OptionalViewsDialog.cxx ${CMAKE_CURRENT_SOURCE_DIR}/OptionalViewsDialog.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/OptionalViewsDialog.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/OptionalViewsDialog.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_SourcePanelAnnotationDialog.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_SourcePanelAnnotationDialog.cxx ${CMAKE_CURRENT_SOURCE_DIR}/SourcePanelAnnotationDialog.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/SourcePanelAnnotationDialog.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/SourcePanelAnnotationDialog.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_ChooseExperimentDialog.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_ChooseExperimentDialog.cxx ${CMAKE_CURRENT_SOURCE_DIR}/ChooseExperimentDialog.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/ChooseExperimentDialog.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/ChooseExperimentDialog.hxx
    )

target_include_directories(StatsPanel PUBLIC
    ${QT_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/libopenss-cli
    ${PROJECT_SOURCE_DIR}/libopenss-framework
    ${PROJECT_SOURCE_DIR}/libopenss-gui
    ${PROJECT_SOURCE_DIR}/libopenss-guibase
    ${PROJECT_SOURCE_DIR}/libopenss-guichart
    ${PROJECT_SOURCE_DIR}/libopenss-guidialogs
    ${PROJECT_SOURCE_DIR}/libopenss-guiexprinfo
    ${PROJECT_SOURCE_DIR}/libopenss-guiimages
    ${PROJECT_SOURCE_DIR}/libopenss-guiobjects
    ${PROJECT_SOURCE_DIR}/libopenss-guiplugin
    ${PROJECT_SOURCE_DIR}/libopenss-message
    ${PROJECT_SOURCE_DIR}/libopenss-queries
    ${PROJECT_SOURCE_DIR}/plugins/panels/CustomExperimentPanel
    ${PROJECT_SOURCE_DIR}/plugins/panels/CustomExperimentPanel
    ${PROJECT_SOURCE_DIR}/plugins/panels/ManageProcessesPanel
    )

target_link_libraries(StatsPanel
    openss-gui
    openss-guibase
    openss-guichart
    openss-guidialogs
    openss-CustomExperimentPanel
    ${QT_LIBRARIES}
    ${CMAKE_DL_LIBS}
    )


set_target_properties(StatsPanel PROPERTIES PREFIX "")
set_target_properties(StatsPanel PROPERTIES
        COMPILE_DEFINITIONS "${Dyninst_DEFINES}")


install(TARGETS StatsPanel LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )
