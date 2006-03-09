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
 * Definition of the IOCollector class.
 *
 */
 
#include "IOCollector.hxx"
#include "IODetail.hxx"
#include "blobs.h"

using namespace OpenSpeedShop::Framework;



namespace {


    
    /** Type returned for the IO call time metrics. */
    typedef std::map<StackTrace, std::vector<double> > CallTimes;

    /** Type returned for the IO call detail metrics. */
    typedef std::map<StackTrace, std::vector<IODetail> > CallDetails;
    
    

    /**
     * Traceable function table.
     *
     * Table listing the traceable IO functions. In order for an IO function
     * to actually be traceable, corresponding wrapper(s) must first be written
     * and compiled into this collector's runtime.
     *
     * @note    A function's index position in this table is directly used as
     *          its index position in the io_parameters.traced array. Thus
     *          the order the functions are listed here is significant. If it
     *          is changed, users will find that any saved databases suddenly
     *          trace different IO functions than they did previously.
     */
    const char* TraceableFunctions[] = {

        "dup",
        "dup2",
        "creat",
        "open",
        "close",
        "read",
        "write",
        "pipe",
        "lseek",
        "pread",
        "pwrite",
        "readv",
        "writev",
        "open64",
        "lseek64",
        "creat64",
        "pread64",
        "pwrite64",
	
	// End Of Table Entry
	NULL
    };


    
}    



/**
 * Collector's factory method.
 *
 * Factory method for instantiating a collector implementation. This is the
 * only function that is externally visible from outside the collector plugin.
 *
 * @return    New instance of this collector's implementation.
 */
extern "C" CollectorImpl* io_LTX_CollectorFactory()
{
    return new IOCollector();
}



/**
 * Default constructor.
 *
 * Constructs a new IO collector with the proper metadata.
 */
IOCollector::IOCollector() :
    CollectorImpl("io",
                  "IO Extended Event Tracing",
		  "Intercepts all calls to IO functions that perform any "
		  "significant amount of work (primarily those that send "
		  "messages) and records, for each call, the current stack "
		  "trace and start/end time. Detailed ancillary data is also "
		  "stored such as the source and destination rank, number of "
		  "bytes sent, message tag, communicator used, and message "
		  "data type.")
{
    // Declare our parameters
    declareParameter(Metadata("traced_functions", "Traced Functions",
			      "Set of IO functions to be traced.",
			      typeid(std::map<std::string, bool>)));
    
    // Declare our metrics
    declareMetric(Metadata("inclusive_times", "Inclusive Times",
			   "Inclusive IO call times in seconds.",
			   typeid(CallTimes)));
    declareMetric(Metadata("exclusive_times", "Exclusive Times",
			   "Exclusive IO call times in seconds.",
			   typeid(CallTimes)));
    declareMetric(Metadata("inclusive_details", "Inclusive Details",
			   "Inclusive IO call details.",
			   typeid(CallDetails)));
    declareMetric(Metadata("exclusive_details", "Exclusive Details",
			   "Exclusive IO call details.",
			   typeid(CallDetails)));
}



/**
 * Get the default parameter values.
 *
 * Implement getting our default parameter values.
 *
 * @return    Blob containing the default parameter values.
 */
Blob IOCollector::getDefaultParameterValues() const
{
    // Setup an empty parameter structure    
    io_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));

    // Set the default parameters
    for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i)
	parameters.traced[i] = true;
    
    // Return the encoded blob to the caller
    return Blob(reinterpret_cast<xdrproc_t>(xdr_io_parameters), &parameters);
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
void IOCollector::getParameterValue(const std::string& parameter,
				      const Blob& data, void* ptr) const
{
    // Decode the blob containing the parameter values
    io_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_io_parameters),
                        &parameters);

    // Handle the "traced_functions" parameter
    if(parameter == "traced_functions") {
	std::map<std::string, bool>* value =
	    reinterpret_cast<std::map<std::string, bool>*>(ptr);    
        for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i)
	    value->insert(std::make_pair(TraceableFunctions[i],
					  parameters.traced[i]));
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
void IOCollector::setParameterValue(const std::string& parameter,
				      const void* ptr, Blob& data) const
{
    // Decode the blob containing the parameter values
    io_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_io_parameters),
                        &parameters);
    
    // Handle the "traced_functions" parameter
    if(parameter == "traced_functions") {
	const std::map<std::string, bool>* value = 
	    reinterpret_cast<const std::map<std::string, bool>*>(ptr);
	for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i)
	    parameters.traced[i] =
		(value->find(TraceableFunctions[i]) != value->end()) &&
		value->find(TraceableFunctions[i])->second;
    }
    
    // Re-encode the blob containing the parameter values
    data = Blob(reinterpret_cast<xdrproc_t>(xdr_io_parameters), &parameters);
}



/**
 * Start data collection.
 *
 * Implement starting data collection for a particular thread.
 *
 * @param collector    Collector starting data collection.
 * @param thread       Thread for which to start collecting data.
 */
void IOCollector::startCollecting(const Collector& collector,
				    const Thread& thread) const
{
    // Get the set of traced functions for this collector
    std::map<std::string, bool> traced;
    collector.getParameterValue("traced_functions", traced);
    
    // Assemble and encode arguments to io_start_tracing()
    io_start_tracing_args args;
    memset(&args, 0, sizeof(args));
    getECT(collector, thread, args.experiment, args.collector, args.thread);
    Blob arguments(reinterpret_cast<xdrproc_t>(xdr_io_start_tracing_args),
                   &args);
    
    // Execute io_stop_tracing() when we enter exit() for the thread
    executeAtEntry(collector, thread,
                   "exit", "io-rt: io_stop_tracing", Blob());
    
    // Execute io_start_tracing() in the thread
    executeNow(collector, thread,
               "io-rt: io_start_tracing", arguments);

    // Execute our wrappers in place of the real IO functions
    for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i)	
	if((traced.find(TraceableFunctions[i]) != traced.end()) &&
	   traced.find(TraceableFunctions[i])->second) {
	    // Wrap the IO function
	    // What if the traceable function is not found???
	    executeInPlaceOf(
		collector, thread, 
		TraceableFunctions[i],
		std::string("io-rt: io") + TraceableFunctions[i]
		);
	}
}



/**
 * Stops data collection.
 *
 * Implement stopping data collection for a particular thread.
 *
 * @param collector    Collector stopping data collection.
 * @param thread       Thread for which to stop collecting data.
 */
void IOCollector::stopCollecting(const Collector& collector,
				   const Thread& thread) const
{
    // Execute io_stop_tracing() in the thread
    executeNow(collector, thread,
               "io-rt: io_stop_tracing", Blob());
    
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

void IOCollector::getMetricValues(const std::string& metric,
				    const Collector& collector,
				    const Thread& thread,
				    const Extent& extent,
				    const Blob& blob,
				    const ExtentGroup& subextents,
				    void* ptr) const
{
    // Only the "inclusive_times" and "exclusive_times" metrics return anything
    if((metric != "inclusive_times") && (metric != "exclusive_times") &&
       (metric != "inclusive_details") && (metric != "exclusive_details"))
	return;
    bool is_exclusive = (metric == "exclusive_times"     ||
                         metric == "exclusive_details"   );

    bool is_details =   (metric == "inclusive_details"   ||
                         metric == "exclusive_details"   );

    // Cast the untype pointer into a vector of call details
    std::vector<CallDetails>* dvalues =
            reinterpret_cast<std::vector<CallDetails>*>(ptr);
    // Cast the untype pointer into a vector of call times
    std::vector<CallTimes>* tvalues = 
	reinterpret_cast<std::vector<CallTimes>*>(ptr);
    
    // Check assertions
    if (is_details) {
        Assert(dvalues->size() >= subextents.size());
    } else {
        Assert(tvalues->size() >= subextents.size());
    }

    // Decode this data blob
    io_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_io_data), &data);
    
    // Iterate over each of the events
    for(unsigned i = 0; i < data.events.events_len; ++i) {

	// Get the time interval attributable to this event
	TimeInterval interval(Time(data.events.events_val[i].start_time),
			      Time(data.events.events_val[i].stop_time));

	// Get the stack trace for this event
	StackTrace trace(thread, interval.getBegin());
	for(unsigned j = data.events.events_val[i].stacktrace;
	    data.stacktraces.stacktraces_val[j] != 0;
	    ++j) {
	    trace.push_back(Address(data.stacktraces.stacktraces_val[j]));

	}
	
	// Iterate over each of the frames in this event's stack trace
	for(StackTrace::const_iterator 
		j = trace.begin(); j != trace.end(); ++j) {

	    // Stop after the first frame if this is "exclusive_times"
	    if(is_exclusive && (j != trace.begin()))
		break;
	    
	    // Find the subextents that contain this frame
	    std::set<ExtentGroup::size_type> intersection =
		subextents.getIntersectionWith(
		    Extent(interval, AddressRange(*j))
		    );
	    

	    // Iterate over each subextent in the intersection
	    for(std::set<ExtentGroup::size_type>::const_iterator
		    k = intersection.begin(); k != intersection.end(); ++k) {
		
		// Calculate intersection time (in nS) of subextent and event
		double t_intersection = static_cast<double>
		    ((interval & subextents[*k].getTimeInterval()).getWidth());

		//
		// Add this event's stack trace to the results for this
		// subextent (or find an existing stack trace)
		//
		
                if (is_details) {
                    // metric is for details
                    CallDetails::iterator l = (*dvalues)[*k].insert(
                        std::make_pair(trace, std::vector<IODetail>())
                        ).first;

                    // Add this event's time (in seconds) to the results
                    IODetail details;
                    details.dm_interval = interval;
                    details.dm_time = t_intersection / 1000000000.0;

                    l->second.push_back(details);
                } else {
                    // metric is for times
                    CallTimes::iterator l = (*tvalues)[*k].insert(
                        std::make_pair(trace, std::vector<double>())
                        ).first;

                    // Add this event's time (in seconds) to the results
                    l->second.push_back(t_intersection / 1000000000.0);
                }
		
	    }
	    
	}

    }

    // Free the decoded data blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_io_data),
	     reinterpret_cast<char*>(&data));
}
