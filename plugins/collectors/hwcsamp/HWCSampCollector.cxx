////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010-2014 The Krell Institute. All Rights Reserved.
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
 * Definition of the HWCSampCollector class.
 *
 */
/* #define DEBUG_CLI 1 */
 
#include "HWCSampCollector.hxx"

#include "PCBuffer.hxx"

#include "blobs.h"
#include "HWCSampDetail.hxx"
#include "HWCSampEvents.h"

#include <sstream>

using namespace OpenSpeedShop::Framework;


/**
 * Collector's factory method.
 *
 * Factory method for instantiating a collector implementation. This is the
 * only function that is externally visible from outside the collector plugin.
 *
 * @return    New instance of this collector's implementation.
 */

namespace {

    /** Type returned for the sample detail metrics. */
    typedef std::map<StackTrace, std::vector<HWCSampDetail> > SampleDetail;

}

extern "C" CollectorImpl* hwcsamp_LTX_CollectorFactory()
{
    return new HWCSampCollector();
}



/**
 * Default constructor.
 *
 * Constructs a new PC sampling collector with the proper metadata.
 */
HWCSampCollector::HWCSampCollector() :
    CollectorImpl("hwcsamp",
                  "HWC Sampling",
                  "Periodically interrupts the running thread, obtains the "
                  "current program counter (PC) value, stores it and the current "
		  "counts for the selected hwc events and allows the thread "
		  "to continue execution.")
		
{
    // Declare our parameters
    declareParameter(Metadata("sampling_rate", "Sampling Rate",
                              "Sampling rate in samples/seconds.",
                              typeid(unsigned)));

    declareParameter(Metadata("event", "Hardware Counter Events",
                           "HWCSAMP event.",
                           typeid(std::string)));
    // Declare our metrics
    declareMetric(Metadata("time", "CPU Time",
                           "Exclusive CPU time in seconds.",
                           typeid(double)));

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
Blob HWCSampCollector::getDefaultParameterValues() const
{
    // Setup an empty parameter structure
    hwcsamp_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));

    // Set the default parameters
    parameters.sampling_rate = 100;
    strncpy(parameters.hwcsamp_event,"PAPI_TOT_CYC,PAPI_TOT_INS",strlen("PAPI_TOT_CYC,PAPI_TOT_INS"));

    // Return the encoded blob to the caller
    return Blob(reinterpret_cast<xdrproc_t>(xdr_hwcsamp_parameters),
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
void HWCSampCollector::getParameterValue(const std::string& parameter,
					const Blob& data, void* ptr) const
{
    // Decode the blob containing the parameter values
    hwcsamp_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_hwcsamp_parameters),
                        &parameters);

    // Handle the "sampling_rate" parameter
    if(parameter == "sampling_rate") {
        unsigned* value = reinterpret_cast<unsigned*>(ptr);
        *value = parameters.sampling_rate;
    }
 
    if(parameter == "event") {
	std::string* value = reinterpret_cast<std::string*>(ptr);
	std::string ecstr(parameters.hwcsamp_event);
	*value = ecstr;
    }

    // Free the decoded parameters blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_hwcsamp_parameters),
	     reinterpret_cast<char*>(&parameters));
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
void HWCSampCollector::setParameterValue(const std::string& parameter,
					const void* ptr, Blob& data) const
{
    // Decode the blob containing the parameter values
    hwcsamp_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_hwcsamp_parameters),
                        &parameters);
    
    // Handle the "sampling_rate" parameter
    if(parameter == "sampling_rate") {
        const unsigned* value = reinterpret_cast<const unsigned*>(ptr);
        parameters.sampling_rate = *value;
        std::ostringstream rate;
        rate << parameters.sampling_rate;
        setenv("OPENSS_HWCSAMP_RATE", rate.str().c_str(), 1);
    }
    
    if(parameter == "event") {
        const std::string* papi_event_name =
                                reinterpret_cast<const std::string*>(ptr);
	// clear any previous event preset
	memset(&parameters.hwcsamp_event, 0, sizeof(parameters.hwcsamp_event));
	// copy the noew event preset
	strncpy(parameters.hwcsamp_event,papi_event_name->c_str(),
		strlen(papi_event_name->c_str()));
	setenv("OPENSS_HWCSAMP_EVENTS", papi_event_name->c_str(), 1);
    }

    // Re-encode the blob containing the parameter values
    data = Blob(reinterpret_cast<xdrproc_t>(xdr_hwcsamp_parameters),
                &parameters);

    // Free the decoded parameters blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_hwcsamp_parameters),
	     reinterpret_cast<char*>(&parameters));
}



/**
 * Start data collection.
 *
 * Implement starting data collection for the specified threads.
 *
 * @param collector    Collector starting data collection.
 * @param threads      Threads for which to start collecting data.
 */
void HWCSampCollector::startCollecting(const Collector& collector,
				      const ThreadGroup& threads) const
{
    // Assemble and encode arguments to pcsamp_start_sampling()
    hwcsamp_start_sampling_args args;
    memset(&args, 0, sizeof(args));
    collector.getParameterValue("sampling_rate", args.sampling_rate);

    std::string papi_event_name;
    collector.getParameterValue("event", papi_event_name);
    strncpy(args.hwcsamp_event,papi_event_name.c_str(),strlen(papi_event_name.c_str()));

    args.experiment = getExperimentId(collector);
    args.collector = getCollectorId(collector);
    Blob arguments(reinterpret_cast<xdrproc_t>(xdr_hwcsamp_start_sampling_args),
                   &args);

    // Execute pcsamp_stop_sampling() before we exit the threads
    executeBeforeExit(collector, threads,
		      "hwcsamp-rt: hwcsamp_stop_sampling", Blob());
    
    // Execute pcsamp_start_sampling() in the threads
    executeNow(collector, threads, 
	       "hwcsamp-rt: hwcsamp_start_sampling", arguments);
}



/**
 * Stops data collection.
 *
 * Implement stopping data collection for the specified threads.
 *
 * @param collector    Collector stopping data collection.
 * @param threads      Threads for which to stop collecting data.
 */
void HWCSampCollector::stopCollecting(const Collector& collector,
				     const ThreadGroup& threads) const
{
    // Execute pcsamp_stop_sampling() in the threads
    executeNow(collector, threads, 
	       "hwcsamp-rt: hwcsamp_stop_sampling", Blob());
        
    // Remove instrumentation associated with this collector/threads pairing
    uninstrument(collector, threads);
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
void HWCSampCollector::getMetricValues(const std::string& metric,
				      const Collector& collector,
				      const Thread& thread,
				      const Extent& extent,
				      const Blob& blob,
				      const ExtentGroup& subextents,
				      void* ptr) const
{
    if((metric != "time") && (metric != "exclusive_detail"))
        return;

    bool is_detail = false;

    if(metric == "time") {
	// Cast the untyped pointer into a vector of doubles
	Assert(reinterpret_cast<std::vector<double>*>(ptr)->size() >=
		subextents.size());
    } else if (metric ==  "exclusive_detail") {
	is_detail = true;
	Assert(reinterpret_cast<std::vector<SampleDetail>*>(ptr)->size() >=
               subextents.size());
    }
    

    // Decode this data blob
    hwcsamp_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_hwcsamp_data), &data);
    
    // Check assertions
    Assert(data.pc.pc_len == data.count.count_len);

    // Calculate time (in nS) of data blob's extent
    double t_blob = static_cast<double>(extent.getTimeInterval().getWidth());

    // Iterate over each of the samples
    for(unsigned i = 0; i < data.pc.pc_len; ++i) {
	
	// Find the subextents that contain this sample
	std::set<ExtentGroup::size_type> intersection = 
	    subextents.getIntersectionWith(
		Extent(extent.getTimeInterval(),
		       AddressRange(data.pc.pc_val[i]))
		);
	
	// Calculate the time (in seconds) attributable to this sample
	double t_sample = static_cast<double>(data.count.count_val[i]) *
	    static_cast<double>(data.interval) / 1000000000.0;

	// Get the address for this sample.
	Address addr =  Address(data.pc.pc_val[i]);
	StackTrace trace(thread, extent.getTimeInterval().getBegin());
	trace.push_back(addr);

#if 0
	if (is_detail) {
	    for (int ii = 0; ii < 6; ii++) {
		if (data.events.events_val[i].hwccounts[ii] > 0) {
std::cerr << "HWCSAMP::getMetricValues pc "
	<< static_cast<int>(data.count.count_val[i])  << " "
	<< Address(data.pc.pc_val[i])
	<< " event " << ii << " value is " << data.events.events_val[i].hwccounts[ii]
	<< std::endl;
		}
	    }
	}
#endif

	// Iterate over each subextent in the intersection
	for(std::set<ExtentGroup::size_type>::const_iterator
		j = intersection.begin(); j != intersection.end(); ++j) {
	    
	    // Calculate intersection time (in nS) of subextent and data blob
	    double t_intersection = static_cast<double>
		((extent.getTimeInterval() & 
		  subextents[*j].getTimeInterval()).getWidth());	    

	    // Add (to the subextent's metric value) the appropriate fraction
	    // of the total time attributable to this sample
	    //(*values)[*j] += t_sample * (t_intersection / t_blob);

	    // Handle "[exclusive]_detail" metric
	    if(is_detail) {

	      // Find this address in the subextent's metric value
              SampleDetail::iterator l =
                        (*reinterpret_cast<std::vector<SampleDetail>*>(ptr))[*j]
                        .insert(
                            std::make_pair(trace, std::vector<HWCSampDetail>())
                            ).first;

	      // Add (to the subextent's metric value) the appropriate
	      // fraction of the count and total time attributable to
	      // this sample
              HWCSampDetail details;
	      for (int ii = 0; ii < OpenSS_NUMCOUNTERS; ii++) {
	        details.dm_event_values[ii] += static_cast<uint64_t>(
			static_cast<uint64_t>(data.events.events_val[i].hwccounts[ii]) * 
			(t_intersection / t_blob)
			);
              }
	      details.dm_count += 1;
	      details.dm_time += t_sample;// * (t_intersection / t_blob);
              l->second.push_back(details);
	    }

	    // Handle "[exclusive]_time" metric		
	    else {

	      // Add (to the subextent's metric value) the appropriate
	      // fraction of the total time attributable to this sample
	      (*reinterpret_cast<std::vector<double>*>(ptr))[*j] +=
			t_sample * (t_intersection / t_blob);
		    
	    }
		
	}
	
    }

    // Free the decoded data blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_hwcsamp_data),
	     reinterpret_cast<char*>(&data));
}


void HWCSampCollector::getUniquePCValues( const Thread& thread,
					 const Blob& blob,
					 PCBuffer *buffer) const
{
    // Decode this data blob
    hwcsamp_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_hwcsamp_data), &data);

    // Check assertions
    // Assert(data.pc.pc_len == data.count.count_len);
    if (data.pc.pc_len != data.count.count_len) {
        std::cerr << "ASSERT getPCValues pc_len "
	    << data.pc.pc_len
	    << " != count_len "
	    << data.count.count_len << std::endl;
    } else {
	// todo
    }

    // Iterate over each of the samples
    for(unsigned i = 0; i < data.pc.pc_len; ++i) {
        UpdatePCBuffer(data.pc.pc_val[i], buffer);
    }
    
    // Free the decoded data blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_hwcsamp_data),
	     reinterpret_cast<char*>(&data));
}

void HWCSampCollector::getUniquePCValues( const Thread& thread,
                                         const Blob& blob,
                                         std::set<Address>& uaddresses) const
{

    // Decode this data blob
    hwcsamp_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_hwcsamp_data), &data);

    // Check assertions
    // Assert(data.pc.pc_len == data.count.count_len);
    if (data.pc.pc_len != data.count.count_len) {
        std::cerr << "ASSERT getPCValues pc_len "
	    << data.pc.pc_len
	    << " != count_len "
	    << data.count.count_len << std::endl;
    } else {
	// todo
    }

    // Iterate over each stack trace in the data blob
    for(unsigned i = 0; i < data.pc.pc_len; ++i) {
	if (data.pc.pc_val[i] != 0) {
	    uaddresses.insert(Address(data.pc.pc_val[i]));
	}
    }

    // Free the decoded data blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_hwcsamp_data),
             reinterpret_cast<char*>(&data));
}
