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

set(PLUGIN_SOURCES
    MPIPDetail.hxx MPIPTraceableFunctions.h
    MPIPCollector.hxx MPIPCollector.cxx
)

add_library(mpip MODULE
    ${PLUGIN_SOURCES}
)

target_include_directories(mpip PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CBTF_MESSAGES_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/libopenss-runtime
    ${PROJECT_SOURCE_DIR}/libopenss-framework
    ${CMAKE_CURRENT_BINARY_DIR}/../../../libopenss-runtime
)

target_link_libraries(mpip
    pthread
    openss-framework
    ${CBTF_MESSAGES_LIBRARIES}
    ${CMAKE_DL_LIBS}
)

set_target_properties(mpip PROPERTIES PREFIX "")

install(TARGETS mpip
    LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
)

