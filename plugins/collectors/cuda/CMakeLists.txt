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

add_library(cuda MODULE
    CUDACollector.hxx CUDACollector.cxx
    CUDACountsDetail.hxx
    CUDAExecDetail.hxx
    CUDAXferDetail.hxx
    )

target_include_directories(cuda PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/libopenss-framework
    ${PROJECT_SOURCE_DIR}/libopenss-queries-cuda
    ${Boost_INCLUDE_DIRS}
    ${CBTF_MESSAGES_CUDA_INCLUDE_DIRS}
    ${ARGONAVIS_BASE_INCLUDE_DIRS}
    ${ARGONAVIS_CUDA_INCLUDE_DIRS}
    )

target_link_libraries(cuda
    openss-framework
    openss-queries-cuda
    ${CBTF_MESSAGES_CUDA_LIBRARIES}
    ${ARGONAVIS_BASE_LIBRARIES}
    ${ARGONAVIS_CUDA_LIBRARIES}
    ${CMAKE_DL_LIBS}
    )

set_target_properties(cuda PROPERTIES PREFIX "")
  
install(TARGETS cuda
    LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )
