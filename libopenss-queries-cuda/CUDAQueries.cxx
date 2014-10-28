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
#include <set>

#include "KrellInstitute/Messages/CUDA_data.h"

#include "Address.hxx"
#include "AddressRange.hxx"
#include "Assert.hxx"
#include "DataCache.hxx"
#include "DataQueues.hxx"
#include "Database.hxx"
#include "EntrySpy.hxx"
#include "Extent.hxx"
#include "SmartPtr.hxx"
#include "Time.hxx"

#include "CUDAQueries.hxx"

using namespace boost;
using namespace OpenSpeedShop;
using namespace OpenSpeedShop::Framework;
using namespace OpenSpeedShop::Queries;
using namespace std;



/** Anonymous namespace hiding implementation details. */
namespace {

    /** Type of function invoked when visiting CUDA messages. */
    typedef function<
        void (const CBTF_cuda_data&, const CBTF_cuda_message&,
              const TimeInterval& interval)
        > MessageVisitor;
    
    /** Visit all CUDA messages in the CUDA performance data blobs. */
    void visit(const Collector& collector, const Thread& thread,
               const optional<TimeInterval>& interval,
               MessageVisitor& visitor)
    {
        Assert(collector.getMetadata().getUniqueId() == "cuda");
        Assert(collector.inSameDatabase(thread));

        Extent bounds(
            interval ? *interval : 
                TimeInterval(Time::TheBeginning(), Time::TheEnd()),
            AddressRange(Address::TheLowest(), Address::TheHighest())
            );
        
        set<int> identifiers = 
            DataQueues::TheCache.getIdentifiers(collector, thread, bounds);
        
        SmartPtr<Database> database = EntrySpy(collector).getDatabase();
        
        BEGIN_TRANSACTION(database);
        for (set<int>::const_iterator
                 i = identifiers.begin(); i != identifiers.end(); ++i)
        {
            database->prepareStatement(
                "SELECT data FROM Data WHERE ROWID = ?;"
                );
            database->bindArgument(1, *i);
            while (database->executeStatement())
            {
                CBTF_cuda_data data;
                memset(&data, 0, sizeof(data));

                Blob blob = database->getResultAsBlob(1);
                blob.getXDRDecoding(
                    reinterpret_cast<xdrproc_t>(xdr_CBTF_cuda_data), &data
                    );
                
                for (u_int i = 0; i < data.messages.messages_len; ++i)
                {
                    visitor(data, data.messages.messages_val[i],
                            bounds.getTimeInterval());
                }
                
                xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_cuda_data),
                         reinterpret_cast<char*>(&data));        
            }
        }
        END_TRANSACTION(database);            
    }

    /** Visitor used to compute GetCUDAExecXferBalance. */
    void do_GetCUDAExecXferBalance(const CBTF_cuda_data& data,
                                   const CBTF_cuda_message& message,
                                   const TimeInterval& query_interval,
                                   CUDAExecXferBalance& result)
    {
        switch (message.type)
        {
            
        case CopiedMemory:
            {
                const CUDA_CopiedMemory& msg =
                    message.CBTF_cuda_message_u.copied_memory;

                TimeInterval event_interval(Time(msg.time_begin),
                                            Time(msg.time_end));
                
                result.XferTime += static_cast<double>(
                    (query_interval & event_interval).getWidth()
                    ) / 1000000000.0 /* ns/s */;

                result.XferSize += msg.size;
            }
            break;
            
        case ExecutedKernel:
            {
                const CUDA_ExecutedKernel& msg =
                    message.CBTF_cuda_message_u.executed_kernel;

                TimeInterval event_interval(Time(msg.time_begin),
                                            Time(msg.time_end));
                
                result.ExecTime += static_cast<double>(
                    (query_interval & event_interval).getWidth()
                    ) / 1000000000.0 /* nS/S */;
            }
            break;

        default:
            break;

        }
    }
    
} // namespace <anonymous>



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CUDAExecXferBalance Queries::GetCUDAExecXferBalance(
    const Collector& collector, const Thread& thread,
    const optional<TimeInterval>& interval
    )
{
    CUDAExecXferBalance result;
    memset(&result, 0, sizeof(CUDAExecXferBalance));
    
    MessageVisitor visitor = bind(
        do_GetCUDAExecXferBalance, _1, _2, _3, ref(result)
        );
    
    visit(collector, thread, interval, visitor);
    
    return result;
}
