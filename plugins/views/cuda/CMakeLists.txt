################################################################################
# Copyright (c) 2014-2015 Krell Institute. All Rights Reserved.
# Copyright (c) 2015,2016 Argo Navis Technologies. All Rights Reserved.
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

add_library(cuda_view MODULE
    cuda_exec_view.cxx
    cuda_hwpc_view.cxx
    cuda_xfer_view.cxx
    cuda_view.cxx
    )

target_include_directories(cuda_view PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/libopenss-cli
    ${PROJECT_SOURCE_DIR}/libopenss-runtime
    ${PROJECT_SOURCE_DIR}/libopenss-message
    ${PROJECT_SOURCE_DIR}/libopenss-queries
    ${PROJECT_SOURCE_DIR}/libopenss-queries-cuda
    ${PROJECT_SOURCE_DIR}/plugins/collectors/cuda
    ${Boost_INCLUDE_DIRS}
    ${ARGONAVIS_CUDA_INCLUDE_DIRS}
    ${Python_INCLUDE_DIRS}
    )

target_link_libraries(cuda_view
    openss-cli
    openss-queries-cuda
    ${ARGONAVIS_CUDA_LIBRARIES}
    ${CMAKE_DL_LIBS}
    )

set_target_properties(cuda_view PROPERTIES PREFIX "")

install(TARGETS cuda_view
    LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )
