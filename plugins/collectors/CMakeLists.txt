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

if (LIBUNWIND_FOUND)
    add_subdirectory(usertime)
    add_subdirectory(io)
    add_subdirectory(iot)
endif()

if (PAPI_FOUND)
    add_subdirectory(hwc)
    add_subdirectory(hwcsamp)
if (LIBUNWIND_FOUND)
    add_subdirectory(hwctime)
endif()
endif()

if(HAVE_MPI)
if(LIBUNWIND_FOUND)
    add_subdirectory(mpi)
    add_subdirectory(mpit)
endif()
endif()

#if(MPI_FOUND)
#if(LIBUNWIND_FOUND)
#if(OTF_FOUND)
#if(VT_FOUND)
#add_subdirectory(mpiotf)
#endif()
#endif()
#endif()
#endif()

if(BINUTILS_FOUND)
if(LIBUNWIND_FOUND)
    add_subdirectory(fpe)
endif()
endif()

if(INSTRUMENTOR MATCHES "cbtf")
    add_subdirectory(mem)
    add_subdirectory(pthreads)
    add_subdirectory(iop)
    add_subdirectory(mpip)
    if (CBTF_MESSAGES_CUDA_FOUND)
        add_subdirectory(cuda)
    endif()
endif()


