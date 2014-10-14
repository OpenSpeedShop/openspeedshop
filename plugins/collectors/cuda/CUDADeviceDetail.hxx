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

        /** Constructor from a raw CUDA message. */
        CUDADeviceDetail(const CUDA_DeviceInfo& device_info):
            dm_device_info(device_info),
            dm_name(device_info.name)
        {
        }

        /** Read-only data member accessor function. */
        // TODO: Add read-only data member accessor functions
        
    private:

        /** Raw CUDA message containing the device details. */
        CUDA_DeviceInfo dm_device_info;
        
        /** Device name stored in a copy-safe C++ string. */
        std::string dm_name;
        
    }; // class CUDADeviceDetail
    
} } // namespace OpenSpeedShop::Framework
