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

/** @file Declaration of CUDA extensions to the Queries namespace. */

#pragma once

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <set>
#include <string>

#include <ArgoNavis/Base/StackTrace.hpp>
#include <ArgoNavis/Base/ThreadName.hpp>
#include <ArgoNavis/Base/Time.hpp>
#include <ArgoNavis/Base/TimeInterval.hpp>

#include <ArgoNavis/CUDA/CopyKind.hpp>
#include <ArgoNavis/CUDA/MemoryKind.hpp>
#include <ArgoNavis/CUDA/PerformanceData.hpp>

#include "Address.hxx"
#include "Blob.hxx"
#include "Collector.hxx"
#include "PCBuffer.hxx"
#include "StackTrace.hxx"
#include "Thread.hxx"
#include "Time.hxx"
#include "TimeInterval.hxx"

#include "CUDAExecXferBalance.hxx"
#include "CUDAXferRate.hxx"

namespace OpenSpeedShop { namespace Queries {

    /**
     * Convert the given ArgoNavis::CUDA::CopyKind to a string.
     *
     * @param kind    Copy kind to be converted.
     * @return        Converted copy kind.
     */
    std::string ConvertFromArgoNavis(const ArgoNavis::CUDA::CopyKind& kind);

    /**
     * Convert the given ArgoNavis::CUDA::MemoryKind to a string.
     *
     * @param kind    Memory kind to be converted.
     * @return        Converted memory kind.
     */
    std::string ConvertFromArgoNavis(const ArgoNavis::CUDA::MemoryKind& kind);

    /**
     * Convert the given ArgoNavis::Base::StackTrace to a stack trace.
     *
     * @param trace     Stack trace to be converted.
     * @param thread    Thread in which this stack trace was recorded.
     * @param time      Time at which this stack trace was recorded.
     * @return          Converted stack trace.
     */
    Framework::StackTrace ConvertFromArgoNavis(
        const ArgoNavis::Base::StackTrace& trace,
        const Framework::Thread& thread,
        const Framework::Time& time
        );
    
    /**
     * Convert the given ArgoNavis::Base::Time to a time.
     *
     * @param time    Time to be converted.
     * @return        Converted time.
     */
    Framework::Time ConvertFromArgoNavis(const ArgoNavis::Base::Time& time);

    /**
     * Convert the given ArgoNavis::Base::TimeInterval to a time interval.
     *
     * @param interval    Time interval to be converted.
     * @return            Converted time interval.
     */
    Framework::TimeInterval ConvertFromArgoNavis(
        const ArgoNavis::Base::TimeInterval& interval
        );

    /**
     * Convert the given thread to an ArgoNavis::Base::ThreadName.
     *
     * @param thread    Thread to be converted.
     * @return          Converted thread name.
     */
    ArgoNavis::Base::ThreadName ConvertToArgoNavis(
        const Framework::Thread& thread
        );

    /**
     * Convert the given time interval to an ArgoNavis::Base::TimeInterval.
     *
     * @param interval    Time interval to be converted.
     * @return            Converted time interval.
     */
    ArgoNavis::Base::TimeInterval ConvertToArgoNavis(
        const Framework::TimeInterval& interval
        );

    /**
     * Get the CUDA performance data for the given collector and thread,
     * adding it to the specified ArgoNavis::CUDA::PerformanceData object
     * for use by use by subsequent queries.
     *
     * @pre    Can only be performed for a CUDA collector. An assertion
     *         failure occurs if a different collector is used.
     *
     * @pre    The thread must be in the same experiment as the collector.
     *         An assertion failure occurs if the thread is in a different
     *         experiment than the collector.
     *
     * @param collector    Collector of the performance data to extract.
     * @param thread       Thread generating the performance data to extract.
     * @param data         Object to which the extracted performance data
     *                     should be added.
     */
    void GetCUDAPerformanceData(const Framework::Collector& collector,
                                const Framework::Thread& thread,
                                ArgoNavis::CUDA::PerformanceData& data);

    /**
     * Get metrics for evaluating the balance between the time spent in
     * CUDA kernel executions versus the time spent in CUDA data transfers.
     *
     * @param data        CUDA performance data for which to get the metrics.
     * @param interval    Time interval over which to get the metrics. Has
     *                    a default value meaning of "all possible time".
     * @return            Metrics for evaluating the balance.
     */
    CUDAExecXferBalance GetCUDAExecXferBalance(
        const ArgoNavis::CUDA::PerformanceData& data,
        const Framework::TimeInterval& interval = Framework::TimeInterval(
            Framework::Time::TheBeginning(),
            Framework::Time::TheEnd()
            )
        );

    /**
     * Get metrics for evaluating the CUDA data transfer rate.
     *
     * @param data        CUDA performance data for which to get the metrics.
     * @param interval    Time interval over which to get the metrics. Has
     *                    a default value meaning of "all possible time".
     * @return            Metrics for evaluating the data transfer rate.
     */
    CUDAXferRate GetCUDAXferRate(
        const ArgoNavis::CUDA::PerformanceData& data,
        const Framework::TimeInterval& interval = Framework::TimeInterval(
            Framework::Time::TheBeginning(),
            Framework::Time::TheEnd()
            )
        );

    /**
     * Get the unique PCs in the given blob containing CUDA performance data.
     *
     * @param blob      Blob containing CUDA performance data.
     * @param buffer    Buffer of unique PC values.
     */
    void GetCUDAUniquePCs(const Framework::Blob& blob, PCBuffer* buffer);

    /**
     * Get the unique PCs in the given blob containing CUDA performance data.
     *
     * @param blob         Blob containing CUDA performance data.
     * @param addresses    Set of unique PC values.
     */
    void GetCUDAUniquePCs(const Framework::Blob& blob,
                          std::set<Framework::Address>& addresses);
        
} } // namespace OpenSpeedShop::Queries
