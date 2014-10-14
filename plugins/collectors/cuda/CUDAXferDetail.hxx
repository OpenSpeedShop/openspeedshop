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

/** @file Declaration and definition of the CUDAXferDetail class. */

#pragma once

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "KrellInstitute/Messages/CUDA_data.h"

#include "SmartPtr.hxx"

#include "CUDADeviceDetail.hxx"

namespace OpenSpeedShop { namespace Framework {

    /**
     * CUDA data transfer details.
     *
     * Encapsulate the details metric (inclusive or exclusive) for CUDA data
     * transfers recorded by the CUDA collector.
     */
    class CUDAXferDetail
    {

    public:

        /** Enumeration of the different memory kinds. */
        enum MemoryKind {     
            InvalidMemoryKind = 0,
            UnknownMemoryKind = 1,
            Pageable = 2,
            Pinned = 3,
            Device = 4,
            Array = 5
        };
        
        /** Enumeration of the different data transfer kinds. */
        enum TransferKind {
            InvalidTransferKind = 0,
            UnknownTransferKind = 1,
            HostToDevice = 2,
            DeviceToHost = 3,
            HostToArray = 4,
            ArrayToHost = 5,
            ArrayToArray = 6,
            ArrayToDevice = 7,
            DeviceToArray = 8,
            DeviceToDevice = 9,
            HostToHost = 10
        };

        /** Constructor from raw CUDA messages. */
        CUDAXferDetail(const double& time,
                       const SmartPtr<CUDADeviceDetail>& device_detail,
                       const CUDA_EnqueueRequest& enqueue_request,
                       const CUDA_CopiedMemory& copied_memory) :
            dm_time(time),
            dm_device_detail(device_detail),
            dm_enqueue_request(enqueue_request),
            dm_copied_memory(copied_memory)
        {
        }

        /** Time spent in the kernel execution (in seconds). */
        double getTime() const
        {
            return dm_time;
        }

        /** Device performing the data transfer. */
        const SmartPtr<CUDADeviceDetail>& getDevice() const
        {
            return dm_device_detail;
        }

        /** Time at which the data transfer was enqueued. */
        Time getTimeEnqueue() const
        {
            return dm_enqueue_request.time;
        }

        /** Time at which the data transfer began. */
        Time getTimeBegin() const
        {
            return dm_copied_memory.time_begin;
        }

        /** Time at which the data transfer ended. */
        Time getTimeEnd() const
        {
            return dm_copied_memory.time_end;
        }

        /** Number of bytes being transferred. */
        unsigned long long getSize() const
        {
            return dm_copied_memory.size;
        }
        
        /** Kind of data transfer performed. */
        TransferKind getKind() const
        {
            return static_cast<TransferKind>(dm_copied_memory.kind);
        }
        
        /** Kind of memory from which the data transfer was performed. */
        MemoryKind getSourceKind() const
        {
            return static_cast<MemoryKind>(dm_copied_memory.source_kind);
        }
        
        /** Kind of memory to which the data transfer was performed. */
        MemoryKind getDestinationKind() const
        {
            return static_cast<MemoryKind>(dm_copied_memory.destination_kind);
        }
        
        /** Was the data transfer asynchronous? */
        bool getAsynchronous() const
        {
            return dm_copied_memory.asynchronous;
        }
        
    private:

        /** Time spent in the data transfer. */
        double dm_time;
        
        /** Details for the device performing the data transfer. */
        SmartPtr<CUDADeviceDetail> dm_device_detail;
        
        /** Raw CUDA message describing the enqueued request. */
        CUDA_EnqueueRequest dm_enqueue_request;
        
        /** Raw CUDA message describing the data transfer. */
        CUDA_CopiedMemory dm_copied_memory;

    }; // class CUDAXferDetail
    
} } // namespace OpenSpeedShop::Framework
