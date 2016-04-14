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

find_library(DyninstAPI_LIBRARY NAMES libdyninstAPI.so
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
# to set the Dyninst_INCLUDE_DIR properly.
#
if (EXISTS "${DYNINST_DIR}/include/dyninst")
    find_path(DyninstAPI_INCLUDE_DIR
        NAMES dyninst/BPatch.h
        HINTS $ENV{DYNINST_DIR}
        HINTS ${DYNINST_DIR}
        PATH_SUFFIXES include
    )
    set(include_suffix "/dyninst")
else()
    find_path(DyninstAPI_INCLUDE_DIR
        NAMES BPatch.h
        HINTS $ENV{DYNINST_DIR}
        HINTS ${DYNINST_DIR}
        PATH_SUFFIXES include
    )
    set(include_suffix "")
endif()

find_package_handle_standard_args(
    DyninstAPI DEFAULT_MSG DyninstAPI_LIBRARY DyninstAPI_INCLUDE_DIR
    )

set(DyninstAPI_LIBRARIES ${DyninstAPI_LIBRARY})
set(DyninstAPI_INCLUDE_DIRS ${DyninstAPI_INCLUDE_DIR}${include_suffix})
set(DyninstAPI_DEFINES "HAVE_DYNINST")

mark_as_advanced(DyninstAPI_LIBRARY DyninstAPI_INCLUDE_DIR)

if(DYNINSTAPI_FOUND AND DEFINED DyninstAPI_INCLUDE_DIR)

    if (EXISTS "${DyninstAPI_INCLUDE_DIR}${include_suffix}/version.h")

        file(READ ${DyninstAPI_INCLUDE_DIR}${include_suffix}/version.h DyninstAPI_VERSION_FILE)

        string(REGEX REPLACE
            ".*#[ ]*define DYNINST_MAJOR_VERSION[ ]+([0-9]+)\n.*" "\\1"
            DyninstAPI_VERSION_MAJOR ${DyninstAPI_VERSION_FILE}
        )

        string(REGEX REPLACE
            ".*#[ ]*define DYNINST_MINOR_VERSION[ ]+([0-9]+)\n.*" "\\1"
            DyninstAPI_VERSION_MINOR ${DyninstAPI_VERSION_FILE}
        )

        string(REGEX REPLACE
            ".*#[ ]*define DYNINST_PATCH_VERSION[ ]+([0-9]+)\n.*" "\\1"
            DyninstAPI_VERSION_PATCH ${DyninstAPI_VERSION_FILE}
        )
else()

        file(READ ${DyninstAPI_INCLUDE_DIR}${include_suffix}/BPatch.h DyninstAPI_VERSION_FILE)

        string(REGEX REPLACE
            ".*#[ ]*define DYNINST_MAJOR[ ]+([0-9]+)\n.*" "\\1"
            DyninstAPI_VERSION_MAJOR ${DyninstAPI_VERSION_FILE}
        )

        string(REGEX REPLACE
            ".*#[ ]*define DYNINST_MINOR[ ]+([0-9]+)\n.*" "\\1"
            DyninstAPI_VERSION_MINOR ${DyninstAPI_VERSION_FILE}
        )

        string(REGEX REPLACE
            ".*#[ ]*define DYNINST_SUBMINOR[ ]+([0-9]+)\n.*" "\\1"
            DyninstAPI_VERSION_PATCH ${DyninstAPI_VERSION_FILE}
        )
endif()

    set(DyninstAPI_VERSION_STRING ${DyninstAPI_VERSION_MAJOR}.${DyninstAPI_VERSION_MINOR}.${DyninstAPI_VERSION_PATCH})
  
    message(STATUS "DyninstAPI version: " ${DyninstAPI_VERSION_STRING})
    message(STATUS "DyninstAPI_VERSION_MAJOR version: " ${DyninstAPI_VERSION_MAJOR})

    if(DEFINED DyninstAPI_FIND_VERSION)
        if(${DyninstAPI_VERSION_STRING} VERSION_LESS ${DyninstAPI_FIND_VERSION})

            set(DYNINSTAPI_FOUND FALSE)

            if(DEFINED DyninstAPI_FIND_REQUIRED)
                message(FATAL_ERROR
                    "Could NOT find DyninstAPI  (version < "
                    ${DyninstAPI_FIND_VERSION} ")"
                    )
            else()
                message(STATUS
                    "Could NOT find DyninstAPI  (version < " 
                    ${DyninstAPI_FIND_VERSION} ")"
                    )
            endif()
 
        endif()
    endif()

endif()
