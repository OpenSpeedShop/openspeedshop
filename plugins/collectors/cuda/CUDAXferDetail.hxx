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

/** @file Declaration and definition of the CUDAXferDetail structure. */

#pragma once

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdint.h>

#include "Time.hxx"
#include "TimeInterval.hxx"
#include "TotallyOrdered.hxx"

namespace OpenSpeedShop { namespace Framework {

    /**
     * CUDA data transfer details.
     *
     * Encapsulate the details metric (inclusive or exclusive) for CUDA data
     * transfers recorded by the CUDA collector.
     */
    struct CUDAXferDetail :
        public TotallyOrdered<CUDAXferDetail>
    {
        Time dm_enqueued;          /**< Time the transfer was enqueued. */
        TimeInterval dm_interval;  /**< Begin/End time of the transfer. */
        double dm_time;            /**< Time spent in the transfer. */

        uint64_t dm_size;          /**< Number of bytes transferred. */
        // ...

        /** Operator "<" defined for two CUDAXferDetail objects. */
        bool operator<(const CUDAXferDetail& other) const
        {
            if (dm_enqueued < other.dm_enqueued)
                return true;
            if (dm_enqueued > other.dm_enqueued)
                return false;
            if (dm_interval < other.dm_interval)
                return true;
            if (dm_interval > other.dm_interval)
                return false;
            if (dm_time < other.dm_time)
                return true;
            if (dm_time > other.dm_time)
                return false;

            return dm_size < other.dm_size;
        }
        
    }; // class CUDAXferDetail
    
} } // namespace OpenSpeedShop::Framework
