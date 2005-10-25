////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
 * Definition of the UserTimeCollector class.
 *
 */
 
#include "UserTimeCollector.hxx"
#include "blobs.h"

using namespace OpenSpeedShop::Framework;



/**
 * Collector's factory method.
 *
 * Factory method for instantiating a collector implementation. This is the
 * only function that is externally visible from outside the collector plugin.
 *
 * @return    New instance of this collector's implementation.
 */
extern "C" CollectorImpl* usertime_LTX_CollectorFactory()
{
    return new UserTimeCollector();
}



/**
 * Default constructor.
 *
 * Constructs a new user time  sampling collector with the proper metadata.
 */
UserTimeCollector::UserTimeCollector() :
    CollectorImpl("usertime",
                  "User Time",
                  "Periodically interrupts the running thread, obtains the "
                  "current program stack trace (addresses), stores them, and "
                  "allows the thread to continue execution.")
{
    // Declare our parameters
    declareParameter(Metadata("sampling_rate", "Sampling Rate",
                              "Sampling rate in samples/seconds.",
                              typeid(unsigned)));

    // Declare our metrics

    declareMetric(Metadata("inclusive_time", "Inclusive Time",
                           "Inclusive User time in seconds.",
                           typeid(double)));

    declareMetric(Metadata("exclusive_time", "Exclusive Time",
                           "Exclusive User time in seconds.",
                           typeid(double)));
}



/**
 * Get the default parameter values.
 *
 * Implement getting our default parameter values.
 *
 * @return    Blob containing the default parameter values.
 */
Blob UserTimeCollector::getDefaultParameterValues() const
{
    // Setup an empty parameter structure
    usertime_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));

    // Set the default parameters -- 30 millisecs == 1000/30 I use 35
    parameters.sampling_rate = 35;

    // Return the encoded blob to the caller
    return Blob(reinterpret_cast<xdrproc_t>(xdr_usertime_parameters),
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
void UserTimeCollector::getParameterValue(const std::string& parameter,
					const Blob& data, void* ptr) const
{
    // Decode the blob containing the parameter values
    usertime_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_usertime_parameters),
                        &parameters);

    // Handle the "sampling_rate" parameter
    if(parameter == "sampling_rate") {
        unsigned* value = reinterpret_cast<unsigned*>(ptr);
        *value = parameters.sampling_rate;
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
void UserTimeCollector::setParameterValue(const std::string& parameter,
					const void* ptr, Blob& data) const
{
    // Decode the blob containing the parameter values
    usertime_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_usertime_parameters),
                        &parameters);
    
    // Handle the "sampling_rate" parameter
    if(parameter == "sampling_rate") {
        const unsigned* value = reinterpret_cast<const unsigned*>(ptr);
        parameters.sampling_rate = *value;
    }
    
    // Re-encode the blob containing the parameter values
    data = Blob(reinterpret_cast<xdrproc_t>(xdr_usertime_parameters),
                &parameters);
}


/**
 * Start data collection.
 *
 * Implement starting data collection for a particular thread.
 *
 * @param collector    Collector starting data collection.
 * @param thread       Thread for which to start collecting data.
 */
void UserTimeCollector::startCollecting(const Collector& collector,
				      const Thread& thread) const
{
    // Assemble and encode arguments to usertime_start_sampling()
    usertime_start_sampling_args args;
    memset(&args, 0, sizeof(args));
    collector.getParameterValue("sampling_rate", args.sampling_rate);
    getECT(collector, thread, args.experiment, args.collector, args.thread);
    Blob arguments(reinterpret_cast<xdrproc_t>(xdr_usertime_start_sampling_args),
                   &args);

    // Execute usertime_stop_sampling() when we enter exit() for the thread
    executeAtEntry(collector, thread, 
		   "exit", "usertime-rt: usertime_stop_sampling", Blob());
    
    // Execute usertime_start_sampling() in the thread
    executeNow(collector, thread, 
	       "usertime-rt: usertime_start_sampling", arguments);

}



/**
 * Stops data collection.
 *
 * Implement stopping data collection for a particular thread.
 *
 * @param collector    Collector stopping data collection.
 * @param thread       Thread for which to stop collecting data.
 */
void UserTimeCollector::stopCollecting(const Collector& collector,
				     const Thread& thread) const
{
    // Execute usertime_stop_sampling() in the thread
    executeNow(collector, thread,
               "usertime-rt: usertime_stop_sampling", Blob());

    // Remove all instrumentation associated with this collector/thread pairing
    uninstrument(collector, thread);
}



/**
 * Get a metric value.
 *
 * Implement getting one of our metric values over all subextents
 * of the specified extent for a particular thread, for one of the collected
 * performance data blobs.
 *
 * @param metric       Unique identifier of the metric.
 * @param extent       Extent of the performance data blob.
 * @param blob         Blob containing the performance data.
 * @param subextents   Subextents for which to get values.
 * @param ptr          Untyped pointer to the return value.
 */
void UserTimeCollector::getMetricValues(const std::string& metric,
				        const Extent& extent,
					const Blob& blob,
					const ExtentGroup& subextents,
					void* ptr) const
{
    // Handle the inclusive_time and exclusive_time metrics.

    bool incltime = (metric == "inclusive_time") ? true : false;
    bool excltime = (metric == "exclusive_time") ? true : false;

    if( incltime || excltime ) {

	// Cast the untyped pointer into a vector of doubles
	std::vector<double>* values =
				reinterpret_cast<std::vector<double>*>(ptr);
	
	// Check assertions
	Assert(values->size() >= subextents.size());

	// Decode this data blob
	usertime_data data;
	memset(&data, 0, sizeof(data));
	blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_usertime_data),&data);
    
	// Check assertions
	Assert(data.bt.bt_len == data.count.count_len);

	// Calculate time (in nS) of data blob's extent
	double t_blob =
		static_cast<double>(extent.getTimeInterval().getWidth());

        // Iterate over each of the samples


	int mycount = 0;
	int currentcount = 0;
        for(unsigned i = 0; i < data.bt.bt_len; i++)
	{
		
	    // Find the subextents that contain this sample
	    std::set<ExtentGroup::size_type> intersection = 
	        subextents.getIntersectionWith(
		    Extent(extent.getTimeInterval(),
		       AddressRange(data.bt.bt_val[i])) );

	    // Calculate the time (in seconds) attributable to this sample
	    // If using stacks with counts, an address may appear in
	    // more than one stack.  Need to find count for each stack
	    // that the addr is in. How? Search each stack for addr?
	    // Or make count same size as buffer?

	    bool top_stack_trace = true;
	    bool add_to_values = true;

	    // A count_val > 0 indicates a valid stack in buffer bt.
	    if (data.count.count_val[i] > 0) {
		mycount = data.count.count_val[i];
		currentcount = mycount;
	    } else if (data.count.count_val[i] == 0) {
		top_stack_trace = false;
		currentcount = mycount;
	    }

	    double t_sample = 0.0;
	    if (currentcount > 0)
		t_sample = static_cast<double>(currentcount) *
			   static_cast<double>(data.interval) / 1000000000.0;
	    else
		continue;


	    // The boolean add_to_values is used to determine if we
	    // include the computed t_sample in values.
	    // incltime: always add each t_sample (stack frame) to values.
	    // excltime: only add t_sample for top stack frame to values.
	    if ( excltime) {

		if (top_stack_trace) {
		    // "first" PC of call stack, toggle add_to_values to true
		    // and toggle top_stack_trace to false till next new stack.
		    add_to_values = true;
		    top_stack_trace = false;
		} else {
		    // t_sample for this stack frame not added to values.
		    add_to_values = false;
		}
	    }

	    // add_to_values always true for incltime...
	    if (add_to_values) {
	        // Iterate over each subextent in the intersection
	        for(std::set<ExtentGroup::size_type>::const_iterator
		    j = intersection.begin(); j != intersection.end(); ++j) {
	    
	            // Calculate intersection time (in nS) of subextent and
		    // data blob
	            double t_intersection = static_cast<double>
		        ((extent.getTimeInterval() & 
		          subextents[*j].getTimeInterval()).getWidth());	    

	            // Add (to the subextent's metric value) the appropriate
		    // fraction of the total time attributable to this sample
	            (*values)[*j] += t_sample * (t_intersection / t_blob);
	    
	        } // end for subextent
	    }

	} // end for samples
	    
        // Free the decoded data blob
        xdr_free(reinterpret_cast<xdrproc_t>(xdr_usertime_data),
                 reinterpret_cast<char*>(&data));
    }
}
