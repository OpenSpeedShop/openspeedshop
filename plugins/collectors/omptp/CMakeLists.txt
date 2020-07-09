################################################################################
# Copyright (c) 2016 Krell Institute. All Rights Reserved.
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

set(PLUGIN_SOURCES
    OmptPDetail.hxx
    OmptPCollector.hxx OmptPCollector.cxx
)

add_definitions(
    -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
)

add_library(omptp MODULE
    ${PLUGIN_SOURCES}
)

target_include_directories(omptp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CBTF_MESSAGES_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/libopenss-framework
)

target_link_libraries(omptp
    pthread
    openss-framework
    ${CBTF_KRELL_MESSAGES_BASE_SHARED_LIBRARY}
    ${CBTF_KRELL_MESSAGES_PERFDATA_SHARED_LIBRARY}
    ${CMAKE_DL_LIBS}
)

set_target_properties(omptp PROPERTIES PREFIX "")

install(TARGETS omptp
    LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
)

