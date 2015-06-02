################################################################################
# Copyright (c) 2012-2015 Krell Institute. All Rights Reserved.
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

find_library(KRELL_BASE_LIBRARY NAMES libkrell-base.so
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_path(KRELL_BASE_INCLUDE_DIR KrellInstitute/Base/Address.hpp
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES include
    )

find_package_handle_standard_args(
    Krell-Base DEFAULT_MSG
    KRELL_BASE_LIBRARY KRELL_BASE_INCLUDE_DIR
    )

set(KRELL_BASE_LIBRARIES ${KRELL_BASE_LIBRARY})
set(KRELL_BASE_INCLUDE_DIRS ${KRELL_BASE_INCLUDE_DIR})

mark_as_advanced(KRELL_BASE_LIBRARY KRELL_BASE_INCLUDE_DIR)
