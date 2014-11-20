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

/** @file Definition of CUDA extensions to the Queries namespace. */

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/ref.hpp>
#include <cstring>

#include "CUDAQueries.hxx"

using namespace boost;
using namespace OpenSpeedShop;
using namespace OpenSpeedShop::Framework;
using namespace OpenSpeedShop::Queries;
using namespace std;



/** Anonymous namespace hiding implementation details. */
namespace {

    /** Visitor used to accumulate the total event time. */
    template <typename T>
    void accumulateEventTime(const T& details,
                             const TimeInterval& query_interval,
                             double& time)
    {
        TimeInterval event_interval(details.time_begin, details.time_end);

        time += static_cast<double>(
            (query_interval & event_interval).getWidth()
            ) / 1000000000.0 /* ns/s */;
    }

    /** Visitor used to accumulate the total data transfer size and time. */
    void accumulateXferSizeAndTime(const CUDAData::MemoryCopyDetails& details,
                                   const TimeInterval& query_interval,
                                   uint64_t& size, double& time)
    {
        size += details.size;

        TimeInterval event_interval(details.time_begin, details.time_end);
        
        time += static_cast<double>(
            (query_interval & event_interval).getWidth()
            ) / 1000000000.0 /* ns/s */;
    }
    
} // namespace <anonymous>



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CUDAExecXferBalance Queries::GetCUDAExecXferBalance(
    const CUDAData& data, const TimeInterval& interval
    )
{
    CUDAExecXferBalance result;
    memset(&result, 0, sizeof(CUDAExecXferBalance));

    function<void (const CUDAData::KernelExecutionDetails&)> exec_time = bind(
        &accumulateEventTime<CUDAData::KernelExecutionDetails>,
        _1, cref(interval), ref(result.exec_time)
        );

    function<void (const CUDAData::MemoryCopyDetails&)> xfer_time = bind(
        &accumulateEventTime<CUDAData::MemoryCopyDetails>,
        _1, cref(interval), ref(result.xfer_time)
        );

    data.visit_kernel_executions(exec_time, interval);
    data.visit_memory_copies(xfer_time, interval);
    
    return result;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CUDAXferRate Queries::GetCUDAXferRate(
    const CUDAData& data, const TimeInterval& interval
    )
{
    CUDAXferRate result;
    memset(&result, 0, sizeof(CUDAXferRate));
    
    function<void (const CUDAData::MemoryCopyDetails&)> xfer_rate = bind(
        &accumulateXferSizeAndTime,
        _1, cref(interval), ref(result.size), ref(result.time)
        );
    
    data.visit_memory_copies(xfer_rate, interval);
    
    return result;
}
