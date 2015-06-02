################################################################################
# Copyright (c) 2011-2015 Krell Institute. All Rights Reserved.
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

find_library(XercesC_LIBRARY NAMES libxerces-c.so
    HINTS $ENV{XERCESC_DIR} ${XERCESC_DIR}
    PATH_SUFFIXES lib lib64
    )

find_path(XercesC_INCLUDE_DIR xercesc/util/XercesVersion.hpp
    HINTS $ENV{XERCESC_DIR} ${XERCESC_DIR}
    PATH_SUFFIXES include
    )

find_package_handle_standard_args(
    XercesC DEFAULT_MSG XercesC_LIBRARY XercesC_INCLUDE_DIR
    )

set(XercesC_LIBRARIES ${XercesC_LIBRARY})
set(XercesC_INCLUDE_DIRS ${XercesC_INCLUDE_DIR})

mark_as_advanced(XercesC_LIBRARY XercesC_INCLUDE_DIR)

if(XERCESC_FOUND AND DEFINED XercesC_INCLUDE_DIR)
  
    file(READ
        ${XercesC_INCLUDE_DIR}/xercesc/util/XercesVersion.hpp
        XercesC_VERSION_FILE
        )
  
    string(REGEX REPLACE
        ".*#define XERCES_VERSION_MAJOR[ ]+([0-9]+)\n.*" "\\1"
        XercesC_VERSION_MAJOR ${XercesC_VERSION_FILE}
        )
      
    string(REGEX REPLACE
        ".*#define XERCES_VERSION_MINOR[ ]+([0-9]+)\n.*" "\\1"
        XercesC_VERSION_MINOR ${XercesC_VERSION_FILE}
        )
  
    string(REGEX REPLACE
        ".*#define XERCES_VERSION_REVISION[ ]+([0-9]+)\n.*" "\\1"
        XercesC_VERSION_PATCH ${XercesC_VERSION_FILE}
        )
  
    set(XercesC_VERSION_STRING 
      ${XercesC_VERSION_MAJOR}.${XercesC_VERSION_MINOR}.${XercesC_VERSION_PATCH}
      )
  
    message(STATUS "XercesC version: " ${XercesC_VERSION_STRING})

    if(DEFINED XercesC_FIND_VERSION)
        if(${XercesC_VERSION_STRING} VERSION_LESS ${XercesC_FIND_VERSION})

            set(XERCESC_FOUND FALSE)

            if(DEFINED XercesC_FIND_REQUIRED)
                message(FATAL_ERROR
                    "Could NOT find XercesC  (version < "
                    ${XercesC_FIND_VERSION} ")"
                    )
            else()
                message(STATUS
                    "Could NOT find XercesC  (version < " 
                    ${XercesC_FIND_VERSION} ")"
                    )
            endif()
 
        endif()
    endif()
  
endif()
