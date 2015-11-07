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

find_path(Personality_INCLUDE_DIR
    NAMES spi/include/kernel/cnk/kernel_impl.h
    HINTS $ENV{PERSONALITY_DIR}
    HINTS ${PERSONALITY_DIR}
    PATH_SUFFIXES 
    )

find_package_handle_standard_args(
    Personality DEFAULT_MSG
    Personality_INCLUDE_DIR
    )

set(Personality_INCLUDE_DIRS ${Personality_INCLUDE_DIR} ${Personality_INCLUDE_DIR}/spi/include ${Personality_INCLUDE_DIR}/spi/include/kernel/cnk ${Personality_INCLUDE_DIR}/spi/include/kernel )

GET_FILENAME_COMPONENT(Personality_DIR ${Personality_INCLUDE_DIR} PATH )

mark_as_advanced(
    Personality_DIR
    Personality_INCLUDE_DIR
)

message(STATUS "Personality Components found ${PERSONALITY_FOUND}")




