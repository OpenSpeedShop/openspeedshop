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

cmake_minimum_required(VERSION 2.8.0)

project(OpenSpeedShop)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

include(CheckIncludeFile)

check_include_file("stdint.h" STDINT_H_FOUND)
if (STDINT_H_FOUND)
    add_definitions(-DHAVE_STDINT_H=1)
endif()


#cmake -DVARNAME:STRING=value
set(TLS_MODEL "implicit" CACHE STRING "OpenSpeedShop tls model: [implicit,explicit]")
if (TLS_MODULE MATCHES "explicit")
add_definitions(-DUSE_EXPLICIT_TLS)
message(STATUS "Build runtimes using explicit tls model.")
else()
message(STATUS "Build runtimes using implicit tls model.")
endif()

set(TARGET_OS "" CACHE STRING "OpenSpeedShop target os: none or [bgq,bgp]")
if (TARGET_OS MATCHES "bgq")
add_definitions(-DTARGET_OS=bgq)
message(STATUS "Build runtimes for target bgq.")
elseif (TARGET_OS MATCHES "bgp")
add_definitions(-DTARGET_OS=bgp)
message(STATUS "Build runtimes for target bgp.")
else()
message(STATUS "Build runtimes for localhost.")
endif()

set(BUILD_CLI_TIMING "no" CACHE STRING "build cli timing code: [no,yes]")
if (BUILD_CLI_TIMING MATCHES "yes")
add_definitions(-DBUILD_CLI_TIMING=1)
message(STATUS "Build cli with timing code enabled.")
endif()

#On arm/aarch64 systems, dyninst/symtabapi are not ready so default to bfd
set(RESOLVE_SYMBOLS "bfd" CACHE STRING "resolve symbols using: [bfd,symtabapi]")
if (RESOLVE_SYMBOLS MATCHES "symtabapi")
add_definitions(-DOPENSS_USE_SYMTABAPI=1)
message(STATUS "Resolve symbols using SymtabAPI.")
else()
message(STATUS "Resolve symbols using bfd.")
endif()

set(BUILD_PTGF "no" CACHE STRING "resolve symbols using: [no, direct, all]")
if (BUILD_PTGF MATCHES "direct")
add_definitions(-DBUILD_PTGF_DIRECT=1)
message(STATUS "Build ptgf direct.")
elseif (BUILD_PTGF MATCHES "all")
add_definitions(-DBUILD_PTGF_DIRECT=1)
add_definitions(-DBUILD_PTGF_SERVER=1)
message(STATUS "Build ptgf all.")
endif()

#cmake -DVARNAME:STRING=value
set(INSTRUMENTOR "offline" CACHE STRING "OpenSpeedShop instrumentor type: [offline,cbtf]")

# common needed dependency packages
if (INSTRUMENTOR MATCHES "offline" OR INSTRUMENTOR MATCHES "cbtf")
find_package(Doxygen)
find_package(BISON)
find_package(FLEX)
find_package(OpenMP)
find_package(OpenMPI)
find_package(PythonInterp)
find_package(PythonLibs)
find_package(LibElf)
find_package(LibDwarf)
find_package(Binutils)
find_package(Threads)
find_package(Sqlite3)
find_package(LTDL)
endif()

if (INSTRUMENTOR MATCHES "offline")
find_package(LibMonitor)
find_package(LibUnwind)
find_package(Papi)
message(STATUS "MRNET_ROOT not needed")
endif()

if (INSTRUMENTOR MATCHES "cbtf")
find_package(Boost 1.39.0 REQUIRED
    COMPONENTS date_time filesystem system thread unit_test_framework
    )
find_package(CUDA)
find_package(MRNet 4.0.0)
find_package(XercesC 3.0)
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "ppc64*")
    set(CMAKE_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib64)
    set(LIB_SUFFIX 64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "ppc*")
    set(CMAKE_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib)
elseif(EXISTS /usr/lib64)
    set(LIB_SUFFIX 64)
    set(CMAKE_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
endif()


if(LIBUNWIND_FOUND AND LIBMONITOR_FOUND)
    add_subdirectory(libopenss-runtime)
endif()

add_subdirectory(libopenss-message)
add_subdirectory(libopenss-framework)
add_subdirectory(libopenss-queries)
add_subdirectory(libopenss-cli)

add_subdirectory(plugins)

add_subdirectory(openss)

install(
    DIRECTORY cmake
    DESTINATION share/KrellInstitute
    )
