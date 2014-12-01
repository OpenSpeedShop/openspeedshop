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
    -DPACKAGE_STRING="OpenSpeedShop XXX"
    -DPYTHON_FP_LIB_NAME="${PYTHON_LIBRARY}"
)

set(cbtfinstalldir ${CBTF_ROOT})
set(cbtflibdir ${CBTF_ROOT}/lib${SUFFIX})
set(cbtfruncmd ${CBTF_ROOT}/bin/cbtfrun)
set(cbtfcollectors ${CBTF_ROOT}/lib${SUFFIX}/KrellInstitute/Collectors)
set(cbtfcomponents ${CBTF_ROOT}/lib${SUFFIX}/KrellInstitute/Components)
set(cbtfxmldir ${CBTF_ROOT}/share/KrellInstitute/XML)
set(libmonitordir ${LIBMONITOR_ROOT})
set(libmonitorlibdir ${LIBMONITOR_ROOT}/lib${SUFFIX})
set(oss_installation_dir ${CMAKE_INSTALL_PREFIX})
set(osslibdir ${CMAKE_INSTALL_PREFIX}/lib${SUFFIX})
set(machine_dependent_libdir lib${SUFFIX})
set(alt_machine_dependent_libdir lib${SUFFIX})
set(ossdefaultmpi ${DEFAULT_MPI_IMPL})
set(mpichdir ${MPICH_DIR})
set(mvapichdir ${MVAPICH_DIR})
set(mpich2dir ${MPICH2_DIR})
set(mvapich2dir ${MVAPICH2_DIR})
set(mptdir ${MPT_DIR})
set(openmpidir ${OPENMPI_DIR})
set(target_os ${TARGET_OS})
configure_file(offline.py.in offline.py)
configure_file(cbtf-offline.py.in cbtf-offline.py)
configure_file(mrnet.py.in mrnet.py COPYONLY)

install(PROGRAMS
	${CMAKE_CURRENT_BINARY_DIR}/offline.py
	${CMAKE_CURRENT_BINARY_DIR}/cbtf-offline.py
	${CMAKE_CURRENT_BINARY_DIR}/mrnet.py
	${PROJECT_SOURCE_DIR}/libopenss-cli/init.py
	DESTINATION
	${CMAKE_INSTALL_PREFIX}/lib${SUFFIX}/openspeedshop)


set(GCC_FORMAT_CONTAINS_NUL "-Wno-format-contains-nul")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_FORMAT_CONTAINS_NUL}")

set(CLI_SOURCES
        ArgClass.hxx
        Clip.hxx
        Commander.hxx Commander.cxx
        CommandObject.hxx CommandObject.cxx
        Load_Messages.cxx help_cmd_msg.dxx
        PY_Input.cxx help_general_msg.dxx
        SS_Cmd_Control.hxx SS_Cmd_Control.cxx
        SS_Cmd_Execution.hxx SS_Cmd_Execution.cxx
        SS_savedViewInfo.hxx
        SS_CommandResult.hxx SS_CommandResult.cxx
        SS_Compare.cxx
        SS_Configure.hxx SS_Configure.cxx
        SS_Exp.hxx SS_Exp.cxx
        SS_Input_Manager.hxx help_grammar_msg.dxx
        SS_Output.hxx
        SS_Parse_Lex.lxx help_topic_msg.dxx
        SS_Parse_Param.hxx SS_Parse_Param.cxx
        SS_Parse_Interval.hxx SS_Parse_Interval.cxx
        SS_Parse_Range.hxx SS_Parse_Range.cxx
        SS_Parse_Result.hxx SS_Parse_Result.cxx
        SS_Parse_Target.hxx SS_Parse_Target.cxx
        SS_Parse_Yacc.yxx
        SS_Parse_Yacc.cxx
        SS_Parse_Lex.cxx
        SS_ThreadInfo.hxx SS_ThreadInfo.cxx
        SS_View.hxx SS_View.cxx
        SS_View_init.cxx
        SS_View_detail.hxx SS_View_detail.txx
        SS_View_getmetrics.txx
        SS_View_multi.cxx
        SS_View_output.cxx
        SS_View_stats.cxx
        SS_View_util.cxx
        SS_Watcher.hxx SS_Watcher.cxx
        SS_Start.cxx
        SS_Settings.hxx SS_Settings.cxx
        SS_Parse_Expr.hxx help_cmd_msg.dxx  help_general_msg.dxx 
        help_scripting_msg.dxx help_grammar_msg.dxx  help_topic_msg.dxx
        SS_View_bythread_help.hxx SS_View_bythread_recognize.hxx SS_View_detail.hxx  SS_View.hxx
        SS_View_bythread_locations.hxx SS_View_bythread_setmetrics.hxx SS_View_Expr.hxx
        SS_View_metric_expressions.hxx
        Start_Modes.cxx
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/SS_Parse_Yacc.cxx
        ${CMAKE_CURRENT_BINARY_DIR}/SS_Parse_Yacc.hxx
    SOURCE SS_Parse_Yacc.yxx
    COMMAND ${BISON_EXECUTABLE}
    ARGS -d -y ${PROJECT_SOURCE_DIR}/libopenss-cli/SS_Parse_Yacc.yxx
	 --defines=${CMAKE_CURRENT_BINARY_DIR}/SS_Parse_Yacc.hxx
         -o ${CMAKE_CURRENT_BINARY_DIR}/SS_Parse_Yacc.cxx
    DEPENDS
        SS_Parse_Yacc.yxx SS_Parse_Result.hxx
    )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/SS_Parse_Lex.cxx
    SOURCE SS_Parse_Lex.lxx
    COMMAND ${FLEX_EXECUTABLE}
    ARGS -o ${CMAKE_CURRENT_BINARY_DIR}/SS_Parse_Lex.cxx
	  ${PROJECT_SOURCE_DIR}/libopenss-cli/SS_Parse_Lex.lxx
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/SS_Parse_Yacc.cxx SS_Parse_Lex.lxx
    )

add_library(openss-cli SHARED
	${CLI_SOURCES}
    )


add_definitions(
  -DOpenSpeedShop_LIBRARY_FILE_DIR="${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}"
  )
  

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Sqlite3_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/libopenss-cli
    ${PROJECT_SOURCE_DIR}/libopenss-framework
    ${PROJECT_SOURCE_DIR}/libopenss-guibase
    ${PROJECT_SOURCE_DIR}/libopenss-guiobjects
    ${PROJECT_SOURCE_DIR}/libopenss-queries
    ${PROJECT_SOURCE_DIR}/libopenss-message
    ${PYTHON_INCLUDE_DIR}
    )

target_link_libraries(openss-cli
    openss-framework
    openss-message
    ${Sqlite3_SHARED_LIBRARIES}
    ${PYTHON_LIBRARY}
    ${CMAKE_DL_LIBS}
    ltdl
    )


set_target_properties(openss-cli PROPERTIES VERSION 1.1.0)
set(CMAKE_SHARED_LINKER_FLAGS -Wl,"--defsym,libopenss_cli_LTX_cli_init=cli_init")

install(TARGETS openss-cli
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    )

if(CMAKE_HOST_UNIX) 
    install(CODE " 
    EXECUTE_PROCESS(COMMAND ln -sf libopenss-cli.so PYopenss.so 
       WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib${SUFFIX} 
       ) 
    ") 
endif(CMAKE_HOST_UNIX)
