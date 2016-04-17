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

add_definitions(
        -DBINDIR=\"${VT_DIR}/bin\"
        -DDATADIR=\"${VT_DIR}/share/vampirtrace\"
        -DVT_MPI
        -DRFG 
        -DVT_BFDSYMCOLLECT 
        -DVT_MEMHOOK
)

set(RT_SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/blobs.h
        ${CMAKE_CURRENT_BINARY_DIR}/blobs.c
        runtime.h runtime.c
        vt_sync.c vt_otf_trc.c
        vt_mpireq.c vt_mpireg.c vt_mpicom.c
        vt_mpiwrap-offline.c vt_mpireg.c vt_mpicom.c
)

set(PLUGIN_SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/blobs.h
        ${CMAKE_CURRENT_BINARY_DIR}/blobs.c
	MPIOTFDetail.hxx
        MPIOTFCollector.hxx MPIOTFCollector.cxx
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
    add_library(mpiotf MODULE
        ${PLUGIN_SOURCES}
    )

    target_include_directories(mpiotf PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/libopenss-runtime
        ${PROJECT_SOURCE_DIR}/libopenss-framework
        ${CMAKE_CURRENT_BINARY_DIR}/../../../libopenss-runtime
    )

    target_link_libraries(mpiotf
	pthread
	openss-framework
	${CMAKE_DL_LIBS}
    )

    #set_target_properties(mpiotf PROPERTIES VERSION 1.1.0)

    install(TARGETS mpiotf
	LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )
    set_target_properties(mpiotf PROPERTIES PREFIX "")
endif()

if(LIBMONITOR_FOUND)

    add_definitions(
	-DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
	-DOPENSS_OFFLINE=1
	-funwind-tables
	-fasynchronous-unwind-tables
    )

if(HAVE_OPENMPI)

    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.openmpi.a
        COMMAND cp -r ${VT_LIBRARY_MPI_STATIC} ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.openmpi.a 
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.openmpi.a libvt_mpi_a-vt_mpiwrap.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.openmpi.a libvt_mpi_a-vt_sync.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.openmpi.a libvt_mpi_a-vt-otf_trc.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.openmpi.a libvt_mpi_a-vt-mpireq.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.openmpi.a libvt_mpi_a-vt-mpireg.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.openmpi.a libvt_mpi_a-vt-mpicom.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.openmpi.a libvt_mpi_a-vt-fmpiwrap.o
        DEPENDS ${VT_LIBRARY_MPI_STATIC}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    add_custom_target(runvtopenmpi ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.openmpi.a)

    add_library(mpiotf-openmpi-rt-offline SHARED
        ${RT_SOURCES}
	offline.c
    )

    add_dependencies(mpiotf-openmpi-rt-offline runvtopenmpi)

    target_include_directories(mpiotf-openmpi-rt-offline PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${PROJECT_BUILD_DIR}/libopenss-runtime
	${PROJECT_SOURCE_DIR}/libopenss-runtime/offline
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${OpenMPI_INCLUDE_DIRS}
	${VT_INCLUDE_DIRS}
	${OTF_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}/../../../libopenss-runtime
    )

    target_link_libraries(mpiotf-openmpi-rt-offline
	${CMAKE_DL_LIBS}
	rt
	pthread
	openss-runtime-offline
	openss-runtime-unwind
	${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.openmpi.a
	${OTF_LIBRARY_STATIC}
	${LibZ_LIBRARY_SHARED}
	${LibUnwind_LIBRARY_SHARED}
    )

    set_target_properties(mpiotf-openmpi-rt-offline PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES};${LibUnwind_DEFINES};${OpenMPI_DEFINES}") 

    install(TARGETS mpiotf-openmpi-rt-offline
	LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )
    
    # Create and install static library

    set_target_properties(mpiotf-openmpi-rt-offline PROPERTIES PREFIX "")


    add_library(mpiotf-openmpi-offline STATIC
        ${RT_SOURCES}
	offline.c
    )

    target_include_directories(mpiotf-openmpi-offline PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${PROJECT_BUILD_DIR}/libopenss-runtime
	${PROJECT_SOURCE_DIR}/libopenss-runtime/offline
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${OpenMPI_INCLUDE_DIRS}
	${VT_INCLUDE_DIRS}
	${OTF_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}/../../../libopenss-runtime
    )

    target_link_libraries(mpiotf-openmpi-offline
	${CMAKE_DL_LIBS}
	rt
	pthread
	openss-runtime-offline
	openss-runtime-unwind
	${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.openmpi.a
	${OTF_LIBRARY_STATIC}
	${LibZ_LIBRARY_SHARED}
	${LibUnwind_LIBRARY_SHARED}
    )


    set_target_properties(mpiotf-openmpi-offline PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES};${LibUnwind_DEFINES};${OpenMPI_DEFINES};OPENSS_STATIC") 

    install(TARGETS mpiotf-openmpi-offline
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )

endif()

if(HAVE_MPICH2)

    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich2.a
        COMMAND cp -r ${VT_LIBRARY_MPI_STATIC} ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich2.a 
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich2.a libvt_mpi_a-vt_mpiwrap.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich2.a libvt_mpi_a-vt_sync.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich2.a libvt_mpi_a-vt-otf_trc.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich2.a libvt_mpi_a-vt-mpireq.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich2.a libvt_mpi_a-vt-mpireg.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich2.a libvt_mpi_a-vt-mpicom.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich2.a libvt_mpi_a-vt-fmpiwrap.o
        DEPENDS ${VT_LIBRARY_MPI_STATIC}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    add_custom_target(runvtmpich2 ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich2.a)

    add_library(mpiotf-mpich2-rt-offline SHARED
        ${RT_SOURCES}
	offline.c
    )

    add_dependencies(mpiotf-mpich2-rt-offline runvtmpich2)

    target_include_directories(mpiotf-mpich2-rt-offline PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${PROJECT_BUILD_DIR}/libopenss-runtime
	${PROJECT_SOURCE_DIR}/libopenss-runtime/offline
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich2_INCLUDE_DIRS}
	${VT_INCLUDE_DIRS}
	${OTF_INCLUDE_DIRS}
    )

    target_link_libraries(mpiotf-mpich2-rt-offline
	${CMAKE_DL_LIBS}
	rt
	pthread
        -Wl,-whole-archive
	openss-runtime-offline
	openss-runtime-unwind
        -Wl,-no-whole-archive
	-Bdynamic
	${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich2.a
	${OTF_LIBRARY_STATIC}
	${LibZ_LIBRARY_SHARED}
	${LibUnwind_LIBRARY_SHARED}
    )


    #set_target_properties(mpiotf-mpich2-rt-offline PROPERTIES VERSION 1.1.0)
    set_target_properties(mpiotf-mpich2-rt-offline PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES};${LibUnwind_DEFINES};${Mpich2_DEFINES}") 

    install(TARGETS mpiotf-mpich2-rt-offline
	LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )

    set_target_properties(mpiotf-mpich2-rt-offline PROPERTIES PREFIX "")


    add_library(mpiotf-mpich2-offline STATIC
        ${RT_SOURCES}
	offline.c
    )

    target_include_directories(mpiotf-mpich2-offline PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${PROJECT_BUILD_DIR}/libopenss-runtime
	${PROJECT_SOURCE_DIR}/libopenss-runtime/offline
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich2_INCLUDE_DIRS}
	${VT_INCLUDE_DIRS}
	${OTF_INCLUDE_DIRS}
    )


    target_link_libraries(mpiotf-mpich2-offline
	${CMAKE_DL_LIBS}
	rt
	pthread
        -Wl,-whole-archive
	openss-runtime-offline
	openss-runtime-unwind
        -Wl,-no-whole-archive
	-Bdynamic
	${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich2.a
	${OTF_LIBRARY_STATIC}
	${LibZ_LIBRARY_SHARED}
	${LibUnwind_LIBRARY_SHARED}
    )


    #set_target_properties(mpiotf-mpich2-offline PROPERTIES VERSION 1.1.0)
    set_target_properties(mpiotf-mpich2-offline PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES};${LibUnwind_DEFINES};${Mpich2_DEFINES};OPENSS_STATIC") 

    install(TARGETS mpiotf-mpich2-offline
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )


endif()

if(HAVE_MPICH)

    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich.a
        COMMAND cp -r ${VT_LIBRARY_MPI_STATIC} ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich.a 
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich.a libvt_mpi_a-vt_mpiwrap.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich.a libvt_mpi_a-vt_sync.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich.a libvt_mpi_a-vt-otf_trc.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich.a libvt_mpi_a-vt-mpireq.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich.a libvt_mpi_a-vt-mpireg.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich.a libvt_mpi_a-vt-mpicom.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich.a libvt_mpi_a-vt-fmpiwrap.o
        DEPENDS ${VT_LIBRARY_MPI_STATIC}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    add_custom_target(runvtmpich ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich.a)

    add_library(mpiotf-mpich-rt-offline SHARED
        ${RT_SOURCES}
	offline.c
    )

    add_dependencies(mpiotf-mpich-rt-offline runvtmpich)

    target_include_directories(mpiotf-mpich-rt-offline PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${PROJECT_BUILD_DIR}/libopenss-runtime
	${PROJECT_SOURCE_DIR}/libopenss-runtime/offline
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich_INCLUDE_DIRS}
	${VT_INCLUDE_DIRS}
	${OTF_INCLUDE_DIRS}
    )

    target_link_libraries(mpiotf-mpich-rt-offline
	${CMAKE_DL_LIBS}
	rt
	pthread
        -Wl,-whole-archive
	openss-runtime-offline
	openss-runtime-unwind
        -Wl,-no-whole-archive
	-Bdynamic
	${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich.a
	${OTF_LIBRARY_STATIC}
	${LibZ_LIBRARY_SHARED}
	${LibUnwind_LIBRARY_SHARED}
    )


    #set_target_properties(mpiotf-mpich-rt-offline PROPERTIES VERSION 1.1.0)
    set_target_properties(mpiotf-mpich-rt-offline PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES};${LibUnwind_DEFINES};${Mpich_DEFINES}") 

    install(TARGETS mpiotf-mpich-rt-offline
	LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )

    set_target_properties(mpiotf-mpich-rt-offline PROPERTIES PREFIX "")

    # Create and install static version

    add_library(mpiotf-mpich-offline STATIC
        ${RT_SOURCES}
	offline.c
    )

    target_include_directories(mpiotf-mpich-offline PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${PROJECT_BUILD_DIR}/libopenss-runtime
	${PROJECT_SOURCE_DIR}/libopenss-runtime/offline
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpich_INCLUDE_DIRS}
	${VT_INCLUDE_DIRS}
	${OTF_INCLUDE_DIRS}
    )


    target_link_libraries(mpiotf-mpich-offline
	${CMAKE_DL_LIBS}
	rt
	pthread
        -Wl,-whole-archive
	openss-runtime-offline
	openss-runtime-unwind
        -Wl,-no-whole-archive
	-Bdynamic
	${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpich.a
	${OTF_LIBRARY_STATIC}
	${LibZ_LIBRARY_SHARED}
	${LibUnwind_LIBRARY_SHARED}
    )


    #set_target_properties(mpiotf-mpich-offline PROPERTIES VERSION 1.1.0)
    set_target_properties(mpiotf-mpich-offline PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES};${LibUnwind_DEFINES};${Mpich_DEFINES};OPENSS_STATIC") 

    install(TARGETS mpiotf-mpich-offline
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )

endif()

if(HAVE_MPT)

    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpt.a
        COMMAND cp -r ${VT_LIBRARY_MPI_STATIC} ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpt.a 
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpt.a libvt_mpi_a-vt_mpiwrap.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpt.a libvt_mpi_a-vt_sync.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpt.a libvt_mpi_a-vt-otf_trc.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpt.a libvt_mpi_a-vt-mpireq.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpt.a libvt_mpi_a-vt-mpireg.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpt.a libvt_mpi_a-vt-mpicom.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpt.a libvt_mpi_a-vt-fmpiwrap.o
        DEPENDS ${VT_LIBRARY_MPI_STATIC}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    add_custom_target(runvtmpt ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpt.a)

    add_library(mpiotf-mpt-rt-offline SHARED
        ${RT_SOURCES}
	offline.c
    )

    add_dependencies(mpiotf-mpt-rt-offline runvtmpt)

    target_include_directories(mpiotf-mpt-rt-offline PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${PROJECT_BUILD_DIR}/libopenss-runtime
	${PROJECT_SOURCE_DIR}/libopenss-runtime/offline
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpt_INCLUDE_DIRS}
	${VT_INCLUDE_DIRS}
	${OTF_INCLUDE_DIRS}
    )

    target_link_libraries(mpiotf-mpt-rt-offline
	${CMAKE_DL_LIBS}
	rt
	pthread
        -Wl,-whole-archive
	openss-runtime-offline
	openss-runtime-unwind
        -Wl,-no-whole-archive
	-Bdynamic
	${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpt.a
	${OTF_LIBRARY_STATIC}
	${LibZ_LIBRARY_SHARED}
	${LibUnwind_LIBRARY_SHARED}
    )


    #set_target_properties(mpiotf-mpt-rt-offline PROPERTIES VERSION 1.1.0)
    set_target_properties(mpiotf-mpt-rt-offline PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES};${LibUnwind_DEFINES};${Mpt_DEFINES};${SGIMPT_DEFINES}") 

    install(TARGETS mpiotf-mpt-rt-offline
	LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )

    set_target_properties(mpiotf-mpt-rt-offline PROPERTIES PREFIX "")

    # Create and install static version

    add_library(mpiotf-mpt-offline STATIC
        ${RT_SOURCES}
	offline.c
    )

    target_include_directories(mpiotf-mpt-offline PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${PROJECT_BUILD_DIR}/libopenss-runtime
	${PROJECT_SOURCE_DIR}/libopenss-runtime/offline
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mpt_INCLUDE_DIRS}
	${VT_INCLUDE_DIRS}
	${OTF_INCLUDE_DIRS}
    )


    target_link_libraries(mpiotf-mpt-offline
	${CMAKE_DL_LIBS}
	rt
	pthread
        -Wl,-whole-archive
	openss-runtime-offline
	openss-runtime-unwind
        -Wl,-no-whole-archive
	-Bdynamic
	${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mpt.a
	${OTF_LIBRARY_STATIC}
	${LibZ_LIBRARY_SHARED}
	${LibUnwind_LIBRARY_SHARED}
    )


    #set_target_properties(mpiotf-mpt-offline PROPERTIES VERSION 1.1.0)
    set_target_properties(mpiotf-mpt-offline PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES};${LibUnwind_DEFINES};${Mpt_DEFINES};${SGIMPT_DEFINES};OPENSS_STATIC") 

    install(TARGETS mpiotf-mpt-offline
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )


endif()

if(HAVE_MVAPICH2)

    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich2.a
        COMMAND cp -r ${VT_LIBRARY_MPI_STATIC} ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich2.a 
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich2.a libvt_mpi_a-vt_mpiwrap.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich2.a libvt_mpi_a-vt_sync.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich2.a libvt_mpi_a-vt-otf_trc.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich2.a libvt_mpi_a-vt-mpireq.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich2.a libvt_mpi_a-vt-mpireg.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich2.a libvt_mpi_a-vt-mpicom.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich2.a libvt_mpi_a-vt-fmpiwrap.o
        DEPENDS ${VT_LIBRARY_MPI_STATIC}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    add_custom_target(runvtmvapich2 ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich2.a)

    add_library(mpiotf-mvapich2-rt-offline SHARED
        ${RT_SOURCES}
	offline.c
    )

    add_dependencies(mpiotf-mvapich2-rt-offline runvtmvapich2)

    target_include_directories(mpiotf-mvapich2-rt-offline PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${PROJECT_BUILD_DIR}/libopenss-runtime
	${PROJECT_SOURCE_DIR}/libopenss-runtime/offline
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich2_INCLUDE_DIRS}
	${VT_INCLUDE_DIRS}
	${OTF_INCLUDE_DIRS}
    )

    target_link_libraries(mpiotf-mvapich2-rt-offline
	${CMAKE_DL_LIBS}
	rt
	pthread
        -Wl,-whole-archive
	openss-runtime-offline
	openss-runtime-unwind
        -Wl,-no-whole-archive
	-Bdynamic
	${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich2.a
	${OTF_LIBRARY_STATIC}
	${LibZ_LIBRARY_SHARED}
	${LibUnwind_LIBRARY_SHARED}
    )


    #set_target_properties(mpiotf-mvapich2-rt-offline PROPERTIES VERSION 1.1.0)
    set_target_properties(mpiotf-mvapich2-rt-offline PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES};${LibUnwind_DEFINES};${Mvapich2_DEFINES}") 

    install(TARGETS mpiotf-mvapich2-rt-offline
	LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )

    set_target_properties(mpiotf-mvapich2-rt-offline PROPERTIES PREFIX "")

    # Create and install static version

    add_library(mpiotf-mvapich2-offline STATIC
        ${RT_SOURCES}
	offline.c
    )

    target_include_directories(mpiotf-mvapich2-offline PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${PROJECT_BUILD_DIR}/libopenss-runtime
	${PROJECT_SOURCE_DIR}/libopenss-runtime/offline
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich2_INCLUDE_DIRS}
	${VT_INCLUDE_DIRS}
	${OTF_INCLUDE_DIRS}
    )


    target_link_libraries(mpiotf-mvapich2-offline
	${CMAKE_DL_LIBS}
	rt
	pthread
        -Wl,-whole-archive
	openss-runtime-offline
	openss-runtime-unwind
        -Wl,-no-whole-archive
	-Bdynamic
	${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich2.a
	${OTF_LIBRARY_STATIC}
	${LibZ_LIBRARY_SHARED}
	${LibUnwind_LIBRARY_SHARED}
    )


    #set_target_properties(mpiotf-mvapich2-offline PROPERTIES VERSION 1.1.0)
    set_target_properties(mpiotf-mvapich2-offline PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES};${LibUnwind_DEFINES};${Mvapich2_DEFINES};OPENSS_STATIC") 

    install(TARGETS mpiotf-mvapich2-offline
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )


endif()

if(HAVE_MVAPICH)

    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich.a
        COMMAND cp -r ${VT_LIBRARY_MPI_STATIC} ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich.a 
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich.a libvt_mpi_a-vt_mpiwrap.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich.a libvt_mpi_a-vt_sync.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich.a libvt_mpi_a-vt-otf_trc.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich.a libvt_mpi_a-vt-mpireq.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich.a libvt_mpi_a-vt-mpireg.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich.a libvt_mpi_a-vt-mpicom.o
        COMMAND ar -d ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich.a libvt_mpi_a-vt-fmpiwrap.o
        DEPENDS ${VT_LIBRARY_MPI_STATIC}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    add_custom_target(runvtmvapich ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich.a)

    add_library(mpiotf-mvapich-rt-offline SHARED
        ${RT_SOURCES}
	offline.c
    )

    add_dependencies(mpiotf-mvapich-rt-offline runvtmvapich)

    target_include_directories(mpiotf-mvapich-rt-offline PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${PROJECT_BUILD_DIR}/libopenss-runtime
	${PROJECT_SOURCE_DIR}/libopenss-runtime/offline
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich_INCLUDE_DIRS}
	${VT_INCLUDE_DIRS}
	${OTF_INCLUDE_DIRS}
    )

    target_link_libraries(mpiotf-mvapich-rt-offline
	${CMAKE_DL_LIBS}
	rt
	pthread
        -Wl,-whole-archive
	openss-runtime-offline
	openss-runtime-unwind
        -Wl,-no-whole-archive
	-Bdynamic
	${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich.a
	${OTF_LIBRARY_STATIC}
	${LibZ_LIBRARY_SHARED}
	${LibUnwind_LIBRARY_SHARED}
    )


    #set_target_properties(mpiotf-mvapich-rt-offline PROPERTIES VERSION 1.1.0)
    set_target_properties(mpiotf-mvapich-rt-offline PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES};${LibUnwind_DEFINES};${Mvapich_DEFINES}") 

    install(TARGETS mpiotf-mvapich-rt-offline
	LIBRARY DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )

    set_target_properties(mpiotf-mvapich-rt-offline PROPERTIES PREFIX "")

    add_library(mpiotf-mvapich-offline STATIC
        ${RT_SOURCES}
	offline.c
    )

    target_include_directories(mpiotf-mvapich-offline PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/libopenss-runtime
	${PROJECT_BUILD_DIR}/libopenss-runtime
	${PROJECT_SOURCE_DIR}/libopenss-runtime/offline
	${LibMonitor_INCLUDE_DIRS}
	${LibUnwind_INCLUDE_DIRS}
	${Mvapich_INCLUDE_DIRS}
	${VT_INCLUDE_DIRS}
	${OTF_INCLUDE_DIRS}
    )


    target_link_libraries(mpiotf-mvapich-offline
	${CMAKE_DL_LIBS}
	rt
	pthread
        -Wl,-whole-archive
	openss-runtime-offline
	openss-runtime-unwind
        -Wl,-no-whole-archive
	-Bdynamic
	${CMAKE_CURRENT_BINARY_DIR}/libvt.mpi.mvapich.a
	${OTF_LIBRARY_STATIC}
	${LibZ_LIBRARY_SHARED}
	${LibUnwind_LIBRARY_SHARED}
    )


    #set_target_properties(mpiotf-mvapich-offline PROPERTIES VERSION 1.1.0)
    set_target_properties(mpiotf-mvapich-offline PROPERTIES COMPILE_DEFINITIONS "${TLS_DEFINES};${LibUnwind_DEFINES};${Mvapich_DEFINES};OPENSS_STATIC") 

    install(TARGETS mpiotf-mvapich-offline
	ARCHIVE DESTINATION lib${LIB_SUFFIX}/openspeedshop
    )


endif()

# endif for libmonitor
endif()

file(COPY blobs.x DESTINATION .)
