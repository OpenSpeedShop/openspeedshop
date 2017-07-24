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

cmake_minimum_required(VERSION 2.8.0)

project(OpenSpeedShop C CXX)

# define the version for this build.
# TODO: allow this to be passed as an option via cmake for developers
# to tag non production builds.
add_definitions(
    -DVERSION="2.3.1"
)

# cmake module search path for this project.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# NOTE related to the following C++11 feature checks:
#    This include: CheckCXX11Features.cmake is projected to be in a future version of CMake.
#    When this does get included into the new CMake version, we will update our
#    required CMake version and remove cmake/CheckCXX11Features* files from the cbtf repository.
#
include(cmake/CheckCXX11Features.cmake)

include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckFunctionExists)
include(CheckLibraryExists)

# ensure these includes and defines are set for this project.
check_include_file("stdint.h" STDINT_H_FOUND)
if (STDINT_H_FOUND)
    add_definitions(-DHAVE_STDINT_H=1)
endif()
check_include_file(dirent.h HAVE_DIRENT_H)
check_include_file(dlfcn.h HAVE_DLFCN_H)
check_include_file(errno.h HAVE_ERRNO_H)
check_include_file(execinfo.h HAVE_EXECINFO_H)
check_include_file(fcntl.h HAVE_FCNTL_H)
check_include_file(inttypes.h HAVE_INTTYPES_H)
check_include_file(limits.h HAVE_LIMITS_H)
check_include_file(link.h HAVE_LINK_H)
check_include_file(malloc.h HAVE_MALLOC_H)
check_include_file(malloc/malloc.h HAVE_MALLOC_MALLOC_H)
check_function_exists(fork HAVE_FORK)
check_library_exists(libdl __dlopen "" HAVE_DLOPEN)

# Set the oss_hardware_platform which is used in finding mpi implementation install directories
execute_process(COMMAND uname -i 
	OUTPUT_VARIABLE local_arch OUTPUT_STRIP_TRAILING_WHITESPACE)
set(oss_hardware_platform "${local_arch}")
message(STATUS "oss_hardware_platform: " ${oss_hardware_platform} )



# Some examples if needed.
#-----------------------------------------------------------------------------
# This MACRO checks IF the symbol exists in the library and IF it
# does, allow it.
#-----------------------------------------------------------------------------
#MACRO (CHECK_LIBRARY_EXISTS_CONCAT LIBRARY SYMBOL VARIABLE)
#  CHECK_LIBRARY_EXISTS ("${LIBRARY}" ${SYMBOL} "" ${VARIABLE})
#ENDMACRO (CHECK_LIBRARY_EXISTS_CONCAT)
#cmake -DVARNAME:STRING=value

#
#--------------------------------------------------------------------------------
# Handle the TLS options for building the runtimes
#    - explicit means the tool allocates and manages the thread local storage
#    - implicit means the tool uses the systems thread local storage
#--------------------------------------------------------------------------------
#
set(TLS_MODEL "implicit" CACHE STRING "OpenSpeedShop tls model: [implicit,explicit]")
if (TLS_MODEL MATCHES "explicit")
    # Builder requested explicit tls to be used in the runtimes and collectors
    set(TLS_DEFINES "USE_EXPLICIT_TLS" )
    set(USE_EXPLICIT_TLS 1)
    message(STATUS "Build runtimes using explicit TLS model.")
elseif (TLS_MODEL MATCHES "implicit")
    # Builder requsted implicit tls to be used in the runtimes and collectors
    set(TLS_DEFINES "USE_IMPLICIT_TLS" )
    message(STATUS "Build runtimes using implicit TLS model.")
else()
    # Default to implicit tls in the runtimes and collectors
    set(TLS_DEFINES "USE_IMPLICIT_TLS" )
    message(STATUS "Build runtimes using implicit TLS model.")
endif()

#
#--------------------------------------------------------------------------------
# Handle the target specific build options for building the runtimes for specific 
# platform compute nodes
#--------------------------------------------------------------------------------
#
set(RUNTIME_PLATFORM "" CACHE STRING "OpenSpeedShop target os: none or [bgq,bgp,mic]")

if (RUNTIME_PLATFORM STREQUAL "bgqfe")
    message(STATUS "Build runtimes for target bgq front-end platform.")
    # This is used to build the offline.py correctly when we are running on
    # the bgq.  We execute the offline.py in the FE client of OSS.
    set(RUNTIME_PLATFORM bgq)
elseif (RUNTIME_PLATFORM STREQUAL "bgq")
    add_definitions(-DRUNTIME_PLATFORM=bgq)
    add_definitions(-DRUNTIME_PLATFORM_BGQ=1)
    add_definitions(-DBUILD_TARGETED_BGQ=1)
    set(BUILD_TARGETED_BGQ 1)
    message(STATUS "Build runtimes for target bgq platform.")
elseif (RUNTIME_PLATFORM STREQUAL "bgp")
    add_definitions(-DRUNTIME_PLATFORM=bgp)
    add_definitions(-DRUNTIME_PLATFORM_BGP=1)
    add_definitions(-DBUILD_TARGETED_BGP=1)
    set(BUILD_TARGETED_BGP 1)
    message(STATUS "Build runtimes for target bgp platform.")
elseif (RUNTIME_PLATFORM STREQUAL "bgl")
    add_definitions(-DRUNTIME_PLATFORM=bgl)
    add_definitions(-DRUNTIME_PLATFORM_BGL=1)
    add_definitions(-DBUILD_TARGETED_BGL=1)
    set(BUILD_TARGETED_BGL 1)
    message(STATUS "Build runtimes for target bgl platform.")
elseif (RUNTIME_PLATFORM STREQUAL "mic")
    add_definitions(-DRUNTIME_PLATFORM=mic)
    message(STATUS "Build runtimes for target mic platform.")
elseif (RUNTIME_PLATFORM STREQUAL "cray")
    message(STATUS "Build runtimes for target cray platform.")
elseif (RUNTIME_PLATFORM STREQUAL "cray-xk")
    message(STATUS "Build runtimes for target cray-xk platform.")
elseif (RUNTIME_PLATFORM STREQUAL "cray-xe")
    message(STATUS "Build runtimes for target cray-xe platform.")
elseif (RUNTIME_PLATFORM MATCHES "arm")
    message(STATUS "Build for the ARM platform (32 bit or 64 bit).")
else()
    message(STATUS "Build runtimes for localhost platform.")
endif()

#
#--------------------------------------------------------------------------------
# Handle the enabling of cli timing gathering and printing
#--------------------------------------------------------------------------------
#
set(BUILD_CLI_TIMING "no" CACHE STRING "build cli timing code: [no,yes]")
if (BUILD_CLI_TIMING MATCHES "yes")
    add_definitions(-DBUILD_CLI_TIMING=1)
    message(STATUS "Build cli with timing code enabled.")
endif()

#
#--------------------------------------------------------------------------------
# Handle the enabling of building the Parallel Tools GUI Framework base OpenSpeedShop
# GUI needed CLI support. BUILD_PTGF == all, builds the necessary CLI linked objects
# required to interface with the PTGF OpenSpeedShop GUI
#--------------------------------------------------------------------------------
#
set(BUILD_PTGF "no" CACHE STRING "resolve symbols using: [no, direct, all]")
if (BUILD_PTGF MATCHES "direct")
    set(BUILD_PTGF_DIRECT 1)
    message(STATUS "Build ptgf direct.")
elseif (BUILD_PTGF MATCHES "all")
    set(BUILD_PTGF_DIRECT 1)
    set(BUILD_PTGF_SERVER 1)
    message(STATUS "Build ptgf all.")
endif()
message(STATUS "BUILD_PTGF_DIRECT=" ${BUILD_PTGF_DIRECT} )
message(STATUS "BUILD_PTGF_SERVER=" ${BUILD_PTGF_SERVER} )
#
#--------------------------------------------------------------------------------
# Try to determine where the libraries created by this build should be stored.
# The choices are lib or lib64.  Set the appropriate variables based on checking
# the system processor type.
#--------------------------------------------------------------------------------
#
if(CMAKE_SYSTEM_PROCESSOR MATCHES "ppc64*")
    set(CMAKE_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib64)
    set(LIB_SUFFIX 64)
    set(SUFFIX 64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "ppc*")
    set(CMAKE_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib)
elseif(EXISTS /usr/lib64)
    set(LIB_SUFFIX 64)
    set(SUFFIX 64)
    set(CMAKE_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
else()
    set(CMAKE_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib)
endif()

# Setup CACHE variables that are referenced across the sub-directories of the build

set(BUILD_FE_USING_CN_RUNTIMES "false" CACHE STRING "Special front-end build where the compute node components are passed to the build in addition to the FE related components." )
set(CBTF_KRELL_CN_RUNTIME_DIR "" CACHE STRING "Path to targeted runtime collectors." )

# Set default to runtime only to false.
set(RUNTIME_ONLY "false" CACHE STRING "Build only runtime collectors (offline instrumentor): [true,false]")
set(BUILD_QT3_GUI "true" CACHE STRING "Build the Qt3 GUI: [true,false]")

# Offline related - for finding the compute node versions of OSS and libmonitor
set(RUNTIME_DIR "" CACHE STRING "path to targeted runtime collectors")
set(HAVE_RUNTIME_DIR "false" CACHE STRING "Was the directory path to targeted runtime collectors passed in as an argument.")
set(HAVE_CN_LIBMONITOR "false" CACHE STRING "Was the directory path to targeted libmonitor passed in as an argument.")

# Default to the offline instrumentor.
set(INSTRUMENTOR "offline" CACHE STRING "OpenSpeedShop instrumentor type: [offline,cbtf]")

#
#--------------------------------------------------------------------------------
# Do find package calls for the dependency packages that are needed for both the 
# offline and cbtf versions of OpenSpeedShop
#--------------------------------------------------------------------------------
#
# NOTE: packages that are common to both instrumentors.
#
if (INSTRUMENTOR MATCHES "offline" OR INSTRUMENTOR MATCHES "cbtf")
    find_package(Doxygen)
    find_package(BISON)
    find_package(FLEX)
    find_package(PythonInterp)
    find_package(PythonLibs)
    find_package(LibElf)
    find_package(LibDwarf)
    find_package(Binutils)
    find_package(Threads)
    find_package(Sqlite3)
    find_package(LTDL)
    find_package(SymtabAPI)
    find_package(DyninstAPI)
    if ("${BUILD_QT3_GUI}" STREQUAL "true")
       find_package(Qt3)
    endif()
    # always check for openmp regardless of instrumentor.
    # it is a client thing...
    find_package(OpenMP)
endif()

message(STATUS "BUILD_QT3_GUI: " ${BUILD_QT3_GUI} )

# The old autotools build added this to a config.h which
# many source files included if it existed to define exactly
# this kind of thing.
if (OpenMP_FLAG_DETECTED)
    add_definitions(-DHAVE_OPENMP=1)
endif()

#
#--------------------------------------------------------------------------------
# Do find package calls for offline only and cbtf only versions of OpenSpeedShop 
# in this section of the CMakeLists.txt file.
#
# Some packages are only needed for a particular version.  For the CBTF version 
# of OpenSpeedShop, we need to find where the CBTF components, collectors, and 
# runtime libraries are located.
#--------------------------------------------------------------------------------
#
#--------------------------------------------------------------------------------
# Do find package calls for offline only version of OpenSpeedShop in this section
#--------------------------------------------------------------------------------
#

if (INSTRUMENTOR MATCHES "offline")
    find_package(LibMonitor)
    find_package(LibUnwind)
    find_package(Papi)
    find_package(OpenMPI)
    find_package(Mpich)
    find_package(Mpich2)
    find_package(Mpt)
    find_package(Mvapich2)
    find_package(Mvapich)
    find_package(VT)
    find_package(OTF)
    find_package(LibZ)
    find_package(Personality)
    find_package(CBTF-Krell)
    # Need the boost includes to compile code using the Dyninst component includes
    if (DYNINSTAPI_FOUND)
        find_package(Boost 1.41.0 REQUIRED)
    endif()

    if (PERSONALITY_FOUND)
        set(HAVE_PERSONALITY 1)
    endif()

    if (NOT "${LIBMONITOR_CN_RUNTIME_DIR}" STREQUAL "")
        if(EXISTS ${LIBMONITOR_CN_RUNTIME_DIR} AND IS_DIRECTORY "${LIBMONITOR_CN_RUNTIME_DIR}")
            set (HAVE_CN_LIBMONITOR "true")
            if( EXISTS ${LIBMONITOR_CN_RUNTIME_DIR}/lib${LIB_SUFFIX} AND IS_DIRECTORY "${LIBMONITOR_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}" AND EXISTS "${LIBMONITOR_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}/libmonitor.so")
               # LIBMONITOR_CN_RUNTIME_DIR/lib64 was recognized.
               set (LIBMONITOR_CN_RUNTIME_LIBDIR ${LIBMONITOR_CN_RUNTIME_DIR}/lib${LIB_SUFFIX} CACHE PATH "Location of compute node libmonitor libraries.")
            elseif ( EXISTS ${LIBMONITOR_CN_RUNTIME_DIR}/lib AND IS_DIRECTORY "${LIBMONITOR_CN_RUNTIME_DIR}/lib" AND EXISTS "${LIBMONITOR_CN_RUNTIME_DIR}/lib/libmonitor.so")
               # LIBMONITOR_CN_RUNTIME_DIR/lib was recognized.
               set (LIBMONITOR_CN_RUNTIME_LIBDIR ${LIBMONITOR_CN_RUNTIME_DIR}/lib CACHE PATH "Location of compute node libmonitor libraries.")
            else()
               # Override the settings we had up until now.  We need to find all the targeted libraries to succeed.
               set (HAVE_CN_LIBMONITOR "false")
            endif()
        else()
            set (HAVE_CN_LIBMONITOR "false")
        endif()
    else()
        set (HAVE_CN_LIBMONITOR "false")
    endif()
    message( STATUS "LIBMONITOR_CN_RUNTIME_DIR: " ${LIBMONITOR_CN_RUNTIME_DIR} )
    message( STATUS "HAVE_CN_LIBMONITOR: " ${HAVE_CN_LIBMONITOR} )



#
#--------------------------------------------------------------------------------
# Do find package calls for cbtf only version of OpenSpeedShop in this section
#--------------------------------------------------------------------------------
#
elseif (INSTRUMENTOR MATCHES "cbtf")
    add_definitions(-DBUILD_CBTF=1)
    find_package(Boost 1.41.0 REQUIRED
      COMPONENTS date_time filesystem system thread program_options
    )
    find_package(CBTF)
    find_package(CBTF-Krell)
    find_package(MRNet 4.0.0)
    find_package(XercesC 3.0)
    find_package(CBTF-ArgoNavis)

    if (NOT "${CBTF_KRELL_CN_RUNTIME_DIR}" STREQUAL "")
        if(EXISTS ${CBTF_KRELL_CN_RUNTIME_DIR} AND IS_DIRECTORY "${CBTF_KRELL_CN_RUNTIME_DIR}")
            set ( BUILD_FE_USING_CN_RUNTIMES "true" )
            if( EXISTS ${CBTF_KRELL_CN_RUNTIME_DIR}/lib${LIB_SUFFIX} AND
                IS_DIRECTORY "${CBTF_KRELL_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}" AND 
                EXISTS "${CBTF_KRELL_CN_RUNTIME_DIR}/lib${LIB_SUFFIX}/KrellInstitute/Collectors/pcsamp-collector-monitor-mrnet.so")
               set (CBTF_KRELL_CN_RUNTIME_LIBDIR ${CBTF_KRELL_CN_RUNTIME_DIR}/lib${LIB_SUFFIX} CACHE PATH "Location of compute node cbtf krell libraries.")
               message(STATUS "CBTF_KRELL_CN_RUNTIME_DIR/lib64 was recognized.")
            elseif ( EXISTS "${CBTF_KRELL_CN_RUNTIME_DIR}/lib" AND
                     IS_DIRECTORY "${CBTF_KRELL_CN_RUNTIME_DIR}/lib" AND
                     EXISTS "${CBTF_KRELL_CN_RUNTIME_DIR}/lib/KrellInstitute/Collectors/pcsamp-collector-monitor-mrnet.so")
               set (CBTF_KRELL_CN_RUNTIME_LIBDIR ${CBTF_KRELL_CN_RUNTIME_DIR}/lib CACHE PATH "Location of compute node cbtf krell libraries.")
               message(STATUS "CBTF_KRELL_CN_RUNTIME_DIR/lib was recognized.")
            else()
               # Set BUILD_FE_USING_CN_RUNTIMES to false because we did not find libraries under the top-level directory
               set ( BUILD_FE_USING_CN_RUNTIMES "false" )
            endif()
            if( EXISTS "${CBTF_KRELL_CN_RUNTIME_DIR}/share/KrellInstitute/xml" AND IS_DIRECTORY "${CBTF_KRELL_CN_RUNTIME_DIR}/share/KrellInstitute/xml")
               # CBTF_KRELL_CN_XMLDIR is set to the compute node path to the collector xml files: {CBTF_KRELL_CN_RUNTIME_DIR}/share/KrellInstitute/xml
               set (CBTF_KRELL_CN_XMLDIR ${CBTF_KRELL_CN_RUNTIME_DIR}/share/KrellInstitute/xml CACHE PATH "Location of compute node collector xml files.")
            endif()
        else()
            # Directory specified does not exist or is not a directory
            set ( BUILD_FE_USING_CN_RUNTIMES "false" )
        endif()
    else()
        # No directory specified
        set ( BUILD_FE_USING_CN_RUNTIMES "false" )
    endif()

    message(STATUS "BUILD_FE_USING_CN_RUNTIMES=${BUILD_FE_USING_CN_RUNTIMES}")

endif()

#
#--------------------------------------------------------------------------------
# If Dyninst was found set HAVE_DYNINST which is checked for in other 
# CMakeLists.txt files.
#--------------------------------------------------------------------------------
#
if (DYNINSTAPI_FOUND)
    # This is needed by OfflineExperiment.cxx to call the loop processing code
    set(HAVE_DYNINST "true")
    set(Dyninst_DEFINES "HAVE_DYNINST=1")
else()
    set(Dyninst_DEFINES "")
endif()

#
#--------------------------------------------------------------------------------
# After looking for the DyninstAPI and SymtabAPI packages, reconsile that with
# whether or not the RESOLVE_SYMBOLS request by the builded can be met.
#--------------------------------------------------------------------------------
# 
set(RESOLVE_SYMBOLS "symtabapi" CACHE STRING "resolve symbols using: [bfd,symtabapi]")

if (RESOLVE_SYMBOLS MATCHES "symtabapi")
    if(DYNINSTAPI_FOUND)
      # Use a variable to hold the settings instead of using add_definitions
      # add_definitions is global and we only want to apply this when we want
      # to use symtabapi.  We apply RESOLVE_SYMBOLS_DEFINES using the
      # compile specific set_target_properties command: COMPILE_DEFIINTIONS
      set(RESOLVE_SYMBOLS_DEFINES "OPENSS_USE_SYMTABAPI=1" )
      message(STATUS "Resolve symbols using SymtabAPI.")
    else()
      message(STATUS "Resolve symbols using bfd. No Dyninst package was found.")
      #  Let the rest of OpenSpeedShop (framework and openss) know we are resolving symbols with bfd
      #  RESOLVE_SYMBOLS was set to symtabapi, but Dyninst was not found.
      set(RESOLVE_SYMBOLS "bfd")
    endif()
elseif (RESOLVE_SYMBOLS MATCHES "bfd")
    set(RESOLVE_SYMBOLS_DEFINES "" )
    message(STATUS "Resolve symbols using bfd.")
else ()
    # User specified choice for resolving symbol tables was not one of the allowed types, error condition.
    message(STATUS "Incorrect RESOLVE_SYMBOLS setting: Allowed choices are: [bfd,symtabapi]")
endif()

#
#--------------------------------------------------------------------------------
# Handle MPI settings here.  Set the appropriate flags for the rest of the tool 
# to use when building MPI related libraries (collectors).
#--------------------------------------------------------------------------------
#
set(all_mpi_names "")

if (OPENMPI_FOUND)
    # This is needed by the mpi collectors
    set(HAVE_OPENMPI "true")
    set(HAVE_MPI "true")
    set(DEFAULT_MPI_IMPL_NAME "openmpi")
    set(DEFAULT_MPI_IMPL "OPENMPI")
    set(all_mpi_names ${all_mpi_names} " openmpi ")
endif()

if (MPICH2_FOUND)
    # This is needed by the mpi collectors
    set(HAVE_MPICH2 "true")
    set(HAVE_MPI "true")
    set(DEFAULT_MPI_IMPL_NAME "mpich2")
    set(DEFAULT_MPI_IMPL "MPICH2")
    set(all_mpi_names ${all_mpi_names} " mpich2 ")
endif()

if (MPICH_FOUND)
    # This is needed by the mpi collectors
    set(HAVE_MPICH "true")
    set(HAVE_MPI "true")
    set(DEFAULT_MPI_IMPL_NAME "mpich")
    set(DEFAULT_MPI_IMPL "MPICH")
    set(all_mpi_names ${all_mpi_names} " mpich ")
endif()

if (MPT_FOUND)
    # This is needed by the mpi collectors
    set(HAVE_MPT "true")
    set(HAVE_MPI "true")
    set(DEFAULT_MPI_IMPL_NAME "mpt")
    set(DEFAULT_MPI_IMPL "MPT")
    set(all_mpi_names ${all_mpi_names} " mpt ")
endif()

if (MVAPICH_FOUND)
    # This is needed by the mpi collectors
    set(HAVE_MVAPICH "true")
    set(HAVE_MPI "true")
    set(DEFAULT_MPI_IMPL_NAME "mvapich")
    set(DEFAULT_MPI_IMPL "MVAPICH")
    set(all_mpi_names ${all_mpi_names} " mvapich ")
endif()

if (MVAPICH2_FOUND)
    # This is needed by the mpi collectors
    set(HAVE_MVAPICH2 "true")
    set(HAVE_MPI "true")
    set(DEFAULT_MPI_IMPL_NAME "mvapich2")
    set(DEFAULT_MPI_IMPL "MVAPICH2")
    set(all_mpi_names ${all_mpi_names} " mvapich2 ")
endif()

set(ALL_MPI_IMPL_NAMES ${all_mpi_names})

message( STATUS "ALL_MPI_IMPL_NAMES: " ${ALL_MPI_IMPL_NAMES} )
message( STATUS "DEFAULT_MPI_IMPL: " ${DEFAULT_MPI_IMPL} )
message( STATUS "DEFAULT_MPI_IMPL_NAME: " ${DEFAULT_MPI_IMPL_NAME} )

#
#--------------------------------------------------------------------------------
# If RUNTIME_DIR exists, then we have built a separate version of the portion of
# OpenSpeedShop that runs with the application: runtime libraries and data collectors.
# This is done/needed when the processor type of the compute node (where the application
# executes) is different from the front-end node (where the OpenSpeedShop client 
# components run).  We call that building targeted runtimes, and this is accomplished
# by using the "-DRUNTIME_ONLY" cmake argument in a separate build.
#--------------------------------------------------------------------------------
if (NOT "${RUNTIME_DIR}" STREQUAL "")
    if(EXISTS ${RUNTIME_DIR} AND IS_DIRECTORY "${RUNTIME_DIR}")
        set( HAVE_RUNTIME_DIR "true")
        message(STATUS "RUNTIME_DIR was recognized.")
        message(STATUS "HAVE_RUNTIME_DIR is set to true.")
        if( EXISTS ${RUNTIME_DIR}/lib${LIB_SUFFIX} AND IS_DIRECTORY "${RUNTIME_DIR}/lib${LIB_SUFFIX}")
           set (RUNTIME_LIBDIR  ${RUNTIME_DIR}/lib${LIB_SUFFIX})
        elseif ( EXISTS ${RUNTIME_DIR}/lib AND IS_DIRECTORY "${RUNTIME_DIR}/lib")
           set (RUNTIME_LIBDIR  ${RUNTIME_DIR}/lib)
        else()
           set (RUNTIME_LIBDIR "")
           set( HAVE_RUNTIME_DIR "false")
           message(STATUS "HAVE_RUNTIME_DIR is set to false.")
        endif()
    else()
        message(STATUS "HAVE_RUNTIME_DIR is set to false.")
        set( HAVE_RUNTIME_DIR "false")
        set (RUNTIME_DIR  "")
        set (RUNTIME_LIBDIR  "")
    endif()
else()
    message(STATUS "RUNTIME_DIR matches blanks.")
endif()

#
#--------------------------------------------------------------------------------
# Start building making decisions on what to build based on find_package results
#--------------------------------------------------------------------------------
#
#--------------------------------------------------------------------------------
# The runtime libraries are build both when the RUNTIME_ONLY option is set and
# during the normal build.  Therefore it is not bracketed by the RUNTIME_ONLY checks below.
#--------------------------------------------------------------------------------
#
if(LIBUNWIND_FOUND AND LIBMONITOR_FOUND)
#    add_subdirectory(libopenss-runtime)
endif()

#
#--------------------------------------------------------------------------------
# Build only a subset of OSS (the part that runs on the compute nodes with the application)
# when the -DRUNTIME_ONLY option define is set
# Build the collectors, the runtime directory (see above) and some scripts in the openss directory
# This option is used for building on platforms where the front-end processors are different from 
# the back-end (compute) node processors
#
# What situations is this used in?   The Blue Gene platform has different processors for the compute
# nodes, some Cray machines have different processors for their compute nodes than they do for the
# login nodes where the front-end tool will operate.  Therefore, in these situations, build the runtime first,
# then use that install for the front-end build.
#
# How does it work?  You need to build the runtime only portion first, then
# specify the runtime installation directory in the -DRUNTIME_DIR=<runtime_dir> cmake define when
# building the frontend of OpenSpeedShop.
#-------------------------------------------------------------------------------
#

# TODO: This should only build runtimes for two cases.
# 1. instrumentor == offline and RUNTIME_ONLY was not specified to cmake
#    command line.
# 2. instrumentor == offline and RUNTIME_ONLY was specified to cmake
#    command line.
# The cbtf instrumentor does not need any code from the runtime directory.
#
if (RUNTIME_ONLY MATCHES "true")

# only offline supports this.
    if (INSTRUMENTOR MATCHES "offline")
	add_subdirectory(libopenss-runtime)
	add_subdirectory(plugins/collectors)
	add_subdirectory(openss)
    endif()

    # This is not quite what we would want. Instrumentor choice should not
    # prevent a user from viewing cuda databases. Not sure if ossdumpsym
    # should be tied to instrumentor cbtf.
    if (INSTRUMENTOR MATCHES "cbtf")
        add_subdirectory(ossdumpsym)
    endif()

    if(BUILD_CUDA_ANALYSIS)
        add_subdirectory(libopenss-queries-cuda)
        add_subdirectory(ossdumpcuda)
    endif()

else()
    # In reality, RUNTIME_ONLY should default to false early on in
    # this CMakeLists.txt....
    #set(RUNTIME_ONLY "false")
    if (INSTRUMENTOR MATCHES "offline")
	add_subdirectory(libopenss-runtime)
    endif()

    add_subdirectory(libopenss-message)
    add_subdirectory(libopenss-framework)
    add_subdirectory(libopenss-queries)
    add_subdirectory(libopenss-cli)

    #--------------------------------------------------------------------------------
    # if we have qt3 then install the directories that depend on qt3
    #--------------------------------------------------------------------------------
    if(QT_FOUND)
        message( STATUS "QT_INCLUDE_DIR: "  ${QT_INCLUDE_DIR} )
        message( STATUS "QT_LIBRARIES: "  ${QT_LIBRARIES} )
        add_subdirectory(libopenss-guibase)
        add_subdirectory(libopenss-guiobjects)
        add_subdirectory(libopenss-guichart)
        add_subdirectory(libopenss-guidialogs)
        add_subdirectory(libopenss-guiexprinfo)
        add_subdirectory(libopenss-guiplugin)
        add_subdirectory(libopenss-gui)
    endif()

    #--------------------------------------------------------------------------------
    # build and install collectors, panel, wizards, and views
    #--------------------------------------------------------------------------------
    add_subdirectory(plugins)

    #--------------------------------------------------------------------------------
    # build and install the main driver (openss), ossutil, and the convenience and run scripts
    #--------------------------------------------------------------------------------
    add_subdirectory(openss)

    # This is not quite what we would want. Instrumentor choice should not
    # prevent a user from viewing cuda databases. Not sure if ossdumpsym
    # should be tied to instrumentor cbtf.
    if (INSTRUMENTOR MATCHES "cbtf")
        add_subdirectory(ossdumpsym)
    endif()

    # install include files
    install(DIRECTORY plugins/collectors libopenss-cli libopenss-framework libopenss-message libopenss-queries
        DESTINATION include
        FILES_MATCHING
        PATTERN "*.hxx"
        PATTERN "*.txx"
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

    if(BUILD_CUDA_ANALYSIS)
        add_subdirectory(libopenss-queries-cuda)
        add_subdirectory(ossdumpcuda)

        # install include files
        install(DIRECTORY libopenss-queries-cuda
            DESTINATION include
            FILES_MATCHING
            PATTERN "*.hxx"
            PATTERN "*.txx"
            PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ) 
    endif()

    # Install <openss related>.py files needed when running the OSS python interface
    add_subdirectory(pyscripting)


    #--------------------------------------------------------------------------------
    # install the find_package related cmake files 
    #--------------------------------------------------------------------------------
    install(
        DIRECTORY cmake
        DESTINATION share/KrellInstitute
    )

    # install man directory files
    install(DIRECTORY man
	DESTINATION share
	FILES_MATCHING
	PATTERN "*.1"
	PATTERN "*.3"
	PATTERN "CVS" EXCLUDE
	PATTERN "Makefile.*" EXCLUDE
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

    # demonstrations  design_doc plugin_guide  pyscripting_doc  
    # test_plan  usability_plan  users_guide  webtutorial
    install(DIRECTORY doc
	DESTINATION share/doc/packages/OpenSpeedShop
	FILES_MATCHING
	PATTERN "*.html"
	PATTERN "*.jpg"
	PATTERN "*.adp"
	PATTERN "*.png"
	PATTERN "*.dia"
	PATTERN "*.xml"
	PATTERN "demonstrations" EXCLUDE
	PATTERN "design_doc" EXCLUDE
	PATTERN "test_plan" EXCLUDE
	PATTERN "usability_plan" EXCLUDE
	PATTERN "CVS" EXCLUDE
	PATTERN "Makefile.*" EXCLUDE
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

endif()
