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
    // Load our runtime library into the thread
    loadLibrary(thread, "usertime-rt");
    
    // Assemble and encode arguments to usertime_start_sampling()
    usertime_start_sampling_args args;
    memset(&args, 0, sizeof(args));
    collector.getParameterValue("sampling_rate", args.sampling_rate);
    getECT(collector, thread, args.experiment, args.collector, args.thread);
    Blob arguments(reinterpret_cast<xdrproc_t>(xdr_usertime_start_sampling_args),
                   &args);
    
    // Execute usertime_start_sampling() in the thread
    execute(thread, "usertime-rt", "usertime_start_sampling", arguments);
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
    // WDH: Don't actually remove anything until a new "resource management"
    //      architecture is in place.

    // Execute usertime_stop_sampling() in the thread
    // execute(thread, "usertime-rt", "usertime_stop_sampling", Blob());
    
    // Unload our runtime library from the thread
    // unloadLibrary(thread, "usertime-rt");
}



/**
 * Get a metric value.
 *
 * Implement getting one of our metric values for a particular thread, over a
 * specific address range and time interval.
 *
 * @param metric       Unique identifier of the metric.
 * @param collector    Collector for which to get a value.
 * @param thread       Thread for which to get a value.
 * @param range        Address range over which to get a value.
 * @param interval     Time interval over which to get a value.
 * @param ptr          Untyped pointer to the return value.
 */
void UserTimeCollector::getMetricValue(const std::string& metric,
				     const Collector& collector,
				     const Thread& thread,
				     const AddressRange& range,
				     const TimeInterval& interval,
				     void* ptr) const
{
    // Handle the "time" inclusive time metric
    // FIXME: need ui support to pass inclusive_time and exclusive_time
    if( metric == "inclusive_time" ||
       metric == "exclusive_time" ) {
        double* value = reinterpret_cast<double*>(ptr);

        // Initialize the time metric value to zero
        *value = 0.0;
	
        // Obtain all the data blobs applicable to the requested metric value
        std::vector<Blob> data_blobs =
            getData(collector, thread, range, interval);
	
        // Iterate over each of the data blobs
	bool top_stack_trace = true;
        for(std::vector<Blob>::const_iterator
                i = data_blobs.begin(); i != data_blobs.end(); ++i) {

            // Decode this data blob
            usertime_data data;
            memset(&data, 0, sizeof(data));
            i->getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_usertime_data),
                              &data);
	    
            // Check assertions ???

            // Iterate over each of the samples

            for(unsigned j = 0; j < data.bt.bt_len; j++)
	    {
		
    // FIXME: need ui support to pass inclusive_time and exclusive_time
		if( metric == "inclusive_time" ) {
                    // Is this PC address inside the range?
                    if(range.doesContain(data.bt.bt_val[j])) {
		    
                        // Add this sample time to the time metric value
                        *value +=
			    static_cast<double>(data.interval) /
							1000000000.0;
		    }
		} else if ( metric == "exclusive_time") {
    // FIXME: need ui support to pass inclusive_time and exclusive_time
		    // Loop over each call stack in this BLOB
		    // if "first" PC of call stack inside of requested metric
		    // address range exclusive_time += sample time

		    if (top_stack_trace) {
			if (range.doesContain(data.bt.bt_val[j])) {
                            *value +=
			        static_cast<double>(data.interval) /
							1000000000.0;
			}
			top_stack_trace = false;
		    }

		    if ( !top_stack_trace && data.bt.bt_val[j] == 0) {
		        top_stack_trace = true;
		    }
		}
	    }
	    
            // Free the decoded data blob
            xdr_free(reinterpret_cast<xdrproc_t>(xdr_usertime_data),
                     reinterpret_cast<char*>(&data));
        }
    }
}
