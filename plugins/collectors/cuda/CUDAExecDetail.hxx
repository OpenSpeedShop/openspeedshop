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

/** @file Declaration and definition of the CUDAExecDetail class. */

#pragma once

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/tuple/tuple.hpp>
#include <string>

#include "KrellInstitute/Messages/CUDA_data.h"

#include "SmartPtr.hxx"
#include "Time.hxx"
#include "TotallyOrdered.hxx"

#include "CUDADeviceDetail.hxx"

namespace OpenSpeedShop { namespace Framework {

    /**
     * CUDA kernel execution details.
     *
     * Encapsulate the details metric (inclusive or exclusive) for CUDA kernel
     * executions recorded by the CUDA collector.
     */
    class CUDAExecDetail :
        public TotallyOrdered<CUDAExecDetail>
    {

    public:

        /** Vector of three unsigned integers. */
        typedef boost::tuple<unsigned int, unsigned int, unsigned int> Vector3u;
        
        /** Constructor from raw CUDA messages. */
        CUDAExecDetail(const double& time,
                       const SmartPtr<CUDADeviceDetail>& device_detail,
                       const CUDA_EnqueueRequest& enqueue_request,
                       const CUDA_ExecutedKernel& executed_kernel) :
            dm_time(time),
            dm_device_detail(device_detail),
            dm_enqueue_request(enqueue_request),
            dm_executed_kernel(executed_kernel),
            dm_function(executed_kernel.function)
        {
        }

        /** Operator "<" defined for two CUDAExecDetail objects. */
        bool operator<(const CUDAExecDetail& other) const
        {
            return TimeInterval(getTimeBegin(), getTimeEnd()) <
                TimeInterval(other.getTimeBegin(), other.getTimeEnd());
        }
        
        /** Time spent in the kernel execution (in seconds). */
        double getTime() const
        {
            return dm_time;
        }

        /** Device performing the kernel execution. */
        const SmartPtr<CUDADeviceDetail>& getDevice() const
        {
            return dm_device_detail;
        }

        /** Time at which the kernel execution was enqueued. */
        Time getTimeEnqueue() const
        {
            return dm_enqueue_request.time;
        }

        /** Time at which the kernel execution began. */
        Time getTimeBegin() const
        {
            return dm_executed_kernel.time_begin;
        }

        /** Time at which the kernel execution ended. */
        Time getTimeEnd() const
        {
            return dm_executed_kernel.time_end;
        }

        /** Name of the kernel function being executed. */
        std::string getName() const
        {
            return dm_function;
        }
        
        /** Dimensions of the grid. */
        Vector3u getGrid() const
        {
            return Vector3u(dm_executed_kernel.grid[0],
                            dm_executed_kernel.grid[1],
                            dm_executed_kernel.grid[2]);
        }
        
        /** Dimensions of each block. */
        Vector3u getBlock() const
        {
            return Vector3u(dm_executed_kernel.block[0],
                            dm_executed_kernel.block[1],
                            dm_executed_kernel.block[2]);
        }
        
        /** Cache preference used. */
        std::string getCachePreference() const
        {
            return stringify(dm_executed_kernel.cache_preference);
        }
        
        /** Registers required for each thread. */
        unsigned int getRegistersPerThread() const
        {
            return dm_executed_kernel.registers_per_thread;
        }
        
        /** Total amount (in bytes) of static shared memory reserved. */
        unsigned long long getStaticSharedMemory() const
        {
            return dm_executed_kernel.static_shared_memory;
        }
        
        /** Total amount (in bytes) of dynamic shared memory reserved. */
        unsigned long long getDynamicSharedMemory() const
        {
            return dm_executed_kernel.dynamic_shared_memory;
        }
        
        /** Total amount (in bytes) of local memory reserved. */
        unsigned long long getLocalMemory() const
        {
            return dm_executed_kernel.local_memory;
        }
        
    private:

        /** Stringify a CUDA_CachePreference. */
        std::string stringify(const CUDA_CachePreference& value) const
        {
            switch (value)
            {
            case InvalidCachePreference: return "Invalid";
            case NoPreference: return "None";
            case PreferShared: return "Shared";
            case PreferCache: return "Cache";
            case PreferEqual: return "Equal";
            }
            return "?";
        }

        /** Time spent in the kernel execution. */
        double dm_time;
        
        /** Details for the device performing the kernel execution. */
        SmartPtr<CUDADeviceDetail> dm_device_detail;
        
        /** Raw CUDA message describing the enqueued request. */
        CUDA_EnqueueRequest dm_enqueue_request;

        /** Raw CUDA message describing the kernel execution. */
        CUDA_ExecutedKernel dm_executed_kernel;

        /** Kernel name stored in a copy-safe C++ string. */
        std::string dm_function;
        
    }; // class CUDAExecDetail
    
} } // namespace OpenSpeedShop::Framework
