################################################################################
# Copyright (c) 2012 Argo Navis Technologies. All Rights Reserved.
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

find_library(MRNet_MRNET_LIBRARY NAMES libmrnet.so
    HINTS $ENV{MRNET_DIR}
    HINTS ${MRNET_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(MRNet_XPLAT_LIBRARY NAMES libxplat.so 
    HINTS $ENV{MRNET_DIR}
    HINTS ${MRNET_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(MRNet_MRNET_LW_SHARED_LIBRARY NAMES libmrnet_lightweight.so
    HINTS $ENV{MRNET_DIR}
    HINTS ${MRNET_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(MRNet_XPLAT_LW_SHARED_LIBRARY NAMES libxplat_lightweight.so
    HINTS $ENV{MRNET_DIR}
    HINTS ${MRNET_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(MRNet_MRNET_LW_STATIC_LIBRARY NAMES libmrnet_lightweight.a
    HINTS $ENV{MRNET_DIR}
    HINTS ${MRNET_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(MRNet_XPLAT_LW_STATIC_LIBRARY NAMES libxplat_lightweight.a
    HINTS $ENV{MRNET_DIR}
    HINTS ${MRNET_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(MRNet_MRNET_LWR_SHARED_LIBRARY NAMES libmrnet_lightweight_r.so
    HINTS $ENV{MRNET_DIR}
    HINTS ${MRNET_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(MRNet_XPLAT_LWR_SHARED_LIBRARY NAMES libxplat_lightweight_r.so
    HINTS $ENV{MRNET_DIR}
    HINTS ${MRNET_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(MRNet_MRNET_LWR_STATIC_LIBRARY NAMES libmrnet_lightweight_r.a
    HINTS $ENV{MRNET_DIR}
    HINTS ${MRNET_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(MRNet_XPLAT_LWR_STATIC_LIBRARY NAMES libxplat_lightweight_r.a
    HINTS $ENV{MRNET_DIR}
    HINTS ${MRNET_DIR}
    PATH_SUFFIXES lib lib64
    )

find_path(MRNet_INCLUDE_DIR mrnet/MRNet.h 
    HINTS $ENV{MRNET_DIR}
    HINTS ${MRNET_DIR}
    PATH_SUFFIXES include
    )

if(DEFINED $ENV{SYSROOT_DIR})
  find_library(ALPS_EXTRA_LIBRARY1 NAMES libalps.so libalpslli.so libalpsutil.so
      HINTS "$ENV{SYSROOT_DIR}"
      HINTS "${SYSROOT_DIR}"
      PATH_SUFFIXES  usr/lib/alps
      )

  find_library(ALPS_EXTRA_LIBRARY2 NAMES libxmlrpc-epi.so libexpat.so
      HINTS "$ENV{SYSROOT_DIR}"
      HINTS "${SYSROOT_DIR}"
      PATH_SUFFIXES  usr/lib64
      )
else ()
  if(DEFINED /usr/lib/alps)
    find_library(ALPS_EXTRA_LIBRARY1 NAMES libalps.so libalpslli.so libalpsutil.so
        HINTS /usr/lib
        PATH_SUFFIXES  alps
        )

    find_library(ALPS_EXTRA_LIBRARY2 NAMES libxmlrpc-epi.so 
        HINTS /usr
        PATH_SUFFIXES lib64
        )
  endif()
endif()

find_package_handle_standard_args(
    MRNet DEFAULT_MSG
    MRNet_MRNET_LIBRARY MRNet_XPLAT_LIBRARY
    MRNet_MRNET_LW_SHARED_LIBRARY MRNet_XPLAT_LW_SHARED_LIBRARY
    MRNet_MRNET_LW_STATIC_LIBRARY MRNet_XPLAT_LW_STATIC_LIBRARY
    MRNet_INCLUDE_DIR
    )

if((DEFINED $ENV{SYSROOT_DIR}) OR (DEFINED /usr/lib/alps))
 find_package_handle_standard_args(
      ALPS DEFAULT_MSG
      ALPS_EXTRA_LIBRARY1
      ALPS_EXTRA_LIBRARY2
      )
 set(MRNet_LIBRARIES ${MRNet_MRNET_LIBRARY} ${MRNet_XPLAT_LIBRARY} ${ALPS_EXTRA_LIBRARY1} ${ALPS_EXTRA_LIBRARY2})
else ()
 set(MRNet_LIBRARIES ${MRNet_MRNET_LIBRARY} ${MRNet_XPLAT_LIBRARY})
endif()

set(MRNet_LW_SHARED_LIBRARIES ${MRNet_MRNET_LW_SHARED_LIBRARY} ${MRNet_XPLAT_LW_SHARED_LIBRARY})

set(MRNet_LW_STATIC_LIBRARIES ${MRNet_MRNET_LW_STATIC_LIBRARY} ${MRNet_XPLAT_LW_STATIC_LIBRARY})

set(MRNet_LWR_SHARED_LIBRARIES ${MRNet_MRNET_LWR_SHARED_LIBRARY} ${MRNet_XPLAT_LWR_SHARED_LIBRARY})

set(MRNet_LWR_STATIC_LIBRARIES ${MRNet_MRNET_LWR_STATIC_LIBRARY} ${MRNet_XPLAT_LWR_STATIC_LIBRARY})

set(MRNet_DEFINES "-Dos_linux")

if(MRNET_FOUND AND DEFINED MRNet_INCLUDE_DIR)

    file(READ ${MRNet_INCLUDE_DIR}/mrnet/Types.h MRNet_VERSION_FILE)
  
    string(REGEX REPLACE
        ".*#[ ]*define MRNET_VERSION_MAJOR[ ]+([0-9]+)\n.*" "\\1"
        MRNet_VERSION_MAJOR ${MRNet_VERSION_FILE}
        )
      
    string(REGEX REPLACE
        ".*#[ ]*define MRNET_VERSION_MINOR[ ]+([0-9]+)\n.*" "\\1"
        MRNet_VERSION_MINOR ${MRNet_VERSION_FILE}
        )
  
    string(REGEX REPLACE
        ".*#[ ]*define MRNET_VERSION_REV[ ]+([0-9]+)\n.*" "\\1"
        MRNet_VERSION_PATCH ${MRNet_VERSION_FILE}
        )
  
    set(MRNet_VERSION_STRING 
        ${MRNet_VERSION_MAJOR}.${MRNet_VERSION_MINOR}.${MRNet_VERSION_PATCH}
        )
  
    message(STATUS "MRNet version: " ${MRNet_VERSION_STRING})

    if(DEFINED MRNet_FIND_VERSION)
        if(${MRNet_VERSION_STRING} VERSION_LESS ${MRNet_FIND_VERSION})

            set(MRNET_FOUND FALSE)

            if(DEFINED MRNet_FIND_REQUIRED)
                message(FATAL_ERROR
                    "Could NOT find MRNet  (version < "
                    ${MRNet_FIND_VERSION} ")"
                    )
            else()
                message(STATUS
                    "Could NOT find MRNet  (version < " 
                    ${MRNet_FIND_VERSION} ")"
                    )
            endif()
 
        endif()
    endif()
  
    if(MRNet_VERSION_STRING VERSION_LESS "4.0.0")

        set(MRNet_INCLUDE_DIRS ${MRNet_INCLUDE_DIR})

        mark_as_advanced(
            MRNet_MRNET_LIBRARY MRNet_XPLAT_LIBRARY
            MRNet_MRNET_LW_SHARED_LIBRARY MRNet_XPLAT_LW_SHARED_LIBRARY
            MRNet_MRNET_LW_STATIC_LIBRARY MRNet_XPLAT_LW_STATIC_LIBRARY
            MRNet_MRNET_LWR_STATIC_LIBRARY MRNet_XPLAT_LWR_STATIC_LIBRARY
            MRNet_LWR_SHARED_LIBRARIES MRNet_LWR_STATIC_LIBRARIES 
            MRNet_INCLUDE_DIR
            )

    else()
      
        #
        # Find the MRNet 4 (and up) configuration header files. These are found
        # in the lib[64] subdirectory rather than the include subdirectory where
        # one would expect to find them...
        #

        find_path(
            MRNet_MRNET_CONFIG_INCLUDE_DIR mrnet_config.h
            HINTS $ENV{MRNET_DIR}
            HINTS ${MRNET_DIR}
            PATH_SUFFIXES 
                lib/mrnet-${MRNet_VERSION_STRING}/include
                lib64/mrnet-${MRNet_VERSION_STRING}/include
            )

        if(NOT MRNet_MRNET_CONFIG_INCLUDE_DIR)
            set(MRNET_FOUND FALSE)
            message(STATUS
                "Could NOT find the MRNet " ${MRNet_VERSION_STRING}
                " mrnet configuration header file"
                )
        endif()

        find_path(
            MRNet_XPLAT_CONFIG_INCLUDE_DIR xplat_config.h
            HINTS $ENV{MRNET_DIR}
            HINTS ${MRNET_DIR}
            PATH_SUFFIXES 
                lib/xplat-${MRNet_VERSION_STRING}/include
                lib64/xplat-${MRNet_VERSION_STRING}/include
            )

        if(NOT MRNet_XPLAT_CONFIG_INCLUDE_DIR)
            set(MRNET_FOUND FALSE)
            message(STATUS
                "Could NOT find the MRNet " ${MRNet_VERSION_STRING}
                " xplat configuration header files"
                )
        endif()
        

        GET_FILENAME_COMPONENT(MRNet_LIB_DIR ${MRNet_MRNET_LIBRARY} PATH )
        GET_FILENAME_COMPONENT(MRNet_DIR ${MRNet_INCLUDE_DIR} PATH )

        set(MRNet_INCLUDE_DIRS 
            ${MRNet_INCLUDE_DIR}
            ${MRNet_MRNET_CONFIG_INCLUDE_DIR}
            ${MRNet_XPLAT_CONFIG_INCLUDE_DIR}
            )
        
        mark_as_advanced(
            MRNet_MRNET_LIBRARY MRNet_XPLAT_LIBRARY
            MRNet_MRNET_LW_SHARED_LIBRARY MRNet_XPLAT_LW_SHARED_LIBRARY
            MRNet_MRNET_LW_STATIC_LIBRARY MRNet_XPLAT_LW_STATIC_LIBRARY
            MRNet_MRNET_LWR_SHARED_LIBRARY MRNet_XPLAT_LWR_SHARED_LIBRARY
            MRNet_LWR_SHARED_LIBRARIES MRNet_LWR_STATIC_LIBRARIES 
            MRNet_INCLUDE_DIR
            MRNet_MRNET_CONFIG_INCLUDE_DIR
            MRNet_XPLAT_CONFIG_INCLUDE_DIR
            MRNet_DIR
            MRNet_LIB_DIR
            MRNet_LIBRARIES
            )
        
    endif()
      
endif()


