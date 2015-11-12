################################################################################
# Copyright (c) 2013-2015 Krell Institute. All Rights Reserved.
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

include(FindPackageHandleStandardArgs)

find_library(SymtabAPI_LIBRARY NAMES libsymtabAPI.so
    HINTS $ENV{DYNINST_DIR}
    HINTS ${DYNINST_DIR}
    PATH_SUFFIXES lib lib64
    )
#
# NOTE: We are checking for two Dyninst include install variants
# One where the Dyninst include file are installed into <dyn_inst_dir>/include
# and the other where  where the Dyninst include file 
# are installed into <dyn_inst_dir>/include/dyninst
# We set an include_suffix variable based on the variant and use it
# to set the SymtabAPI_INCLUDE_DIR properly.
#
if (EXISTS "${DYNINST_DIR}/include/dyninst")
    find_path(SymtabAPI_INCLUDE_DIR
        NAMES dyninst/Symtab.h
        HINTS $ENV{DYNINST_DIR}
        HINTS ${DYNINST_DIR}
        PATH_SUFFIXES include
    )
    set(include_suffix "/dyninst")
else()
    find_path(SymtabAPI_INCLUDE_DIR
        NAMES Symtab.h
        HINTS $ENV{DYNINST_DIR}
        HINTS ${DYNINST_DIR}
        PATH_SUFFIXES include
    )
    set(include_suffix "")
endif()

find_package_handle_standard_args(
    SymtabAPI DEFAULT_MSG SymtabAPI_LIBRARY SymtabAPI_INCLUDE_DIR
    )

set(SymtabAPI_LIBRARIES ${SymtabAPI_LIBRARY})
set(SymtabAPI_INCLUDE_DIRS ${SymtabAPI_INCLUDE_DIR}${include_suffix})
GET_FILENAME_COMPONENT(SymtabAPI_LIB_DIR ${SymtabAPI_LIBRARY} PATH )

mark_as_advanced(SymtabAPI_LIBRARY SymtabAPI_INCLUDE_DIR SymtabAPI_LIB_DIR)

if(SYMTABAPI_FOUND AND DEFINED SymtabAPI_INCLUDE_DIR)

    if (EXISTS "${SymtabAPI_INCLUDE_DIR}${include_suffix}/version.h")

        file(READ ${SymtabAPI_INCLUDE_DIR}${include_suffix}/version.h DyninstAPI_VERSION_FILE)

        string(REGEX REPLACE
            ".*#[ ]*define DYNINST_MAJOR_VERSION[ ]+([0-9]+)\n.*" "\\1"
            SymtabAPI_VERSION_MAJOR ${DyninstAPI_VERSION_FILE}
        )

        string(REGEX REPLACE
            ".*#[ ]*define DYNINST_MINOR_VERSION[ ]+([0-9]+)\n.*" "\\1"
            SymtabAPI_VERSION_MINOR ${DyninstAPI_VERSION_FILE}
        )

        string(REGEX REPLACE
            ".*#[ ]*define DYNINST_PATCH_VERSION[ ]+([0-9]+)\n.*" "\\1"
            SymtabAPI_VERSION_PATCH ${DyninstAPI_VERSION_FILE}
        )

else()
        file(READ ${SymtabAPI_INCLUDE_DIR}${include_suffix}/Symtab.h SymtabAPI_VERSION_FILE)

        string(REGEX REPLACE
            ".*#[ ]*define SYM_MAJOR[ ]+([0-9]+)\n.*" "\\1"
            SymtabAPI_VERSION_MAJOR ${SymtabAPI_VERSION_FILE}
        )

        string(REGEX REPLACE
            ".*#[ ]*define SYM_MINOR[ ]+([0-9]+)\n.*" "\\1"
            SymtabAPI_VERSION_MINOR ${SymtabAPI_VERSION_FILE}
            )

        string(REGEX REPLACE
            ".*#[ ]*define SYM_BETA[ ]+([0-9]+)\n.*" "\\1"
            SymtabAPI_VERSION_PATCH ${SymtabAPI_VERSION_FILE}
            )
endif()

    set(SymtabAPI_VERSION_STRING ${SymtabAPI_VERSION_MAJOR}.${SymtabAPI_VERSION_MINOR}.${SymtabAPI_VERSION_PATCH})
  
    message(STATUS "SymtabAPI version: " ${SymtabAPI_VERSION_STRING})

    if(DEFINED SymtabAPI_FIND_VERSION)
        if(${SymtabAPI_VERSION_STRING} VERSION_LESS ${SymtabAPI_FIND_VERSION})

            set(SYMTABAPI_FOUND FALSE)

            if(DEFINED SymtabAPI_FIND_REQUIRED)
                message(FATAL_ERROR
                    "Could NOT find SymtabAPI  (version < "
                    ${SymtabAPI_FIND_VERSION} ")"
                    )
            else()
                message(STATUS
                    "Could NOT find SymtabAPI  (version < " 
                    ${SymtabAPI_FIND_VERSION} ")"
                    )
            endif()
 
        endif()
    endif()

endif()
