################################################################################
# Copyright (c) 2014 Krell Institute. All Rights Reserved.
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
	Assert.h
        ${CMAKE_CURRENT_BINARY_DIR}/OpenSS_DataHeader.h
        ${CMAKE_CURRENT_BINARY_DIR}/OpenSS_DataHeader.c
	OpenSS_DecodeParameters.c
	OpenSS_EncodeParameters.c
	OpenSS_GetAddressOfFunction.c
	OpenSS_GetExecutablePath.c
	OpenSS_GetPCFromContext.c
	OpenSS_InitializeDataHeader.c
	OpenSS_UpdatePCData.c
	OpenSS_UpdateHWCPCData.c
	OpenSS_GetTime.c
	OpenSS_Send.c
	OpenSS_SendToFile.c
	OpenSS_Timer.c
	OpenSS_FPEHandler.c
	OpenSS_SetPCInContext.c
	OpenSS_Path_From_Pid.c
	RuntimeAPI.h
    )

set(OFFLINE_SOURCES
	offline/OpenSS_GetDLInfo.c
	offline/OpenSS_InitializeParameters.c
	offline/openss_send.c
	offline/offline_monitor.c
	offline/OpenSS_Monitor.h
	offline/OpenSS_Offline.h
    )

set(UNWIND_SOURCES
	OpenSS_GetStackTraceFromContext.c
	UnwindAPI.h
    )

set(PAPI_SOURCES
	OpenSS_Overflow.c
	PapiAPI.c
	PapiAPI.h
    )

set(BINUTILS_SOURCES
	BinutilsAPI.h 
        OpenSS_GetInstrLength.c
    )

add_library(openss-runtime STATIC
	${RT_SOURCES}
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/OpenSS_DataHeader.h
        ${CMAKE_CURRENT_BINARY_DIR}/OpenSS_DataHeader.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/OpenSS_DataHeader.h
        ${CMAKE_CURRENT_BINARY_DIR}/OpenSS_DataHeader.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/OpenSS_DataHeader.c
        ${CMAKE_CURRENT_BINARY_DIR}/OpenSS_DataHeader.x
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/OpenSS_DataHeader.x
    )

if(LIBMONITOR_FOUND)
  add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/offline.h
        ${CMAKE_CURRENT_BINARY_DIR}/offline.c
    COMMAND rpcgen -h -o
        ${CMAKE_CURRENT_BINARY_DIR}/offline.h
        ${CMAKE_CURRENT_BINARY_DIR}/offline.x
    COMMAND rpcgen -c -o
        ${CMAKE_CURRENT_BINARY_DIR}/offline.c
        ${CMAKE_CURRENT_BINARY_DIR}/offline.x
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/offline.x
    )
    file(COPY offline/offline.x DESTINATION .)
endif()

add_definitions(
  -D_GNU_SOURCE
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )
  

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/libopenss-runtime
    ${LibMonitor_INCLUDE_DIRS}
    ${LibUnwind_INCLUDE_DIRS}
    )

target_link_libraries(openss-runtime
    rt
    pthread
    ${CMAKE_DL_LIBS}
    )


set_target_properties(openss-runtime PROPERTIES VERSION 1.1.0)
#set_target_properties(openss-runtime PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_BINARY_DIR}/offline.h:${CMAKE_CURRENT_BINARY_DIR}/OpenSS_DataHeader.h")

install(TARGETS openss-runtime
    ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/offline.h
	${CMAKE_CURRENT_BINARY_DIR}/OpenSS_DataHeader.h
	DESTINATION include/openspeedshop
    )
add_definitions(
	-fasynchronous-unwind-tables
	-funwind-tables
    )

if(LIBMONITOR_FOUND)
    include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${PROJECT_SOURCE_DIR}/libopenss-runtime/offline
	${LibMonitor_INCLUDE_DIRS}
    )
    add_definitions(
	-DOPENSS_OFFLINE
    )
    add_library(openss-runtime-offline STATIC
        ${CMAKE_CURRENT_BINARY_DIR}/offline.h
        ${CMAKE_CURRENT_BINARY_DIR}/offline.c
	${RT_SOURCES}
	${OFFLINE_SOURCES}
    )
    target_link_libraries(openss-runtime-offline
	rt
	pthread
	${LibMonitor_SHARED_LIBRARIES}
	${CMAKE_DL_LIBS}
    )
    install(TARGETS openss-runtime-offline
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

if(PAPI_FOUND)
    include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${Papi_INCLUDE_DIRS}
    )
    add_library(openss-runtime-papi STATIC
	${PAPI_SOURCES}
    )
    target_link_libraries(openss-runtime-papi
	papi
	pfm
	rt
	pthread
	${CMAKE_DL_LIBS}
    )
    install(TARGETS openss-runtime-papi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

endif()

if(LIBUNWIND_FOUND)
    add_definitions(${LibUnwind_DEFINES})
    include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${LibUnwind_INCLUDE_DIRS}
    )
    add_library(openss-runtime-unwind STATIC
	${UNWIND_SOURCES}
    )
    target_link_libraries(openss-runtime-unwind
	${LibUnwind_STATIC_LIBRARIES}
	${CMAKE_DL_LIBS}
    )
    install(TARGETS openss-runtime-unwind
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

endif()

if(BINUTILS_FOUND)
    #add_definitions(${LibUnwind_DEFINES})
    include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${Binutils_INCLUDE_DIRS}
    )
    add_library(openss-runtime-binutils STATIC
	${BINUTILS_SOURCES}
    )
    target_link_libraries(openss-runtime-binutils
	${Bfd_STATIC_LIBRARIES}
	${Iberty_STATIC_LIBRARIES}
	${CMAKE_DL_LIBS}
    )
    install(TARGETS openss-runtime-binutils
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

endif()

file(COPY OpenSS_DataHeader.x DESTINATION .)
