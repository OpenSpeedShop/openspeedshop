################################################################################
# Copyright (c) 2016 Krell Institute. All Rights Reserved.
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

set(SOURCES omptp_view.cxx)

set(GCC_FORMAT_CONTAINS_NUL "-Wno-format-contains-nul")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_FORMAT_CONTAINS_NUL}")

add_definitions(
	-DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )

add_library(omptp_view MODULE ${SOURCES})
  
target_include_directories(omptp_view PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-cli
	${PROJECT_BUILD_DIR}/libopenss-cli
	${PROJECT_SOURCE_DIR}/libopenss-framework
	${PROJECT_SOURCE_DIR}/plugins/collectors/omptp
	${PROJECT_SOURCE_DIR}/libopenss-message
	${PROJECT_SOURCE_DIR}/libopenss-queries
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${PYTHON_INCLUDE_DIR}
    )

target_link_libraries(omptp_view
	pthread
	openss-cli
	openss-queries
	openss-framework
	${CMAKE_DL_LIBS}
    )

#set_target_properties(omptp_view PROPERTIES VERSION 1.1.0)
set_target_properties(omptp_view PROPERTIES PREFIX "")

install(TARGETS omptp_view
	LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )
