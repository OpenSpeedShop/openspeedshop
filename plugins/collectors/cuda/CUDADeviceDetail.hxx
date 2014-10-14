////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Argo Navis Technologies. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file Declaration and definition of the CUDADeviceDetail class. */

#pragma once

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/tuple/tuple.hpp>
#include <string>

#include "KrellInstitute/Messages/CUDA_data.h"

namespace OpenSpeedShop { namespace Framework {

    /**
     * CUDA device details.
     *
     * Encapsulate the device details associated with CUDA kernel executions
     * and data transfers recorded by the CUDA collector.
     */
    class CUDADeviceDetail
    {

    public:

        /** Vector of two unsigned integers. */
        typedef boost::tuple<unsigned int, unsigned int> Vector2u;

        /** Vector of three unsigned integers. */
        typedef boost::tuple<unsigned int, unsigned int, unsigned int> Vector3u;

        /** Constructor from a raw CUDA message. */
        CUDADeviceDetail(const CUDA_DeviceInfo& device_info):
            dm_device_info(device_info),
            dm_name(device_info.name)
        {
        }
        
        /* Name of this device. */
        std::string getName() const
        {
            return dm_name;
        }

        /** Compute capability (major/minor) of this device. */
        Vector2u getComputeCapability() const
        {
            return Vector2u(dm_device_info.compute_capability[0],
                            dm_device_info.compute_capability[1]);
        }
        
        /** Maximum allowed dimensions of grids with this device. */
        Vector3u getMaxGrid() const
        {
            return Vector3u(dm_device_info.max_grid[0],
                            dm_device_info.max_grid[1],
                            dm_device_info.max_grid[2]);
        }

        /** Maximum allowed dimensions of grids blocks this device. */
        Vector3u getMaxBlock() const
        {
            return Vector3u(dm_device_info.max_block[0],
                            dm_device_info.max_block[1],
                            dm_device_info.max_block[2]);
        }

        /** Global memory bandwidth of this device (in KBytes/sec). */
        unsigned long long getGlobalMemoryBandwidth() const
        {
            return dm_device_info.global_memory_bandwidth;
        }
        
        /** Global memory size of this device (in bytes). */
        unsigned long long getGlobalMemorySize() const
        {
            return dm_device_info.global_memory_size;
        }

        /** Constant memory size of this device (in bytes). */
        unsigned long long getConstantMemorySize() const
        {
            return dm_device_info.constant_memory_size;
        }
        
        /** L2 cache size of this device (in bytes). */
        unsigned int getL2CacheSize() const
        {
            return dm_device_info.l2_cache_size;
        }
        
        /** Number of threads per warp for this device. */
        unsigned int getThreadsPerWarp() const
        {
            return dm_device_info.threads_per_warp;
        }
        
        /** Core clock rate of this device (in KHz). */
        unsigned int getCoreClockRate() const
        {
            return dm_device_info.core_clock_rate;
        }
        
        /** Number of memory copy engines on this device. */
        unsigned int getMemcpyEngines() const
        {
            return dm_device_info.memcpy_engines;
        }
        
        /** Number of multiprocessors on this device. */
        unsigned int getMultiprocessors() const
        {
            return dm_device_info.multiprocessors;
        }
        
        /**
         * Maximum instructions/cycle possible on this device's multiprocessors.
         */
        unsigned int getMaxIPC() const
        {
            return dm_device_info.max_ipc;
        }
        
        /** Maximum warps/multiprocessor for this device. */
        unsigned int getMaxWarpsPerMultiprocessor() const
        {
            return dm_device_info.max_warps_per_multiprocessor;
        }
        
        /** Maximum blocks/multiprocessor for this device. */
        unsigned int getMaxBlocksPerMultiprocessor() const
        {
            return dm_device_info.max_blocks_per_multiprocessor;
        }
        
        /** Maximum registers/block for this device. */
        unsigned int getMaxRegistersPerBlock() const
        {
            return dm_device_info.max_registers_per_block;
        }
        
        /** Maximium shared memory / block for this device. */
        unsigned int getMaxSharedMemoryPerBlock() const
        {
            return dm_device_info.max_shared_memory_per_block;
        }
        
        /** Maximum threads/block for this device. */
        unsigned int getMaxThreadsPerBlock() const
        {
            return dm_device_info.max_threads_per_block;
        }
                
    private:

        /** Raw CUDA message containing the device details. */
        CUDA_DeviceInfo dm_device_info;
        
        /** Device name stored in a copy-safe C++ string. */
        std::string dm_name;
        
    }; // class CUDADeviceDetail
    
} } // namespace OpenSpeedShop::Framework
