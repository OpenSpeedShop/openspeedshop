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


find_path(Mpt_INCLUDE_DIR
    NAMES mpi.h
    HINTS $ENV{MPT_DIR}
    HINTS ${MPT_DIR}
    PATH_SUFFIXES include 
    )

find_library(Mpt_LIBRARY_SHARED NAMES mpi
    HINTS $ENV{MPT_DIR}
    HINTS ${MPT_DIR}
    PATH_SUFFIXES lib lib64
    )

find_package_handle_standard_args(
    Mpt DEFAULT_MSG
    Mpt_LIBRARY_SHARED
    Mpt_INCLUDE_DIR
    )

set(Mpt_SHARED_LIBRARIES ${Mpt_LIBRARY_SHARED})
set(Mpt_INCLUDE_DIRS ${Mpt_INCLUDE_DIR})
set(Mpt_DEFINES "")
set(SGIMPT_DEFINES "")

GET_FILENAME_COMPONENT(Mpt_LIB_DIR ${Mpt_LIBRARY_SHARED} PATH )
GET_FILENAME_COMPONENT(Mpt_DIR ${Mpt_INCLUDE_DIR} PATH )
#message(STATUS "Mpt Mpt_SHARED_LIBRARIES: " ${Mpt_SHARED_LIBRARIES})
#message(STATUS "Mpt Mpt_INCLUDE_DIR: " ${Mpt_INCLUDE_DIR})
#message(STATUS "Mpt Mpt_LIB_DIR: " ${Mpt_LIB_DIR})
message(STATUS "Mpt found: " ${MPT_FOUND})
message(STATUS "Mpt location: " ${Mpt_LIB_DIR})

if (MPT_FOUND)
    set(Mpt_DEFINES "HAVE_MPT=1")
    set(SGIMPT_DEFINES "SGI_MPT")
else()
    set(Mpt_DEFINES "")
    set(SGIMPT_DEFINES "")
endif()

if(MPT_FOUND)
  set(MPT_HEADER ${Mpt_INCLUDE_DIR}/mpi.h )

  set(MPT_MAJ_VERS "2")
  set(MPT_SUB_VERS "0")
  
# tmp_value=`grep MPT_VERSION "$MPT_HEADER" 2>/dev/null | grep 2.11 2>/dev/null`
#   if test -n "$tmp_value"; then
#      MPT_SUB_VERS=11
#   fi
#   tmp_value=`grep MPT_VERSION "$MPT_HEADER" 2>/dev/null | grep 2.10 2>/dev/null`
#   if test -n "$tmp_value"; then
#      MPT_SUB_VERS=10
#   fi
#   tmp_value=`grep MPT_VERSION "$MPT_HEADER" 2>/dev/null | grep 2.08 2>/dev/null`
#   if test -n "$tmp_value"; then
#      MPT_SUB_VERS=8
#   fi
#
endif()

mark_as_advanced(
            Mpt_LIBRARY_SHARED 
            Mpt_INCLUDE_DIR
            Mpt_LIB_DIR
            )
