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

find_path(Mpich_INCLUDE_DIR
    NAMES mpi.h
    HINTS $ENV{MPICH_DIR}
    HINTS ${MPICH_DIR}
    PATH_SUFFIXES include include/mpich-${oss_hardware_platform} include/mpich include64 
    )

find_library(Mpich_LIBRARY_SHARED NAMES mpich mpi
    HINTS $ENV{MPICH_DIR}
    HINTS ${MPICH_DIR}
    PATHS /usr/lib64/mpich /usr/lib/mpich
    PATH_SUFFIXES lib lib64 lib/shared lib64/shared
    )

find_package_handle_standard_args(
    Mpich DEFAULT_MSG
    Mpich_LIBRARY_SHARED
    Mpich_INCLUDE_DIR
    )

set(Mpich_SHARED_LIBRARIES ${Mpich_LIBRARY_SHARED})
set(Mpich_INCLUDE_DIRS ${Mpich_INCLUDE_DIR})
set(Mpich_DEFINES "")

GET_FILENAME_COMPONENT(Mpich_LIB_DIR ${Mpich_LIBRARY_SHARED} PATH )
GET_FILENAME_COMPONENT(Mpich_DIR ${Mpich_INCLUDE_DIR} PATH )
#message(STATUS "Mpich Mpich_SHARED_LIBRARIES: " ${Mpich_SHARED_LIBRARIES})
#message(STATUS "Mpich Mpich_INCLUDE_DIR: " ${Mpich_INCLUDE_DIR})
#message(STATUS "Mpich Mpich_LIB_DIR: " ${Mpich_LIB_DIR})
message(STATUS "Mpich found: " ${MPICH_FOUND})
message(STATUS "Mpich location: " ${Mpich_LIB_DIR})

if (MPICH_FOUND)
    set(Mpich_DEFINES "HAVE_MPICH=1")
else()
    set(Mpich_DEFINES "")
endif()


mark_as_advanced(
            Mpich_LIBRARY_SHARED 
            Mpich_INCLUDE_DIR
            Mpich_LIB_DIR
            )
