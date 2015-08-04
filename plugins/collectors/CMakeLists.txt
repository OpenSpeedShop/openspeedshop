################################################################################
# Copyright (c) 2014-2015 Krell Institute. All Rights Reserved.
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

add_subdirectory(pcsamp)
add_subdirectory(usertime)
add_subdirectory(io)
add_subdirectory(iot)
add_subdirectory(hwc)
add_subdirectory(hwcsamp)
add_subdirectory(hwctime)
add_subdirectory(mpi)
add_subdirectory(mpit)
add_subdirectory(fpe)

if (INSTRUMENTOR MATCHES "cbtf")
    add_subdirectory(mem)
    add_subdirectory(pthreads)
    add_subdirectory(iop)
    add_subdirectory(mpip)

    # If cuda related plugins and messages were found
    # then build the client portion of the cuda collector.
    if (CBTF-ARGO-MESSAGESCUDA_FOUND)
        add_subdirectory(cuda)
    endif()
endif()



