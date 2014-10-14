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

#include <string>

#include "KrellInstitute/Messages/CUDA_data.h"

#include "SmartPtr.hxx"

#include "CUDADeviceDetail.hxx"

namespace OpenSpeedShop { namespace Framework {

    /**
     * CUDA kernel execution details.
     *
     * Encapsulate the details metric (inclusive or exclusive) for CUDA kernel
     * executions recorded by the CUDA collector.
     */
    class CUDAExecDetail
    {

    public:

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
        
        /** Read-only data member accessor function. */
        // TODO: Add read-only data member accessor functions
        
    private:

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
