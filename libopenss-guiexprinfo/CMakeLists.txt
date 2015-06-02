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

set(GUIEXPRINFO_SOURCES
	MetricInfo.hxx
        MetricInfo.cxx
	CollectorInfo.hxx
        CollectorInfo.cxx 
    )

add_library(openss-guiexprinfo SHARED
	${GUIEXPRINFO_SOURCES}
    )

add_definitions(
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )

target_include_directories(openss-guiexprinfo PUBLIC
    ${QT_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/libopenss-guibase
    )

target_link_libraries(openss-guiexprinfo
    ${QT_LIBRARIES}
    openss-guibase
    ${CMAKE_DL_LIBS}
    )


set_target_properties(openss-guiexprinfo PROPERTIES VERSION 1.1.0)

install(TARGETS openss-guiexprinfo LIBRARY DESTINATION lib${LIB_SUFFIX}
    )

