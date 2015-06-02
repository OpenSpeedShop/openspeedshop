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

set(GUIDIALOGS_SOURCES
	AttachArgDialog.hxx 
	AttachArgDialog.cxx
	AttachProcessDialog.hxx 
	AttachProcessDialog.cxx
	AttachMPProcessDialog.hxx
	AttachMPProcessDialog.cxx
	AttachProgramDialog.hxx
	AttachProgramDialog.cxx
	AttachMPProgramDialog.hxx
	AttachMPProgramDialog.cxx
	ManageProcessesDialog.hxx
	ManageProcessesDialog.cxx
	PanelListViewItem.hxx
	PanelListViewItem.cxx
	GenericProgressDialog.hxx
	GenericProgressDialog.cxx
	SelectTimeSegmentDialog.hxx
	SelectTimeSegmentDialog.cxx
	SelectExperimentDialog.hxx
	SelectExperimentDialog.cxx
	SaveAsExperimentDialog.hxx
	SaveAsExperimentDialog.cxx
	AboutDialog.hxx AboutDialog.cxx
	moc_AttachArgDialog.cxx
	moc_AttachProcessDialog.cxx
	moc_AttachMPProcessDialog.cxx
	moc_AttachProgramDialog.cxx
	moc_AttachMPProgramDialog.cxx
	moc_ManageProcessesDialog.cxx
	moc_GenericProgressDialog.cxx
	moc_SelectTimeSegmentDialog.cxx
	moc_SelectExperimentDialog.cxx
	moc_SaveAsExperimentDialog.cxx

    )

add_library(openss-guidialogs SHARED
	${GUIDIALOGS_SOURCES}
    )

add_definitions(
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_AttachArgDialog.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_AttachArgDialog.cxx ${CMAKE_CURRENT_SOURCE_DIR}/AttachArgDialog.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/AttachArgDialog.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/AttachArgDialog.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_AttachProcessDialog.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_AttachProcessDialog.cxx ${CMAKE_CURRENT_SOURCE_DIR}/AttachProcessDialog.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/AttachProcessDialog.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/AttachProcessDialog.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_AttachMPProcessDialog.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_AttachMPProcessDialog.cxx ${CMAKE_CURRENT_SOURCE_DIR}/AttachMPProcessDialog.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/AttachMPProcessDialog.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/AttachMPProcessDialog.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_AttachProgramDialog.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_AttachProgramDialog.cxx ${CMAKE_CURRENT_SOURCE_DIR}/AttachProgramDialog.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/AttachProgramDialog.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/AttachProgramDialog.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_AttachMPProgramDialog.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_AttachMPProgramDialog.cxx ${CMAKE_CURRENT_SOURCE_DIR}/AttachMPProgramDialog.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/AttachMPProgramDialog.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/AttachMPProgramDialog.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_ManageProcessesDialog.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_ManageProcessesDialog.cxx ${CMAKE_CURRENT_SOURCE_DIR}/ManageProcessesDialog.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/ManageProcessesDialog.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/ManageProcessesDialog.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_SelectExperimentDialog.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_SelectExperimentDialog.cxx ${CMAKE_CURRENT_SOURCE_DIR}/SelectExperimentDialog.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/SelectExperimentDialog.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/SelectExperimentDialog.hxx
    )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_SaveAsExperimentDialog.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_SaveAsExperimentDialog.cxx ${CMAKE_CURRENT_SOURCE_DIR}/SaveAsExperimentDialog.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/SaveAsExperimentDialog.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/SaveAsExperimentDialog.hxx
    )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_GenericProgressDialog.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_GenericProgressDialog.cxx ${CMAKE_CURRENT_SOURCE_DIR}/GenericProgressDialog.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/GenericProgressDialog.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/GenericProgressDialog.hxx
    )


add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_SelectTimeSegmentDialog.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_SelectTimeSegmentDialog.cxx ${CMAKE_CURRENT_SOURCE_DIR}/SelectTimeSegmentDialog.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/SelectTimeSegmentDialog.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/SelectTimeSegmentDialog.hxx
    )

target_include_directories(openss-guidialogs PUBLIC
    ${QT_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/libopenss-framework
    ${PROJECT_SOURCE_DIR}/libopenss-cli
    ${PROJECT_SOURCE_DIR}/libopenss-queries
    ${PROJECT_SOURCE_DIR}/libopenss-message
    ${PROJECT_SOURCE_DIR}/libopenss-guiimages
    ${PROJECT_SOURCE_DIR}/libopenss-guibase
    ${PROJECT_SOURCE_DIR}/libopenss-guiplugin
    ${PROJECT_SOURCE_DIR}/libopenss-guiobjects
    ${PROJECT_SOURCE_DIR}/libopenss-guichart
    ${PROJECT_SOURCE_DIR}/libopenss-gui
    )

target_link_libraries(openss-guidialogs
    ${QT_LIBRARIES}
    ${CMAKE_DL_LIBS}
    -export-dynamic 
    )

set_target_properties(openss-guidialogs PROPERTIES VERSION 1.1.0)

install(TARGETS openss-guidialogs LIBRARY DESTINATION lib${LIB_SUFFIX}
    )

