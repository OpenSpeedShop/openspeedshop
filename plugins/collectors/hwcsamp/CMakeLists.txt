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

set(RT_SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/blobs.h
        ${CMAKE_CURRENT_BINARY_DIR}/blobs.c
	runtime.c
    )
set(PLUGIN_SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/blobs.h
        ${CMAKE_CURRENT_BINARY_DIR}/blobs.c
        HWCSampCollector.hxx HWCSampCollector.cxx
        HWCSampEvents.h HWCSampDetail.hxx
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/blobs.h
        ${CMAKE_CURRENT_BINARY_DIR}/blobs.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/blobs.h
        ${CMAKE_CURRENT_BINARY_DIR}/blobs.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/blobs.c
        ${CMAKE_CURRENT_BINARY_DIR}/blobs.x
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/blobs.x
    )


add_definitions(
    -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
)
  

# Do not build if only creating runtimes (RUNTIME_ONLY == true)
if (RUNTIME_ONLY MATCHES "false")

    add_library(hwcsamp MODULE
	${PLUGIN_SOURCES}
    )

    target_include_directories(hwcsamp PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${PROJECT_SOURCE_DIR}/libopenss-framework
	${CMAKE_CURRENT_BINARY_DIR}/../../../libopenss-runtime
    )

    target_link_libraries(hwcsamp
	pthread
	openss-framework
	${CMAKE_DL_LIBS}
    )

    set_target_properties(hwcsamp PROPERTIES PREFIX "")

    install(TARGETS hwcsamp
	LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )

endif()

if (INSTRUMENTOR MATCHES "offline")

    add_definitions(
	-DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
	-DOPENSS_OFFLINE=1
    )

    add_library(hwcsamp-rt-offline SHARED
	${RT_SOURCES}
	offline.c
    )

    target_include_directories(hwcsamp-rt-offline PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${PROJECT_BUILD_DIR}/libopenss-runtime
	${PROJECT_SOURCE_DIR}/libopenss-runtime/offline
	${LibMonitor_INCLUDE_DIRS}
	${Papi_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}/../../../libopenss-runtime
    )

    target_link_libraries(hwcsamp-rt-offline
	${CMAKE_DL_LIBS}
	openss-runtime-papi
	${Papi_SHARED_LIBRARIES}
	rt
	pthread
	-Wl,-whole-archive
	openss-runtime-offline
	-Wl,-no-whole-archive
    )

    set_target_properties(hwcsamp-rt-offline PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES}")

    install(TARGETS hwcsamp-rt-offline
	LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )

    set_target_properties(hwcsamp-rt-offline PROPERTIES PREFIX "")

    # Create and install static lib version
    add_library(hwcsamp-offline STATIC
	${RT_SOURCES}
	offline.c
    )

    target_include_directories(hwcsamp-offline PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${PROJECT_BUILD_DIR}/libopenss-runtime
	${PROJECT_SOURCE_DIR}/libopenss-runtime/offline
	${LibMonitor_INCLUDE_DIRS}
	${Papi_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}/../../../libopenss-runtime
    )

    target_link_libraries(hwcsamp-offline
	${CMAKE_DL_LIBS}
	openss-runtime-papi
	${Papi_SHARED_LIBRARIES}
	rt
	pthread
        -Wl,-whole-archive
	openss-runtime-offline
        -Wl,-no-whole-archive
    )

    set_target_properties(hwcsamp-offline PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES}") 

    install(TARGETS hwcsamp-offline
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )

endif() # endif for INSTRUMENTOR MATCHES offline

file(COPY blobs.x DESTINATION .)
