################################################################################
# Copyright (c) 2013-2015 Krell Institute. All Rights Reserved.
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


find_path(LibDwarf_INCLUDE_DIR NAMES dwarf.h 
    HINTS $ENV{LIBDWARF_DIR} 
    HINTS ${LIBDWARF_DIR}
    PATH_SUFFIXES include
    )

find_library(LibDwarf_LIBRARY_SHARED NAMES libdwarf.so
    HINTS $ENV{LIBDWARF_DIR} 
    HINTS ${LIBDWARF_DIR} 
    PATH_SUFFIXES lib lib64
    )

find_library(LibDwarf_LIBRARY_STATIC NAMES libdwarf.a
    HINTS $ENV{LIBDWARF_DIR}
    HINTS ${LIBDWARF_DIR}
    PATH_SUFFIXES lib lib64
    )

find_package_handle_standard_args(
    LibDwarf DEFAULT_MSG
    LibDwarf_LIBRARY_SHARED LibDwarf_LIBRARY_STATIC
    LibDwarf_INCLUDE_DIR
    )

set(LibDwarf_SHARED_LIBRARIES ${LibDwarf_LIBRARY_SHARED})
set(LibDwarf_STATIC_LIBRARIES ${LibDwarf_LIBRARY_STATIC})
set(LibDwarf_INCLUDE_DIRS ${LibDwarf_INCLUDE_DIR})

set(LibDwarf_DEFINES "")

if(LIBDWARF_FOUND)
#  CMAKE_REQUIRED_FLAGS = string of compile command line flags
#  CMAKE_REQUIRED_DEFINITIONS = list of macros to define (-DFOO=bar)
#  CMAKE_REQUIRED_INCLUDES = list of include directories
#  CMAKE_REQUIRED_LIBRARIES = list of libraries to link
    SET(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} -lelf ${LibDwarf_LIBRARY_SHARED})
    SET(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${LibDwarf_INCLUDE_DIRS})
    CHECK_SYMBOL_EXISTS(dwarf_next_cu_header_c "libdwarf.h" HAVE_DWARF)
    #CHECK_FUNCTION_EXISTS(dwarf_next_cu_header_c HAVE_DWARF)
    message(STATUS "LibDwarf HAVE_dwarf_next_cu_header_c : " ${HAVE_DWARF})

# Clean up, so that future find_package calls do not find the settings
# (used above) for these variables
    SET(CMAKE_REQUIRED_LIBRARIES "")
    SET(CMAKE_REQUIRED_INCLUDES "")

endif()

if (NOT ${HAVE_DWARF})
    message(FATAL "LibDwarf does not contain the needed dwarf_next_cu_header_c symbol")
endif()

GET_FILENAME_COMPONENT(LibDwarf_LIB_DIR ${LibDwarf_LIBRARY_SHARED} PATH )
GET_FILENAME_COMPONENT(LibDwarf_DIR ${LibDwarf_INCLUDE_DIR} PATH )
#message(STATUS "LibDwarf dwarf_next_cu_header_c symbol : " ${HAVE_DWARF})
#message(STATUS "LibDwarf LibDwarf_SHARED_LIBRARIES: " ${LibDwarf_SHARED_LIBRARIES})
#message(STATUS "LibDwarf LibDwarf_STATIC_LIBRARIES: " ${LibDwarf_STATIC_LIBRARIES})
#message(STATUS "LibDwarf LibDwarf_INCLUDE_DIR: " ${LibDwarf_INCLUDE_DIR})
#message(STATUS "LibDwarf LibDwarf_LIB_DIR: " ${LibDwarf_LIB_DIR})
message(STATUS "LibDwarf found: " ${LIBDWARF_FOUND})
message(STATUS "LibDwarf location: " ${LibDwarf_DIR})


mark_as_advanced(
            LibDwarf_LIBRARY_SHARED 
            LibDwarf_LIBRARY_STATIC
            LibDwarf_INCLUDE_DIR
            LibDwarf_LIB_DIR
            )
