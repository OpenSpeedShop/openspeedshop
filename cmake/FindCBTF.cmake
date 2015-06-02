################################################################################
# Copyright (c) 2012 Argo Navis Technologies. All Rights Reserved.
# Copyright (c) 2012-2015 Krell Institute. All Rights Reserved.
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

find_library(CBTF_LIBRARY NAMES libcbtf.so
    HINTS ${CBTF_DIR} $ENV{CBTF_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_XML_LIBRARY NAMES libcbtf-xml.so
    HINTS ${CBTF_DIR} $ENV{CBTF_DIR}
    PATH_SUFFIXES lib lib64
    )

find_library(CBTF_MRNET_LIBRARY NAMES libcbtf-mrnet.so
    HINTS ${CBTF_DIR} $ENV{CBTF_DIR}
    PATH_SUFFIXES lib lib64
    )

find_path(CBTF_INCLUDE_DIR KrellInstitute/CBTF/Component.hpp
    HINTS ${CBTF_DIR} $ENV{CBTF_DIR}
    PATH_SUFFIXES include
    )

find_package_handle_standard_args(
    CBTF DEFAULT_MSG
    CBTF_LIBRARY CBTF_XML_LIBRARY CBTF_MRNET_LIBRARY CBTF_INCLUDE_DIR
    )

GET_FILENAME_COMPONENT(CBTF_LIB_DIR ${CBTF_LIBRARY} PATH )
set(CBTF_LIBRARIES ${CBTF_LIBRARY} ${CBTF_XML_LIBRARY} ${CBTF_MRNET_LIBRARY})
set(CBTF_INCLUDE_DIRS ${CBTF_INCLUDE_DIR})

mark_as_advanced(
    CBTF_LIBRARY CBTF_XML_LIBRARY CBTF_MRNET_LIBRARY CBTF_INCLUDE_DIR CBTF_LIB_DIR
)
