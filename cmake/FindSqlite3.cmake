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


find_path(Sqlite3_INCLUDE_DIR
    NAMES sqlite3.h
    HINTS $ENV{SQLITE3_DIR}
    HINTS ${SQLITE3_DIR}
    PATH_SUFFIXES include
    )

find_library(Sqlite3_LIBRARY_SHARED NAMES sqlite3
    HINTS $ENV{SQLITE3_DIR}
    HINTS ${SQLITE3_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(Sqlite3_LIBRARY_STATIC NAMES libsqlite3.a
    HINTS $ENV{SQLITE3_DIR}
    HINTS ${SQLITE3_DIR}
    PATH_SUFFIXES lib lib64
    )



find_package_handle_standard_args(
    Sqlite3 DEFAULT_MSG
    Sqlite3_LIBRARY_SHARED
    Sqlite3_INCLUDE_DIR
    )

set(Sqlite3_SHARED_LIBRARIES ${Sqlite3_LIBRARY_SHARED})
set(Sqlite3_STATIC_LIBRARIES ${Sqlite3_LIBRARY_STATIC})
set(Sqlite3_INCLUDE_DIRS ${Sqlite3_INCLUDE_DIR})


GET_FILENAME_COMPONENT(Sqlite3_LIB_DIR ${Sqlite3_LIBRARY_SHARED} PATH )
GET_FILENAME_COMPONENT(Sqlite3_DIR ${Sqlite3_INCLUDE_DIR} PATH )

#message(STATUS "Sqlite3 Sqlite3_SHARED_LIBRARIES: " ${Sqlite3_SHARED_LIBRARIES})
#message(STATUS "Sqlite3 Sqlite3_STATIC_LIBRARIES: " ${Sqlite3_STATIC_LIBRARIES})
#message(STATUS "Sqlite3 Sqlite3_INCLUDE_DIR: " ${Sqlite3_INCLUDE_DIR})
#message(STATUS "Sqlite3 Sqlite3_LIB_DIR: " ${Sqlite3_LIB_DIR})
message(STATUS "Sqlite3 found: " ${SQLITE3_FOUND})
message(STATUS "Sqlite3 location: " ${Sqlite3_DIR})


mark_as_advanced(
            Sqlite3_LIBRARY_SHARED 
            Sqlite3_INCLUDE_DIR
            Sqlite3_LIB_DIR
            )
