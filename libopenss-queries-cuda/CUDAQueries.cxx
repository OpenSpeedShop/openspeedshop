////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014-2017 Argo Navis Technologies. All Rights Reserved.
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

#include <boost/algorithm/string/predicate.hpp>
#include <boost/bind.hpp>
#include <boost/cstdint.hpp>
#include <boost/ref.hpp>
#include <cstring>

#include <ArgoNavis/Base/AddressVisitor.hpp>
#include <ArgoNavis/Base/ThreadVisitor.hpp>

#include <ArgoNavis/CUDA/DataTransfer.hpp>
#include <ArgoNavis/CUDA/DataTransferVisitor.hpp>
#include <ArgoNavis/CUDA/KernelExecution.hpp>
#include <ArgoNavis/CUDA/KernelExecutionVisitor.hpp>
#include <ArgoNavis/CUDA/stringify.hpp>

#include "AddressRange.hxx"
#include "Assert.hxx"
#include "Blob.hxx"
#include "Database.hxx"
#include "EntrySpy.hxx"
#include "Function.hxx"
#include "LinkedObject.hxx"
#include "Path.hxx"

#include "CUDAQueries.hxx"

using namespace boost;
using namespace ArgoNavis;
using namespace OpenSpeedShop;
using namespace OpenSpeedShop::Framework;
using namespace OpenSpeedShop::Queries;
using namespace std;



/** Anonymous namespace hiding implementation details. */
namespace {

    /** Visitor used to accumulate the total event time. */
    template <typename T>
    bool accumulateEventTime(const T& details,
                             const TimeInterval& query_interval,
                             double& time)
    {
        TimeInterval event_interval(
            Time(details.time_begin), Time(details.time_end)
            );

        time += static_cast<double>(
            (query_interval & event_interval).getWidth()
            ) / 1000000000.0 /* ns/s */;

        return true; // Always continue the visitation
    }

    /** Visitor used to accumulate the total data transfer size and time. */
    bool accumulateXferSizeAndTime(const CUDA::DataTransfer& details,
                                   const TimeInterval& query_interval,
                                   boost::uint64_t& size, double& time)
    {
        size += details.size;

        TimeInterval event_interval(
            Time(details.time_begin), Time(details.time_end)
            );
        
        time += static_cast<double>(
            (query_interval & event_interval).getWidth()
            ) / 1000000000.0 /* ns/s */;

        return true; // Always continue the visitation
    }

    /** Visitor used to call another visitor. */
    bool callExec(const Base::ThreadName& thread,
                  const CUDA::PerformanceData& data,
                  const TimeInterval& query_interval,
                  CUDA::KernelExecutionVisitor& visitor)
    {
        data.visitKernelExecutions(
            thread, ConvertToArgoNavis(query_interval), visitor
            );
        
        return true; // Always continue the visitation
    }

    /** Visitor used to call another visitor. */
    bool callXfer(const Base::ThreadName& thread,
                  const CUDA::PerformanceData& data,
                  const TimeInterval& query_interval,
                  CUDA::DataTransferVisitor& visitor)
    {
        data.visitDataTransfers(
            thread, ConvertToArgoNavis(query_interval), visitor
            );
        
        return true; // Always continue the visitation
    }

    /** Visitor used to insert an address into a buffer of unique addresses. */
    bool insertIntoAddressBuffer(Base::Address address, PCBuffer& buffer)
    {
        UpdatePCBuffer(address, &buffer);

        return true; // Always continue the visitation
    }

    /** Visitor used to insert an address into a set of unique addresses. */
    bool insertIntoAddressSet(Base::Address address, set<Address>& addresses)
    {
        addresses.insert(Address(address));

        return true; // Always continue the visitation
    }
    
} // namespace <anonymous>



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
string Queries::ConvertFromArgoNavis(const CUDA::CopyKind& kind)
{
    return CUDA::stringify<CUDA::CopyKind>(kind);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
string Queries::ConvertFromArgoNavis(const CUDA::MemoryKind& kind)
{
    return CUDA::stringify<CUDA::MemoryKind>(kind);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
StackTrace Queries::ConvertFromArgoNavis(const Base::StackTrace& trace,
                                         const Thread& thread,
                                         const Time& time)
{
    StackTrace result(thread, time);

    // Initially include ALL frames for the call site of this CUDA request
    for (Base::StackTrace::const_iterator
             i = trace.begin(); i != trace.end(); ++i)
    {
        result.push_back(Address(static_cast<boost::uint64_t>(*i)));
    }

    //
    // Trim all of the frames that preceeded the actual entry into main(),
    // assuming, of course, that main() can actually be found.
    //
    
    for (int i = 0; i < result.size(); ++i)
    {
        pair<bool, Function> function = result.getFunctionAt(i);
        if (function.first && (function.second.getName() == "main"))
        {
            result.erase(result.begin() + i + 1, result.end());
            break;
        }            
    }

    //
    // Now iterate over the frames from main() towards the final call site,
    // looking for the first frame that is part of the CUDA implementation
    // or our collector. Trim the stack trace from that point all the way
    // to the final call site.
    //
    
    for (int i = result.size(); i > 0; --i)
    {
        pair<bool, LinkedObject> linked_object = result.getLinkedObjectAt(i);
        if (linked_object.first)
        {
            Path base_name = linked_object.second.getPath().getBaseName();
            if (starts_with(base_name, "cuda-collector") ||
                starts_with(base_name, "libcu"))
            {
                result.erase(result.begin(), result.begin() + i + 1);
                break;
            }
        }
        
        pair<bool, Function> function = result.getFunctionAt(i);
        if (function.first &&
            (starts_with(function.second.getName(), "__device_stub") ||
             starts_with(function.second.getName(), "cudart::")))
        {
            result.erase(result.begin(), result.begin() + i + 1);
            break;
        }
    }
        
    return result;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Time Queries::ConvertFromArgoNavis(const Base::Time& time)
{
    return Time(static_cast<boost::uint64_t>(time));
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
TimeInterval Queries::ConvertFromArgoNavis(const Base::TimeInterval& interval)
{
    return TimeInterval(
        Time(static_cast<boost::uint64_t>(interval.begin())),
        Time(static_cast<boost::uint64_t>(interval.end() + 1))
        );
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Base::ThreadName Queries::ConvertToArgoNavis(const Thread& thread)
{
    pair<bool, pthread_t> tid = thread.getPosixThreadId();
    pair<bool, int> rank = thread.getMPIRank();
    
    return Base::ThreadName(
        thread.getHost(),
        static_cast<boost::uint64_t>(thread.getProcessId()),
        tid.first ?
            optional<boost::uint64_t>(
                static_cast<boost::uint64_t>(tid.second)
                ) :
            none,
        rank.first ?
            optional<boost::uint32_t>(
                static_cast<boost::uint32_t>(rank.second)
                ) : 
            none
        );
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Base::TimeInterval Queries::ConvertToArgoNavis(const TimeInterval& interval)
{
    return Base::TimeInterval(
        Base::Time(interval.getBegin().getValue()),
        Base::Time(interval.getEnd().getValue()) - 1
        );
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Queries::GetCUDAPerformanceData(const Collector& collector,
                                     const Thread& thread,
                                     CUDA::PerformanceData& data)
{
    Assert(collector.getMetadata().getUniqueId() == "cuda");
    Assert(collector.inSameDatabase(thread));

    Base::ThreadName name = ConvertToArgoNavis(thread);
    
    SmartPtr<Database> database = EntrySpy(collector).getDatabase();
    
    BEGIN_TRANSACTION(database);
    database->prepareStatement(
        "SELECT data FROM Data WHERE collector = ? AND thread = ?;"
        );
    database->bindArgument(1, EntrySpy(collector).getEntry());
    database->bindArgument(2, EntrySpy(thread).getEntry());
    while (database->executeStatement())
    {
        CBTF_cuda_data message;
        memset(&message, 0, sizeof(message));

        Blob blob = database->getResultAsBlob(1);
        blob.getXDRDecoding(
            reinterpret_cast<xdrproc_t>(xdr_CBTF_cuda_data), &message
            );
    
        data.apply(name, message);

        xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_cuda_data),
                 reinterpret_cast<char*>(&message));
    }
    END_TRANSACTION(database);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CUDAExecXferBalance Queries::GetCUDAExecXferBalance(
    const CUDA::PerformanceData& data, const TimeInterval& interval
    )
{
    CUDAExecXferBalance result;
    memset(&result, 0, sizeof(CUDAExecXferBalance));

    CUDA::KernelExecutionVisitor exec_time = bind(
        &accumulateEventTime<CUDA::KernelExecution>,
        _1, boost::cref(interval), boost::ref(result.exec_time)
        );

    Base::ThreadVisitor call_exec_time = bind(
        &callExec, _1, boost::cref(data), boost::cref(interval), boost::ref(exec_time)
        );

    data.visitThreads(call_exec_time);

    CUDA::DataTransferVisitor xfer_time = bind(
        &accumulateEventTime<CUDA::DataTransfer>,
        _1, boost::cref(interval), boost::ref(result.xfer_time)
        );

    Base::ThreadVisitor call_xfer_time = bind(
        &callXfer, _1, boost::cref(data), boost::cref(interval), boost::ref(xfer_time)
        );

    data.visitThreads(call_xfer_time);
    
    return result;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CUDAXferRate Queries::GetCUDAXferRate(
    const CUDA::PerformanceData& data, const TimeInterval& interval
    )
{
    CUDAXferRate result;
    memset(&result, 0, sizeof(CUDAXferRate));
    
    CUDA::DataTransferVisitor xfer_rate = bind(
        &accumulateXferSizeAndTime,
        _1, boost::cref(interval), boost::ref(result.size), boost::ref(result.time)
        );

    Base::ThreadVisitor call_xfer_rate = bind(
        &callXfer, _1, boost::cref(data), boost::cref(interval), boost::ref(xfer_rate)
        );

    data.visitThreads(call_xfer_rate);
    
    return result;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Queries::GetCUDAUniquePCs(const Blob& blob, PCBuffer* buffer)
{
    CBTF_cuda_data message;
    memset(&message, 0, sizeof(message));

    blob.getXDRDecoding(
        reinterpret_cast<xdrproc_t>(xdr_CBTF_cuda_data), &message
        );

    Base::AddressVisitor visitor = bind(
        &insertIntoAddressBuffer, _1, boost::ref(*buffer)
        );

    CUDA::PerformanceData::visitPCs(message, visitor);

    xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_cuda_data),
             reinterpret_cast<char*>(&message));
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Queries::GetCUDAUniquePCs(const Blob& blob,
                               std::set<Address>& addresses)
{
    CBTF_cuda_data message;
    memset(&message, 0, sizeof(message));

    blob.getXDRDecoding(
        reinterpret_cast<xdrproc_t>(xdr_CBTF_cuda_data), &message
        );

    Base::AddressVisitor visitor = bind(
        &insertIntoAddressSet, _1, boost::ref(addresses)
        );

    CUDA::PerformanceData::visitPCs(message, visitor);

    xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_cuda_data),
             reinterpret_cast<char*>(&message));
}
