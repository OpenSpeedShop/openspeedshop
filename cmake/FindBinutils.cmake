################################################################################
# Copyright (c) 2014 Krell Institute. All Rights Reserved.
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


find_path(Binutils_INCLUDE_DIR
    NAMES bfd.h
    PATHS /usr /usr/local
    HINTS $ENV{BINUTILS_ROOT}
    HINTS ${BINUTILS_ROOT}
    PATH_SUFFIXES include
    )

find_path(Binutils_Iberty_INCLUDE_DIR
    NAMES libiberty.h
    PATHS /usr /usr/local
    HINTS $ENV{BINUTILS_ROOT}
    HINTS ${BINUTILS_ROOT}
    PATH_SUFFIXES include include/libiberty
    )

find_library(Bfd_LIBRARY_SHARED NAMES bfd
    HINTS $ENV{BINUTILS_ROOT}
    HINTS ${BINUTILS_ROOT}
    PATHS /usr /usr/local
    PATH_SUFFIXES lib lib64
    )

find_library(Iberty_LIBRARY_SHARED NAMES iberty
    HINTS $ENV{BINUTILS_ROOT}
    HINTS ${BINUTILS_ROOT}
    PATHS /usr /usr/local
    PATH_SUFFIXES lib lib64
    )

find_library(IbertyPIC_LIBRARY_SHARED NAMES iberty_pic
    HINTS $ENV{BINUTILS_ROOT}
    HINTS ${BINUTILS_ROOT}
    PATHS /usr /usr/local
    PATH_SUFFIXES lib lib64
    )

find_library(Bfd_LIBRARY_STATIC NAMES libbfd.a
    HINTS $ENV{BINUTILS_ROOT}
    HINTS ${BINUTILS_ROOT}
    PATHS /usr /usr/local
    PATH_SUFFIXES lib lib64
    )

find_library(Iberty_LIBRARY_STATIC NAMES libiberty.a
    HINTS $ENV{BINUTILS_ROOT}
    HINTS ${BINUTILS_ROOT}
    PATHS /usr /usr/local
    PATH_SUFFIXES lib lib64
    )



find_package_handle_standard_args(
    Binutils DEFAULT_MSG
    Bfd_LIBRARY_SHARED Bfd_LIBRARY_STATIC
    Iberty_LIBRARY_SHARED Iberty_LIBRARY_STATIC
    Binutils_INCLUDE_DIR
    )

set(Bfd_SHARED_LIBRARIES ${Bfd_LIBRARY_SHARED})
set(Bfd_STATIC_LIBRARIES ${Bfd_LIBRARY_STATIC})
set(Iberty_SHARED_LIBRARIES ${Iberty_LIBRARY_SHARED})
set(IbertyPIC_SHARED_LIBRARIES ${IbertyPIC_LIBRARY_SHARED})
set(Iberty_STATIC_LIBRARIES ${Iberty_LIBRARY_STATIC})
set(Binutils_INCLUDE_DIRS ${Binutils_INCLUDE_DIR})
set(Binutils_Iberty_INCLUDE_DIRS ${Binutils_Iberty_INCLUDE_DIR})

message(STATUS "Bfd_SHARED_LIBRARIES: " ${Bfd_LIBRARY_SHARED})
message(STATUS "Bfd_STATIC_LIBRARIES: " ${Bfd_LIBRARY_STATIC})
message(STATUS "Iberty_SHARED_LIBRARIES: " ${Iberty_LIBRARY_SHARED})
message(STATUS "IbertyPIC_SHARED_LIBRARIES: " ${IbertyPIC_LIBRARY_SHARED})
message(STATUS "Iberty_STATIC_LIBRARIES: " ${Iberty_LIBRARY_STATIC})
message(STATUS "Binutils Binutils_INCLUDE_DIR: " ${Binutils_INCLUDE_DIR})
message(STATUS "Binutils Binutils_Iberty_INCLUDE_DIR: " ${Binutils_Iberty_INCLUDE_DIR})
message(STATUS "Binutils Binutils_FOUND: " ${BINUTILS_FOUND})

GET_FILENAME_COMPONENT(Binutils_LIB_DIR ${Bfd_LIBRARY_SHARED} PATH )

message(STATUS "Binutils Binutils_LIB_DIR: " ${Binutils_LIB_DIR})


mark_as_advanced(
            Bfd_LIBRARY_SHARED 
            Bfd_LIBRARY_STATIC 
            Iberty_LIBRARY_SHARED
            IbertyPIC_LIBRARY_SHARED
            Iberty_LIBRARY_STATIC
            Binutils_INCLUDE_DIR
            Binutils_LIB_DIR
            )
