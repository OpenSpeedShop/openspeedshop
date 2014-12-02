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

include(CheckIncludeFile)

check_include_file("stdint.h" STDINT_H_FOUND)
if (STDINT_H_FOUND)
    add_definitions(-DHAVE_STDINT_H=1)
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/../libopenss-runtime
    ${Sqlite3_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/libopenss-framework
    ${PROJECT_BUILD_DIR}/libopenss-runtime
    ${LTDL_INCLUDE_DIR}
    )

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
        OpenSS_DataHeader.c
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
	offline/Instrumentor.cxx
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
	offline.c
	BFDSymbols.hxx
	BFDSymbols.cxx
    )

set(OFFLINE_SOURCES
    )

add_library(openss-framework SHARED
	${FW_SOURCES}
    )

add_definitions(
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )
  

target_link_libraries(openss-framework
    ${Sqlite3_SHARED_LIBRARIES}
    ${Iberty_STATIC_LIBRARIES}
    ${LTDL_LIBRARY_SHARED}
    ${CMAKE_DL_LIBS}
    )


set_target_properties(openss-framework PROPERTIES VERSION 1.1.0)

install(TARGETS openss-framework
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    )

if(BINUTILS_FOUND)

    #file(COPY ${CMAKE_CURRENT_BINARY_DIR}/../libopenss-runtime/offline.h DESTINATION ./offline)

        #${CMAKE_CURRENT_BINARY_DIR}/../libopenss-runtime/offline.c
    add_library(openss-framework-binutils SHARED
	${BINUTILS_SOURCES}
	OfflineExperiment.hxx
	OfflineExperiment.cxx
    )

    include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/offline
	${Binutils_INCLUDE_DIR}
	${Binutils_Iberty_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-framework
        ${CMAKE_CURRENT_BINARY_DIR}/../libopenss-framework
    )

    target_link_libraries(openss-framework-binutils
	${Bfd_LIBRARY_SHARED}
	${Iberty_LIBRARY_STATIC}
	${CMAKE_DL_LIBS}
    )


    set_target_properties(openss-framework-binutils PROPERTIES VERSION 1.1.0)

    install(TARGETS openss-framework-binutils
	LIBRARY DESTINATION lib${LIB_SUFFIX}
    )

endif()
