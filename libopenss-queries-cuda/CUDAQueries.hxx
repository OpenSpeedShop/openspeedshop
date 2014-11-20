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

/** @file Declaration of CUDA extensions to the Queries namespace. */

#pragma once

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Time.hxx"
#include "TimeInterval.hxx"

#include "CUDAData.hxx"
#include "CUDAExecXferBalance.hxx"
#include "CUDAXferRate.hxx"

namespace OpenSpeedShop { namespace Queries {

    /**
     * Get metrics for evaluating the balance between the time spent in
     * CUDA kernel executions versus the time spent in CUDA data transfers.
     *
     * @param data        CUDA data for which to get the metrics.
     * @param interval    Time interval over which to get the metrics. Has
     *                    a default value meaning of "all possible time".
     * @return            Metrics for evaluating the balance.
     */
    CUDAExecXferBalance GetCUDAExecXferBalance(
        const CUDAData& data,
        const Framework::TimeInterval& interval = Framework::TimeInterval(
            Framework::Time::TheBeginning(),
            Framework::Time::TheEnd()
            )
        );

    /**
     * Get metrics for evaluating the CUDA data transfer rate.
     *
     * @param data        CUDA data for which to get the metrics.
     * @param interval    Time interval over which to get the metrics. Has
     *                    a default value meaning of "all possible time".
     * @return            Metrics for evaluating the data transfer rate.
     */
    CUDAXferRate GetCUDAXferRate(
        const CUDAData& data,
        const Framework::TimeInterval& interval = Framework::TimeInterval(
            Framework::Time::TheBeginning(),
            Framework::Time::TheEnd()
            )
        );
    
} } // namespace OpenSpeedShop::Queries
