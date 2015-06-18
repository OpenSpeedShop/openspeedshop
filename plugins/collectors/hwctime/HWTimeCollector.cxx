////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
// Copyright (c) 2006-2012 Krell Institute. All Rights Reserved.
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

/** @file
 *
 * Definition of the HWTimeCollector class.
 *
 */
 
#include "HWTimeCollector.hxx"
#include "HWTimeDetail.hxx"
#include "blobs.h"

using namespace OpenSpeedShop::Framework;

#include <vector>
#include <string>
#include <iostream>
#if defined(REGISTER_HWC_EVENTS)
#include "OpenSS_Papi_Events.h"
#endif


namespace {

    /** Type returned for the sample detail metrics. */
    typedef std::map<StackTrace, HWTimeDetail> SampleDetail;

}



/**
 * Collector's factory method.
 *
 * Factory method for instantiating a collector implementation. This is the
 * only function that is externally visible from outside the collector plugin.
 *
 * @return    New instance of this collector's implementation.
 */
extern "C" CollectorImpl* hwctime_LTX_CollectorFactory()
{
    return new HWTimeCollector();
}



/**
 * Default constructor.
 *
 * Constructs a new hardware time sampling collector with the proper metadata.
 */
HWTimeCollector::HWTimeCollector() :
    CollectorImpl("hwctime",
                  "Hardware Time",
                  "Periodically interrupts the running thread, obtains the "
                  "current program stack trace (addresses), stores them, and "
                  "allows the thread to continue execution.")
{
    // Declare our parameters
    declareParameter(Metadata("sampling_rate", "Sampling Rate",
                              "Sampling rate in samples/seconds.",
                              typeid(uint64_t)));
    declareParameter(Metadata("event", "Hardware Time Counter Event",
			      "HWCTime event.",
			      typeid(std::string)));

    // Declare our metrics
    declareMetric(Metadata("inclusive_overflows", "Inclusive Overflows",
                           "Inclusive hwc overflow counts.",
                           typeid(uint64_t)));
    declareMetric(Metadata("exclusive_overflows", "Exclusive Overflows",
                           "Exclusive hwc overflow counts.",
                           typeid(uint64_t)));
    declareMetric(Metadata("inclusive_detail", "Inclusive Detail",
                           "Inclusive sample detail.",
                           typeid(SampleDetail)));
    declareMetric(Metadata("exclusive_detail", "Exclusive Detail",
                           "Exclusive sample detail.",
                           typeid(SampleDetail)));    
    declareMetric(Metadata("threadAverage", "Thread Average Time",
                           "Average Exclusive event counts across threads or ranks.",
                           typeid(uint64_t)));
    declareMetric(Metadata("threadMin", "Thread Minimum Time",
                           "Minimum Exclusive event counts across threads or ranks.",
                           typeid(uint64_t)));
    declareMetric(Metadata("threadMax", "Thread Maximum Time",
                           "Maximum Exclusive event counts across threads or ranks.",
                           typeid(uint64_t)));
    declareMetric(Metadata("percent", "Percent of Exclusive event counts",
                           "Percent of Exclusive event counts.",
                           typeid(double)));

}



/**
 * Get the default parameter values.
 *
 * Implement getting our default parameter values.
 *
 * @return    Blob containing the default parameter values.
 */
Blob HWTimeCollector::getDefaultParameterValues() const
{
    // Setup an empty parameter structure
    hwctime_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));

    // THRESHOLD is defined in PapiAPI.h. We do not want any
    // dependency on papi here. We define it anyways to 10000000.
    parameters.sampling_rate = 10000000*2;
    
    strncpy(parameters.hwctime_event,"PAPI_TOT_CYC",strlen("PAPI_TOT_CYC"));

    // Return the encoded blob to the caller
    return Blob(reinterpret_cast<xdrproc_t>(xdr_hwctime_parameters),
		&parameters);
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
void HWTimeCollector::getParameterValue(const std::string& parameter,
					const Blob& data, void* ptr) const
{
    // Decode the blob containing the parameter values
    hwctime_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_hwctime_parameters),
                        &parameters);
    // Handle the "sampling_rate" parameter
    if(parameter == "sampling_rate") {
        uint64_t* value = reinterpret_cast<uint64_t*>(ptr);
        *value = parameters.sampling_rate;
    }

    // Handle the "hwctime_event" parameter
    if(parameter == "event") {
        std::string* value = reinterpret_cast<std::string*>(ptr);
	std::string ecstr(parameters.hwctime_event);
	*value = ecstr;
    }
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
void HWTimeCollector::setParameterValue(const std::string& parameter,
					const void* ptr, Blob& data) const
{
    // Decode the blob containing the parameter values
    hwctime_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_hwctime_parameters),
                        &parameters);
    
    // Handle the "sampling_rate" parameter
    if(parameter == "sampling_rate") {
        const uint64_t* value = reinterpret_cast<const uint64_t*>(ptr);
        parameters.sampling_rate = *value;
	std::ostringstream rate;
	rate << parameters.sampling_rate;
	setenv("OPENSS_HWCTIME_THRESHOLD", rate.str().c_str(), 1);
    }

    // Handle the "hwctime_event" parameter
    if(parameter == "event") {

	const std::string* papi_event_name =
				reinterpret_cast<const std::string*>(ptr);

	// clear any default name present
	memset(&parameters.hwctime_event,0,sizeof(parameters.hwctime_event));
	// copy the new event name
	strncpy(parameters.hwctime_event,papi_event_name->c_str(),
		strlen(papi_event_name->c_str()));
	setenv("OPENSS_HWCTIME_EVENT", papi_event_name->c_str(), 1);
    }
    
    // Re-encode the blob containing the parameter values
    data = Blob(reinterpret_cast<xdrproc_t>(xdr_hwctime_parameters),
                &parameters);
}



/**
 * Start data collection.
 *
 * Implement starting data collection for the specified threads.
 *
 * @param collector    Collector starting data collection.
 * @param threads      Threads for which to start collecting data.
 */
void HWTimeCollector::startCollecting(const Collector& collector,
				      const ThreadGroup& threads) const
{
    // Assemble and encode arguments to hwctime_start_sampling()
    hwctime_start_sampling_args args;
    memset(&args, 0, sizeof(args));
    collector.getParameterValue("sampling_rate", args.sampling_rate);

    std::string papi_event_name;
    collector.getParameterValue("event", papi_event_name);
    strncpy(args.hwctime_event,papi_event_name.c_str(),strlen(papi_event_name.c_str()));

    args.experiment = getExperimentId(collector);
    args.collector = getCollectorId(collector);
    Blob arguments(reinterpret_cast<xdrproc_t>(xdr_hwctime_start_sampling_args),
                   &args);

    // Execute hwctime_stop_sampling() before we exit the threads
    executeBeforeExit(collector, threads,
		      "hwctime-rt: hwctime_stop_sampling", Blob());

    // Execute hwctime_start_sampling() in the threads
    executeNow(collector, threads,
               "hwctime-rt: hwctime_start_sampling", arguments);
}



/**
 * Stops data collection.
 *
 * Implement stopping data collection for the specified threads.
 *
 * @param collector    Collector stopping data collection.
 * @param threads      Threads for which to stop collecting data.
 */
void HWTimeCollector::stopCollecting(const Collector& collector,
				     const ThreadGroup& threads) const
{
    // Execute hwctime_stop_sampling() in the threads
    executeNow(collector, threads,
               "hwctime-rt: hwctime_stop_sampling", Blob());

    // Remove instrumentation associated with this collector/threads pairing
    uninstrument(collector, threads);
}



/**
 * Get a metric value.
 *
 * Implement getting one of our metric values over all subextents
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
 *
 */
void HWTimeCollector::getMetricValues(const std::string& metric,
				      const Collector& collector,
				      const Thread& thread,
                                      const Extent& extent,
                                      const Blob& blob,
                                      const ExtentGroup& subextents,
				      void* ptr) const
{
    // Only "[inclusive|exclusive]_[overflows|detail]" metrics return anything
    if((metric != "inclusive_overflows") && (metric != "exclusive_overflows") &&
       (metric != "inclusive_detail") && (metric != "exclusive_detail"))
	return;

    // Is this "exclusive_[overflows|detail]"?
    bool is_exclusive = 
	(metric == "exclusive_overflows") || (metric == "exclusive_detail");

    // Is this "[inclusive|exclusive]_detail"?
    bool is_detail = 
	(metric == "inclusive_detail") || (metric == "exclusive_detail");

    // Check assertions
    if(is_detail) {
	Assert(reinterpret_cast<std::vector<SampleDetail>*>(ptr)->size() >=
	       subextents.size());
    } else {
	Assert(reinterpret_cast<std::vector<uint64_t>*>(ptr)->size() >=
	       subextents.size());
    }

    // Decode this data blob
    hwctime_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_hwctime_data), &data);
    
    // Check assertions
    Assert(data.bt.bt_len == data.count.count_len);

    // Calculate time (in nS) of data blob's extent
    double t_blob = static_cast<double>(extent.getTimeInterval().getWidth());

    // Iterate over each stack trace in the data blob    
    for(unsigned ib = 0, ie = 0; ie < data.bt.bt_len; ib = ie) {
	
	// Find the end of the current stack trace
	for(ie = ib + 1; ie < data.bt.bt_len; ++ie)
	     if(data.count.count_val[ie] > 0)
		   break;

	// Calculate the events attributable to this sample
	uint64_t e_sample = static_cast<uint64_t>(data.count.count_val[ib]) *
	     static_cast<uint64_t>(data.interval);	
	
	// Get the stack trace for this sample
	StackTrace trace(thread, extent.getTimeInterval().getBegin());
	for(unsigned j = ib; j < ie; ++j) {

	    if (data.bt.bt_val[j] == 0) {
		    // for some reason pthreaded calltrees have an extra
		    // frame with address of 0x0.  Do not pass these on to view
		    // code.  FIXME.  look at unwind code for the real cause...
		    continue;
	    }
	    trace.push_back(Address(data.bt.bt_val[j]));
	}
	
	// Iterate over each of the frames in the current stack trace
	for(StackTrace::const_iterator
		j = trace.begin(); j != trace.end(); ++j) {
	    
	    // Stop after first frame if this is "exclusive_[overflows|detail]"
	    if(is_exclusive && (j != trace.begin()))
		break;
	    
	    // Find the subextents that contain this frame
	    std::set<ExtentGroup::size_type> intersection =
		subextents.getIntersectionWith(
		    Extent(extent.getTimeInterval(), AddressRange(*j))
		    );
	    
	    // Iterate over each subextent in the intersection
	    for(std::set<ExtentGroup::size_type>::const_iterator
		    k = intersection.begin(); k != intersection.end(); ++k) {

		// Calculate intersection time (in nS) of subextent and blob
		double t_intersection = static_cast<double>
		    ((extent.getTimeInterval() &
		      subextents[*k].getTimeInterval()).getWidth());

		// Handle "[inclusive|exclusive]_detail" metric
		if(is_detail) {

		    // Find this stack trace in the subextent's metric value
		    SampleDetail::iterator l =
			(*reinterpret_cast<std::vector<SampleDetail>*>(ptr))[*k]
			.insert(
			    std::make_pair(trace, HWTimeDetail())
			    ).first;
		    
		    // Add (to the subextent's metric value) the appropriate
		    // fraction of the count and events attributable to this
		    // sample
		    l->second.dm_count += static_cast<uint64_t>(
		         static_cast<double>(data.count.count_val[ib]) * 
		         (t_intersection / t_blob)
		         );
		    
		    l->second.dm_events += static_cast<uint64_t>(
			static_cast<double>(e_sample) *
			(t_intersection / t_blob)
			);
		    
		}

		// Handle "[inclusive|exclusive]_overflows" metric		
		else {

		    // Add (to the subextent's metric value) the appropriate
		    // fraction of the events attributable to this sample
		    (*reinterpret_cast<std::vector<uint64_t>*>(ptr))[*k] +=
			static_cast<uint64_t>(
			    static_cast<double>(e_sample) * 
			    (t_intersection / t_blob)
			    );

		}
		
	    }
	    
	}
	
    }
	
    // Free the decoded data blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_hwctime_data),
             reinterpret_cast<char*>(&data));
}



void HWTimeCollector::getUniquePCValues( const Thread& thread,
					 const Blob& blob,
					 PCBuffer *buffer) const
{
    // Decode this data blob
    hwctime_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_hwctime_data), &data);

    if (data.bt.bt_len == 0) {
	// todo
    }

    // Iterate over each stack trace in the data blob
    for(unsigned i = 0; i < data.bt.bt_len; ++i) {
	if (data.bt.bt_val[i] != 0) {
	    UpdatePCBuffer(data.bt.bt_val[i], buffer);
	}
    }
 
    // Free the decoded data blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_hwctime_data),
             reinterpret_cast<char*>(&data));
}

void HWTimeCollector::getUniquePCValues( const Thread& thread,
                                         const Blob& blob,
                                         std::set<Address>& uaddresses) const
{

    // Decode this data blob
    hwctime_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_hwctime_data), &data);

    if (data.bt.bt_len == 0) {
	// todo
    }

    // Iterate over each stack trace in the data blob
    for(unsigned i = 0; i < data.bt.bt_len; ++i) {
	if (data.bt.bt_val[i] != 0) {
	    uaddresses.insert(Address(data.bt.bt_val[i]));
	}
    }

    // Free the decoded data blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_hwctime_data),
             reinterpret_cast<char*>(&data));
}
