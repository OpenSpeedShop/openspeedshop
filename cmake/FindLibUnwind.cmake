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


find_path(LibUnwind_INCLUDE_DIR
    NAMES libunwind.h
    HINTS $ENV{LIBUNWIND_DIR}
    HINTS ${LIBUNWIND_DIR}
    PATH_SUFFIXES include
    )

find_library(LibUnwind_LIBRARY_SHARED NAMES unwind
    HINTS $ENV{LIBUNWIND_DIR}
    HINTS ${LIBUNWIND_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(LibUnwind_LIBRARY_STATIC NAMES libunwind.a
    HINTS $ENV{LIBUNWIND_DIR}
    HINTS ${LIBUNWIND_DIR}
    PATH_SUFFIXES lib lib64
    )



find_package_handle_standard_args(
    LibUnwind DEFAULT_MSG
    LibUnwind_LIBRARY_SHARED LibUnwind_LIBRARY_STATIC
    LibUnwind_INCLUDE_DIR
    )

set(LibUnwind_SHARED_LIBRARIES ${LibUnwind_LIBRARY_SHARED})
set(LibUnwind_STATIC_LIBRARIES ${LibUnwind_LIBRARY_STATIC})
set(LibUnwind_INCLUDE_DIRS ${LibUnwind_INCLUDE_DIR})
set(LibUnwind_DEFINES "UNW_LOCAL_ONLY")


GET_FILENAME_COMPONENT(LibUnwind_LIB_DIR ${LibUnwind_LIBRARY_SHARED} PATH )
GET_FILENAME_COMPONENT(LibUnwind_DIR ${LibUnwind_INCLUDE_DIRS} PATH )

#message(STATUS "LibUnwind LibUnwind_LIB_DIR: " ${LibUnwind_LIB_DIR})
#message(STATUS "LibUnwind LibUnwind_SHARED_LIBRARIES: " ${LibUnwind_SHARED_LIBRARIES})
#message(STATUS "LibUnwind LibUnwind_STATIC_LIBRARIES: " ${LibUnwind_STATIC_LIBRARIES})
#message(STATUS "LibUnwind LibUnwind_INCLUDE_DIR: " ${LibUnwind_INCLUDE_DIR})
message(STATUS "LibUnwind found: " ${LIBUNWIND_FOUND})
message(STATUS "LibUnwind location: " ${LibUnwind_DIR})


mark_as_advanced(
            LibUnwind_LIBRARY_SHARED 
            LibUnwind_LIBRARY_STATIC
            LibUnwind_INCLUDE_DIR
            LibUnwind_LIB_DIR
            )
