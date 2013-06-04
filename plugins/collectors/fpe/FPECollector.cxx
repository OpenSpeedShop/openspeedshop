////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
// Copyright (c) 2008-2012 The Krell Institute. All Rights Reserved.
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
 * Definition of the FPECollector class.
 *
 */
 
#include "FPECollector.hxx"
#include "FPEDetail.hxx"
#include "blobs.h"
#include <fenv.h>

using namespace OpenSpeedShop::Framework;



namespace {

    /** Type returned for the FPE detail metrics. */
    typedef std::map<StackTrace, std::vector<FPEDetail> > ExceptionDetails;
    
    

    /**
     * Traceable fpe table.
     *
     * Table listing the traceable FPE's.
     *
     * @note   TODO. FIXME: This needs to be done.
     */

     #include "TraceableFPES.h"

}    



/**
 * Collector's factory method.
 *
 * Factory method for instantiating a collector implementation. This is the
 * only function that is externally visible from outside the collector plugin.
 *
 * @return    New instance of this collector's implementation.
 */
extern "C" CollectorImpl* fpe_LTX_CollectorFactory()
{
    return new FPECollector();
}



/**
 * Default constructor.
 *
 * Constructs a new FPE collector with the proper metadata.
 */
FPECollector::FPECollector() :
    CollectorImpl("fpe",
		"FPE Extended Event Tracing",
		  "Use this experiment when you experience high system time when executing the "
		  "relevant executable and/or the relevant executable contains a high"
                  " number of floating point operations."
                  "  The Floating Point Exception experiment, also known as, FPE Extended Event Tracing, "
		  "intercepts floating point exceptions "
		  "and records for each exception: the current stack "
		  "trace, the exception type, and start time."
		  )
{
    // Declare our parameters
    declareParameter(Metadata("event", "Traced FP Exceptions",
			      "Set of Floating Point Exceptions to be traced.",
			      typeid(std::map<std::string, bool>)));
    
    // Declare our metrics
    declareMetric(Metadata("total_count",
			   "Total Fpe Counts",
                           "Number of Floating Point Events.",
                           typeid(uint64_t)));

    declareMetric(Metadata("inexact_result_count",
			   "Inexact Result Count",
                           "Number of inexact results.",
                           typeid(uint64_t)));

    declareMetric(Metadata("division_by_zero_count",
			   "Division-By-Zero Count",
                           "Number of divisions by zero.",
                           typeid(uint64_t))); 

    declareMetric(Metadata("underflow_count",
			   "Underflow Count",
                           "Number of underflows.",
                           typeid(uint64_t))); 

    declareMetric(Metadata("overflow_count",
			   "Overflow Count",
                           "Number of Overflows.",
                           typeid(uint64_t))); 

    declareMetric(Metadata("invalid_count",
			   "Invalid Count",
                           "Number of Invalid.",
                           typeid(uint64_t))); 

    declareMetric(Metadata("unnormal_count",
			   "Unnormal Count",
                           "Number of Unnormal exceptions.",
                           typeid(uint64_t))); 

    declareMetric(Metadata("unknown_count",
			   "Unknown Count",
                           "Number of Unknown exceptions.",
                           typeid(uint64_t))); 

    declareMetric(Metadata("inclusive_details", "Inclusive Details",
			   "Inclusive floating-point exception details.",
			   typeid(ExceptionDetails)));

    declareMetric(Metadata("exclusive_details", "Exclusive Details",
			   "Exclusive floating-point exception details.",
			   typeid(ExceptionDetails)));
}



/**
 * Get the default parameter values.
 *
 * Implement getting our default parameter values.
 *
 * @return    Blob containing the default parameter values.
 */
Blob FPECollector::getDefaultParameterValues() const
{
    // Setup an empty parameter structure    
    fpe_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));

    // Set the default parameters
    for(unsigned i = 0; TraceableFPE[i] != NULL; ++i)
	parameters.traced[i] = true;
    
    // Return the encoded blob to the caller
    return Blob(reinterpret_cast<xdrproc_t>(xdr_fpe_parameters), &parameters);
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
void FPECollector::getParameterValue(const std::string& parameter,
				      const Blob& data, void* ptr) const
{
    // Decode the blob containing the parameter values
    fpe_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_fpe_parameters),
                        &parameters);

    // Handle the "event" parameter
    if(parameter == "event") {
	std::map<std::string, bool>* value =
	    reinterpret_cast<std::map<std::string, bool>*>(ptr);    
	bool All_Traced = true;
	for(unsigned i = 0; TraceableFPE[i] != NULL; ++i) {
	    if ((std::string("all") != TraceableFPE[i]) &&
	       (!parameters.traced[i])) {
		All_Traced = false;
		break;
	    }
	}
        if (All_Traced) value->insert(std::make_pair("all",true));
        for(unsigned i = 0; TraceableFPE[i] != NULL; ++i) {
	    value->insert(std::make_pair(TraceableFPE[i],
					  parameters.traced[i]));
	}
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
void FPECollector::setParameterValue(const std::string& parameter,
				      const void* ptr, Blob& data) const
{
    // Decode the blob containing the parameter values
    fpe_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_fpe_parameters),
                        &parameters);
    
    // Handle the "event" parameter
    if(parameter == "event") {
        std::string env_param;
	const std::map<std::string, bool>* value = 
	    reinterpret_cast<const std::map<std::string, bool>*>(ptr);
        if ((value->find("all") != value->end()) &&
             value->find("all")->second) {

            for(unsigned i = 0; TraceableFPE[i] != NULL; ++i) {
		parameters.traced[i] = true;
		env_param = env_param + TraceableFPE[i] + ":";
	    }
	  
	} else {
            for(unsigned i = 0; TraceableFPE[i] != NULL; ++i) {

		parameters.traced[i] =
		  (value->find(TraceableFPE[i]) != value->end()) &&
		  value->find(TraceableFPE[i])->second;

		if(parameters.traced[i]) {
		    env_param = env_param + TraceableFPE[i] + ":";
		}
            }
        }
	if (env_param.size() > 0) {
	    setenv("OPENSS_FPE_TRACED", (char *)env_param.c_str(), 1);	
	}
	
    }
    
    // Re-encode the blob containing the parameter values
    data = Blob(reinterpret_cast<xdrproc_t>(xdr_fpe_parameters), &parameters);
}



/**
 * Start data collection.
 *
 * Implement starting data collection for the specified threads.
 *
 * @param collector    Collector starting data collection.
 * @param threads      Threads for which to start collecting data.
 */
void FPECollector::startCollecting(const Collector& collector,
				   const ThreadGroup& threads) const
{
    // Get the set of traced fpes for this collector
    std::map<std::string, bool> traced;
    
    // Assemble and encode arguments to fpe_start_tracing()
    fpe_start_tracing_args args;
    memset(&args, 0, sizeof(args));
    collector.getParameterValue("event", traced);
    args.experiment = getExperimentId(collector);
    args.collector = getCollectorId(collector);
    Blob arguments(reinterpret_cast<xdrproc_t>(xdr_fpe_start_tracing_args),
                   &args);
    
    // Execute fpe_stop_tracing() before we exit the threads
    executeBeforeExit(collector, threads,
		      "fpe-rt: fpe_stop_tracing", Blob());

    // Execute fpe_enable_fpes() when we enter main() for the threads
    // FIXME: This is a workaround since the call to feenableexcept
    // in fpe_enable_fpes does not seem to keep fpe trapping enabled.
    // So we can not attach this collector to a process at this time.
#if 0
    executeAtEntry(collector, threads,
                   "main", "fpe-rt: fpe_enable_fpes", Blob());
#endif
    
    // Execute fpe_start_tracing() in the threads
    executeNow(collector, threads,
               "fpe-rt: fpe_start_tracing", arguments, true);

    // FPE trapping is enabled.
    executeNow(collector, threads,
               "fpe-rt: fpe_enable_fpes", Blob(), true);

}



/**
 * Stops data collection.
 *
 * Implement stopping data collection for the specified threads.
 *
 * @param collector    Collector stopping data collection.
 * @param threads      Threads for which to stop collecting data.
 */
void FPECollector::stopCollecting(const Collector& collector,
				  const ThreadGroup& threads) const
{
    // Execute fpe_stop_tracing() in the threads
    executeNow(collector, threads,
               "fpe-rt: fpe_stop_tracing", Blob());
    
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
void FPECollector::getMetricValues(const std::string& metric,
				   const Collector& collector,
				   const Thread& thread,
				   const Extent& extent,
				   const Blob& blob,
				   const ExtentGroup& subextents,
				   void* ptr) const
{
    // Don't return anything if an invalid metric was specified
    if((metric != "exclusive_details") &&
       (metric != "inclusive_details") &&
       (metric != "total_count") &&
       (metric != "inexact_result_count") &&
       (metric != "division_by_zero_count") &&
       (metric != "underflow_count") &&
       (metric != "overflow_count") &&
       (metric != "invalid_count") &&
       (metric != "unnormal_count") &&
       (metric != "unknown_count") ) {

	return;
    }

    // Determine which metric was specified
    bool is_details = ((metric == "inclusive_details") ||
		       (metric == "exclusive_details") );

    bool is_count = (metric == "total_count" ||
                     metric == "inexact_result_count" ||
		     metric == "division_by_zero_count" ||
		     metric == "underflow_count" ||
		     metric == "overflow_count" ||
		     metric == "invalid_count" ||
		     metric == "unnormal_count" ||
		     metric == "unknown_count" );
     
    // Check assertions
    if(is_details) {
	Assert(reinterpret_cast<std::vector<ExceptionDetails>*>(ptr)->size() >=
	       subextents.size());
    } else {
        Assert(reinterpret_cast<std::vector<uint64_t>*>(ptr)->size() >=
               subextents.size());
    }

    // Decode this data blob
    fpe_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_fpe_data), &data);
    
    // Iterate over each of the events
    for(unsigned i = 0; i < data.events.events_len; ++i) {

	// Get the time interval attributable to this event
	TimeInterval interval(Time(data.events.events_val[i].start_time));

	// Get the stack trace for this event
	StackTrace trace(thread, interval.getBegin());
	for(unsigned j = data.events.events_val[i].stacktrace;
	    data.stacktraces.stacktraces_val[j] != 0;
	    ++j)
	    trace.push_back(Address(data.stacktraces.stacktraces_val[j]));
	
	// Iterate over each of the frames in this event's stack trace
	for(StackTrace::const_iterator 
		j = trace.begin(); j != trace.end(); ++j) {

	    // Stop after the first frame if this is "exclusive" anything
            if((is_count || (metric == "exclusive_details") ) &&
               (j != trace.begin()))
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

		// Add this event to the results for this subextent
		if (is_details) {

		    // Find this event's stack trace in the results (or add it)
		    ExceptionDetails::iterator l =
			(*reinterpret_cast<std::vector<ExceptionDetails>*>(ptr))
			[*k].insert(
			    std::make_pair(trace, std::vector<FPEDetail>())
			    ).first;
		    
		    // Add this event's details structure to the results
		    FPEDetail details;
		    details.dm_time = Time(data.events.events_val[i].start_time);

		    switch(data.events.events_val[i].fpexception) {
    			case FPE_FE_INEXACT:
        		    details.dm_type = InexactResult;
			    break;
			case FPE_FE_UNDERFLOW:
        		    details.dm_type = Underflow;
			    break;
			case FPE_FE_OVERFLOW:
        		    details.dm_type = Overflow;
			    break;
			case FPE_FE_DIVBYZERO:
        		    details.dm_type = DivisionByZero;
			    break;
			case FPE_FE_UNNORMAL:
        		    details.dm_type = Unnormal;
			    break;
			case FPE_FE_INVALID:
        		    details.dm_type = Invalid;
			    break;
			case FPE_FE_UNKNOWN:
			default:
		            details.dm_type = Unknown;
			    break;
		    }

		    l->second.push_back(details);
		    
		}
                // Handle "*_count" metric
                else {

                    // Add (to the subextent's metric value) the appropriate
                    // fraction of the total time attributable to this sample

		    bool updatecount = false;
		    if (metric == "total_count")
			updatecount = true;
		    else switch(data.events.events_val[i].fpexception) {
    			case FPE_FE_INEXACT:
			    if (metric == "inexact_result_count")
				updatecount = true;
			    break;
			case FPE_FE_UNDERFLOW:
			    if (metric == "underflow_count")
				updatecount = true;
			    break;
			case FPE_FE_OVERFLOW:
			    if (metric == "overflow_count")
				updatecount = true;
			    break;
			case FPE_FE_DIVBYZERO:
			    if (metric == "division_by_zero_count")
				updatecount = true;
			    break;
			case FPE_FE_UNNORMAL:
			    if (metric == "unnormal_count")
				updatecount = true;
			    break;
			case FPE_FE_INVALID:
			    if (metric == "invalid_count")
				updatecount = true;
			    break;
			case FPE_FE_UNKNOWN:
			default:
			    if (metric == "unknown_count")
				updatecount = true;
			    break;
		    }

		    if (updatecount) {
		      (*reinterpret_cast<std::vector<uint64_t>*>(ptr))[*k] += 1;
		      updatecount = false;
		    }
                }
	    }
	    
	}

    }

    // Free the decoded data blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_fpe_data),
	     reinterpret_cast<char*>(&data));
}


void FPECollector::getUniquePCValues( const Thread& thread,
                                     const Blob& blob,
                                     PCBuffer *buffer) const
{

    // Decode this data blob
    fpe_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_fpe_data), &data);

    if (data.stacktraces.stacktraces_len == 0) {
    }

    // Iterate over each stack trace in the data blob
    for(unsigned i = 0; i < data.stacktraces.stacktraces_len; ++i) {
	if (data.stacktraces.stacktraces_val[i] != 0) {
	    UpdatePCBuffer(data.stacktraces.stacktraces_val[i], buffer);
	}
    }

    // Free the decoded data blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_fpe_data),
	     reinterpret_cast<char*>(&data));
}


void FPECollector::getUniquePCValues( const Thread& thread,
                                         const Blob& blob,
                                         std::set<Address>& uaddresses) const
{

    // Decode this data blob
    fpe_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_fpe_data), &data);

    if (data.stacktraces.stacktraces_len == 0) {
	// todo
    }

    // Iterate over each stack trace in the data blob
    for(unsigned i = 0; i < data.stacktraces.stacktraces_len; ++i) {
	if (data.stacktraces.stacktraces_val[i] != 0) {
	    uaddresses.insert(Address(data.stacktraces.stacktraces_val[i]));
	}
    }

    // Free the decoded data blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_fpe_data),
             reinterpret_cast<char*>(&data));
}
