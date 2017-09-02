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

/** @file Definition of the CUDACollector class. */

#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <map>
#include <stddef.h>
#include <utility>
#include <vector>

#include <ArgoNavis/Base/PeriodicSamplesGroup.hpp>
#include <ArgoNavis/Base/PeriodicSamples.hpp>
#include <ArgoNavis/Base/PeriodicSampleVisitor.hpp>
#include <ArgoNavis/Base/TimeInterval.hpp>

#include <ArgoNavis/CUDA/DataTransfer.hpp>
#include <ArgoNavis/CUDA/DataTransferVisitor.hpp>
#include <ArgoNavis/CUDA/KernelExecution.hpp>
#include <ArgoNavis/CUDA/KernelExecutionVisitor.hpp>
 
#include "AddressRange.hxx"
#include "Metadata.hxx"
#include "StackTrace.hxx"
#include "TimeInterval.hxx"

#include "CUDAQueries.hxx"

#include "CUDACollector.hxx"
#include "CUDACountsDetail.hxx"
#include "CUDAExecDetail.hxx"
#include "CUDAXferDetail.hxx"

using namespace ArgoNavis;
using namespace boost;
using namespace OpenSpeedShop::Framework;
using namespace OpenSpeedShop::Queries;
using namespace std;



/** Anonymous namespace hiding implementation details. */
namespace {

    /** Type returned for the CUDA kernel execution detail metrics. */
    typedef map<StackTrace, vector<CUDAExecDetail> > ExecDetails;

    /** Type returned for the CUDA data transfer detail metrics. */
    typedef map<StackTrace, vector<CUDAXferDetail> > XferDetails;

    /** Visitor used to compute the count_exclusive_details metric. */
    bool computeCounts(const Base::Time& time,
                       const vector<boost::uint64_t>& counts,
                       vector<CUDACountsDetail>& results)
    {
        results.push_back(CUDACountsDetail(time, counts));
        
        return true; // Always continue the visitation
    }
    
    /**
     * Visitor used to compute [exec|xfer]_[exclusive|inclusive]_detail metrics.
     */
    template <typename T, typename U, bool Inclusive>
    bool computeDetails(const T& info,
                        const CUDA::PerformanceData& data,
                        const Thread& thread,
                        const ExtentGroup& subextents,
                        vector<U>& results)
    {
        TimeInterval interval = ConvertFromArgoNavis(
            Base::TimeInterval(info.time_begin, info.time_end)
            );
        
        StackTrace trace = ConvertFromArgoNavis(
            data.sites()[info.call_site],
            thread,
            ConvertFromArgoNavis(info.time)
            );

        for (StackTrace::const_iterator
                 i = trace.begin(); i != trace.end(); ++i)
        {
            if (!Inclusive && (i != trace.begin()))
            {
                break;
            }
            
            set<ExtentGroup::size_type> intersection = 
                subextents.getIntersectionWith(
                    Extent(interval, AddressRange(*i))
                    );
            
            for (set<ExtentGroup::size_type>::const_iterator
                     j = intersection.begin(); j != intersection.end(); ++j)
            {
                double t_intersection = static_cast<double>(
                    (interval & subextents[*j].getTimeInterval()).getWidth()
                    ) / 1000000000.0;

                typename U::iterator k = results[*j].insert(
                    make_pair(trace, typename U::mapped_type())
                    ).first;

                typename U::mapped_type::value_type details(
                    info, data.devices()[info.device], t_intersection
                    );
                
                k->second.push_back(details);
            }
        }

        return true; // Always continue the visitation
    }

    /** Visitor used to compute [exec|xfer]_time metrics. */
    template <typename T>
    bool computeTime(const T& info,
                     const CUDA::PerformanceData& data,
                     const Thread& thread,
                     const ExtentGroup& subextents,
                     vector<double>& results)
    {
        TimeInterval interval = ConvertFromArgoNavis(
            Base::TimeInterval(info.time_begin, info.time_end)
            );
        
        StackTrace trace = ConvertFromArgoNavis(
            data.sites()[info.call_site],
            thread,
            ConvertFromArgoNavis(info.time)
            );
        
        set<ExtentGroup::size_type> intersection = 
            subextents.getIntersectionWith(
                Extent(interval, AddressRange(trace[0]))
                );
        
        for (set<ExtentGroup::size_type>::const_iterator
                 i = intersection.begin(); i != intersection.end(); ++i)
        {
            double t_intersection = static_cast<double>(
                (interval & subextents[*i].getTimeInterval()).getWidth()
                ) / 1000000000.0;
            
            results[*i] += t_intersection;
        }

        return true; // Always continue the visitation
    }
    
} // namespace <anonymous>



/**
 * Collector's factory method.
 *
 * Factory method for instantiating a collector implementation. This is the
 * only function that is externally visible from outside the collector plugin.
 *
 * @return    New instance of this collector's implementation.
 */
extern "C" CollectorImpl* cuda_LTX_CollectorFactory()
{
    return new CUDACollector();
}



/**
 * Default constructor.
 *
 * Constructs a new CUDA collector with the proper metadata.
 */
CUDACollector::CUDACollector() :
    CollectorImpl("cuda", "CUDA",
                  "Intercepts all calls to CUDA data transfers and kernel "
                  "executions and records, for each call, the current stack "
                  "trace and start/end time, as well as additional relevant "
                  "information depending on the operation. In addition, has "
                  "the ability to periodically sample hardware event counts "
                  "via PAPI for both the CPU and GPU."),
    dm_current(),
    dm_data(),
    dm_threads()
{
    // Declare our metrics

    declareMetric(Metadata("exec_time",
                           "CUDA Kernel Execution Time",
                           "CUDA kernel execution time in seconds.",
                           typeid(double)));
    declareMetric(Metadata("exec_inclusive_details",
                           "Inclusive CUDA Kernel Execution Details",
                           "Inclusive CUDA kernel execution details.",
                           typeid(ExecDetails)));
    declareMetric(Metadata("exec_exclusive_details",
                           "Exclusive CUDA Kernel Execution Details",
                           "Exclusive CUDA kernel execution details.",
                           typeid(ExecDetails)));

    declareMetric(Metadata("xfer_time",
                           "CUDA Data Transfer Time",
                           "CUDA data transfer time in seconds.",
                           typeid(double)));
    declareMetric(Metadata("xfer_inclusive_details",
                           "Inclusive CUDA Data Transfer Details",
                           "Inclusive CUDA data transfer details.",
                           typeid(XferDetails)));
    declareMetric(Metadata("xfer_exclusive_details",
                           "Exclusive CUDA Data Transfer Details",
                           "Exclusive CUDA data transfer details.",
                           typeid(XferDetails)));

    declareMetric(Metadata("count_counters",
                           "Sampled HWC Names",
                          "Names of all sampled hardware performance counters.",
                           typeid(vector<string>)));
    
    declareMetric(Metadata("count_exclusive_details",
                           "Exclusive HWC Details",
                           "Exclusive hardware performance counter details.",
                           typeid(vector<CUDACountsDetail>)));

    declareMetric(Metadata("periodic_samples",
                           "Periodic HWC Samples",
                           "Periodic hardware performance counter samples.",
                           typeid(Base::PeriodicSamplesGroup)));
}



/**
 * Get the default parameter values.
 *
 * Implement getting our default parameter values.
 *
 * @return    Blob containing the default parameter values.
 */
Blob CUDACollector::getDefaultParameterValues() const
{
    // This method's implementation is intentionally empty.
    //
    // The CUDA collector is only available for the CBTF-based data collection
    // model. And that data collection model doesn't use this method.

    return Blob();
}



/**
 * Get a parameter value.
 *
 * Implement getting one of our parameter values.
 *
 * @param parameter    Unique identifier of the parameter.
 * @param data         Blob containing the parameter values.
 * @retval ptr         Untyped pointer to the parameter value.
 */
void CUDACollector::getParameterValue(const string& parameter,
                                      const Blob& data, void* ptr) const
{
    // This method's implementation is intentionally empty.
    //
    // The CUDA collector is only available for the CBTF-based data collection
    // model. And that data collection model doesn't use this method.
}



/**
 * Set a parameter value.
 *
 * Implements setting one of our parameter values.
 *
 * @param parameter    Unique identifier of the parameter.
 * @param ptr          Untyped pointer to the parameter value.
 * @retval data        Blob containing the parameter values.
 */
void CUDACollector::setParameterValue(const string& parameter,
                                      const void* ptr, Blob& data) const
{
    // This method's implementation is intentionally empty.
    //
    // The CUDA collector is only available for the CBTF-based data collection
    // model. And that data collection model doesn't use this method.
}



/**
 * Start data collection.
 *
 * Implement starting data collection for the specified threads.
 *
 * @param collector    Collector starting data collection.
 * @param threads      Threads for which to start collecting data.
 */
void CUDACollector::startCollecting(const Collector& collector,
                                    const ThreadGroup& threads) const
{
    // This method's implementation is intentionally empty.
    //
    // The CUDA collector is only available for the CBTF-based data collection
    // model. And that data collection model doesn't use this method.
}



/**
 * Stops data collection.
 *
 * Implement stopping data collection for the specified threads.
 *
 * @param collector    Collector stopping data collection.
 * @param threads      Threads for which to stop collecting data.
 */
void CUDACollector::stopCollecting(const Collector& collector,
                                   const ThreadGroup& threads) const
{
    // This method's implementation is intentionally empty.
    //
    // The CUDA collector is only available for the CBTF-based data collection
    // model. And that data collection model doesn't use this method.
}



/**
 * Get metric values.
 *
 * Implements getting one of this collector's metric values over all subextents
 * of the specified extent for a particular thread, for one of the collected
 * performance data blobs.
 *
 * @param metric        Unique identifier of the metric.
 * @param collector     Collector for which to get values.
 * @param thread        Thread for which to get values.
 * @param extent        Extent of the performance data blob.
 * @param blob          Blob containing the performance data.
 * @param subextents    Subextents for which to get values.
 * @retval ptr          Untyped pointer to the values of the metric.
 */
void CUDACollector::getMetricValues(const string& metric,
                                    const Collector& collector,
                                    const Thread& thread,
                                    const Extent& extent,
                                    const Blob& blob,
                                    const ExtentGroup& subextents,
                                    void* ptr) const
{
    //
    // CUDA allows requests to be executed asynchronously. Thus the issue and
    // completion of a given request can be found within 2 separate blobs. As
    // a consequence, it is necessary to have a stateful collector in order to
    // correctly compute certain metrics. But the collector API isn't designed
    // for that. I.e. there is no direct indication of whether a given call to
    // getMetricValues() is part of a previous or new metric computation. The
    // fix employed here is to:
    //
    //     * Always find and pre-process all of the CUDA performance data
    //       blobs for a given thread the first time that thread is seen.
    //
    //     * Compare the thread and subextents of each getMetricValues()
    //       call against the same values from the previous call and, when
    //       they differ, assume a new metric computation is beginning.
    //
    //     * Generate all of the metric values immediately upon the first
    //       call to getMetricValues() for a new metric computation.
    //

    if (dm_current &&
        (dm_current->first == thread) && (dm_current->second == subextents))
    {
        // No need to do anything else if the thread and subextents
        // for this call matches that of the previous call (if any).
        return;
    }
    
    if (dm_threads.find(thread) == dm_threads.end())
    {
        dm_threads.insert(thread);
        GetCUDAPerformanceData(collector, thread, dm_data);
    }

    dm_current = make_pair(thread, subextents);

    if (metric == "count_counters")
    {
        vector<vector<string> >& data = 
            *reinterpret_cast<vector<vector<string> >*>(ptr);
        
        for (ExtentGroup::size_type i = 0; i < subextents.size(); ++i)
        {
            data[i].clear();

            for (std::size_t j = 0; j < dm_data.counters().size(); ++j)
            {
                data[i].push_back(dm_data.counters()[j].name);
            }
        }
    }

    else if (metric == "count_exclusive_details")
    {
        vector<vector<CUDACountsDetail> >& data = 
            *reinterpret_cast<vector<vector<CUDACountsDetail> >*>(ptr);

        for (ExtentGroup::size_type i = 0; i < subextents.size(); ++i)
        {
            Base::PeriodicSampleVisitor visitor = bind(
                &computeCounts, _1, _2, boost::ref(data[i])
                );
            
            dm_data.visitPeriodicSamples(
                ConvertToArgoNavis(thread),
                ConvertToArgoNavis(subextents[i].getTimeInterval()),
                visitor
                );
        }
    }

    else if (metric == "exec_time")
    {
        CUDA::KernelExecutionVisitor visitor = bind(
            &computeTime<CUDA::KernelExecution>, _1,
            boost::cref(dm_data), boost::cref(thread), boost::cref(subextents),
            boost::ref(*reinterpret_cast<vector<double>*>(ptr))
            );
        
        dm_data.visitKernelExecutions(
            ConvertToArgoNavis(thread),
            ConvertToArgoNavis(subextents.getBounds().getTimeInterval()),
            visitor
            );
    }

    else if (metric == "exec_inclusive_details")
    {
        CUDA::KernelExecutionVisitor visitor = bind(
            &computeDetails<CUDA::KernelExecution, ExecDetails, true>, _1,
            boost::cref(dm_data), boost::cref(thread), boost::cref(subextents),
            boost::ref(*reinterpret_cast<vector<ExecDetails>*>(ptr))
            );
        
        dm_data.visitKernelExecutions(
            ConvertToArgoNavis(thread),
            ConvertToArgoNavis(subextents.getBounds().getTimeInterval()),
            visitor
            );
    }

    else if (metric == "exec_exclusive_details")
    {
        CUDA::KernelExecutionVisitor visitor = bind(
            &computeDetails<CUDA::KernelExecution, ExecDetails, false>, _1,
            boost::cref(dm_data), boost::cref(thread), boost::cref(subextents),
            boost::ref(*reinterpret_cast<vector<ExecDetails>*>(ptr))
            );
        
        dm_data.visitKernelExecutions(
            ConvertToArgoNavis(thread),
            ConvertToArgoNavis(subextents.getBounds().getTimeInterval()),
            visitor
            );
    }

    else if (metric == "xfer_time")
    {
        CUDA::DataTransferVisitor visitor = bind(
            &computeTime<CUDA::DataTransfer>, _1,
            boost::cref(dm_data), boost::cref(thread), boost::cref(subextents),
            boost::ref(*reinterpret_cast<vector<double>*>(ptr))
            );
        
        dm_data.visitDataTransfers(
            ConvertToArgoNavis(thread),
            ConvertToArgoNavis(subextents.getBounds().getTimeInterval()),
            visitor
            );
    }

    else if (metric == "xfer_inclusive_details")
    {
        CUDA::DataTransferVisitor visitor = bind(
            &computeDetails<CUDA::DataTransfer, XferDetails, true>, _1,
            boost::cref(dm_data), boost::cref(thread), boost::cref(subextents),
            boost::ref(*reinterpret_cast<vector<XferDetails>*>(ptr))
            );
        
        dm_data.visitDataTransfers(
            ConvertToArgoNavis(thread),
            ConvertToArgoNavis(subextents.getBounds().getTimeInterval()),
            visitor
            );
    }

    else if (metric == "xfer_exclusive_details")
    {
        CUDA::DataTransferVisitor visitor = bind(
            &computeDetails<CUDA::DataTransfer, XferDetails, false>, _1,
            boost::cref(dm_data), boost::cref(thread), boost::cref(subextents),
            boost::ref(*reinterpret_cast<vector<XferDetails>*>(ptr))
            );
        
        dm_data.visitDataTransfers(
            ConvertToArgoNavis(thread),
            ConvertToArgoNavis(subextents.getBounds().getTimeInterval()),
            visitor
            );
    }

    else if (metric == "periodic_samples")
    {
        vector<Base::PeriodicSamplesGroup>& data = 
            *reinterpret_cast<vector<Base::PeriodicSamplesGroup>*>(ptr);

        for (ExtentGroup::size_type i = 0; i < subextents.size(); ++i)
        {
            data[i].clear();

            for (std::size_t j = 0; j < dm_data.counters().size(); ++j)
            {
                Base::PeriodicSamples samples = dm_data.periodic(
                    ConvertToArgoNavis(thread),
                    ConvertToArgoNavis(subextents[i].getTimeInterval()), j
                    );

                if (samples.size() > 0)
                {
                    data[i].push_back(samples);
                }
            }
        }
    }
}



/**
 * Get unique PC values.
 *
 * Implements getting all of the unique program counter (PC) values encountered
 * for a particular thread, for one of the collected performance data blobs.
 *
 * @param thread     Thread for which to get unique PC values.
 * @param blob       Blob containing the performance data.
 * @retval buffer    Buffer of the unique PC values.
 */
void CUDACollector::getUniquePCValues(const Thread& thread,
                                      const Blob& blob,
                                      PCBuffer* buffer) const
{
    GetCUDAUniquePCs(blob, buffer);
}



/**
 * Get unique PC values.
 *
 * Implements getting all of the unique program counter (PC) values encountered
 * for a particular thread, for one of the collected performance data blobs.
 *
 * @param thread        Thread for which to get unique PC values.
 * @param blob          Blob containing the performance data.
 * @retval addresses    Set of unique PC values.
 */
void CUDACollector::getUniquePCValues(const Thread& thread,
                                      const Blob& blob,
                                      set<Address>& addresses) const
{
    GetCUDAUniquePCs(blob, addresses);
}
