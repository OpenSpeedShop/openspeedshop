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

find_path(Mpich2_INCLUDE_DIR
    NAMES mpi.h
    HINTS $ENV{MPICH2_DIR}
    HINTS ${MPICH2_DIR}
    PATH_SUFFIXES include include/mpich-${oss_hardware_platform} include/mpich2 include64 include/mpich2-${oss_hardware_platform} include/mpich 
    )

find_library(Mpich2_LIBRARY_SHARED NAMES mpich mpi
    HINTS $ENV{MPICH2_DIR}
    HINTS ${MPICH2_DIR}
    PATH_SUFFIXES lib lib64 lib/shared lib64/shared
    )

find_package_handle_standard_args(
    Mpich2 DEFAULT_MSG
    Mpich2_LIBRARY_SHARED
    Mpich2_INCLUDE_DIR
    )

set(Mpich2_SHARED_LIBRARIES ${Mpich2_LIBRARY_SHARED})
set(Mpich2_INCLUDE_DIRS ${Mpich2_INCLUDE_DIR})
set(Mpich2_DEFINES "")

GET_FILENAME_COMPONENT(Mpich2_LIB_DIR ${Mpich2_LIBRARY_SHARED} PATH )
GET_FILENAME_COMPONENT(Mpich2_DIR ${Mpich2_INCLUDE_DIR} PATH )
#message(STATUS "Mpich2 Mpich2_SHARED_LIBRARIES: " ${Mpich2_SHARED_LIBRARIES})
#message(STATUS "Mpich2 Mpich2_INCLUDE_DIR: " ${Mpich2_INCLUDE_DIR})
#message(STATUS "Mpich2 Mpich2_LIB_DIR: " ${Mpich2_LIB_DIR})
message(STATUS "Mpich2 found: " ${MPICH2_FOUND})
message(STATUS "Mpich2 location: " ${Mpich2_LIB_DIR})

if (MPICH2_FOUND)
    set(Mpich2_DEFINES "HAVE_MPICH2=1")
else()
    set(Mpich2_DEFINES "")
endif()


mark_as_advanced(
            Mpich2_LIBRARY_SHARED 
            Mpich2_INCLUDE_DIR
            Mpich2_LIB_DIR
            )
