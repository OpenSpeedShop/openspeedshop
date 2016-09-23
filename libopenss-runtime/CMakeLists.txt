################################################################################
# Copyright (c) 2014-2016 Krell Institute. All Rights Reserved.
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
        ${CMAKE_CURRENT_BINARY_DIR}/offline.h
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

if (USE_EXPLICIT_TLS)
    LIST(APPEND OFFLINE_SOURCES OpenSS_TLS.c)
endif()

if (BUILD_TARGETED_BGQ)
    LIST(APPEND RT_SOURCES OpenSS_BGQNodeName.c)
endif()

if (BUILD_TARGETED_BGP)
    LIST(APPEND RT_SOURCES OpenSS_BGPNodeName.c)
endif()

if (BUILD_TARGETED_BGL)
    LIST(APPEND RT_SOURCES OpenSS_BGLNodeName.c)
endif()

set(OFFLINE_SOURCES
	offline/OpenSS_GetDLInfo.c
	offline/OpenSS_InitializeParameters.c
	offline/openss_send.c
	offline/offline_monitor.c
	offline/OpenSS_Monitor.h
	offline/OpenSS_Offline.h
    )

if (USE_EXPLICIT_TLS)
    LIST(APPEND OFFLINE_SOURCES OpenSS_TLS.c)
endif()


set(UNWIND_SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/OpenSS_DataHeader.h
	OpenSS_GetStackTraceFromContext.c
	UnwindAPI.h
    )

set(PAPI_SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/OpenSS_DataHeader.h
        ${CMAKE_CURRENT_BINARY_DIR}/OpenSS_Papi_Events.h
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
  
if (CMAKE_HAVE_PTHREAD_CREATE)
   add_definitions(
	-DHAVE_TARGET_POSIX_THREADS
	)
endif()
if (HAVE_DLOPEN)
   add_definitions(
	-DHAVE_TARGET_DLOPEN
	)
endif()
if (HAVE_FORK)
   add_definitions(
	-DHAVE_TARGET_FORK
	)
endif()

if (HAVE_PERSONALITY)
    target_include_directories(openss-runtime PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/libopenss-runtime
        ${LibMonitor_INCLUDE_DIRS}
        ${LibUnwind_INCLUDE_DIRS}
        ${Personality_INCLUDE_DIRS}
    )
else()
    target_include_directories(openss-runtime PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/libopenss-runtime
        ${LibMonitor_INCLUDE_DIRS}
        ${LibUnwind_INCLUDE_DIRS}
    )
endif()

target_link_libraries(openss-runtime
    rt
    pthread
    ${CMAKE_DL_LIBS}
    )

set_target_properties(openss-runtime PROPERTIES VERSION 1.1.0)
# Add the tls defines to the compile line for this target build
set_target_properties(openss-runtime PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES}")

#set_target_properties(openss-runtime PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_BINARY_DIR}/offline.h:${CMAKE_CURRENT_BINARY_DIR}/OpenSS_DataHeader.h")

install(TARGETS openss-runtime
    ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/offline.h
	${CMAKE_CURRENT_BINARY_DIR}/OpenSS_DataHeader.h
	DESTINATION include/openspeedshop
    )

if(LIBMONITOR_FOUND)
    add_library(openss-runtime-offline STATIC
        ${CMAKE_CURRENT_BINARY_DIR}/offline.h
        ${CMAKE_CURRENT_BINARY_DIR}/offline.c
	${RT_SOURCES}
	${OFFLINE_SOURCES}
    )

    if (HAVE_PERSONALITY)
        target_include_directories(openss-runtime-offline PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/libopenss-runtime
        ${PROJECT_SOURCE_DIR}/libopenss-runtime/offline
        ${LibMonitor_INCLUDE_DIRS}
        ${Personality_INCLUDE_DIRS}
        )
    else()
        target_include_directories(openss-runtime-offline PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/libopenss-runtime
        ${PROJECT_SOURCE_DIR}/libopenss-runtime/offline
        ${LibMonitor_INCLUDE_DIRS}
        )
    endif()

    add_definitions(
	-DOPENSS_OFFLINE
    )
    target_link_libraries(openss-runtime-offline
	rt
	pthread
	${LibMonitor_SHARED_LIBRARIES}
	${CMAKE_DL_LIBS}
    )

    # Add the tls defines to the compile line for this target build
    set_target_properties(openss-runtime-offline PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES}")

    install(TARGETS openss-runtime-offline
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )

    add_custom_command(
        POST_BUILD
        OUTPUT ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/openspeedshop/offline_monitor.o
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/openspeedshop
        COMMAND ar -x libopenss-runtime-offline.a
        COMMAND cp offline_monitor.c.o ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/openspeedshop/offline_monitor.o
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libopenss-runtime-offline.a
        COMMENT "Copying offline_monitor.o to the install directory for static linking via osslink..."
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} 
    )
    add_custom_target(runofflinemonitor ALL DEPENDS ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/openspeedshop/offline_monitor.o)

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

if(PAPI_FOUND)
    add_library(openss-runtime-papi STATIC
	${PAPI_SOURCES}
    )

    target_include_directories(openss-runtime-papi PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${Papi_INCLUDE_DIRS}
    )

    target_link_libraries(openss-runtime-papi
	${Papi_SHARED_LIBRARIES}
	rt
	pthread
	${CMAKE_DL_LIBS}
    )

    # Add the tls defines to the compile line for this target build
    set_target_properties(openss-runtime-papi PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES}")

    install(TARGETS openss-runtime-papi
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

# If runtime_only is true then we have a targetted papi installation, so do
# not call papi_avail. If we have a runtime_dir specified then we have a
# targetted papi specified as well or could.  Might want to check on this.  On
# some heterogeneous machines the only papi installation available is the
# compute node version.  So, then we do not want to call papa_avail from the
# front-end node.

if(PAPI_FOUND AND RUNTIME_ONLY MATCHES "false" AND HAVE_RUNTIME_DIR MATCHES "false")
    add_custom_command(
       POST_BUILD
       OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/OpenSS_Papi_Events.h
       COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/create_events_include ${CMAKE_CURRENT_SOURCE_DIR}/OpenSS_Papi_Events.h ${PAPI_DIR}
       DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/create_events_include
       COMMENT "Generating OpenSS_Papi_Events.h..."
    )

else()
    add_custom_command(
       POST_BUILD
       OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/OpenSS_Papi_Events.h
       COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/create_events_include ${CMAKE_CURRENT_SOURCE_DIR}/OpenSS_Papi_Events.h
       DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/create_events_include
       COMMENT "Generating OpenSS_Papi_Events.h..."
    )
endif()

if(LIBUNWIND_FOUND)

    # For ARM based systems (arm and aarch64) these compiler definitions must be added
    # See: https://wiki.linaro.org/KenWerner/Sandbox/libunwind for additional information
    # And/Or: https://archive.fosdem.org/2015/schedule/event/arm_perf/
    if (RUNTIME_PLATFORM MATCHES "arm")
        add_definitions(
        	-fasynchronous-unwind-tables
        	-funwind-tables
        )
    endif()

    add_library(openss-runtime-unwind STATIC
	${UNWIND_SOURCES}
    )

    target_include_directories(openss-runtime-unwind PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${LibUnwind_INCLUDE_DIRS}
    )
    target_link_libraries(openss-runtime-unwind
	${LibUnwind_STATIC_LIBRARIES}
	${CMAKE_DL_LIBS}
    )

    # Add the tls defines to the compile line for this target build
    set_target_properties(openss-runtime-unwind PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES};${LibUnwind_DEFINES}")

    install(TARGETS openss-runtime-unwind
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

endif()

if(BINUTILS_FOUND)
    add_library(openss-runtime-binutils STATIC
	${BINUTILS_SOURCES}
    )
    target_include_directories(openss-runtime-binutils PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${Binutils_INCLUDE_DIRS}
    )
    target_link_libraries(openss-runtime-binutils
	${Bfd_STATIC_LIBRARIES}
	${Iberty_STATIC_LIBRARIES}
	${CMAKE_DL_LIBS}
    )
    install(TARGETS openss-runtime-binutils
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
    )

    # Work around problem defined by:
    # https://stackoverflow.com/questions/11748035/binutils-bfd-h-wants-config-h-now
    # Also - Add the tls defines to the compile line for this target build
    set_target_properties(openss-runtime-binutils PROPERTIES COMPILE_DEFINITIONS "PACKAGE=1;PACKAGE_VERSION=1;${TLS_DEFINES}")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

endif()

file(COPY OpenSS_DataHeader.x DESTINATION .)
