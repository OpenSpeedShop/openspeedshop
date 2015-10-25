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

find_path(Mvapich_INCLUDE_DIR
    NAMES mpi.h
    HINTS $ENV{MVAPICH_DIR}
    HINTS ${MVAPICH_DIR}
    PATH_SUFFIXES include 
    )

find_library(Mvapich_LIBRARY_SHARED NAMES mpich
    HINTS $ENV{MVAPICH_DIR}
    HINTS ${MVAPICH_DIR}
    PATH_SUFFIXES lib lib64 lib/shared lib64/shared
    )

find_library(Mvapich_umad_LIBRARY_SHARED NAMES umad
    HINTS $ENV{MVAPICH_DIR}
    HINTS ${MVAPICH_DIR}
    PATH_SUFFIXES lib lib64 lib/shared lib64/shared
    )


find_package_handle_standard_args(
    Mvapich DEFAULT_MSG
    Mvapich_LIBRARY_SHARED
    Mvapich_INCLUDE_DIR
    )

set(Mvapich_SHARED_LIBRARIES ${Mvapich_LIBRARY_SHARED} ${Mvapich_umad_LIBRARY_SHARED})
set(Mvapich_INCLUDE_DIRS ${Mvapich_INCLUDE_DIR})
set(Mvapich_DEFINES "")

GET_FILENAME_COMPONENT(Mvapich_LIB_DIR ${Mvapich_LIBRARY_SHARED} PATH )
GET_FILENAME_COMPONENT(Mvapich_DIR ${Mvapich_INCLUDE_DIR} PATH )
#message(STATUS "Mvapich Mvapich_SHARED_LIBRARIES: " ${Mvapich_SHARED_LIBRARIES})
#message(STATUS "Mvapich Mvapich_INCLUDE_DIR: " ${Mvapich_INCLUDE_DIR})
#message(STATUS "Mvapich Mvapich_LIB_DIR: " ${Mvapich_LIB_DIR})
message(STATUS "Mvapich found: " ${MVAPICH_FOUND})
message(STATUS "Mvapich location: " ${Mvapich_LIB_DIR})

if (MVAPICH_FOUND)
    set(Mvapich_DEFINES "HAVE_MVAPICH=1")
else()
    set(Mvapich_DEFINES "")
endif()


mark_as_advanced(
            Mvapich_LIBRARY_SHARED 
            Mvapich_INCLUDE_DIR
            Mvapich_LIB_DIR
            )
