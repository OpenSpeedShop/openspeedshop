################################################################################
# Copyright (c) 2014-2016 Krell Institute. All Rights Reserved.
# Copyright (c) 2015,2016 Argo Navis Technologies. All Rights Reserved.
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

# FIXME iot has problems with syscall types on ARM
if (NOT RUNTIME_PLATFORM MATCHES "arm")
   add_subdirectory(iot)
endif()

add_subdirectory(hwc)
add_subdirectory(hwcsamp)
add_subdirectory(hwctime)
add_subdirectory(mpi)
add_subdirectory(mpit)
add_subdirectory(fpe)
if (VT_FOUND)
    add_subdirectory(mpiotf)
endif()

if (INSTRUMENTOR MATCHES "cbtf")
    add_subdirectory(mem)
    add_subdirectory(pthreads)
    add_subdirectory(iop)
    add_subdirectory(mpip)
    add_subdirectory(omptp)
endif()

if(BUILD_CUDA_ANALYSIS)
    add_subdirectory(cuda)
endif()
