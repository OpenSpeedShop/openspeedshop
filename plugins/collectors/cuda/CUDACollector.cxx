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

/** @file Definition of the CUDACollector class. */
 
#include "CUDACollector.hxx"

#include "KrellInstitute/Messages/CUDA_data.h"

using namespace OpenSpeedShop::Framework;



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
                  "Intercepts all calls to CUDA memory copy/set and kernel"
                  "executions and records, for each call, the current stack"
                  "trace and start/end time, as well as additional relevant"
                  "information depending on the operation. In addition, has"
                  "the ability to periodically sample hardware event counts"
                  "via PAPI for both the CPU and GPU.")
{
    // Declare our parameters
    // ...

    // Declare our metrics
    // ...
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
    // ...

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
void CUDACollector::getParameterValue(const std::string& parameter,
                                      const Blob& data, void* ptr) const
{
    // ...
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
void CUDACollector::setParameterValue(const std::string& parameter,
                                      const void* ptr, Blob& data) const
{
    // ...
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
    // ...
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
    // ...
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
void CUDACollector::getMetricValues(const std::string& metric,
                                    const Collector& collector,
                                    const Thread& thread,
                                    const Extent& extent,
                                    const Blob& blob,
                                    const ExtentGroup& subextents,
                                    void* ptr) const
{
    // ...
}



/**
 * Get unique PC values.
 *
 * Implements getting all of the unique program counter (PC) values encountered
 * for a particular thread, for one of the collected performance data blobs.
 *
 * @param thread     Thread for which to get unique PC values.
 * @param blob       Blob containing the performance data.
 * @retval buffer    Buffer to the unique PC values.
 */
void CUDACollector::getUniquePCValues(const Thread& thread,
                                      const Blob& blob,
                                      PCBuffer* buffer) const
{
    // ...
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
                                      std::set<Address>& addresses) const
{
    // ...
}
