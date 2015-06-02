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

set(CompareWizardPanel_SOURCES
	CompareWizardPanel.hxx
	CompareWizardPanel.cxx
	moc_CompareWizardPanel.cxx
	CompareDescription.hxx
	local_plugin_info.hxx
    )

add_library(CompareWizardPanel MODULE
	${CompareWizardPanel_SOURCES}
    )

add_definitions(
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_CompareWizardPanel.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_CompareWizardPanel.cxx ${CMAKE_CURRENT_SOURCE_DIR}/CompareWizardPanel.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/CompareWizardPanel.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/CompareWizardPanel.hxx
    )

target_include_directories(CompareWizardPanel PUBLIC
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
    )

target_link_libraries(CompareWizardPanel
    openss-guibase
    openss-guiobjects
    openss-framework
    openss-queries
    ${QT_LIBRARIES}
    ${CMAKE_DL_LIBS}
    )


set_target_properties(CompareWizardPanel PROPERTIES PREFIX "")

install(TARGETS CompareWizardPanel LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )

