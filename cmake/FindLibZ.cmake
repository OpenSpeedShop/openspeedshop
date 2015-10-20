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

find_library(LibZ_LIBRARY_SHARED NAMES z
    HINTS $ENV{LibZ_DIR}
    HINTS ${LibZ_DIR}
    PATH_SUFFIXES lib lib64
    )

find_package_handle_standard_args(
    LibZ DEFAULT_MSG
    LibZ_LIBRARY_SHARED
    )
set(LibZ_SHARED_LIBRARIES ${LibZ_LIBRARY_SHARED})

GET_FILENAME_COMPONENT(LibZ_LIB_DIR ${LibZ_LIBRARY_SHARED} PATH )

mark_as_advanced(
    LibZ_LIB_DIR
    LibZ_DIR
    LibZ_LIBRARY_SHARED
)

message(STATUS "Open Trace Format Components found ${LibZ_FOUND}")

