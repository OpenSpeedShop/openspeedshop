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
 * Definition of the PCSampCollector class.
 *
 */
 
#include "PCSampCollector.hxx"
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
extern "C" CollectorImpl* pcsamp_LTX_CollectorFactory()
{
    return new PCSampCollector();
}



/**
 * Default constructor.
 *
 * Constructs a new PC sampling collector with the proper metadata.
 */
PCSampCollector::PCSampCollector() :
    CollectorImpl("pcsamp",
                  "PC Sampling",
                  "Periodically interrupts the running thread, obtains the "
                  "current program counter (PC) value, stores it, and allows "
                  "the thread to continue execution.")
{
    // Declare our parameters
    declareParameter(Metadata("sampling_rate", "Sampling Rate",
                              "Sampling rate in samples/seconds.",
                              typeid(unsigned)));

    // Declare our metrics
    declareMetric(Metadata("time", "CPU Time",
                           "Exclusive CPU time in seconds.",
                           typeid(double)));
}



/**
 * Get the default parameter values.
 *
 * Implement getting our default parameter values.
 *
 * @return    Blob containing the default parameter values.
 */
Blob PCSampCollector::getDefaultParameterValues() const
{
    // Setup an empty parameter structure
    pcsamp_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));

    // Set the default parameters
    parameters.sampling_rate = 100;

    // Return the encoded blob to the caller
    return Blob(reinterpret_cast<xdrproc_t>(xdr_pcsamp_parameters),
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
void PCSampCollector::getParameterValue(const std::string& parameter,
					const Blob& data, void* ptr) const
{
    // Decode the blob containing the parameter values
    pcsamp_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_pcsamp_parameters),
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
void PCSampCollector::setParameterValue(const std::string& parameter,
					const void* ptr, Blob& data) const
{
    // Decode the blob containing the parameter values
    pcsamp_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_pcsamp_parameters),
                        &parameters);
    
    // Handle the "sampling_rate" parameter
    if(parameter == "sampling_rate") {
        const unsigned* value = reinterpret_cast<const unsigned*>(ptr);
        parameters.sampling_rate = *value;
    }
    
    // Re-encode the blob containing the parameter values
    data = Blob(reinterpret_cast<xdrproc_t>(xdr_pcsamp_parameters),
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
void PCSampCollector::startCollecting(const Collector& collector,
				      const Thread& thread) const
{
    // Load our runtime library into the thread
    loadLibrary(thread, "pcsamp-rt");
    
    // Assemble and encode arguments to pcsamp_start_sampling()
    pcsamp_start_sampling_args args;
    memset(&args, 0, sizeof(args));
    collector.getParameterValue("sampling_rate", args.sampling_rate);
    getECT(collector, thread, args.experiment, args.collector, args.thread);
    Blob arguments(reinterpret_cast<xdrproc_t>(xdr_pcsamp_start_sampling_args),
                   &args);
    
    // Execute pcsamp_start_sampling() in the thread
    execute(thread, "pcsamp-rt", "pcsamp_start_sampling", arguments);
}



/**
 * Stops data collection.
 *
 * Implement stopping data collection for a particular thread.
 *
 * @param collector    Collector stopping data collection.
 * @param thread       Thread for which to stop collecting data.
 */
void PCSampCollector::stopCollecting(const Collector& collector,
				     const Thread& thread) const
{
    // WDH: Don't actually remove anything until a new "resource management"
    //      architecture is in place.

    // Execute pcsamp_stop_sampling() in the thread
    // execute(thread, "pcsamp-rt", "pcsamp_stop_sampling", Blob());
    
    // Unload our runtime library from the thread
    // unloadLibrary(thread, "pcsamp-rt");
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
void PCSampCollector::getMetricValue(const std::string& metric,
				     const Collector& collector,
				     const Thread& thread,
				     const AddressRange& range,
				     const TimeInterval& interval,
				     void* ptr) const
{
    // Handle the "time" metric
    if(metric == "time") {
        double* value = reinterpret_cast<double*>(ptr);

        // Initialize the time metric value to zero
        *value = 0.0;
	
        // Obtain all the data blobs applicable to the requested metric value
        std::vector<Blob> data_blobs =
            getData(collector, thread, range, interval);
	
        // Iterate over each of the data blobs
        for(std::vector<Blob>::const_iterator
                i = data_blobs.begin(); i != data_blobs.end(); ++i) {

            // Decode this data blob
            pcsamp_data data;
            memset(&data, 0, sizeof(data));
            i->getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_pcsamp_data),
                              &data);
	    
            // Check assertions
            Assert(data.pc.pc_len == data.count.count_len);

            // Iterate over each of the samples
            for(unsigned j = 0; j < data.pc.pc_len; ++j)
		
                // Is this PC address inside the range?
                if(range.doesContain(data.pc.pc_val[j]))
		    
                    // Add this sample count's time to the time metric value
                    *value += static_cast<double>(data.count.count_val[j]) *
                        static_cast<double>(data.interval) /
                        1000000000.0;
	    
            // Free the decoded data blob
            xdr_free(reinterpret_cast<xdrproc_t>(xdr_pcsamp_data),
                     reinterpret_cast<char*>(&data));
	    
        }

    }
}
