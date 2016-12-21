################################################################################
# Copyright (c) 2014-2016 Krell Institute. All Rights Reserved.
# Copyright (c) 2015,2016 Argo Navis Technologies. All Rights Reserved.
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

# FIXME: It is very likely these are used buy offline.
set(LIBRARY_DIR "${CMAKE_INSTALL_PREFIX}")
set(PLUGIN_DIR "openspeedshop")
set(LIB_DIR_NAME "lib${SUFFIX}")

# FIXME: Does anyone needs thes CBTF related directories.
# and if so, it should be only needed when INSTRUMENTOR==cbtf.
set(cbtfinstalldir ${CBTF_DIR})
set(cbtflibdir ${CBTF_DIR}/lib${SUFFIX})

# FIXME: We need to add a section for cbtf-argonavis cuda here.
# Will the affected code be able to sue multiple paths?

# FIXME: These are specific to INSTRUMENTOR==cbtf.
# The cbtfrun, xml,Collectors,Components all have defined names now
# in FindCBTF-Krell.cmake. 
set(cbtfruncmd ${CBTF_KRELL_CBTFRUN})
set(cbtfcollectors ${CBTF_KRELL_COLLECTORS})
set(cbtfcomponents ${CBTF_KRELL_COMPONENTS})
if (BUILD_FE_USING_CN_RUNTIMES MATCHES "true")
    set(cbtfxmldir ${CBTF_KRELL_CN_XMLDIR})
else()
    set(cbtfxmldir ${CBTF_KRELL_XMLDIR})
endif()

# FIXME: should be only needed when INSTRUMENTOR==offline.
set(libmonitordir ${LIBMONITOR_DIR})
set(libmonitorlibdir ${LibMonitor_LIB_DIR})

set(libunwinddir ${LIBUNWIND_DIR})
set(libunwindlibdir ${LibUnwind_LIB_DIR})

set(papidir ${PAPI_DIR})
set(papilibdir ${Papi_LIB_DIR})


# FIXME: Who exactly sets RUNTIME_name?  Document that here.
if (HAVE_RUNTIME_DIR MATCHES "true")
    set(oss_installation_dir ${RUNTIME_DIR})
    set(ossinstalldir ${RUNTIME_DIR})
    set(osslibdir ${RUNTIME_LIBDIR})
else()
    set(oss_installation_dir ${CMAKE_INSTALL_PREFIX})
    set(ossinstalldir ${CMAKE_INSTALL_PREFIX})
    set(osslibdir ${CMAKE_INSTALL_PREFIX}/lib${SUFFIX})
endif()

# FIXME: Any of these specific to instrumentor types?
# if so, these should just be set per instrumentor...
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
set(oss_targetos ${RUNTIME_PLATFORM})

if (RUNTIME_ONLY MATCHES "true")
  # FIXME: In the openss world only offline instrumentor needs the
  # ossrun and osslink scripts.  The ossanon and ossdbtool are not
  # needed for RUNTIME_ONLY either since the runtimes do no create
  # databases....
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
    # FIXME: ossrun, and likely osslink are offline instrumentor files.
    # The cbtf-krell collection code uses cbtfrun (internally) and
    # cbtflink. So we need to move ossrun and osslink to the offline section.
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

#
# The info below is not quite accurate.  While $pragma omp parallel is used
# in the Queries::GetMetricValues function, it is not actuall compiled into
# libopenss-queries even if -fopenmp is used.  But the actual callsites
# in the cli that call Queries::GetMetricValues (via Queries.hxx include)
# will in fact use openmp if the cli if built with -fopenmp. Also, if the
# libopenss-cli is linked with the openmp runtime library then the openss
# client does not need to do any of this openmp stuff here.
#
# We use openmp in the queries library to speedup the getMetricValues calls.
# NOTE: Ideally those libraries (e.g. libopenss-queries) that use OpenMP would
#       be linked (via @OPENMP_LDFLAGS@ and @OPENMP_LIBS@) to its runtime. For
#       some unknown reason, however, that runtime cannot be dlopen()ed into a
#       process. The error "libgomp.so.1: shared object cannot be dlopen()ed"
#       is produced and the process immediately aborts. The fix is to have the
#       main "openss" executable link to the runtime, thus indirectly providing
#       the runtime for the other libraries that actually use it.
#

    # AFAICT, no code in the openss directory is using openmp.
    # Shutting this down for cbtf instrumentor builds...
    if (${INSTRUMENTOR} MATCHES "offline")
      if (OpenMP_FLAG_DETECTED)
        add_definitions(${OpenMP_C_FLAGS})
        if (${OpenMP_C_FLAGS} MATCHES "-fopenmp")
           set(OpenMP_RT_NAME "gomp")
        elseif (${OpenMP_C_FLAGS} MATCHES "-qopenmp")
           set(OpenMP_RT_NAME "iomp5")
        elseif (${OpenMP_C_FLAGS} MATCHES "-openmp")
           set(OpenMP_RT_NAME "iomp5")
        elseif (${OpenMP_C_FLAGS} MATCHES "-qopenmp")
           set(OpenMP_RT_NAME "iomp5")
        else()
           set(OpenMP_RT_NAME "")
        endif()
      else()
        set(OpenMP_RT_NAME "")
      endif()
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

    # openss-framework-cbtf is the cbtf instrumentor dso and
    # is built separtely from the framework. It also links
    # openss-framework-symtabapi directly since it does not
    # support bfd symbol resolution at this time.
    #
    if (${INSTRUMENTOR} MATCHES "cbtf")
	target_link_libraries(openss
	    -Wl,--no-as-needed
	    openss-framework
	    openss-framework-cbtf
	    ${LTDL_LIBRARY_SHARED}
	    ${CMAKE_DL_LIBS}
    )
    else()
	target_link_libraries(openss
	    -Wl,--no-as-needed
	    ${OpenMP_RT_NAME}
	    ${LTDL_LIBRARY_SHARED}
	    ${CMAKE_DL_LIBS}
    )
    endif()

    install(TARGETS openss
        RUNTIME DESTINATION bin
    )

    if (${INSTRUMENTOR} MATCHES "offline")
        add_executable(ossutil
    	ossutil.cxx
        )

        if (RESOLVE_SYMBOLS MATCHES "bfd")

            target_link_libraries(ossutil
	        -Wl,--no-as-needed
                openss-framework
                openss-framework-binutils
	        ${OpenMP_RT_NAME}
                ${LIB_LTDL}
                ${CMAKE_DL_LIBS}
            )
        else()
    
            target_link_libraries(ossutil
	        -Wl,--no-as-needed
                openss-framework
                openss-framework-symtabapi
	        ${OpenMP_RT_NAME}
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
	    ${CBTF_KRELL_CORE_INCLUDE_DIR}
	    ${DyninstAPI_INCLUDE_DIRS}
            ${LTDL_INCLUDE_DIR}
        )

        target_link_libraries(osscollect
	    -Wl,--no-as-needed
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
	    ${CBTF_KRELL_CORE_SYMTABAPI_LIBRARY}
	    ${CBTF_KRELL_CORE_MRNET_LIBRARY}
	    openss-framework
	    openss-framework-cbtf
	    openss-framework-symtabapi
	    ${OpenMP_RT_NAME}
            pthread
	    ${OpenMP_RT_NAME}
            ${CMAKE_DL_LIBS}
            rt
        )

        # This is defined in FindCBTF-Krell.cmake. Is anyone using XMLDIR directly?
	# or should this be CBTF_KRELL_XMLDIR.  Will cbtf-argonavis cuda need
	# it's own definitions here?
        # Handle cbtf targeted build where the xmldir is in the compute node version install directories
        if (BUILD_FE_USING_CN_RUNTIMES MATCHES "true" )
            set_target_properties(osscollect PROPERTIES COMPILE_DEFINITIONS XMLDIR=${CBTF_KRELL_CN_XMLDIR})
        else()
            set_target_properties(osscollect PROPERTIES COMPILE_DEFINITIONS XMLDIR=${CBTF_KRELL_XMLDIR})
        endif()

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
	# FIXME: The mpiotf collector is not available for cbtf instrumentation!
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
            install(CODE " EXECUTE_PROCESS(COMMAND ln -sf ossdriver ossomptp
	        WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        endif()

        # create cuda driver link
        if(BUILD_CUDA_COLLECTION)
            install(CODE " EXECUTE_PROCESS(COMMAND ln -sf ossdriver osscuda
            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin) ")
        endif()

    endif(CMAKE_HOST_UNIX)

endif() #endif for RUNTIME_ONLY
