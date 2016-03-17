////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014-2016 Argo Navis Technologies. All Rights Reserved.
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

#include <ArgoNavis/Base/TimeInterval.hpp>
#include <ArgoNavis/CUDA/DataTransfer.hpp>
#include <ArgoNavis/CUDA/Device.hpp>

#include "TotallyOrdered.hxx"

namespace OpenSpeedShop { namespace Framework {

    /**
     * CUDA data transfer details.
     *
     * Encapsulate the details metric (inclusive or exclusive) for CUDA data
     * transfers recorded by the CUDA collector.
     */
    class CUDAXferDetail :
        public ArgoNavis::CUDA::DataTransfer,
        public TotallyOrdered<CUDAXferDetail>
    {

    public:

        /** Constructor. */
        CUDAXferDetail(const ArgoNavis::CUDA::DataTransfer& event,
                       const ArgoNavis::CUDA::Device& device,
                       const double& time):
            ArgoNavis::CUDA::DataTransfer(event),
            dm_device(device),
            dm_time(time)
        {
        }

        /** Operator "<" defined for two CUDAXferDetail objects. */
        bool operator<(const CUDAXferDetail& other) const
        {
            return ArgoNavis::Base::TimeInterval(time_begin, time_end) <
                ArgoNavis::Base::TimeInterval(other.time_begin, other.time_end);
        }

        /** Device performing the data transfer. */
        const ArgoNavis::CUDA::Device& getDevice() const
        {
            return dm_device;
        }
         
        /** Time spent in the kernel execution (in seconds). */
        double getTime() const
        {
            return dm_time;
        }

    private:
 
        /** Device performing the data transfer. */
        ArgoNavis::CUDA::Device dm_device;

        /** Time spent in the data transfer. */
        double dm_time;
        
    }; // class CUDAXferDetail
    
} } // namespace OpenSpeedShop::Framework
