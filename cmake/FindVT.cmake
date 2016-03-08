################################################################################
# Copyright (c) 2015 Krell Institute. All Rights Reserved.
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


find_path(VT_INCLUDE_DIR
    NAMES vt_user.h
    HINTS $ENV{VT_DIR}
    HINTS ${VT_DIR}
    PATH_SUFFIXES include
    )

find_library(VT_LIBRARY_MPI_STATIC NAMES libvt.mpi.a
    HINTS $ENV{VT_DIR}
    HINTS ${VT_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(VT_LIBRARY_STATIC NAMES libvt.a
    HINTS $ENV{VT_DIR}
    HINTS ${VT_DIR}
    PATH_SUFFIXES lib lib64
    )

find_package_handle_standard_args(
    VT DEFAULT_MSG
    VT_INCLUDE_DIR VT_LIBRARY_STATIC VT_LIBRARY_MPI_STATIC
    )

set(VT_STATIC_LIBRARIES ${VT_LIBRARY_STATIC})
set(VT_STATIC_MPI_LIBRARIES ${VT_LIBRARY_MPI_STATIC})

set(VT_INCLUDE_DIRS ${VT_INCLUDE_DIR})

GET_FILENAME_COMPONENT(VT_LIB_DIR ${VT_LIBRARY_STATIC} PATH )
GET_FILENAME_COMPONENT(VT_DIR ${VT_INCLUDE_DIR} PATH )

mark_as_advanced(
    VT_LIB_DIR
    VT_DIR
    VT_LIBRARY_STATIC
    VT_LIBRARY_MPI_STATIC
    VT_INCLUDE_DIR
)
message(STATUS "Vampirtrace found ${VT_FOUND}")
message(STATUS "Vampirtrace VT_LIB_DIR=${VT_LIB_DIR}")

