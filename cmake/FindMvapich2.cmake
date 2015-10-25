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

include(FindPackageHandleStandardArgs)
INCLUDE (CheckSymbolExists)
INCLUDE (CheckFunctionExists)

SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")

#MESSAGE( STATUS "/usr/include/mpich-${oss_hardware_platform}: " ${oss_hardware_platform} )

find_path(Mvapich2_INCLUDE_DIR
    NAMES mpi.h
    HINTS $ENV{MVAPICH2_DIR}
    HINTS ${MVAPICH2_DIR}
    PATH_SUFFIXES include 
    )

find_library(Mvapich2_LIBRARY_SHARED NAMES mpich
    HINTS $ENV{MVAPICH2_DIR}
    HINTS ${MVAPICH2_DIR}
    PATH_SUFFIXES lib lib64 lib/shared lib64/shared
    )

find_library(Mvapich2_verbs_LIBRARY_SHARED NAMES verbs
    HINTS $ENV{MVAPICH2_DIR}
    HINTS ${MVAPICH2_DIR}
    PATH_SUFFIXES lib lib64 lib/shared lib64/shared
    )

find_library(Mvapich2_common_LIBRARY_SHARED NAMES common
    HINTS $ENV{MVAPICH2_DIR}
    HINTS ${MVAPICH2_DIR}
    PATH_SUFFIXES lib lib64 lib/shared lib64/shared
    )

find_package_handle_standard_args(
    Mvapich2 DEFAULT_MSG
    Mvapich2_LIBRARY_SHARED
    Mvapich2_INCLUDE_DIR
    )

set(Mvapich2_SHARED_LIBRARIES ${Mvapich2_LIBRARY_SHARED} ${Mvapich2_verbs_LIBRARY_SHARED} ${Mvapich2_common_LIBRARY_SHARED})
set(Mvapich2_INCLUDE_DIRS ${Mvapich2_INCLUDE_DIR})
set(Mvapich2_DEFINES "")

GET_FILENAME_COMPONENT(Mvapich2_LIB_DIR ${Mvapich2_LIBRARY_SHARED} PATH )
GET_FILENAME_COMPONENT(Mvapich2_DIR ${Mvapich2_INCLUDE_DIR} PATH )
#message(STATUS "Mvapich2 Mvapich2_SHARED_LIBRARIES: " ${Mvapich2_SHARED_LIBRARIES})
#message(STATUS "Mvapich2 Mvapich2_INCLUDE_DIR: " ${Mvapich2_INCLUDE_DIR})
#message(STATUS "Mvapich2 Mvapich2_LIB_DIR: " ${Mvapich2_LIB_DIR})
message(STATUS "Mvapich2 found: " ${MVAPICH2_FOUND})
message(STATUS "Mvapich2 location: " ${Mvapich2_LIB_DIR})

if (MVAPICH2_FOUND)
    set(Mvapich2_DEFINES "HAVE_MVAPICH2=1")
else()
    set(Mvapich2_DEFINES "")
endif()


mark_as_advanced(
            Mvapich2_LIBRARY_SHARED 
            Mvapich2_INCLUDE_DIR
            Mvapich2_LIB_DIR
            )
