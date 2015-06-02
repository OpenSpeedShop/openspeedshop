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

set(IntroWizardPanel_SOURCES
	IntroWizardPanel.hxx
	IntroWizardPanel.cxx
	moc_IntroWizardPanel.cxx
	local_plugin_info.hxx
    )

add_library(IntroWizardPanel MODULE
	${IntroWizardPanel_SOURCES}
    )

add_definitions(
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/moc_IntroWizardPanel.cxx
    COMMAND ${QT_MOC_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/moc_IntroWizardPanel.cxx ${CMAKE_CURRENT_SOURCE_DIR}/IntroWizardPanel.hxx
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/IntroWizardPanel.cxx 
        ${CMAKE_CURRENT_SOURCE_DIR}/IntroWizardPanel.hxx
    )


target_include_directories(IntroWizardPanel PUBLIC
    ${QT_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/libopenss-gui
    ${PROJECT_SOURCE_DIR}/libopenss-guibase
    ${PROJECT_SOURCE_DIR}/libopenss-guiimages
    ${PROJECT_SOURCE_DIR}/libopenss-guiobjects
    ${PROJECT_SOURCE_DIR}/libopenss-guiplugin
    )

target_link_libraries(IntroWizardPanel
    openss-guibase
    openss-guiobjects
    openss-framework
    openss-queries
    ${QT_LIBRARIES}
    ${CMAKE_DL_LIBS}
    )


set_target_properties(IntroWizardPanel PROPERTIES PREFIX "")

install(TARGETS IntroWizardPanel LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )

# copy these files into the build area.  add_custom_command seems to need this
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/IntroWizardPanel.hxx DESTINATION .)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/IntroWizardPanel.cxx DESTINATION .)
