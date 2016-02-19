################################################################################
# Copyright (c) 2006-2015 Krell Institute. All Rights Reserved.
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


configure_file(openss.py.in  openss.py)
configure_file(openss_classes.py.in  openss_classes.py)
configure_file(openss_commands.py.in  openss_commands.py)
configure_file(openss_parse.py.in  openss_parse.py)
configure_file(__init__.py.in  __init__.py)

install(PROGRAMS
        ${CMAKE_CURRENT_BINARY_DIR}/openss.py
        ${CMAKE_CURRENT_BINARY_DIR}/openss_classes.py
        ${CMAKE_CURRENT_BINARY_DIR}/openss_commands.py
        ${CMAKE_CURRENT_BINARY_DIR}/openss_parse.py
        ${CMAKE_CURRENT_BINARY_DIR}/__init__.py
        DESTINATION
        ${CMAKE_INSTALL_PREFIX}/lib${SUFFIX}/openspeedshop
)

