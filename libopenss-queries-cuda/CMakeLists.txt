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

check_include_file("stdint.h" STDINT_H_FOUND)
if (STDINT_H_FOUND)
    add_definitions(-DHAVE_STDINT_H=1)
endif()
if (OpenMP_FLAG_DETECTED)
    add_definitions(-fopenmp)
endif()

set(QUERIES_CUDA_SOURCES
	CUDAData.hxx
	CUDAData.cxx
	CUDAExecXferBalance.hxx
	CUDAQueries.hxx
	CUDAQueries.cxx
	CUDAXferRate.hxx
    )

add_library(openss-queries-cuda SHARED
	${QUERIES_CUDA_SOURCES}
    )

add_definitions(
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )
  

include_directories(
    ${CUDA_INCLUDE_DIR}
    ${MESSAGES_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/libopenss-cli
    ${PROJECT_SOURCE_DIR}/libopenss-framework
    )

target_link_libraries(openss-queries
    openss-framework
    ${CUDA_LIBRARIES}
    ${MESSAGES_LIBRARIES}
    ${CMAKE_DL_LIBS}
    )


set_target_properties(openss-queries-cuda PROPERTIES VERSION 1.1.0)

install(TARGETS openss-queries-cuda
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    )
