################################################################################
# Copyright (c) 2012 Argo Navis Technologies. All Rights Reserved.
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

find_library(CBTF_MESSAGES_BASE_SHARED_LIBRARY
    NAMES libcbtf-messages-base.so
    HINTS $ENV{CBTF_ROOT} $ENV{CBTF_PREFIX}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_MESSAGES_BASE_STATIC_LIBRARY
    NAMES libcbtf-messages-base.a
    HINTS $ENV{CBTF_ROOT} $ENV{CBTF_PREFIX}
    PATH_SUFFIXES lib lib64
    )
  
find_library(CBTF_MESSAGES_COLLECTOR_SHARED_LIBRARY
    NAMES libcbtf-messages-collector.so
    HINTS $ENV{CBTF_ROOT} $ENV{CBTF_PREFIX}
    PATH_SUFFIXES lib lib64
    )
  
find_library(CBTF_MESSAGES_COLLECTOR_STATIC_LIBRARY
    NAMES libcbtf-messages-collector.a
    HINTS $ENV{CBTF_ROOT} $ENV{CBTF_PREFIX}
    PATH_SUFFIXES lib lib64
    )
  
find_library(CBTF_MESSAGES_EVENTS_SHARED_LIBRARY
    NAMES libcbtf-messages-events.so
    HINTS $ENV{CBTF_ROOT} $ENV{CBTF_PREFIX}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_MESSAGES_EVENTS_STATIC_LIBRARY
    NAMES libcbtf-messages-events.a
    HINTS $ENV{CBTF_ROOT} $ENV{CBTF_PREFIX}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_MESSAGES_INSTRUMENTATION_SHARED_LIBRARY
    NAMES libcbtf-messages-instrumentation.so
    HINTS $ENV{CBTF_ROOT} $ENV{CBTF_PREFIX}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_MESSAGES_INSTRUMENTATION_STATIC_LIBRARY
    NAMES libcbtf-messages-instrumentation.a
    HINTS $ENV{CBTF_ROOT} $ENV{CBTF_PREFIX}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_MESSAGES_PERFDATA_SHARED_LIBRARY
    NAMES libcbtf-messages-perfdata.so
    HINTS $ENV{CBTF_ROOT} $ENV{CBTF_PREFIX}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_MESSAGES_PERFDATA_STATIC_LIBRARY
    NAMES libcbtf-messages-perfdata.a
    HINTS $ENV{CBTF_ROOT} $ENV{CBTF_PREFIX}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_MESSAGES_SYMTAB_SHARED_LIBRARY
    NAMES libcbtf-messages-symtab.so
    HINTS $ENV{CBTF_ROOT} $ENV{CBTF_PREFIX}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_MESSAGES_SYMTAB_STATIC_LIBRARY
    NAMES libcbtf-messages-symtab.a
    HINTS $ENV{CBTF_ROOT} $ENV{CBTF_PREFIX}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_MESSAGES_THREAD_SHARED_LIBRARY
    NAMES libcbtf-messages-thread.so
    HINTS $ENV{CBTF_ROOT} $ENV{CBTF_PREFIX}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_MESSAGES_THREAD_STATIC_LIBRARY
    NAMES libcbtf-messages-thread.a
    HINTS $ENV{CBTF_ROOT} $ENV{CBTF_PREFIX}
    PATH_SUFFIXES lib lib64
    )

find_path(CBTF_MESSAGES_INCLUDE_DIR
    KrellInstitute/Messages/Address.h
    HINTS $ENV{CBTF_ROOT} $ENV{CBTF_PREFIX}
    PATH_SUFFIXES include
    )

find_package_handle_standard_args(
    CBTF-Messages DEFAULT_MSG
    CBTF_MESSAGES_BASE_SHARED_LIBRARY
    CBTF_MESSAGES_BASE_STATIC_LIBRARY
    CBTF_MESSAGES_COLLECTOR_SHARED_LIBRARY
    CBTF_MESSAGES_COLLECTOR_STATIC_LIBRARY
    CBTF_MESSAGES_EVENTS_SHARED_LIBRARY
    CBTF_MESSAGES_EVENTS_STATIC_LIBRARY
    CBTF_MESSAGES_INSTRUMENTATION_SHARED_LIBRARY
    CBTF_MESSAGES_INSTRUMENTATION_STATIC_LIBRARY
    CBTF_MESSAGES_PERFDATA_SHARED_LIBRARY
    CBTF_MESSAGES_PERFDATA_STATIC_LIBRARY
    CBTF_MESSAGES_SYMTAB_SHARED_LIBRARY
    CBTF_MESSAGES_SYMTAB_STATIC_LIBRARY
    CBTF_MESSAGES_THREAD_SHARED_LIBRARY
    CBTF_MESSAGES_THREAD_STATIC_LIBRARY
    CBTF_MESSAGES_INCLUDE_DIR
    )

set(CBTF_MESSAGES_LIBRARIES
    ${CBTF_MESSAGES_BASE_SHARED_LIBRARY}
    ${CBTF_MESSAGES_COLLECTOR_SHARED_LIBRARY}
    ${CBTF_MESSAGES_EVENTS_SHARED_LIBRARY}
    ${CBTF_MESSAGES_INSTRUMENTATION_SHARED_LIBRARY}
    ${CBTF_MESSAGES_PERFDATA_SHARED_LIBRARY}
    ${CBTF_MESSAGES_SYMTAB_SHARED_LIBRARY}
    ${CBTF_MESSAGES_THREAD_SHARED_LIBRARY}
    )

set(CBTF_MESSAGES_INCLUDE_DIRS ${CBTF_MESSAGES_INCLUDE_DIR})

mark_as_advanced(
    CBTF_MESSAGES_BASE_SHARED_LIBRARY
    CBTF_MESSAGES_BASE_STATIC_LIBRARY
    CBTF_MESSAGES_COLLECTOR_SHARED_LIBRARY
    CBTF_MESSAGES_COLLECTOR_STATIC_LIBRARY
    CBTF_MESSAGES_EVENTS_SHARED_LIBRARY
    CBTF_MESSAGES_EVENTS_STATIC_LIBRARY
    CBTF_MESSAGES_INSTRUMENTATION_SHARED_LIBRARY
    CBTF_MESSAGES_INSTRUMENTATION_STATIC_LIBRARY
    CBTF_MESSAGES_PERFDATA_SHARED_LIBRARY
    CBTF_MESSAGES_PERFDATA_STATIC_LIBRARY
    CBTF_MESSAGES_SYMTAB_SHARED_LIBRARY
    CBTF_MESSAGES_SYMTAB_STATIC_LIBRARY
    CBTF_MESSAGES_THREAD_SHARED_LIBRARY
    CBTF_MESSAGES_THREAD_STATIC_LIBRARY
    CBTF_MESSAGES_INCLUDE_DIR
    )
