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

/** @file Definition of the CUDAData class. */

#include <boost/algorithm/string/predicate.hpp>
#include <cstring>
#include <utility>

#include "KrellInstitute/Messages/CUDA_data.h"

#include "AddressRange.hxx"
#include "Assert.hxx"
#include "Blob.hxx"
#include "Database.hxx"
#include "EntrySpy.hxx"
#include "Function.hxx"
#include "LinkedObject.hxx"
#include "Path.hxx"

#include "CUDAData.hxx"

using namespace boost;
using namespace OpenSpeedShop;
using namespace OpenSpeedShop::Framework;
using namespace OpenSpeedShop::Queries;
using namespace std;



/** Anonymous namespace hiding implementation details. */
namespace {

    /** Convert a CUDA_CachePreference value to CUDAData::CachePreferences. */
    CUDAData::CachePreferences convert(const CUDA_CachePreference& value)
    {
        switch (value)
        {
        case InvalidCachePreference: return CUDAData::kInvalidCachePreference;
        case NoPreference: return CUDAData::kNoPreference;
        case PreferShared: return CUDAData::kPreferShared;
        case PreferCache: return CUDAData::kPreferCache;
        case PreferEqual: return CUDAData::kPreferEqual;
        default: return CUDAData::kInvalidCachePreference;
        }
    }

    /** Convert a CUDA_CopyKind value to CUDAData::CopyKinds. */
    CUDAData::CopyKinds convert(const CUDA_CopyKind& value)
    {
        switch (value)
        {
        case InvalidCopyKind: return CUDAData::kInvalidCopyKind;
        case UnknownCopyKind: return CUDAData::kUnknownCopyKind;
        case HostToDevice: return CUDAData::kHostToDevice;
        case DeviceToHost: return CUDAData::kDeviceToHost;
        case HostToArray: return CUDAData::kHostToArray;
        case ArrayToHost: return CUDAData::kArrayToHost;
        case ArrayToArray: return CUDAData::kArrayToArray;
        case ArrayToDevice: return CUDAData::kArrayToDevice;
        case DeviceToArray: return CUDAData::kDeviceToArray;
        case DeviceToDevice: return CUDAData::kDeviceToDevice;
        case HostToHost: return CUDAData::kHostToHost;
        default: return CUDAData::kInvalidCopyKind;
        }
    }

    /** Convert a CUDA_MemoryKind value to CUDAData::MemoryKinds. */
    CUDAData::MemoryKinds convert(const CUDA_MemoryKind& value)
    {
        switch (value)
        {
        case InvalidMemoryKind: return CUDAData::kInvalidMemoryKind;
        case UnknownMemoryKind: return CUDAData::kUnknownMemoryKind;
        case Pageable: return CUDAData::kPageable;
        case Pinned: return CUDAData::kPinned;
        case Device: return CUDAData::kDevice;
        case Array: return CUDAData::kArray;
        default: return CUDAData::kInvalidMemoryKind;
        }
    }

    /** Find the first|last event whose time succeeds|preceeds or equals t. */
    template <typename T>
    typename vector<T>::size_type find(const vector<T>& events,
                                       const Time& t,
                                       bool is_lower)
    {
        int64_t min = 0, max = events.size() - 1;
        
        while (max > min)
        {
            int64_t mid = (min + max) / 2;
            
            if (events[mid].time == t)
            {
                min = mid;
                max = mid;
            }
            else if (events[mid].time < t)
            {
                min = mid + 1;
            }
            else
            {
                max = mid - 1;
            }
        }

        return is_lower ? min : max;
    }

    /** Find the first|last sample whose time succeeds|preceeds or equals t. */
    vector<uint64_t>::size_type find(const vector<uint64_t>& samples,
                                     const vector<uint64_t>::size_type& N,
                                     const Time& t,
                                     bool is_lower)
    {
        int64_t min = 0, max = (samples.size() - 1) / N;

        while (max > min)
        {
            int64_t mid = (min + max) / 2;
            
            if (Time(samples[mid * N]) == t)
            {
                min = mid;
                max = mid;
            }
            else if (Time(samples[mid * N]) < t)
            {
                min = mid + 1;
            }
            else
            {
                max = mid - 1;
            }
        }

        return (is_lower ? min : max) * N;
    }
    
} // namespace <anonymous>



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
string CUDAData::stringify(CachePreferences value)
{
    switch (value)
    {
    case kInvalidCachePreference: return "InvalidCachePreference";
    case kNoPreference: return "NoPreference";
    case kPreferShared: return "PreferShared";
    case kPreferCache: return "PreferCache";
    case kPreferEqual: return "PreferEqual";
    }
    return "?";
}


    
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
string CUDAData::stringify(CopyKinds value)
{
    switch (value)
    {
    case kInvalidCopyKind: return "InvalidCopyKind";
    case kUnknownCopyKind: return "UnknownCopyKind";
    case kHostToDevice: return "HostToDevice";
    case kDeviceToHost: return "DeviceToHost";
    case kHostToArray: return "HostToArray";
    case kArrayToHost: return "ArrayToHost";
    case kArrayToArray: return "ArrayToArray";
    case kArrayToDevice: return "ArrayToDevice";
    case kDeviceToArray: return "DeviceToArray";
    case kDeviceToDevice: return "DeviceToDevice";
    case kHostToHost: return "HostToHost";
    }
    return "?";
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
string CUDAData::stringify(MemoryKinds value)
{
    switch (value)
    {
    case kInvalidMemoryKind: return "InvalidMemoryKind";
    case kUnknownMemoryKind: return "UnknownMemoryKind";
    case kPageable: return "Pageable";
    case kPinned: return "Pinned";
    case kDevice: return "Device";
    case kArray: return "Array";
    }
    return "?";
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CUDAData::CUDAData(const Collector& collector, const Thread& thread) :
    dm_call_sites(),
    dm_counters(),
    dm_devices(),
    dm_kernel_executions(),
    dm_known_devices(),
    dm_known_contexts(),
    dm_memory_copies(),
    dm_memory_sets(),
    dm_periodic_samples(),
    dm_requests(),
    dm_time_origin(Time::TheEnd())
{
    Assert(collector.getMetadata().getUniqueId() == "cuda");
    Assert(collector.inSameDatabase(thread));

    SmartPtr<Database> database = EntrySpy(collector).getDatabase();
        
    BEGIN_TRANSACTION(database);
    database->prepareStatement(
        "SELECT time_begin, data FROM Data WHERE collector = ? AND thread = ?;"
        );
    database->bindArgument(1, EntrySpy(collector).getEntry());
    database->bindArgument(2, EntrySpy(thread).getEntry());
    while (database->executeStatement())
    {
        CBTF_cuda_data data;
        memset(&data, 0, sizeof(data));

        Time t = database->getResultAsTime(1);
        if (t < dm_time_origin)
        {
            dm_time_origin = t;
        }
        
        Blob blob = database->getResultAsBlob(2);
        blob.getXDRDecoding(
            reinterpret_cast<xdrproc_t>(xdr_CBTF_cuda_data), &data
            );

        const CUDA_OverflowSamples* overflow_samples = NULL;
        const CUDA_PeriodicSamples* periodic_samples = NULL;
        
        for (u_int i = 0; i < data.messages.messages_len; ++i)
        {
            const CBTF_cuda_message& message = data.messages.messages_val[i];

            switch (message.type)
            {

            case ContextInfo:
                process(message.CBTF_cuda_message_u.context_info);
                break;

            case CopiedMemory:
                process(message.CBTF_cuda_message_u.copied_memory);
                break;

            case DeviceInfo:
                process(message.CBTF_cuda_message_u.device_info);
                break;
                
            case EnqueueRequest:
                process(message.CBTF_cuda_message_u.enqueue_request,
                        data, thread);
                break;

            case ExecutedKernel:
                process(message.CBTF_cuda_message_u.executed_kernel);
                break;

            case OverflowSamples:
                if (overflow_samples == NULL)
                {
                    overflow_samples = 
                        &message.CBTF_cuda_message_u.overflow_samples;
                }
#ifndef NDEBUG
                else
                {
                    cerr << "WARNING: CUDAData ignored an extra "
                         << "CUDA_OverflowSamples message within a "
                         << "performance data blob."
                         << endl;
                }
#endif
                break;

            case PeriodicSamples:
                if (periodic_samples == NULL)
                {
                    periodic_samples =
                        &message.CBTF_cuda_message_u.periodic_samples;
                }
#ifndef NDEBUG
                else
                {
                    cerr << "WARNING: CUDAData ignored an extra "
                         << "CUDA_PeriodicSamples message within a "
                         << "performance data blob."
                         << endl;
                }
#endif
                break;

            case SamplingConfig:
                process(message.CBTF_cuda_message_u.sampling_config);
                break;
                
            case SetMemory:
                process(message.CBTF_cuda_message_u.set_memory);
                break;
                
            }
        }

        if (overflow_samples)
        {
            process(*overflow_samples);
        }

        if (periodic_samples)
        {
            process(*periodic_samples);
        }
        
        xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_cuda_data),
                 reinterpret_cast<char*>(&data));        
    }
    END_TRANSACTION(database);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CUDAData::~CUDAData()
{
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
vector<uint64_t> CUDAData::get_counts(const TimeInterval& interval) const
{
    vector<uint64_t> counts(dm_counters.size(), 0);
    
    if (dm_periodic_samples.empty())
    {
        return counts;
    }
    
    vector<uint64_t>::size_type N = 1 + dm_counters.size();
    
    vector<uint64_t>::size_type i_min = 
        find(dm_periodic_samples, N, interval.getBegin(), true);
    vector<uint64_t>::size_type i_max = 
        find(dm_periodic_samples, N, interval.getEnd(), false);
    
    if (i_min >= i_max)
    {
        return counts;
    }
    
    TimeInterval sample_interval(
        Time(dm_periodic_samples[i_min]), Time(dm_periodic_samples[i_max])
        );
    
    uint64_t interval_width = (interval & sample_interval).getWidth();
    uint64_t sample_width = sample_interval.getWidth();
    
    for (vector<uint64_t>::size_type c = 1; c < N; ++c)
    {
        uint64_t count_delta = 
            dm_periodic_samples[i_max + c] - dm_periodic_samples[i_min + c];
        
        counts[c - 1] = count_delta * interval_width / sample_width;
    }
    
    return counts;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CUDAData::visit_kernel_executions(
    function<void (const KernelExecutionDetails&)>& visitor,
    const TimeInterval& interval
    ) const
{
    if (dm_kernel_executions.empty())
    {
        return;
    }

    vector<KernelExecutionDetails>::size_type i_min = 
        find(dm_kernel_executions, interval.getBegin(), true);
    vector<KernelExecutionDetails>::size_type i_max = 
        find(dm_kernel_executions, interval.getEnd(), false);
    
    for (vector<KernelExecutionDetails>::size_type i = i_min; i <= i_max; ++i)
    {
        const KernelExecutionDetails& details = dm_kernel_executions[i];
        
        if (TimeInterval(details.time,
                         details.time_end).doesIntersect(interval))
        {
            visitor(details);
        }
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CUDAData::visit_memory_copies(
    function<void (const MemoryCopyDetails&)>& visitor,
    const TimeInterval& interval
    ) const
{
    if (dm_memory_copies.empty())
    {
        return;
    }

    vector<MemoryCopyDetails>::size_type i_min = 
        find(dm_memory_copies, interval.getBegin(), true);
    vector<MemoryCopyDetails>::size_type i_max = 
        find(dm_memory_copies, interval.getEnd(), false);
    
    for (vector<MemoryCopyDetails>::size_type i = i_min; i <= i_max; ++i)
    {
        const MemoryCopyDetails& details = dm_memory_copies[i];
        
        if (TimeInterval(details.time,
                         details.time_end).doesIntersect(interval))
        {
            visitor(details);
        }
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CUDAData::visit_memory_sets(
    function<void (const MemorySetDetails&)>& visitor,
    const TimeInterval& interval
    ) const
{
    if (dm_memory_sets.empty())
    {
        return;
    }

    vector<MemorySetDetails>::size_type i_min = 
        find(dm_memory_sets, interval.getBegin(), true);
    vector<MemorySetDetails>::size_type i_max = 
        find(dm_memory_sets, interval.getEnd(), false);
    
    for (vector<MemorySetDetails>::size_type i = i_min; i <= i_max; ++i)
    {
        const MemorySetDetails& details = dm_memory_sets[i];
        
        if (TimeInterval(details.time,
                         details.time_end).doesIntersect(interval))
        {
            visitor(details);
        }
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CUDAData::visit_periodic_samples(
    function<void (const Time&, const vector<uint64_t>&)>& visitor,
    const TimeInterval& interval
    ) const
{
    if (dm_periodic_samples.empty())
    {
        return;
    }
    
    vector<uint64_t>::size_type N = 1 + dm_counters.size();
    
    vector<uint64_t>::size_type i_min = 
        find(dm_periodic_samples, N, interval.getBegin(), true);
    vector<uint64_t>::size_type i_max = 
        find(dm_periodic_samples, N, interval.getEnd(), false);
    
    vector<uint64_t> counts(dm_counters.size(), 0);
    
    for (vector<uint64_t>::size_type i = i_min; i <= i_max; i += N)
    {
        Time t(dm_periodic_samples[i]);
        
        if (interval.doesContain(t))
        {   
            for (vector<uint64_t>::size_type c = 1; c < N; ++c)
            {
                counts[c - 1] = dm_periodic_samples[i + c];
            }
            
            visitor(t, counts);
        }
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
vector<CUDAData::DeviceDetails>::size_type CUDAData::device_from_context(
    const Address& context
    ) const
{
    map<Address, uint32_t>::const_iterator i = dm_known_contexts.find(context);
    
    if (i == dm_known_contexts.end())
    {
#ifndef NDEBUG
        cerr << "WARNING: CUDAData encountered an unknown CUDA "
             << "context (" << context << ")." << endl;
#endif
        
        return 0;
    }
    
    map<uint32_t, vector<DeviceDetails>::size_type>::const_iterator j =
        dm_known_devices.find(i->second);

    if (j == dm_known_devices.end())
    {
#ifndef NDEBUG
        cerr << "WARNING: CUDAData encountered an unknown CUDA "
             << "device (" << device << ")." << endl;
#endif
        
        return 0;
    }

    return j->second;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CUDAData::process(const CUDA_ContextInfo& message)
{
    if (dm_known_contexts.find(message.context) != dm_known_contexts.end())
    {
        return;
    }
    
    dm_known_contexts.insert(make_pair(message.context, message.device));
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CUDAData::process(const CUDA_CopiedMemory& message)
{
    list<Request>::iterator i = dm_requests.begin();
    for (; i != dm_requests.end(); ++i)
    {
        if ((i->message->type == MemoryCopy) &&
            (i->message->context == message.context) &&
            (i->message->stream == message.stream))
        {
            break;
        }
    }
    
    if (i == dm_requests.end())
    {
#ifndef NDEBUG
        cerr << "WARNING: CUDAData encountered an unmatched "
             << "CUDA_CopiedMemory request." << endl;
#endif
        return;
    }

    MemoryCopyDetails details;
    details.device = device_from_context(i->message->context);
    details.call_site = i->call_site;
    details.time = i->message->time;
    details.time_begin = message.time_begin;
    details.time_end = message.time_end;
    details.size = message.size;
    details.kind = convert(message.kind);
    details.source_kind = convert(message.source_kind);
    details.destination_kind = convert(message.destination_kind);
    details.asynchronous = message.asynchronous;

    dm_memory_copies.push_back(details);
    dm_requests.erase(i);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CUDAData::process(const CUDA_DeviceInfo& message)
{
    if (dm_known_devices.find(message.device) != dm_known_devices.end())
    {
#ifndef NDEBUG
        cerr << "WARNING: CUDAData encountered a duplicate "
             << "CUDA_DeviceInfo (device " << message.device
             << " )." << endl;
#endif
        return;
    }
    
    DeviceDetails details;
    details.name = message.name;
    details.compute_capability = Vector2u(message.compute_capability[0],
                                          message.compute_capability[1]);
    details.max_grid = Vector3u(message.max_grid[0],
                                message.max_grid[1],
                                message.max_grid[2]);
    details.max_block = Vector3u(message.max_block[0],
                                 message.max_block[1],
                                 message.max_block[2]);
    details.global_memory_bandwidth = message.global_memory_bandwidth;
    details.global_memory_size = message.global_memory_size;
    details.constant_memory_size = message.constant_memory_size;
    details.l2_cache_size = message.l2_cache_size;
    details.threads_per_warp = message.threads_per_warp;
    details.core_clock_rate = message.core_clock_rate;
    details.memcpy_engines = message.memcpy_engines;
    details.multiprocessors = message.multiprocessors;
    details.max_ipc = message.max_ipc;
    details.max_warps_per_multiprocessor = message.max_warps_per_multiprocessor;
    details.max_blocks_per_multiprocessor = 
        message.max_blocks_per_multiprocessor;
    details.max_registers_per_block = message.max_registers_per_block;
    details.max_shared_memory_per_block = message.max_shared_memory_per_block;
    details.max_threads_per_block = message.max_threads_per_block;
    
    dm_devices.push_back(details);
    dm_known_devices.insert(make_pair(message.device, dm_devices.size() - 1));
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CUDAData::process(const CUDA_EnqueueRequest& message,
                       const CBTF_cuda_data& data, const Thread& thread)
{
    // Initially include ALL frames for the call site of this CUDA request
    StackTrace trace(thread, message.time);
    for (uint32_t i = message.call_site;
         (i < data.stack_traces.stack_traces_len) &&
             (data.stack_traces.stack_traces_val[i] != 0);
         ++i)
    {
        trace.push_back(data.stack_traces.stack_traces_val[i]);
    }
    
    //
    // Trim all of the frames that preceeded the actual entry into main(),
    // assuming, of course, that main() can actually be found.
    //
    
    for (int i = 0; i < trace.size(); ++i)
    {
        pair<bool, Function> function = trace.getFunctionAt(i);
        if (function.first && (function.second.getName() == "main"))
        {
            trace.erase(trace.begin() + i + 1, trace.end());
            break;
        }            
    }

    //
    // Now iterate over the frames from main() towards the final call site,
    // looking for the first frame that is part of the CUDA implementation
    // or our collector. Trim the stack trace from that point all the way
    // to the final call site.
    //
    
    for (int i = trace.size(); i > 0; --i)
    {
        pair<bool, LinkedObject> linked_object = trace.getLinkedObjectAt(i);
        if (linked_object.first)
        {
            Path base_name = linked_object.second.getPath().getBaseName();
            if (starts_with(base_name, "cuda-collector") ||
                starts_with(base_name, "libcu"))
            {
                trace.erase(trace.begin(), trace.begin() + i + 1);
                break;
            }
        }
        
        pair<bool, Function> function = trace.getFunctionAt(i);
        if (function.first &&
            starts_with(function.second.getName(), "__device_stub"))
        {
            trace.erase(trace.begin(), trace.begin() + i + 1);
            break;
        }
    }

    //
    // Search the existing call sites for this stack trace. The existing call
    // site is reused if one is found. Otherwise this stack trace is added to
    // the call sites.
    //
    // Note that a linear search is currently being employed here because the
    // number of unique CUDA call sites is expected to be low. If that ends up
    // not being the case, and performance is inadequate, a hash of each stack
    // trace could be computed and used to accelerate this search.
    //

    vector<StackTrace>::size_type call_site;
    for (call_site = 0; call_site < dm_call_sites.size(); ++call_site)
    {
        if (dm_call_sites[call_site] == trace)
        {
            break;
        }
    }

    if (call_site == dm_call_sites.size())
    {
        dm_call_sites.push_back(trace);
    }

    // Add this request to the list of pending requests

    Request request;
    request.message = shared_ptr<CUDA_EnqueueRequest>(
        new CUDA_EnqueueRequest(message)
        );
    request.call_site = call_site;
    dm_requests.push_back(request);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CUDAData::process(const CUDA_ExecutedKernel& message)
{
    list<Request>::iterator i = dm_requests.begin();
    for (; i != dm_requests.end(); ++i)
    {
        if ((i->message->type == LaunchKernel) &&
            (i->message->context == message.context) &&
            (i->message->stream == message.stream))
        {
            break;
        }
    }
    
    if (i == dm_requests.end())
    {
#ifndef NDEBUG
        cerr << "WARNING: CUDAData encountered an unmatched "
             << "CUDA_ExecutedKernel request." << endl;
#endif
        return;
    }

    KernelExecutionDetails details;
    details.device = device_from_context(i->message->context);
    details.call_site = i->call_site;
    details.time = i->message->time;
    details.time_begin = message.time_begin;
    details.time_end = message.time_end;
    details.function = message.function;
    details.grid = Vector3u(message.grid[0], message.grid[1], message.grid[2]);
    details.block = 
        Vector3u(message.block[0], message.block[1], message.block[2]);
    details.cache_preference = convert(message.cache_preference);
    details.registers_per_thread = message.registers_per_thread;
    details.static_shared_memory = message.static_shared_memory;
    details.dynamic_shared_memory = message.dynamic_shared_memory;
    details.local_memory = message.local_memory;

    dm_kernel_executions.push_back(details);
    dm_requests.erase(i);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CUDAData::process(const CUDA_OverflowSamples& message)
{
    // TODO: Eventually we probably should make overflow samples available too.

#ifndef NDEBUG
    cerr << "WARNING: CUDAData ignored a CUDA_OverflowSamples message." << endl;
#endif
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CUDAData::process(const CUDA_PeriodicSamples& message)
{
    static int kAdditionalBytes[4] = { 0, 2, 3, 8 };

    if (dm_counters.empty())
    {
#ifndef NDEBUG
        cerr << "WARNING: CUDAData ignored a CUDA_PeriodicSamples message."
             << endl;
#endif
        return;
    }

    vector<uint64_t>::size_type n = 0, N = 1 + dm_counters.size();
    vector<uint64_t> samples(N, 0);

    for (const uint8_t* ptr = &message.deltas.deltas_val[0];
         ptr != &message.deltas.deltas_val[message.deltas.deltas_len];)
    {
        uint8_t encoding = *ptr >> 6;

        uint64_t delta = 0;
        if (encoding < 3)
        {
            delta = static_cast<uint64_t>(*ptr) & 0x3F;
        }
        ++ptr;
        for (int i = 0; i < kAdditionalBytes[encoding]; ++i)
        {
            delta <<= 8;
            delta |= static_cast<uint64_t>(*ptr++);
        }

        samples[n++] += delta;

        if (n == N)
        {
            dm_periodic_samples.insert(
                dm_periodic_samples.end(), samples.begin(), samples.end()
                );
            n = 0;
        }
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CUDAData::process(const CUDA_SamplingConfig& message)
{
    if (dm_counters.empty())
    {
        for (u_int i = 0; i < message.events.events_len; ++i)
        {
            dm_counters.push_back(message.events.events_val[i].name);
        }
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CUDAData::process(const CUDA_SetMemory& message)
{
    list<Request>::iterator i = dm_requests.begin();
    for (; i != dm_requests.end(); ++i)
    {
        if ((i->message->type == MemorySet) &&
            (i->message->context == message.context) &&
            (i->message->stream == message.stream))
        {
            break;
        }
    }
    
    if (i == dm_requests.end())
    {
#ifndef NDEBUG
        cerr << "WARNING: CUDAData encountered an unmatched "
             << "CUDA_SetMemory request." << endl;
#endif
        return;
    }

    MemorySetDetails details;
    details.device = device_from_context(i->message->context);
    details.call_site = i->call_site;
    details.time = i->message->time;
    details.time_begin = message.time_begin;
    details.time_end = message.time_end;
    details.size = message.size;

    dm_memory_sets.push_back(details);
    dm_requests.erase(i);
}
