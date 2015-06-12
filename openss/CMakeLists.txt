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

include(CheckIncludeFile)

check_include_file("stdint.h" STDINT_H_FOUND)
if (STDINT_H_FOUND)
    add_definitions(-DHAVE_STDINT_H=1)
endif()

set(LIBRARY_DIR "${CMAKE_INSTALL_PREFIX}")
set(PLUGIN_DIR "openspeedshop")
set(LIB_DIR_NAME "lib${SUFFIX}")

set(cbtfinstalldir ${CBTF_DIR})
set(cbtflibdir ${CBTF_DIR}/lib${SUFFIX})
set(cbtfruncmd ${CBTF_KRELL_DIR}/bin/cbtfrun)
set(cbtfcollectors ${CBTF_KRELL_DIR}/lib${SUFFIX}/KrellInstitute/Collectors)
set(cbtfcomponents ${CBTF_KRELL_DIR}/lib${SUFFIX}/KrellInstitute/Components)
set(cbtfxmldir ${CBTF_DIR}/share/KrellInstitute/XML)
set(libmonitordir ${LIBMONITOR_DIR})
set(libmonitorlibdir ${LIBMONITOR_DIR}/lib${SUFFIX})
set(libunwinddir ${LIBUNWIND_DIR})
set(libunwindlibdir ${LIBUNWIND_DIR}/lib${SUFFIX})

if (HAVE_RUNTIME_DIR MATCHES "true")
    set(oss_installation_dir ${RUNTIME_DIR})
    set(ossinstalldir ${RUNTIME_DIR})
    set(osslibdir ${RUNTIME_LIBDIR})
else()
    set(oss_installation_dir ${CMAKE_INSTALL_PREFIX})
    set(ossinstalldir ${CMAKE_INSTALL_PREFIX})
    set(osslibdir ${CMAKE_INSTALL_PREFIX}/lib${SUFFIX})
endif()

set(oss_instrumentor ${INSTRUMENTOR})
set(machine_dependent_libdir lib${SUFFIX})
set(machinelibdir lib${SUFFIX})
set(alt_machine_dependent_libdir lib${SUFFIX})
set(ossdefaultmpi ${DEFAULT_MPI_IMPL})
set(mpichdir ${MPICH_DIR})
set(mvapichdir ${MVAPICH_DIR})
set(mpich2dir ${MPICH2_DIR})
set(mvapich2dir ${MVAPICH2_DIR})
set(mptdir ${MPT_DIR})
set(openmpidir ${OPENMPI_DIR})
set(openmpilibdir ${OpenMPI_LIB_DIR})
#set(target_os ${TARGET_OS})
set(osstargetos ${TARGET_OS})

if (RUNTIME_ONLY MATCHES "true")
  configure_file(ossrun.in ossrun @ONLY)
  configure_file(osslink.in osslink @ONLY)
  install(PROGRAMS
	${CMAKE_CURRENT_BINARY_DIR}/ossrun
	${CMAKE_CURRENT_BINARY_DIR}/osslink
	${PROJECT_SOURCE_DIR}/openss/ossanon
	${PROJECT_SOURCE_DIR}/openss/ossdbtool
	DESTINATION
	${CMAKE_INSTALL_PREFIX}/bin)

else()
    configure_file(ossrun.in ossrun @ONLY)
    configure_file(osslink.in osslink @ONLY)
    configure_file(ossdriver.in ossdriver @ONLY)
    configure_file(ossdriver_compare.in ossdriver_compare COPYONLY)

    install(PROGRAMS
	${CMAKE_CURRENT_BINARY_DIR}/ossrun
	${CMAKE_CURRENT_BINARY_DIR}/osslink
	${CMAKE_CURRENT_BINARY_DIR}/ossdriver
	${CMAKE_CURRENT_BINARY_DIR}/ossdriver_compare
	${PROJECT_SOURCE_DIR}/openss/ossanon
	${PROJECT_SOURCE_DIR}/openss/ossdbtool
	DESTINATION
	${CMAKE_INSTALL_PREFIX}/bin)


    #set(GCC_FORMAT_CONTAINS_NUL "-Wno-format-contains-nul")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_FORMAT_CONTAINS_NUL}")

    add_definitions(
      -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
      )
  
    if (OpenMP_FLAG_DETECTED)
        add_definitions(-fopenmp)
        set(GOMP "gomp")
    else()
        set(GOMP "")
    endif()

    add_executable(openss
	openss.cxx
    )

    target_include_directories(openss PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/libopenss-framework
        ${LTDL_INCLUDE_DIR}
    )
    target_link_libraries(openss
        ${GOMP}
        ${LTDL_LIBRARY_SHARED}
        ${CMAKE_DL_LIBS}
    )
    install(TARGETS openss
        RUNTIME DESTINATION bin
    )

    if (${INSTRUMENTOR} MATCHES "offline")
        add_executable(ossutil
    	ossutil.cxx
        )

        if (RESOLVE_SYMBOLS MATCHES "bfd")

            target_link_libraries(ossutil
                openss-framework
                openss-framework-binutils
                ${LIB_LTDL}
                ${CMAKE_DL_LIBS}
            )
        else()
    
            target_link_libraries(ossutil
                openss-framework
                openss-framework-symtabapi
                ${LIB_LTDL}
                ${CMAKE_DL_LIBS}
            )
        endif()

        set_target_properties(ossutil PROPERTIES
                          COMPILE_DEFINITIONS "LIBRARY_DIR;PLUGIN_DIR;LIB_DIR_NAME")
   
        install(TARGETS ossutil
            RUNTIME DESTINATION bin
        )
    elseif (${INSTRUMENTOR} MATCHES "cbtf")

        add_executable(osscollect
    	    osscollect.cxx
        )

        target_include_directories(osscollect PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_BINARY_DIR}
            ${PROJECT_SOURCE_DIR}/libopenss-framework
            ${PROJECT_SOURCE_DIR}/libopenss-framework/cbtf
	    ${Boost_INCLUDE_DIRS}
	    ${MRNet_INCLUDE_DIRS}
	    ${CBTF_INCLUDE_DIRS}
	    ${CBTF_CORE_INCLUDE_DIR}
	    ${DyninstAPI_INCLUDE_DIRS}
            ${LTDL_INCLUDE_DIR}
        )

        target_link_libraries(osscollect
            ${LTDL_LIBRARY_SHARED}
	    ${Boost_FILESYSTEM_LIBRARY}
	    ${Boost_SYSTEM_LIBRARY}
	    ${Boost_THREAD_LIBRARY}
	    ${Boost_PROGRAM_OPTIONS_LIBRARY}
            ${LibElf_SHARED_LIBRARIES}
            ${LibDwarf_SHARED_LIBRARIES}
            ${XercesC_LIBRARIES}
            ${MRNet_LIBRARIES}
            ${DyninstAPI_LIBRARIES}
	    ${CBTF_CORE_SYMTABAPI_LIBRARY}
	    ${CBTF_CORE_MRNET_LIBRARY}
	    openss-framework
	    openss-framework-symtabapi
            pthread
            ${CMAKE_DL_LIBS}
        )

        #set(XMLDIR ${CBTF_KRELL_DIR}/share/KrellInstitute/xml)
        set_target_properties(osscollect PROPERTIES COMPILE_DEFINITIONS XMLDIR=${CBTF_KRELL_DIR}/share/KrellInstitute/xml)
        #set_property(TARGET osscollect APPEND PROPERTY COMPILE_DEFINITIONS BUILD_CBTF=1)

        install(TARGETS osscollect
            RUNTIME DESTINATION bin
        )
    endif()

    if(CMAKE_HOST_UNIX)
        # create osscompare link
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf ossdriver_compare osscompare
	    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")

        # create driver links
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf ossdriver osspcsamp
	    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf ossdriver ossusertime
	    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf ossdriver osshwc
	    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf ossdriver osshwcsamp
	    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf ossdriver osshwctime
	    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf ossdriver ossio
	    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf ossdriver ossiot
	    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf ossdriver ossfpe
	    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")

        # create mpi driver links; TODO: verify mpi collectors configd.
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf ossdriver ossmpi
	    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf ossdriver ossmpit
	    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        install(CODE " EXECUTE_PROCESS(COMMAND ln -sf ossdriver ossmpiotf
    	    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")

        # create oss cbtf related links here if instrumentor is cbtf

        if (${INSTRUMENTOR} MATCHES "cbtf")
            install(CODE " EXECUTE_PROCESS(COMMAND ln -sf ossdriver ossmem
	        WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
            install(CODE " EXECUTE_PROCESS(COMMAND ln -sf ossdriver osspthreads
	        WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
            install(CODE " EXECUTE_PROCESS(COMMAND ln -sf ossdriver ossmpip
	        WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
            install(CODE " EXECUTE_PROCESS(COMMAND ln -sf ossdriver ossiop
	        WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
            if (FOUND_CUDA)
                install(CODE " EXECUTE_PROCESS(COMMAND ln -sf ossdriver osscuda
	            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
                install(CODE " EXECUTE_PROCESS(COMMAND ln -sf ossdriver osscudaio
	            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
            endif()
        endif()
    endif(CMAKE_HOST_UNIX)

endif() #endif for RUNTIME_ONLY
