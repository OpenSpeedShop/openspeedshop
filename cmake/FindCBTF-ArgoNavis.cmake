################################################################################
# Copyright (c) 2006-2015 Krell Institute. All Rights Reserved.
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

include(FindPackageHandleStandardArgs)

find_library(ARGONAVIS_BASE_SHARED_LIBRARY
    NAMES libargonavis-base.so
    HINTS ${CBTF_ARGONAVIS_DIR} $ENV{CBTF_ARGONAVIS_DIR}
    PATH_SUFFIXES lib lib64
    )

find_path(ARGONAVIS_BASE_INCLUDE_DIR
    ArgoNavis/Base/LinkedObject.hpp
    HINTS ${CBTF_ARGONAVIS_DIR} $ENV{CBTF_ARGONAVIS_DIR}
    PATH_SUFFIXES include
    )

find_package_handle_standard_args(
    ArgoNavis-Base DEFAULT_MSG
    ARGONAVIS_BASE_SHARED_LIBRARY
    ARGONAVIS_BASE_INCLUDE_DIR
    )

set(ARGONAVIS_BASE_LIBRARIES ${ARGONAVIS_BASE_SHARED_LIBRARY})
set(ARGONAVIS_BASE_INCLUDE_DIRS ${ARGONAVIS_BASE_INCLUDE_DIR})
mark_as_advanced(ARGONAVIS_BASE_SHARED_LIBRARY ARGONAVIS_BASE_INCLUDE_DIR)

find_library(CBTF_MESSAGES_CUDA_SHARED_LIBRARY
    NAMES libcbtf-messages-cuda.so
    HINTS ${CBTF_ARGONAVIS_DIR} $ENV{CBTF_ARGONAVIS_DIR}
    PATH_SUFFIXES lib lib64
    )

find_path(CBTF_MESSAGES_CUDA_INCLUDE_DIR
    KrellInstitute/Messages/CUDA_data.h
    HINTS ${CBTF_ARGONAVIS_DIR} $ENV{CBTF_ARGONAVIS_DIR}
    PATH_SUFFIXES include
    )

find_package_handle_standard_args(
    CBTF-Messages-CUDA DEFAULT_MSG
    CBTF_MESSAGES_CUDA_SHARED_LIBRARY
    CBTF_MESSAGES_CUDA_INCLUDE_DIR
    )

set(CBTF_MESSAGES_CUDA_LIBRARIES ${CBTF_MESSAGES_CUDA_SHARED_LIBRARY})
set(CBTF_MESSAGES_CUDA_INCLUDE_DIRS ${CBTF_MESSAGES_CUDA_INCLUDE_DIR})
mark_as_advanced(CBTF_MESSAGES_CUDA_SHARED_LIBRARY CBTF_MESSAGES_INCLUDE_DIR)

find_library(ARGONAVIS_CUDA_SHARED_LIBRARY
    NAMES libargonavis-cuda.so
    HINTS ${CBTF_ARGONAVIS_DIR} $ENV{CBTF_ARGONAVIS_DIR}
    PATH_SUFFIXES lib lib64
    )

find_path(ARGONAVIS_CUDA_INCLUDE_DIR
    ArgoNavis/CUDA/PerformanceData.hpp
    HINTS ${CBTF_ARGONAVIS_DIR} $ENV{CBTF_ARGONAVIS_DIR}
    PATH_SUFFIXES include
    )

find_package_handle_standard_args(
    ArgoNavis-CUDA DEFAULT_MSG
    ARGONAVIS_CUDA_SHARED_LIBRARY
    ARGONAVIS_CUDA_INCLUDE_DIR
    )

set(ARGONAVIS_CUDA_LIBRARIES ${ARGONAVIS_CUDA_SHARED_LIBRARY})
set(ARGONAVIS_CUDA_INCLUDE_DIRS ${ARGONAVIS_CUDA_INCLUDE_DIR})
mark_as_advanced(ARGONAVIS_CUDA_SHARED_LIBRARY ARGONAVIS_CUDA_INCLUDE_DIR)

find_file(ARGONAVIS_CUDA_XML
    NAMES cuda.xml
    HINTS 
        ${CBTF_ARGONAVIS_DIR}/share/KrellInstitute/xml
        $ENV{CBTF_ARGONAVIS_DIR}/share/KrellInstitute/xml
    )

mark_as_advanced(ARGONAVIS_CUDA_XML)

if (CBTF-MESSAGES-CUDA_FOUND AND ARGONAVIS-BASE_FOUND AND ARGONAVIS-CUDA_FOUND)
    set(BUILD_CUDA_ANALYSIS "TRUE")
    if (ARGONAVIS_CUDA_XML)    
        set(BUILD_CUDA_COLLECTION "TRUE")
    endif()
endif()
