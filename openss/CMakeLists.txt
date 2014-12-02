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

add_definitions(
    -DLIBRARY_DIR="${CMAKE_INSTALL_PREFIX}"
    -DPLUGIN_DIR="openspeedshop"
    -DLIB_DIR_NAME="lib${SUFFIX}"
)

set(INSTRUMENTOR "offline")

set(cbtfinstalldir ${CBTF_ROOT})
set(cbtflibdir ${CBTF_ROOT}/lib${SUFFIX})
set(cbtfruncmd ${CBTF_ROOT}/bin/cbtfrun)
set(cbtfcollectors ${CBTF_ROOT}/lib${SUFFIX}/KrellInstitute/Collectors)
set(cbtfcomponents ${CBTF_ROOT}/lib${SUFFIX}/KrellInstitute/Components)
set(cbtfxmldir ${CBTF_ROOT}/share/KrellInstitute/XML)
set(libmonitordir ${LIBMONITOR_ROOT})
set(libmonitorlibdir ${LIBMONITOR_ROOT}/lib${SUFFIX})
set(oss_installation_dir ${CMAKE_INSTALL_PREFIX})
set(ossinstalldir ${CMAKE_INSTALL_PREFIX})
set(oss_instrumentor ${INSTRUMENTOR})
set(osslibdir ${CMAKE_INSTALL_PREFIX}/lib${SUFFIX})
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
set(target_os ${TARGET_OS})
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
  

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/libopenss-framework
    ${LTDL_INCLUDE_DIR}
    )

add_executable(openss
	openss.cxx
    )
target_link_libraries(openss
    openss-framework
    ${LTDL_LIBRARY_SHARED}
    ${CMAKE_DL_LIBS}
    )
install(TARGETS openss
    RUNTIME DESTINATION bin
    )

add_executable(ossutil
	ossutil.cxx
    )
target_link_libraries(ossutil
    openss-framework
    openss-framework-binutils
    ${LIB_LTDL}
    ${CMAKE_DL_LIBS}
    )

install(TARGETS ossutil
    RUNTIME DESTINATION bin
    )
if (${INSTRUMENTOR} EQUAL "offline")
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
endif(CMAKE_HOST_UNIX)
