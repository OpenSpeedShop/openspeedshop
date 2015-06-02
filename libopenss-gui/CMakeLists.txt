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

set(GUI_SOURCES
     AboutClass.cxx AboutClass.hxx
     CLIInterface.hxx CLIInterface.cxx
     gui.cxx
     openspeedshop.hxx openspeedshop.cxx
     openspeedshop.ui.hxx
     preferencesdialog.hxx preferencesdialog.cxx
     preferencesdialog.ui.hxx
     qmake_image_collection.cxx
     moc_AboutClass.cxx
     moc_CLIInterface.cxx
     moc_openspeedshop.cxx
     moc_preferencesdialog.cxx
    )

add_library(openss-gui SHARED
	${GUI_SOURCES}
    )

add_definitions(
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_AboutClass.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_AboutClass.cxx ${CMAKE_CURRENT_SOURCE_DIR}/AboutClass.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/AboutClass.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/AboutClass.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_CLIInterface.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_CLIInterface.cxx ${CMAKE_CURRENT_SOURCE_DIR}/CLIInterface.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/CLIInterface.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/CLIInterface.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_openspeedshop.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_openspeedshop.cxx ${CMAKE_CURRENT_SOURCE_DIR}/openspeedshop.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/openspeedshop.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/openspeedshop.hxx
    )
  
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_preferencesdialog.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_preferencesdialog.cxx ${CMAKE_CURRENT_SOURCE_DIR}/preferencesdialog.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/preferencesdialog.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/preferencesdialog.hxx
    )

target_include_directories(openss-gui PUBLIC
    ${QT_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/libopenss-framework
    ${PROJECT_SOURCE_DIR}/libopenss-cli
    ${PROJECT_SOURCE_DIR}/libopenss-framework
    ${PROJECT_SOURCE_DIR}/libopenss-guibase
    ${PROJECT_SOURCE_DIR}/libopenss-guiimages
    ${PROJECT_SOURCE_DIR}/libopenss-guiobjects
    ${PROJECT_SOURCE_DIR}/libopenss-guidialogs
    ${PROJECT_SOURCE_DIR}/libopenss-guiplugin
    ${PROJECT_SOURCE_DIR}/libopenss-message
    ${PROJECT_SOURCE_DIR}/libopenss-queries
    ${PROJECT_SOURCE_DIR}/plugins/panels/DebugPanel
    )

target_link_libraries(openss-gui
    openss-guibase
    openss-guidialogs
    openss-guiobjects
    openss-guiplugin
    openss-framework
    openss-cli
    ${QT_LIBRARIES}
    ${QT_QT_LIBRARY}
    ${CMAKE_DL_LIBS}
    )


set_target_properties(openss-gui PROPERTIES VERSION 1.1.0)

install(TARGETS openss-gui LIBRARY DESTINATION lib${LIB_SUFFIX}
    )

