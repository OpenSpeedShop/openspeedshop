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

#include "blobs.h" 
#include "ExampleCollector.hxx"

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
		  "Periodically interrupts the running thread, obtains the "
		  "current program counter (PC) value, increments a counter "
		  "associated with that PC value, and allows the thread to "
		  "continue execution.")
{
    // Declare our parameters
    declareParameter(Metadata("sampling_rate", "Sampling Rate",
                              "Sampling rate in samples/milliseconds.",
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
    memset(&parameters, 0, sizeof(example_parameters));
    
    // Set the default parameters
    parameters.sampling_rate = 10;
    
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
    memset(&parameters, 0, sizeof(example_parameters));
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
    memset(&parameters, 0, sizeof(example_parameters));
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
 * Implement starting data collection for a particular thread. ...
 *
 * @param collector    Collector starting data collection.
 * @param thread       Thread for which to start collecting data.
 */
void ExampleCollector::startCollecting(const Collector& collector,
				       const Thread& thread) const
{
    // Load our runtime library into the thread
    loadLibrary(thread, "example-rt");
}



/**
 * Stops data collection.
 *
 * Implement stopping data collection for a particular thread. ...
 *
 * @param collector    Collector stopping data collection.
 * @param thread       Thread for which to stop collecting data.
 */
void ExampleCollector::stopCollecting(const Collector& collector,
				      const Thread& thread) const
{
    // Unload our runtime library from the thread
    unloadLibrary(thread, "example-rt");
}



/**
 * Get a metric value.
 *
 * Implement getting one of our metric values for a particular thread, over a
 * specific address range and time interval. ...
 *
 * @param metric      Unique identifier of the metric.
 * @param thread      Thread for which to get a value.
 * @param range       Address range over which to get a value.
 * @param interval    Time interval over which to get a value.
 * @param ptr         Untyped pointer to the return value.
 */
void ExampleCollector::getMetricValue(const std::string& metric,
				      const Thread& thread,
				      const AddressRange& range,
				      const TimeInterval& interval,
				      void* ptr) const
{
    // Handle the "time" metric
    if(metric == "time") {
	double* value = reinterpret_cast<double*>(ptr);
	*value = 123.456;  // Dummy value for now
    }
}
