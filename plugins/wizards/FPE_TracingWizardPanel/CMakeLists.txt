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

set(FPE_TracingWizardPanel_SOURCES
	FPE_TracingWizardPanel.hxx
	FPE_TracingWizardPanel.cxx
	moc_FPE_TracingWizardPanel.cxx
	FPE_TracingDescription.hxx
	local_plugin_info.hxx
    )

add_library(FPE_TracingWizardPanel MODULE
	${FPE_TracingWizardPanel_SOURCES}
    )

add_definitions(
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_FPE_TracingWizardPanel.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_FPE_TracingWizardPanel.cxx ${CMAKE_CURRENT_SOURCE_DIR}/FPE_TracingWizardPanel.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/FPE_TracingWizardPanel.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/FPE_TracingWizardPanel.hxx
    )


target_include_directories(FPE_TracingWizardPanel PUBLIC
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

target_link_libraries(FPE_TracingWizardPanel
    openss-guibase
    openss-guiobjects
    openss-framework
    openss-queries
    ${QT_LIBRARIES}
    ${CMAKE_DL_LIBS}
    )


set_target_properties(FPE_TracingWizardPanel PROPERTIES PREFIX "")

install(TARGETS FPE_TracingWizardPanel LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )

# copy these files into the build area.  add_custom_command seems to need this
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/FPE_TracingWizardPanel.hxx DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/FPE_TracingWizardPanel.cxx DESTINATION .)
