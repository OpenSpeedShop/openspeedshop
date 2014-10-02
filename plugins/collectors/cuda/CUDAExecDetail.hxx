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

/** @file Declaration and definition of the CUDAExecDetail structure. */

#pragma once

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Time.hxx"
#include "TimeInterval.hxx"
#include "TotallyOrdered.hxx"

namespace OpenSpeedShop { namespace Framework {

    /**
     * CUDA kernel execution details.
     *
     * Encapsulate the details metric (inclusive or exclusive) for CUDA kernel
     * executions recorded by the CUDA collector.
     */
    struct CUDAExecDetail :
        public TotallyOrdered<CUDAExecDetail>
    {
        Time dm_enqueued;          /**< Time the execution was enqueued. */
        TimeInterval dm_interval;  /**< Begin/End time of the execution. */
        double dm_time;            /**< Time spent in the execution. */	

        // ...

        /** Operator "<" defined for two CUDAExecDetail objects. */
        bool operator<(const CUDAExecDetail& other) const
        {
            if (dm_enqueued < other.dm_enqueued)
                return true;
            if (dm_enqueued > other.dm_enqueued)
                return false;
            if (dm_interval < other.dm_interval)
                return true;
            if (dm_interval > other.dm_interval)
                return false;
            return dm_time < other.dm_time;
        }
        
    }; // class CUDAExecDetail
    
} } // namespace OpenSpeedShop::Framework
