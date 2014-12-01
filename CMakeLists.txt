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

find_package(Doxygen)
find_package(CUDA)
find_package(BISON)
find_package(FLEX)
find_package(OpenMP)
find_package(OpenMPI)
find_package(Papi)
find_package(PythonInterp)
find_package(PythonLibs)
find_package(LibUnwind)
find_package(LibMonitor)
find_package(LibElf)
find_package(LibDwarf)
find_package(Binutils)
find_package(Boost 1.39.0 REQUIRED
    COMPONENTS date_time filesystem system thread unit_test_framework
    )
find_package(MRNet 4.0.0)
find_package(Threads)
find_package(XercesC 3.0)
find_package(Sqlite3)
find_package(LTDL)

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
