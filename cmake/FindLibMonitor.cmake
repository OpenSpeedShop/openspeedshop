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


find_path(LibMonitor_INCLUDE_DIR
    NAMES monitor.h
    HINTS $ENV{LIBMONITOR_DIR}
    HINTS ${LIBMONITOR_DIR}
    PATH_SUFFIXES include
    )

find_library(LibMonitor_LIBRARY_SHARED NAMES monitor
    HINTS $ENV{LIBMONITOR_DIR}
    HINTS ${LIBMONITOR_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(LibMonitor_LIBRARY_STATIC NAMES libmonitor_wrap.a
    HINTS $ENV{LIBMONITOR_DIR}
    HINTS ${LIBMONITOR_DIR}
    PATH_SUFFIXES lib lib64
    )



find_package_handle_standard_args(
    LibMonitor DEFAULT_MSG
    LibMonitor_LIBRARY_SHARED LibMonitor_LIBRARY_STATIC
    LibMonitor_INCLUDE_DIR
    )

set(LibMonitor_SHARED_LIBRARIES ${LibMonitor_LIBRARY_SHARED})
set(LibMonitor_STATIC_LIBRARIES ${LibMonitor_LIBRARY_STATIC})
set(LibMonitor_INCLUDE_DIRS ${LibMonitor_INCLUDE_DIR})


GET_FILENAME_COMPONENT(LibMonitor_LIB_DIR ${LibMonitor_LIBRARY_SHARED} PATH )
GET_FILENAME_COMPONENT(LibMonitor_DIR ${LibMonitor_INCLUDE_DIR} PATH )

#message(STATUS "LibMonitor LibMonitor_SHARED_LIBRARIES: " ${LibMonitor_SHARED_LIBRARIES})
#message(STATUS "LibMonitor LibMonitor_STATIC_LIBRARIES: " ${LibMonitor_STATIC_LIBRARIES})
#message(STATUS "LibMonitor LibMonitor_INCLUDE_DIR: " ${LibMonitor_INCLUDE_DIR})
#message(STATUS "LibMonitor LibMonitor_LIB_DIR: " ${LibMonitor_LIB_DIR})
message(STATUS "LibMonitor found: " ${LIBMONITOR_FOUND})
message(STATUS "LibMonitor location: " ${LibMonitor_DIR})


mark_as_advanced(
            LibMonitor_LIBRARY_SHARED 
            LibMonitor_LIBRARY_STATIC
            LibMonitor_INCLUDE_DIR
            LibMonitor_LIB_DIR
            )
