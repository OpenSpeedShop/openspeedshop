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


find_path(OTF_INCLUDE_DIR
    NAMES otf.h
    HINTS $ENV{OTF_DIR}
    HINTS ${OTF_DIR}
    PATH_SUFFIXES include
    )

find_library(OTF_LIBRARY_STATIC NAMES libotf.a
    HINTS $ENV{OTF_DIR}
    HINTS ${OTF_DIR}
    PATH_SUFFIXES lib lib64
    )

find_package_handle_standard_args(
    OTF DEFAULT_MSG
    OTF_INCLUDE_DIR OTF_LIBRARY_STATIC
    )

set(OTF_STATIC_LIBRARIES ${OTF_LIBRARY_STATIC})

set(OTF_INCLUDE_DIRS ${OTF_INCLUDE_DIR})

GET_FILENAME_COMPONENT(OTF_LIB_DIR ${OTF_LIBRARY_STATIC} PATH )
GET_FILENAME_COMPONENT(OTF_DIR ${OTF_INCLUDE_DIR} PATH )

mark_as_advanced(
    OTF_LIB_DIR
    OTF_DIR
    OTF_LIBRARY_STATIC
    OTF_INCLUDE_DIR
)

message(STATUS "Open Trace Format Components found ${OTF_FOUND}")

