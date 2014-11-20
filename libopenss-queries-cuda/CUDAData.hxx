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

/** @file Declaration of the CUDAData class. */

#pragma once

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <list>
#include <map>
#include <stdint.h>
#include <string>
#include <vector>

#include "Address.hxx"
#include "Collector.hxx"
#include "StackTrace.hxx"
#include "Thread.hxx"
#include "Time.hxx"
#include "TimeInterval.hxx"

namespace OpenSpeedShop { namespace Queries {
        
    /**
     * High-level interface for accessing one thread's CUDA performance data.
     */
    class CUDAData :
        boost::noncopyable
    {

    public:

        /** Vector of two unsigned integers. */
        typedef boost::tuple<uint32_t, uint32_t> Vector2u;
        
        /** Vector of three unsigned integers. */
        typedef boost::tuple<uint32_t, uint32_t, uint32_t> Vector3u;

        /** Enumeration of the different cache preferences. */
        enum CachePreferences
        {
            kInvalidCachePreference, kNoPreference,
            kPreferShared, kPreferCache, kPreferEqual
        };
            
        /** Enumeration of the different memory copy kinds. */
        enum CopyKinds
        {
            kInvalidCopyKind, kUnknownCopyKind,
            kHostToDevice, kDeviceToHost, kHostToArray,
            kArrayToHost, kArrayToArray, kArrayToDevice,
            kDeviceToArray, kDeviceToDevice, kHostToHost
        };
     
        /** Enumeration of the different memory kinds. */
        enum MemoryKinds
        {
            kInvalidMemoryKind, kUnknownMemoryKind,
            kPageable, kPinned, kDevice, kArray
        };
        
        /** Detailed information for a CUDA device. */
        struct DeviceDetails
        {
            /** Name of this device. */
            std::string name;
            
            /** Compute capability (major/minor) of this device. */
            Vector2u compute_capability;
            
            /** Maximum allowed dimensions of grids with this device. */
            Vector3u max_grid;
            
            /** Maximum allowed dimensions of blocks with this device. */
            Vector3u max_block;
            
            /** Global memory bandwidth of this device (in KBytes/sec). */
            uint64_t global_memory_bandwidth;
            
            /** Global memory size of this device (in bytes). */
            uint64_t global_memory_size;
            
            /** Constant memory size of this device (in bytes). */
            uint32_t constant_memory_size;
            
            /** L2 cache size of this device (in bytes). */
            uint32_t l2_cache_size;

            /** Number of threads per warp for this device. */
            uint32_t threads_per_warp;
            
            /** Core clock rate of this device (in KHz). */
            uint32_t core_clock_rate;
            
            /** Number of memory copy engines on this device. */
            uint32_t memcpy_engines;
            
            /** Number of multiprocessors on this device. */
            uint32_t multiprocessors;
            
            /** 
             * Maximum instructions/cycle possible on this device's
             * multiprocessors.
             */
            uint32_t max_ipc;
            
            /** Maximum warps/multiprocessor for this device. */
            uint32_t max_warps_per_multiprocessor;
            
            /** Maximum blocks/multiprocessor for this device. */
            uint32_t max_blocks_per_multiprocessor;
            
            /** Maximum registers/block for this device. */
            uint32_t max_registers_per_block;
            
            /** Maximium shared memory / block for this device. */
            uint32_t max_shared_memory_per_block;
            
            /** Maximum threads/block for this device. */
            uint32_t max_threads_per_block;
        };

        /** Detailed information for a CUDA kernel execution. */
        struct KernelExecutionDetails
        {
            /** Device on which the kernel execution occurred. */
            size_t device;
            
            /** Call site of the kernel execution. */
            size_t call_site;
            
            /** Time at which the kernel execution was requested. */
            Framework::Time time;

            /** Time at which the kernel execution began. */
            Framework::Time time_begin;
            
            /** Time at which the kernel execution ended. */
            Framework::Time time_end;

            /** Name of the kernel function being executed. */
            std::string function;

            /** Dimensions of the grid. */
            Vector3u grid;
    
            /** Dimensions of each block. */
            Vector3u block;

            /** Cache preference used. */
            CachePreferences cache_preference;

            /** Registers required for each thread. */
            uint32_t registers_per_thread;

            /** Total amount (in bytes) of static shared memory reserved. */
            uint32_t static_shared_memory;

            /** Total amount (in bytes) of dynamic shared memory reserved. */
            uint32_t dynamic_shared_memory;

            /** Total amount (in bytes) of local memory reserved. */
            uint32_t local_memory;
        };

        /** Detailed information for a CUDA memory copy. */
        struct MemoryCopyDetails
        {
            /** Device on which the memory copy occurred. */
            size_t device;
            
            /** Call site of the memory copy. */
            size_t call_site;
            
            /** Time at which the memory copy was requested. */
            Framework::Time time;
            
            /** Time at which the memory copy began. */
            Framework::Time time_begin;
            
            /** Time at which the memory copy ended. */
            Framework::Time time_end;
            
            /** Number of bytes being copied. */
            uint64_t size;

            /** Kind of copy performed. */
            CopyKinds kind;
            
            /** Kind of memory from which the copy was performed. */
            MemoryKinds source_kind;
            
            /** Kind of memory to which the copy was performed. */
            MemoryKinds destination_kind;
            
            /** Was the copy asynchronous? */
            bool asynchronous;  
        };

        /** Detailed information for a CUDA memory set. */
        struct MemorySetDetails
        {
            /** Device on which the memory set occurred. */
            size_t device;
            
            /** Call site of the memory set. */
            size_t call_site;
            
            /** Time at which the memory set was requested. */
            Framework::Time time;
            
            /** Time at which the memory set began. */
            Framework::Time time_begin;
            
            /** Time at which the memory set ended. */
            Framework::Time time_end;
            
            /** Number of bytes being set. */
            uint64_t size;            
        };

        /**
         * Preprocess the data for the given collector and thread.
         *
         * @pre    Can only be performed for a CUDA collector. An assertion
         *         failure occurs if a different collector is used.
         *
         * @pre    The thread must be in the same experiment as the collector.
         *         An assertion failure occurs if the thread is in a different
         *         experiment than the collector.
         *
         * @param collector    Collector for which to access performance data.
         * @parma thread       Thread for which to access performance data.
         */
        CUDAData(const Framework::Collector& collector,
                 const Framework::Thread& thread);
        
        /** Destructor. */
        virtual ~CUDAData();
        
        /** Call sites for all known CUDA requests. */       
        const std::vector<Framework::StackTrace> call_sites() const
        {
            return dm_call_sites;
        }
        
        /** Names of all sampled hardware performance counters. */
        const std::vector<std::string>& counters() const
        {
            return dm_counters;
        }
        
        /** Detailed information for all known CUDA devices. */
        const std::vector<DeviceDetails>& devices() const
        {
            return dm_devices;
        }

        /**
         * Get the counts for all sampled hardware peformance counters between
         * the specified time interval (if any).
         *
         * @note    The name of the counter corresponding to any particular
         *          count in the returned vector can be found by using that
         *          count's index within the vector returned by counters().
         *
         * @note    When the specified time interval does not lie exactly on
         *          the hardware performance counter sample times, the counts
         *          provided are estimates based on the nearest samples.
         *
         * @param interval    Time interval over which to get counts. Has
         *                    a default value meaning "all possible time".
         * @return            Counts over the specified time interval.
         */
        std::vector<uint64_t> get_counts(
            const Framework::TimeInterval& interval = Framework::TimeInterval(
                Framework::Time::TheBeginning(),
                Framework::Time::TheEnd()
                )
            ) const;
            
        /**
         * Visit those CUDA kernel executions whose request-to-completion time
         * interval intersects the specified time interval (if any).
         *
         * @param visitor     Visitor invoked for each kernel execution.
         * @param interval    Time interval for the visitation. Has a
         *                    default value meaning "all possible time".
         */
        void visit_kernel_executions(
            boost::function<void (const KernelExecutionDetails&)>& visitor,
            const Framework::TimeInterval& interval = Framework::TimeInterval(
                Framework::Time::TheBeginning(),
                Framework::Time::TheEnd()
                )
            ) const;
        
        /**
         * Visit those CUDA memory copies whose request-to-completion time
         * interval intersects the specified time interval (if any).
         *
         * @param visitor     Visitor invoked for each memory copy.
         * @param interval    Time interval for the visitation. Has a
         *                    default value meaning "all possible time".
         */
        void visit_memory_copies(
            boost::function<void (const MemoryCopyDetails&)>& visitor,
            const Framework::TimeInterval& interval = Framework::TimeInterval(
                Framework::Time::TheBeginning(),
                Framework::Time::TheEnd()
                )
            ) const;

        /**
         * Visit those CUDA memory sets whose request-to-completion time
         * interval intersects the specified time interval (if any).
         *
         * @param visitor     Visitor invoked for each memory set.
         * @param interval    Time interval for the visitation. Has a
         *                    default value meaning "all possible time".
         */
        void visit_memory_sets(
            boost::function<void (const MemorySetDetails&)>& visitor,
            const Framework::TimeInterval& interval = Framework::TimeInterval(
                Framework::Time::TheBeginning(),
                Framework::Time::TheEnd()
                )
            ) const;
        
    private:

        /** Description of a single pending request. */
        struct Request
        {
            /** Original CUDA message describing the enqueued request. */
            boost::shared_ptr<struct CUDA_EnqueueRequest> message;
            
            /** Call site of the enqueued request. */
            std::vector<Framework::StackTrace>::size_type call_site;
        };

        /** Call sites for all known CUDA requests. */
        std::vector<Framework::StackTrace> dm_call_sites;
        
        /** Names of all sampled hardware performance counters. */
        std::vector<std::string> dm_counters;

        /** Detailed information for all known CUDA devices. */
        std::vector<DeviceDetails> dm_devices;

        /** Ordered list of CUDA kernel executions. */
        std::vector<KernelExecutionDetails> dm_kernel_executions;

        /** Index in dm_devices for each known device ID. */
        std::map<
            uint32_t, std::vector<DeviceDetails>::size_type
            > dm_known_devices;
        
        /** Device ID for each known context address. */
        std::map<Framework::Address, uint32_t> dm_known_contexts;

        /** Ordered list of CUDA memory copies. */
        std::vector<MemoryCopyDetails> dm_memory_copies;

        /** Ordered list of CUDA memory sets. */
        std::vector<MemorySetDetails> dm_memory_sets;

        /** Ordered list of periodic samples. */
        std::vector<uint64_t> dm_periodic_samples;
        
        /** Pending requests. */
        std::list<Request> dm_requests;

        /** Get the index in dm_devices for the specified context address. */
        std::vector<DeviceDetails>::size_type device_from_context(
            const Framework::Address& context
            ) const;
        
        /** Process a CUDA_ContextInfo message. */
        void process(const struct CUDA_ContextInfo& message);

        /** Process a CUDA_CopiedMemory message. */
        void process(const struct CUDA_CopiedMemory& message);

        /** Process a CUDA_DeviceInfo message. */
        void process(const struct CUDA_DeviceInfo& message);
        
        /** Process a CUDA_EnqueueRequest message. */
        void process(const struct CUDA_EnqueueRequest& message,
                     const struct CBTF_cuda_data& data,
                     const Framework::Thread& thread);
        
        /** Process a CUDA_ExecutedKernel message. */
        void process(const struct CUDA_ExecutedKernel& message);

        /** Process a CUDA_OverflowSamples message. */
        void process(const struct CUDA_OverflowSamples& message);
        
        /** Process a CUDA_PeriodicSamples message. */
        void process(const struct CUDA_PeriodicSamples& message);
        
        /** Process a CUDA_SamplingConfig message. */
        void process(const struct CUDA_SamplingConfig& message);

        /** Process a CUDA_SetMemory message. */
        void process(const struct CUDA_SetMemory& message);

    }; // class CUDAData

} } // namespace OpenSpeedShop::Queries
