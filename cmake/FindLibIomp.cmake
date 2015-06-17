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


find_path(LibIomp_INCLUDE_DIR
    NAMES ompt.h
    HINTS $ENV{LIBIOMP_DIR}
    HINTS ${LIBIOMP_DIR}
    PATH_SUFFIXES include 
    )

find_library(LibIomp_LIBRARY_SHARED NAMES iomp5
    HINTS $ENV{LIBIOMP_DIR}
    HINTS ${LIBIOMP_DIR}
    PATH_SUFFIXES lib lib64
    )

find_package_handle_standard_args(
    LibIomp DEFAULT_MSG
    LibIomp_LIBRARY_SHARED
    LibIomp_INCLUDE_DIR
    )

set(LibIomp_SHARED_LIBRARIES ${LibIomp_LIBRARY_SHARED})
set(LibIomp_INCLUDE_DIRS ${LibIomp_INCLUDE_DIR})
set(LibIomp_DEFINES "")

GET_FILENAME_COMPONENT(LibIomp_LIB_DIR ${LibIomp_LIBRARY_SHARED} PATH )
GET_FILENAME_COMPONENT(LibIomp_DIR ${LibIomp_INCLUDE_DIR} PATH )
#message(STATUS "LibIomp LibIomp_SHARED_LIBRARIES: " ${LibIomp_SHARED_LIBRARIES})
#message(STATUS "LibIomp LibIomp_INCLUDE_DIR: " ${LibIomp_INCLUDE_DIR})
#message(STATUS "LibIomp LibIomp_LIB_DIR: " ${LibIomp_LIB_DIR})
message(STATUS "LibIomp found: " ${LibIomp_FOUND})
message(STATUS "LibIomp location: " ${LibIomp_LIB_DIR})

set(LibIomp_DEFINES "HAVE_OMPT=${LibIomp_FOUND}")


mark_as_advanced(
            LibIomp_LIBRARY_SHARED 
            LibIomp_INCLUDE_DIR
            LibIomp_DIR
            LibIomp_LIB_DIR
            )
