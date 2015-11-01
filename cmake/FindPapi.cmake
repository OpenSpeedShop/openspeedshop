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


find_path(Papi_INCLUDE_DIR
    NAMES papi.h
    HINTS $ENV{PAPI_DIR}
    HINTS ${PAPI_DIR}
    PATH_SUFFIXES include
    )

find_library(Papi_LIBRARY_SHARED NAMES papi
    HINTS $ENV{PAPI_DIR}
    HINTS ${PAPI_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(Papi_pfm_LIBRARY_SHARED NAMES pfm
    HINTS $ENV{PAPI_DIR}
    HINTS ${PAPI_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(Papi_LIBRARY_STATIC NAMES libpapi.a
    HINTS $ENV{PAPI_DIR}
    HINTS ${PAPI_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(Papi_pfm_LIBRARY_STATIC NAMES libpfm.a
    HINTS $ENV{PAPI_DIR}
    HINTS ${PAPI_DIR}
    PATH_SUFFIXES lib lib64
    )

find_package_handle_standard_args(
    Papi DEFAULT_MSG
    Papi_LIBRARY_SHARED Papi_LIBRARY_STATIC
    Papi_INCLUDE_DIR
    )

# if pfm library is found set the shared libraries variables
if (${Papi_pfm_LIBRARY_SHARED})
    set(Papi_SHARED_LIBRARIES ${Papi_LIBRARY_SHARED} ${Papi_pfm_LIBRARY_SHARED})
    set(Papi_pfm_SHARED_LIBRARIES ${Papi_pfm_LIBRARY_SHARED})
else()
    set(Papi_SHARED_LIBRARIES ${Papi_LIBRARY_SHARED})
endif()

set(Papi_STATIC_LIBRARIES ${Papi_LIBRARY_STATIC})
# if pfm library is found set the static libraries variable
if (${Papi_pfm_LIBRARY_STATIC})
    set(Papi_pfm_STATIC_LIBRARIES ${Papi_pfm_LIBRARY_STATIC})
endif()

set(Papi_INCLUDE_DIRS ${Papi_INCLUDE_DIR})

GET_FILENAME_COMPONENT(Papi_LIB_DIR ${Papi_LIBRARY_SHARED} PATH )
GET_FILENAME_COMPONENT(Papi_DIR ${Papi_INCLUDE_DIR} PATH )

message(STATUS "Papi Papi_SHARED_LIBRARIES: " ${Papi_SHARED_LIBRARIES})
message(STATUS "Papi Papi_STATIC_LIBRARIES: " ${Papi_STATIC_LIBRARIES})
message(STATUS "Papi Papi_pfm_SHARED_LIBRARIES: " ${Papi_pfm_SHARED_LIBRARIES})
message(STATUS "Papi Papi_pfm_STATIC_LIBRARIES: " ${Papi_pfm_STATIC_LIBRARIES})
message(STATUS "Papi Papi_INCLUDE_DIR: " ${Papi_INCLUDE_DIR})
message(STATUS "Papi Papi_LIB_DIR: " ${Papi_LIB_DIR})
message(STATUS "Papi found: " ${PAPI_FOUND})
message(STATUS "Papi location: " ${Papi_DIR})


mark_as_advanced(
            Papi_LIBRARY_SHARED 
            Papi_LIBRARY_STATIC
            Papi_INCLUDE_DIR
            Papi_LIB_DIR
            )

if (${Papi_pfm_LIBRARY_STATIC})
    mark_as_advanced(
            Papi_pfm_LIBRARY_STATIC
   )
endif()

if (${Papi_pfm_LIBRARY_SHARED})
    mark_as_advanced(
            Papi_pfm_LIBRARY_SHARED
   )
endif()
