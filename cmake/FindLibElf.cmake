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


find_path(LibElf_INCLUDE_DIR
    NAMES libelf.h elf.h 
    HINTS $ENV{LIBELF_DIR}
    HINTS ${LIBELF_DIR}
    PATH_SUFFIXES include include/libelf
    )

find_library(LibElf_LIBRARY_SHARED NAMES elf
    HINTS $ENV{LIBELF_DIR}
    HINTS ${LIBELF_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(LibElf_LIBRARY_STATIC NAMES libelf.a
    HINTS $ENV{LIBELF_DIR}
    HINTS ${LIBELF_DIR}
    PATH_SUFFIXES lib lib64
    )



find_package_handle_standard_args(
    LibElf DEFAULT_MSG
    LibElf_LIBRARY_SHARED LibElf_LIBRARY_STATIC
    LibElf_INCLUDE_DIR
    )

set(LibElf_SHARED_LIBRARIES ${LibElf_LIBRARY_SHARED})
set(LibElf_STATIC_LIBRARIES ${LibElf_LIBRARY_STATIC})
set(LibElf_INCLUDE_DIRS ${LibElf_INCLUDE_DIR})

GET_FILENAME_COMPONENT(LibElf_LIB_DIR ${LibElf_LIBRARY_SHARED} PATH )
GET_FILENAME_COMPONENT(LibElf_DIR ${LibElf_INCLUDE_DIR} PATH )
#message(STATUS "LibElf LibElf_SHARED_LIBRARIES: " ${LibElf_SHARED_LIBRARIES})
#message(STATUS "LibElf LibElf_STATIC_LIBRARIES: " ${LibElf_STATIC_LIBRARIES})
#message(STATUS "LibElf LibElf_INCLUDE_DIR: " ${LibElf_INCLUDE_DIR})
#message(STATUS "LibElf LibElf_LIB_DIR: " ${LibElf_LIB_DIR})
message(STATUS "LibElf found: " ${LIBELF_FOUND})
message(STATUS "LibElf location: " ${LibElf_DIR})


mark_as_advanced(
            LibElf_LIBRARY_SHARED 
            LibElf_LIBRARY_STATIC
            LibElf_INCLUDE_DIR
            LibElf_LIB_DIR
            )
