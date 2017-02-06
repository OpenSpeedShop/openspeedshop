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

include(CheckIncludeFile)

check_include_file("stdint.h" STDINT_H_FOUND)
if (STDINT_H_FOUND)
    add_definitions(-DHAVE_STDINT_H=1)
endif()
check_include_file("stat.h" STAT_H_FOUND)
if (STAT_H_FOUND)
    add_definitions(-DHAVE_STAT_H=1)
endif()
check_include_file("sys/stat.h" SYS_STAT_H_FOUND)
if (SYS_STAT_H_FOUND)
    add_definitions(-DHAVE_SYS_STAT_H=1)
endif()

if(NOT HAS_CXX11_AUTO)
  message(STATUS "No support for C++11 found.")
else()
  message(STATUS "C++11 support found, required flags are: ${CXX11_COMPILER_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_COMPILER_FLAGS}")
endif()


file(COPY ${PROJECT_SOURCE_DIR}/libopenss-runtime/OpenSS_DataHeader.x DESTINATION .)
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
endif()
  file(COPY ${PROJECT_SOURCE_DIR}/libopenss-runtime/offline/offline.x DESTINATION .)
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

set(FW_SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/OpenSS_DataHeader.c
        ${CMAKE_CURRENT_BINARY_DIR}/OpenSS_DataHeader.h
        ${CMAKE_CURRENT_BINARY_DIR}/offline.h
        Address.hxx
        AddressBitmap.hxx AddressBitmap.cxx
        AddressRange.hxx
        AddressSpace.hxx AddressSpace.cxx
        Assert.hxx
        Blob.hxx Blob.cxx
        Collector.hxx Collector.cxx
        CollectorAPI.hxx
        CollectorGroup.hxx CollectorGroup.cxx
        CollectorImpl.hxx CollectorImpl.cxx
        CollectorPluginTable.hxx CollectorPluginTable.cxx
        Database.hxx Database.cxx
        DataBucket.hxx
        DataCache.hxx DataCache.cxx
        DataQueues.hxx DataQueues.cxx
        Entry.hxx Entry.cxx
        EntrySpy.hxx
        Exception.hxx Exception.cxx
        Experiment.hxx Experiment.cxx
        Extent.hxx
        ExtentGroup.hxx ExtentGroup.cxx
        ExtentTable.hxx
        Function.hxx Function.cxx
        FunctionCache.hxx FunctionCache.cxx
        Guard.hxx
        Interval.hxx
        LinkedObject.hxx LinkedObject.cxx
        Lockable.hxx
        Loop.hxx Loop.cxx
        LoopCache.hxx LoopCache.cxx
        Metadata.hxx
        NonCopyable.hxx
        Path.hxx Path.cxx
        PCBuffer.hxx PCBuffer.cxx
        SmartPtr.hxx
        StackTrace.hxx
        Statement.hxx Statement.cxx
        StatementCache.hxx StatementCache.cxx
        SymbolTable.hxx SymbolTable.cxx
        Thread.hxx Thread.cxx
        ThreadGroup.hxx ThreadGroup.cxx
        ThreadName.hxx ThreadName.cxx
        Time.hxx
        TimeInterval.hxx
        ToolAPI.hxx
        OfflineParameters.hxx
        TotallyOrdered.hxx
    )

set(BINUTILS_SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/OpenSS_DataHeader.h
	offline.c
	BFDSymbols.hxx
	BFDSymbols.cxx
    )

if (DYNINSTAPI_FOUND)
set(SYMTABAPI_SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/OpenSS_DataHeader.h
	offline.c
	SymtabAPISymbols.hxx 
	SymtabAPISymbols.cxx
	DyninstSymbols.hxx
	DyninstSymbols.cxx
    )
else()
set(SYMTABAPI_SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/OpenSS_DataHeader.h
	offline.c
	SymtabAPISymbols.hxx 
	SymtabAPISymbols.cxx
    )
endif()

set(OFFLINE_SOURCES  )


add_definitions(
  -D_GNU_SOURCE
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  -DDyninstAPI_VERSION_MAJOR=${DyninstAPI_VERSION_MAJOR}
  )

if(INSTRUMENTOR MATCHES "cbtf")

    set(CBTF_SOURCES
	cbtf/Callbacks.cxx
	cbtf/FEThread.hxx
	cbtf/Frontend.hxx
	cbtf/ThreadTable.cxx
	cbtf/Utility.cxx
	cbtf/Callbacks.hxx
	cbtf/FEThread.cxx
	cbtf/Frontend.cxx
	cbtf/Instrumentor.cxx
	cbtf/ThreadTable.hxx
	cbtf/Utility.hxx
	Instrumentor.hxx
    )

    # CBTF-Krell.cmake now sets CBTF_KRELL_XMLDIR rather than CBTF_TOOLS_XMLDIR.
    # This is for the openss-framework-cbtf target.
    if (BUILD_FE_USING_CN_RUNTIMES MATCHES "true" )
        add_definitions(
           -DCBTF_LIB_DIR="${CBTF_LIB_DIR}"
           -DCBTF_KRELL_LIB_DIR="${CBTF_KRELL_LIBDIR}"
           -DCBTF_KRELL_XMLDIR="${CBTF_KRELL_CN_XMLDIR}"
        )
    else()
        add_definitions(
           -DCBTF_LIB_DIR="${CBTF_LIB_DIR}"
           -DCBTF_KRELL_LIB_DIR="${CBTF_KRELL_LIBDIR}"
           -DCBTF_KRELL_XMLDIR="${CBTF_KRELL_XMLDIR}"
        )
    endif()

    add_library(openss-framework SHARED ${FW_SOURCES})

    # DataQueues.cxx uses DataHeader from cbtf-krell/messages
    # for the cbtf instrumentor.
    target_include_directories(openss-framework PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/../libopenss-runtime
	${CBTF_KRELL_MESSAGES_INCLUDE_DIRS}
	${Sqlite3_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-framework
	${PROJECT_BUILD_DIR}/libopenss-runtime
	${LTDL_INCLUDE_DIR}
    )

    target_link_libraries(openss-framework
	${CBTF_KRELL_MESSAGES_PERFDATA_SHARED_LIBRARY}
	${Sqlite3_SHARED_LIBRARIES}
	${Iberty_STATIC_LIBRARIES}
	${LTDL_LIBRARY_SHARED}
	${CMAKE_DL_LIBS}
    )

    set_target_properties(openss-framework PROPERTIES VERSION 1.1.0)
    set_target_properties(openss-framework PROPERTIES COMPILE_DEFINITIONS "BUILD_CBTF=1;")

    install(TARGETS openss-framework
        LIBRARY DESTINATION lib${LIB_SUFFIX}
    )

    add_library(openss-framework-cbtf SHARED ${CBTF_SOURCES})

    target_include_directories(openss-framework-cbtf PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${Boost_INCLUDE_DIRS}
	${CBTF_INCLUDE_DIRS}
	${CBTF_KRELL_MESSAGES_INCLUDE_DIRS}
	${MRNet_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/libopenss-framework
	${PROJECT_SOURCE_DIR}/libopenss-framework/cbtf
    )

    target_link_libraries(openss-framework-cbtf
        openss-framework-symtabapi
	${CBTF_LIBRARIES}
        ${CBTF_KRELL_CORE_LIBRARIES}
	${CBTF_KRELL_CORE_MRNET_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_THREAD_LIBRARY}
	${XercesC_LIBRARIES}
	${MRNet_LIBRARIES}
	${Iberty_STATIC_LIBRARIES}
	${CMAKE_DL_LIBS}
    )

    set_target_properties(openss-framework-cbtf PROPERTIES VERSION 1.1.0)
    set_target_properties(openss-framework-cbtf PROPERTIES COMPILE_DEFINITIONS
	"BUILD_CBTF=1;${RESOLVE_SYMBOLS_DEFINES};${DyninstAPI_DEFINES}")

    install(TARGETS openss-framework-cbtf
	LIBRARY DESTINATION lib${LIB_SUFFIX}
    )

elseif (INSTRUMENTOR MATCHES "offline")
    add_library(openss-framework SHARED ${FW_SOURCES}
	offline/Instrumentor.cxx
    )

    target_include_directories(openss-framework PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/../libopenss-runtime
        ${Sqlite3_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/libopenss-framework
        ${PROJECT_BUILD_DIR}/libopenss-runtime
        ${LTDL_INCLUDE_DIR}
    )

    target_link_libraries(openss-framework
        ${Sqlite3_SHARED_LIBRARIES}
        ${Iberty_STATIC_LIBRARIES}
        ${LTDL_LIBRARY_SHARED}
        ${CMAKE_DL_LIBS}
	rt
    )

    set_target_properties(openss-framework PROPERTIES VERSION 1.1.0)
    set_target_properties(openss-framework PROPERTIES COMPILE_DEFINITIONS "${RESOLVE_SYMBOLS_DEFINES}")

    install(TARGETS openss-framework
        LIBRARY DESTINATION lib${LIB_SUFFIX}
    )

    if(BINUTILS_FOUND)

        add_library(openss-framework-binutils SHARED
	    ${BINUTILS_SOURCES}
	    OfflineExperiment.hxx
	    OfflineExperiment.cxx
        )

        target_include_directories(openss-framework-binutils PUBLIC
	    ${CMAKE_CURRENT_SOURCE_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}/offline
	    ${Binutils_INCLUDE_DIR}
	    ${Binutils_Iberty_INCLUDE_DIR}
	    ${PROJECT_SOURCE_DIR}/libopenss-framework
            ${CMAKE_CURRENT_BINARY_DIR}/../libopenss-framework
            ${LTDL_INCLUDE_DIR}
        )

        target_link_libraries(openss-framework-binutils
	    ${Bfd_LIBRARY_SHARED}
	    ${Iberty_STATIC_LIBRARIES}
	    ${CMAKE_DL_LIBS}
        )
        # Without this the resulting code from OfflineExperiment.cxx calls the getSymbols function in symtabapi
        if(SYMTABAPI_FOUND)
           if (RESOLVE_SYMBOLS MATCHES "symtabapi")
              # Not using add_definitions(-DOPENSS_USE_SYMTABAPI=1) which is
              # global in nature and overrides the bfd symbol resolution option
              # because the symtabapi code is built instead.
              set(RESOLVE_SYMBOLS_DEFINES "OPENSS_USE_SYMTABAPI=1")
           endif()
        endif()

        set_target_properties(openss-framework-binutils PROPERTIES VERSION 1.1.0)

        # Work around problem defined by:
        # https://stackoverflow.com/questions/11748035/binutils-bfd-h-wants-config-h-now
        set_target_properties(openss-framework-binutils PROPERTIES COMPILE_DEFINITIONS "PACKAGE=1;PACKAGE_VERSION=1;${RESOLVE_SYMBOLS_DEFINES}")

        install(TARGETS openss-framework-binutils
	    LIBRARY DESTINATION lib${LIB_SUFFIX}
        )

    endif()
endif()

if(SYMTABAPI_FOUND)
    add_library(openss-framework-symtabapi SHARED
		${SYMTABAPI_SOURCES}
		OfflineExperiment.hxx
		OfflineExperiment.cxx
    )

    if (DYNINSTAPI_FOUND)
      if(INSTRUMENTOR MATCHES "cbtf")
	target_include_directories(openss-framework-symtabapi PUBLIC
	    ${CMAKE_CURRENT_SOURCE_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}/offline
	    ${Boost_INCLUDE_DIRS}
	    ${SymtabAPI_INCLUDE_DIRS}
	    ${DyninstAPI_INCLUDE_DIRS}
	    ${Binutils_Iberty_INCLUDE_DIR}
	    ${CBTF_KRELL_MESSAGES_INCLUDE_DIRS}
	    ${PROJECT_SOURCE_DIR}/libopenss-framework
            ${CMAKE_CURRENT_BINARY_DIR}/../libopenss-framework
            ${LTDL_INCLUDE_DIR}
	)
	target_link_libraries(openss-framework-symtabapi
	    ${SymtabAPI_LIBRARIES}
	    ${DyninstAPI_LIBRARIES}
	    ${CBTF_KRELL_MESSAGES_EVENTS_SHARED_LIBRARY}
	    ${Iberty_STATIC_LIBRARIES}
	    ${CMAKE_DL_LIBS}
	)
      else()
	target_include_directories(openss-framework-symtabapi PUBLIC
	    ${CMAKE_CURRENT_SOURCE_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}/offline
	    ${Boost_INCLUDE_DIRS}
	    ${SymtabAPI_INCLUDE_DIRS}
	    ${DyninstAPI_INCLUDE_DIRS}
	    ${Binutils_Iberty_INCLUDE_DIR}
	    ${PROJECT_SOURCE_DIR}/libopenss-framework
            ${CMAKE_CURRENT_BINARY_DIR}/../libopenss-framework
            ${LTDL_INCLUDE_DIR}
	)
	target_link_libraries(openss-framework-symtabapi
	    ${SymtabAPI_LIBRARIES}
	    ${DyninstAPI_LIBRARIES}
	    ${Iberty_STATIC_LIBRARIES}
	    ${CMAKE_DL_LIBS}
	)
      endif()
    else()
      if(INSTRUMENTOR MATCHES "cbtf")
	target_include_directories(openss-framework-symtabapi PUBLIC
	    ${CMAKE_CURRENT_SOURCE_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}/offline
	    ${Boost_INCLUDE_DIRS}
	    ${SymtabAPI_INCLUDE_DIRS}
	    ${Binutils_Iberty_INCLUDE_DIR}
	    ${CBTF_KRELL_MESSAGES_INCLUDE_DIRS}
	    ${PROJECT_SOURCE_DIR}/libopenss-framework
	    ${CMAKE_CURRENT_BINARY_DIR}/../libopenss-framework
	    ${LTDL_INCLUDE_DIR}
	)
	target_link_libraries(openss-framework-symtabapi
	    ${SymtabAPI_LIBRARIES}
	    ${CBTF_KRELL_MESSAGES_EVENTS_SHARED_LIBRARY}
	    ${Iberty_STATIC_LIBRARIES}
	    ${CMAKE_DL_LIBS}
	)
      else()
	target_include_directories(openss-framework-symtabapi PUBLIC
	    ${CMAKE_CURRENT_SOURCE_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}
	    ${CMAKE_CURRENT_BINARY_DIR}/offline
	    ${Boost_INCLUDE_DIRS}
	    ${SymtabAPI_INCLUDE_DIRS}
	    ${Binutils_Iberty_INCLUDE_DIR}
	    ${PROJECT_SOURCE_DIR}/libopenss-framework
	    ${CMAKE_CURRENT_BINARY_DIR}/../libopenss-framework
	    ${LTDL_INCLUDE_DIR}
	)
	target_link_libraries(openss-framework-symtabapi
	    ${SymtabAPI_LIBRARIES}
	    ${Iberty_STATIC_LIBRARIES}
	    ${CMAKE_DL_LIBS}
	)
      endif()
    endif()

    set_target_properties(openss-framework-symtabapi PROPERTIES VERSION 1.1.0)

    # Work around problem defined by:
    # https://stackoverflow.com/questions/11748035/binutils-bfd-h-wants-config-h-now
    set_target_properties(openss-framework-symtabapi PROPERTIES COMPILE_DEFINITIONS "PACKAGE=1;PACKAGE_VERSION=1;${RESOLVE_SYMBOLS_DEFINES};${DyninstAPI_DEFINES}")

    install(TARGETS openss-framework-symtabapi
	LIBRARY DESTINATION lib${LIB_SUFFIX}
    )

endif()
