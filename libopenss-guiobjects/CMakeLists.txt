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

set(GUIOBJECTS_SOURCES
	HighlightObject.hxx
	ProcessEntryClass.hxx
	CollectorEntryClass.hxx
	CompareInfo.hxx
	MessageObject.hxx MessageObject.cxx
	SourceObject.hxx SourceObject.cxx
	UpdateObject.hxx UpdateObject.cxx
	ExpIDInUseObject.hxx ExpIDInUseObject.cxx
	FocusObject.hxx FocusObject.cxx
	FocusCompareObject.hxx FocusCompareObject.cxx
	ArgumentObject.hxx ArgumentObject.cxx
	ClosingDownObject.hxx ClosingDownObject.cxx
	PrepareToRerunObject.hxx PrepareToRerunObject.cxx
	PreferencesChangedObject.hxx PreferencesChangedObject.cxx
	LoadAttachObject.hxx LoadAttachObject.cxx
	SaveAsObject.hxx SaveAsObject.cxx
	ControlObject.hxx ControlObject.cxx
	ProcessListObject.hxx ProcessListObject.cxx
	CollectorListObject.hxx CollectorListObject.cxx
	OutputObject.hxx OutputObject.cxx
	AnimatedQPushButton.hxx AnimatedQPushButton.cxx
	LocalToolbar.hxx LocalToolbar.cxx
	InfoEventFilter.hxx InfoEventFilter.cxx
	ProcessControlObject.hxx ProcessControlObject.cxx
	DescriptionClassObject.hxx DescriptionClassObject.cxx
	CollectorParameterEntryClass.hxx
	CollectorMetricEntryClass.hxx
	moc_ProcessControlObject.cxx
	moc_InfoEventFilter.cxx
    )

add_library(openss-guiobjects SHARED
	${GUIOBJECTS_SOURCES}
    )

add_definitions(
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_ProcessControlObject.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_ProcessControlObject.cxx ProcessControlObject.hxx
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/ProcessControlObject.cxx 
        ${CMAKE_CURRENT_BINARY_DIR}/ProcessControlObject.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_InfoEventFilter.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_InfoEventFilter.cxx InfoEventFilter.hxx
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/InfoEventFilter.cxx 
        ${CMAKE_CURRENT_BINARY_DIR}/InfoEventFilter.hxx
    )
  
target_include_directories(openss-guiobjects PUBLIC
    ${QT_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/libopenss-framework
    ${PROJECT_SOURCE_DIR}/libopenss-cli
    ${PROJECT_SOURCE_DIR}/libopenss-framework
    ${PROJECT_SOURCE_DIR}/libopenss-guibase
    ${PROJECT_SOURCE_DIR}/libopenss-gui
    ${PROJECT_SOURCE_DIR}/libopenss-guiplugin
    ${PROJECT_SOURCE_DIR}/libopenss-guiimages
    ${PROJECT_SOURCE_DIR}/libopenss-message
    ${PROJECT_SOURCE_DIR}/libopenss-queries
    ${PROJECT_SOURCE_DIR}/plugins/panels/DebugPanel
    )

target_link_libraries(openss-guiobjects
    ${QT_LIBRARIES}
    ${CMAKE_DL_LIBS}
    )


set_target_properties(openss-guiobjects PROPERTIES VERSION 1.1.0)

install(TARGETS openss-guiobjects LIBRARY DESTINATION lib${LIB_SUFFIX}
    )

# copy these files into the build area.  add_custom_command seems to need this
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ProcessControlObject.hxx DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/InfoEventFilter.hxx DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ProcessControlObject.cxx DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/InfoEventFilter.cxx DESTINATION .)
