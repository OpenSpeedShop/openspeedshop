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
 * Definition of the ExampleCollector class.
 *
 */

#include "ExampleCollector.hxx"
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
extern "C" CollectorImpl* example_LTX_CollectorFactory()
{
    return new ExampleCollector();
}



/**
 * Default constructor.
 *
 * Constructs a new example collector with the proper metadata.
 */
ExampleCollector::ExampleCollector() :
    CollectorImpl("example",
		  "Example",
		  "Simple example performance data collector illustrating the "
		  "basics of the Collector API. Periodically interrupts the "
		  "running thread, obtains the current program counter (PC) "
		  "value, stores it, and allows the thread to continue "
		  "execution.")
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
Blob ExampleCollector::getDefaultParameterValues() const
{
    // Setup an empty parameter structure
    example_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));

    // Set the default parameters
    parameters.sampling_rate = 100;

    // Return the encoded blob to the caller
    return Blob(reinterpret_cast<xdrproc_t>(xdr_example_parameters),
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
void ExampleCollector::getParameterValue(const std::string& parameter,
					 const Blob& data, void* ptr) const
{
    // Decode the blob containing the parameter values
    example_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_example_parameters),
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
void ExampleCollector::setParameterValue(const std::string& parameter,
					 const void* ptr, Blob& data) const
{
    // Decode the blob containing the parameter values
    example_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_example_parameters),
                        &parameters);

    // Handle the "sampling_rate" parameter
    if(parameter == "sampling_rate") {
        const unsigned* value = reinterpret_cast<const unsigned*>(ptr);
        parameters.sampling_rate = *value;
    }

    // Re-encode the blob containing the parameter values
    data = Blob(reinterpret_cast<xdrproc_t>(xdr_example_parameters),
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
void ExampleCollector::startCollecting(const Collector& collector,
				       const Thread& thread) const
{
    // Assemble and encode arguments to example_start_sampling()
    example_start_sampling_args args;
    memset(&args, 0, sizeof(args));
    collector.getParameterValue("sampling_rate", args.sampling_rate);
    getECT(collector, thread, args.experiment, args.collector, args.thread);
    Blob arguments(reinterpret_cast<xdrproc_t>(xdr_example_start_sampling_args),
                   &args);

    // Execute example_stop_sampling() before we exit the thread
    executeBeforeExit(collector, thread,
                      "example-rt: example_stop_sampling", Blob());

    // Execute example_start_sampling() in the thread
    executeNow(collector, thread,
               "example-rt: example_start_sampling", arguments);
}



/**
 * Stops data collection.
 *
 * Implement stopping data collection for a particular thread.
 *
 * @param collector    Collector stopping data collection.
 * @param thread       Thread for which to stop collecting data.
 */
void ExampleCollector::stopCollecting(const Collector& collector,
				      const Thread& thread) const
{
    // Execute example_stop_sampling() in the thread
    executeNow(collector, thread,
               "example-rt: example_stop_sampling", Blob());
    
    // Remove all instrumentation associated with this collector/thread pairing
    uninstrument(collector, thread);
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
void ExampleCollector::getMetricValues(const std::string& metric,
				       const Collector& collector,
				       const Thread& thread,
				       const Extent& extent,
				       const Blob& blob,
				       const ExtentGroup& subextents,
				       void* ptr) const
{
    // Only the "time" metric returns anything
    if(metric != "time")
	return;
    
    // Cast the untyped pointer into a vector of doubles
    std::vector<double>* values = reinterpret_cast<std::vector<double>*>(ptr);
    
    // Check assertions
    Assert(values->size() >= subextents.size());
    
    // Decode this data blob
    example_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_example_data), &data);
    
    // Calculate the time (in seconds) attributable to each sample
    double t_sample =
	static_cast<double>(data.interval) / 1000000000.0;
    
    // Calculate time (in nS) of data blob's extent
    double t_blob = 
	static_cast<double>(extent.getTimeInterval().getWidth());
    
    // Iterate over each of the samples
    for(unsigned i = 0; i < data.pc.pc_len; ++i) {
	
	// Find the subextents that contain this sample
	std::set<ExtentGroup::size_type> intersection = 
	    subextents.getIntersectionWith(
		Extent(extent.getTimeInterval(),
		       AddressRange(data.pc.pc_val[i]))
		);
	
	// Iterate over each subextent in the intersection
	for(std::set<ExtentGroup::size_type>::const_iterator
		j = intersection.begin(); j != intersection.end(); ++j) {
	    
	    // Calculate intersection time (in nS) of subextent and data blob
	    double t_intersection = static_cast<double>
		((extent.getTimeInterval() & 
		  subextents[*j].getTimeInterval()).getWidth());	    
	    
	    // Add (to the subextent's metric value) the appropriate fraction
	    // of the total time attributable to this sample
	    (*values)[*j] += t_sample * (t_intersection / t_blob);
	    
	}
	
    }
    
    // Free the decoded data blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_example_data),
	     reinterpret_cast<char*>(&data));
}
