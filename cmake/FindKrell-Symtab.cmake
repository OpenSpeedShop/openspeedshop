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

find_library(KRELL_SYMTAB_LIBRARY NAMES libkrell-symtab.so
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES lib lib64
    )

find_path(KRELL_SYMTAB_INCLUDE_DIR KrellInstitute/SymbolTable/LinkedObject.hpp
    HINTS $ENV{CBTF_KRELL_DIR}
    PATH_SUFFIXES include
    )

find_package_handle_standard_args(
    Krell-Symtab DEFAULT_MSG
    KRELL_SYMTAB_LIBRARY KRELL_SYMTAB_INCLUDE_DIR
    )

set(KRELL_SYMTAB_LIBRARIES ${KRELL_SYMTAB_LIBRARY})
set(KRELL_SYMTAB_INCLUDE_DIRS ${KRELL_SYMTAB_INCLUDE_DIR})

mark_as_advanced(KRELL_SYMTAB_LIBRARY KRELL_SYMTAB_INCLUDE_DIR)
