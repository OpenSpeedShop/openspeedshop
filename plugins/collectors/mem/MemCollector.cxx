////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012-2014 The Krell Institute. All Rights Reserved.
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
 * Definition of the MemCollector class.
 *
 */
 
#include "MemCollector.hxx"
#include "MemDetail.hxx"

#include "KrellInstitute/Messages/Mem.h"
#include "KrellInstitute/Messages/Mem_data.h"

using namespace OpenSpeedShop::Framework;



namespace {


    
    /** Type returned for the Mem call time metrics. */
    typedef std::map<StackTrace, std::vector<double> > CallTimes;

    /** Type returned for the Mem call detail metrics. */
    typedef std::map<StackTrace, std::vector<MemDetail> > CallDetails;
    
    

    /**
     * Traceable function table.
     *
     * Table listing the traceable Mem functions. In order for an Mem function
     * to actually be traceable, corresponding wrapper(s) must first be written
     * and compiled into this collector's runtime.
     *
     * @note    A function's index position in this table is directly used as
     *          its index position in the CBTF_mem_parameters.traced array. Thus
     *          the order the functions are listed here is significant. If it
     *          is changed, users will find that any saved databases suddenly
     *          trace different Mem functions than they did previously.
     */
    #include "MemTraceableFunctions.h"

    
}    



/**
 * Collector's factory method.
 *
 * Factory method for instantiating a collector implementation. This is the
 * only function that is externally visible from outside the collector plugin.
 *
 * @return    New instance of this collector's implementation.
 */
extern "C" CollectorImpl* mem_LTX_CollectorFactory()
{
    return new MemCollector();
}



/**
 * Default constructor.
 *
 * Constructs a new Mem collector with the proper metadata.
 */
MemCollector::MemCollector() :
    CollectorImpl("mem",
                  "Mem Event Tracing",
		  "Intercepts all calls to Mem functions. "
		  "Records for each call, details of call, "
		  "a stacktrace and the start/end time.")
{
    // Declare our parameters
    declareParameter(Metadata("traced_functions", "Traced Functions",
			      "Set of Mem functions to be traced.",
			      typeid(std::map<std::string, bool>)));
    
    // Declare our metrics
    declareMetric(Metadata("unique_inclusive_details", "Unique Call Path Inclusive Details",
                          "Unique Call Path Inclusive Mem call details.",
                          typeid(CallDetails)));
    declareMetric(Metadata("highwater_inclusive_details", "HighWater Call Path Inclusive Details",
                          "HighWater Call Path Inclusive Mem call details.",
                          typeid(CallDetails)));
    declareMetric(Metadata("leaked_inclusive_details", "Memory Leak Call Path Inclusive Details",
                          "Memory Leak Call Path Inclusive Mem call details.",
                          typeid(CallDetails)));
    declareMetric(Metadata("time", "Mem Call Time",
			   "Exclusive Mem call time in seconds.",
			   typeid(double)));
    declareMetric(Metadata("inclusive_times", "Inclusive Times",
			   "Inclusive Mem call times in seconds.",
			   typeid(CallTimes)));
    declareMetric(Metadata("exclusive_times", "Exclusive Times",
			   "Exclusive Mem call times in seconds.",
			   typeid(CallTimes)));
    declareMetric(Metadata("inclusive_details", "Inclusive Details",
			   "Inclusive Mem call details.",
			   typeid(CallDetails)));
    declareMetric(Metadata("exclusive_details", "Exclusive Details",
			   "Exclusive Mem call details.",
			   typeid(CallDetails)));
    declareMetric(Metadata("min", "Minimum Time",
			   "Mininum call times in seconds.",
			   typeid(double)));
    declareMetric(Metadata("max", "Maximum Time",
			   "Maximum call times in seconds.",
			   typeid(double)));
    declareMetric(Metadata("average", "Average Time",
			   "Average call times in seconds.",
			   typeid(double)));
    declareMetric(Metadata("threadAverage", "Thread Average Time",
			   "Average Exclusive Mem call times in seconds across threads or ranks.",
			   typeid(double)));
    declareMetric(Metadata("threadMin", "Thread Minimum Time",
			   "Minimum Exclusive Mem call times in seconds across threads or ranks.",
			   typeid(double)));
    declareMetric(Metadata("threadMax", "Thread Maximum Time",
			   "Maximum Exclusive Mem call times in seconds across threads or ranks.",
			   typeid(double)));
    declareMetric(Metadata("stddev", "Standard Deviation Time",
			   "Standard Deviation call times in seconds.",
			   typeid(double)));
    declareMetric(Metadata("count", "Number of Calls",
			   "Number of calls to this function.",
			   typeid(uint64_t)));
    declareMetric(Metadata("size1", "Value of size_t (1)",
			   "Value of the first or only size_t argument."
			   " Only meaningfull for malloc,calloc,realloc,memalign,posix_memalign.",
			   typeid(uint64_t)));
    declareMetric(Metadata("size2", "Value of size_t (2)",
			   "Value of the second size_t argument."
			   " Only meaningfull for calloc,memalign,posix_memalign.",
			   typeid(uint64_t)));
    declareMetric(Metadata("ptr", "Value of ptr",
			   "Value of the ptr argument."
			   " Only meaningfull for realloc,free,posix_memalign.",
			   typeid(uint64_t)));
    declareMetric(Metadata("retval", "Call dependent return value",
			   "Call dependent return value of this memory call."
			   " A pointer to memory is returned by malloc,calloc,realloc,memalign."
			   " The free call returns no value.",
			   typeid(uint64_t)));
}



/**
 * Get the default parameter values.
 *
 * Implement getting our default parameter values.
 *
 * @return    Blob containing the default parameter values.
 */
Blob MemCollector::getDefaultParameterValues() const
{
    // Setup an empty parameter structure    
    CBTF_mem_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));

    // Set the default parameters
    for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i)
	parameters.traced[i] = true;
    
    // Return the encoded blob to the caller
    return Blob(reinterpret_cast<xdrproc_t>(xdr_CBTF_mem_parameters), &parameters);
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
void MemCollector::getParameterValue(const std::string& parameter,
				    const Blob& data, void* ptr) const
{
    // Decode the blob containing the parameter values
    CBTF_mem_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_mem_parameters),
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
void MemCollector::setParameterValue(const std::string& parameter,
				    const void* ptr, Blob& data) const
{
    // Decode the blob containing the parameter values
    CBTF_mem_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_mem_parameters),
                        &parameters);
    
    // Handle the "traced_functions" parameter
    if(parameter == "traced_functions") {
	std::string env_param;
	const std::map<std::string, bool>* value = 
	    reinterpret_cast<const std::map<std::string, bool>*>(ptr);
	for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i) {
	    parameters.traced[i] =
		(value->find(TraceableFunctions[i]) != value->end()) &&
		value->find(TraceableFunctions[i])->second;

            if(parameters.traced[i]) {
		env_param = env_param + TraceableFunctions[i] + ",";
            }
        }
        if (env_param.size() > 0) {
            setenv("CBTF_MEM_TRACED", (char *)env_param.c_str(), 1);
        }
    }
    
    // Re-encode the blob containing the parameter values
    data = Blob(reinterpret_cast<xdrproc_t>(xdr_CBTF_mem_parameters), &parameters);
}



/**
 * Start data collection.
 *
 * Implement starting data collection for the specified threads.
 *
 * @param collector    Collector starting data collection.
 * @param threads      Threads for which to start collecting data.
 */
void MemCollector::startCollecting(const Collector& collector,
				  const ThreadGroup& threads) const
{
#if 0
    // Get the set of traced functions for this collector
    std::map<std::string, bool> traced;
    collector.getParameterValue("traced_functions", traced);
    
    // Assemble and encode arguments to mem_start_tracing()
    mem_start_tracing_args args;
    memset(&args, 0, sizeof(args));
    args.experiment = getExperimentId(collector);
    args.collector = getCollectorId(collector);
    Blob arguments(reinterpret_cast<xdrproc_t>(xdr_mem_start_tracing_args),
                   &args);
    
    // Execute mem_stop_tracing() before we exit the threads
    executeBeforeExit(collector, threads,
		      "io-rt: mem_stop_tracing", Blob());
    
    // Execute mem_start_tracing() in the threads
    executeNow(collector, threads,
               "io-rt: mem_start_tracing", arguments);

    // Execute our wrappers in place of the real Mem functions
    for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i)	
	if((traced.find(TraceableFunctions[i]) != traced.end()) &&
	   traced.find(TraceableFunctions[i])->second) {
	    // Wrap the Mem function
	    // What if the traceable function is not found???
	    executeInPlaceOf(
		collector, threads, 
		TraceableFunctions[i],
		std::string("io-rt: io") + TraceableFunctions[i]
		);
	}
#endif
}



/**
 * Stops data collection.
 *
 * Implement stopping data collection for the specified threads.
 *
 * @param collector    Collector stopping data collection.
 * @param threads      Threads for which to stop collecting data.
 */
void MemCollector::stopCollecting(const Collector& collector,
				 const ThreadGroup& threads) const
{
#if 0
    // Execute mem_stop_tracing() in the threads
    executeNow(collector, threads,
               "io-rt: mem_stop_tracing", Blob());
    
    // Remove instrumentation associated with this collector/threads pairing
    uninstrument(collector, threads);
#endif
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

void MemCollector::getMetricValues(const std::string& metric,
				  const Collector& collector,
				  const Thread& thread,
				  const Extent& extent,
				  const Blob& blob,
				  const ExtentGroup& subextents,
				  void* ptr) const
{
    // Determine which metric was specified
    bool is_time = (metric == "time");
    bool is_inclusive_details = (metric == "inclusive_details");
    bool is_unique_inclusive_details = (metric == "unique_inclusive_details");
    bool is_leaked_inclusive_details = (metric == "leaked_inclusive_details");
    bool is_highwater_inclusive_details = (metric == "highwater_inclusive_details");
    bool is_details = true;
    CBTF_mem_reason view_reason = CBTF_MEM_REASON_UNIQUE_CALLPATH;

    bool debug_metrics = false;
    if (getenv("OPENSS_DEBUG_MEM_METRICS") != NULL) {
	debug_metrics = true;
    }

    // Decode this data blob
    CBTF_mem_exttrace_data data;
    memset(&data, 0, sizeof(data));
    CBTF_mem_trace_data olddata;
    memset(&olddata, 0, sizeof(olddata));
    unsigned datasize = blob.getAndVerifyXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_mem_exttrace_data), &data);

    bool is_old_mem_data = false;
    if (datasize == 0) {
	datasize = blob.getAndVerifyXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_mem_trace_data), &olddata);
	if (datasize == 0) {
	    std::cerr << "xdr_CBTF_mem_trace_data failed datasize:" << datasize << std::endl;
	    return;
	} else {
	    is_old_mem_data = true;
	}
    }

    // Don't return anything if an invalid metric was specified
    if (is_old_mem_data) {
	if(!is_time && !is_inclusive_details) {
	    return;
	}
	if(is_time) {
	    Assert(reinterpret_cast<std::vector<double>*>(ptr)->size() >=
	       subextents.size());
	}
	if(is_inclusive_details) {
	    Assert(reinterpret_cast<std::vector<CallDetails>*>(ptr)->size() >=
	       subextents.size()); 
	}
    } else {
	if(!is_time && !is_inclusive_details && !is_unique_inclusive_details &&
	   !is_leaked_inclusive_details && !is_highwater_inclusive_details) {
	    return;
	}
	if(is_time) {
	    Assert(reinterpret_cast<std::vector<double>*>(ptr)->size() >=
	       subextents.size());
	} else {
	    if(is_inclusive_details) {
	    view_reason = CBTF_MEM_REASON_UNIQUE_CALLPATH;
	    } else if (is_unique_inclusive_details) {
	    view_reason = CBTF_MEM_REASON_UNIQUE_CALLPATH;
	    } else if (is_leaked_inclusive_details) {
	    view_reason = CBTF_MEM_REASON_STILLALLOCATED;
	    } else if (is_highwater_inclusive_details) {
	    view_reason = CBTF_MEM_REASON_HIGHWATER_SET;
	    }
	    Assert(reinterpret_cast<std::vector<CallDetails>*>(ptr)->size() >=
	       subextents.size()); 
	}
    }
     
    // HANDLE PREVIOUS mem data from old collector.
    if (is_old_mem_data) {
      // Iterate over each of the events
      for(unsigned i = 0; i < olddata.events.events_len; ++i) {
	Time start(olddata.events.events_val[i].start_time);
	Time stop(olddata.events.events_val[i].stop_time);
	if (start == stop || start >= stop) {
	    stop = start + 1;
	}
	TimeInterval interval(start, stop);
	StackTrace trace(thread, interval.getBegin());
        if( olddata.events.events_val[i].stacktrace < olddata.stacktraces.stacktraces_len) {
	  for(unsigned j = olddata.events.events_val[i].stacktrace;
	      olddata.stacktraces.stacktraces_val[j] != 0; ++j) {
	    trace.push_back(Address(olddata.stacktraces.stacktraces_val[j]));
	  }
	}
	// Iterate over each of the frames in this event's stack trace
	for(StackTrace::const_iterator 
		j = trace.begin(); j != trace.end(); ++j) {
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
		if (is_details) {
		    // Find this event's stack trace in the results (or add it)
		    CallDetails::iterator l =
			(*reinterpret_cast<std::vector<CallDetails>*>(ptr))
			[*k].insert(
			    std::make_pair(trace, std::vector<MemDetail>())
			    ).first;
		    // Add this event's details structure to the results
		    MemDetail details;

		    // The dm_id detail is used to display the pid or rank and
		    // thread id of a -v trace event.
                    std::pair<bool, int> prank = thread.getMPIRank();
                    pid_t processID = thread.getProcessId();
                    if (prank.first) {
                       details.dm_id.first = prank.second;
                    } else {
                       details.dm_id.first = processID;
                    }

		    // Prefer simple int thread id.
                    details.dm_id.second = 0;
		    std::pair<bool, int> threadID = thread.getOpenMPThreadId();
                    if ( threadID.first ) {
                       details.dm_id.second = threadID.second;
                    }

		    details.dm_time = t_intersection / 1000000000.0;
		    details.dm_memtype = olddata.events.events_val[i].mem_type;
		    details.dm_interval = interval;
		    details.dm_count = data.events.events_val[i].count;
		    details.dm_reason = CBTF_MEM_REASON_UNKNOWN;
		    details.dm_max = 0;
		    details.dm_min = 0;
		    details.dm_total_allocation = 0;
		    switch (olddata.events.events_val[i].mem_type) {

			case CBTF_MEM_MALLOC: {
			    details.dm_retval = olddata.events.events_val[i].retval;
			    details.dm_ptr = olddata.events.events_val[i].ptr;
			    details.dm_size1 = olddata.events.events_val[i].size1;
			    details.dm_size2 = 0;
			    l->second.push_back(details);
			break;
			}

			case CBTF_MEM_CALLOC: {
			    details.dm_retval = olddata.events.events_val[i].retval;
			    details.dm_ptr = 0;
			    details.dm_size1 = olddata.events.events_val[i].size1;
			    details.dm_size2 = olddata.events.events_val[i].size2;
			    l->second.push_back(details);
			break;
			}

			case CBTF_MEM_REALLOC: {
			    details.dm_retval = olddata.events.events_val[i].retval;
			    details.dm_ptr = olddata.events.events_val[i].ptr;
			    details.dm_size1 = olddata.events.events_val[i].size1;
			    details.dm_size2 = 0;
			    l->second.push_back(details);
			break;
			}

			case CBTF_MEM_FREE: {
			    details.dm_retval = olddata.events.events_val[i].retval;
			    details.dm_ptr = olddata.events.events_val[i].ptr;
			    details.dm_size1 = 0;
			    details.dm_size2 = 0;
			    l->second.push_back(details);
			break;
			}

			case CBTF_MEM_MEMALIGN: {
			    details.dm_retval = olddata.events.events_val[i].retval;
			    details.dm_ptr = 0;
			    details.dm_size1 = olddata.events.events_val[i].size1;
			    details.dm_size2 = olddata.events.events_val[i].size2;
			    l->second.push_back(details);
			break;
			}

			case CBTF_MEM_POSIX_MEMALIGN: {
			    details.dm_retval = olddata.events.events_val[i].retval;
			    details.dm_ptr = olddata.events.events_val[i].ptr;
			    details.dm_size1 = olddata.events.events_val[i].size1;
			    details.dm_size2 = olddata.events.events_val[i].size2;
			    l->second.push_back(details);
			break;
			}

			case CBTF_MEM_UNKNOWN: {
			    details.dm_retval = olddata.events.events_val[i].retval;
			    details.dm_ptr = 0;
			    details.dm_size1 = 0;
			    details.dm_size2 = 0;
			    l->second.push_back(details);
			break;
			}

			default: {
			    details.dm_retval = olddata.events.events_val[i].retval;
			    details.dm_ptr = 0;
			    details.dm_size1 = 0;
			    details.dm_size2 = 0;
			    l->second.push_back(details);
			break;
			}
		    }
		} // if is_details
	    } // for intersection
	} // for trace
#if 0
std::cerr << "OLD MEM EVENT: mem_type:" << olddata.events.events_val[i].mem_type
	<< " retval:" << Address(olddata.events.events_val[i].retval)
	<< " ptr:" << Address(olddata.events.events_val[i].ptr)
	<< " size1:" << olddata.events.events_val[i].size1
	<< " size2:" << olddata.events.events_val[i].size2
	<< std::endl;
#endif
      } // for events.

    // HANDLE REDUCED mem data from new collector.
    } else {
      // Iterate over each of the events
      for(unsigned i = 0; i < data.events.events_len; ++i) {
	Time start(data.events.events_val[i].start_time);
	Time stop(data.events.events_val[i].stop_time);
	if (start == stop || start >= stop) {
	    stop = start + 1;
	}
	TimeInterval interval(start, stop);
	StackTrace trace(thread, interval.getBegin());

        // ignore all events not marked with view_reason.
        if (data.events.events_val[i].reason == view_reason) {
	  for(unsigned j = data.events.events_val[i].stacktrace;
	    data.stacktraces.stacktraces_val[j] != 0;
	    ++j) {
	    trace.push_back(Address(data.stacktraces.stacktraces_val[j]));
	  }
        }
	
	// Iterate over each of the frames in this event's stack trace
	for(StackTrace::const_iterator 
		j = trace.begin(); j != trace.end(); ++j) {

            // Stop after the first frame if this is "exclusive" anything
            if((is_time /*|| is_exclusive_times || is_exclusive_details*/) &&
                 (j != trace.begin())) {
                break;
	    }

	    bool is_metric_frame  = (j == trace.begin());
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

                if(is_time) {

                    (*reinterpret_cast<std::vector<double>*>(ptr))[*k] +=
                        t_intersection / 1000000000.0;

                } else if (is_details) {
		    // Find this event's stack trace in the results (or add it)
		    CallDetails::iterator l =
			(*reinterpret_cast<std::vector<CallDetails>*>(ptr))
			[*k].insert(
			    std::make_pair(trace, std::vector<MemDetail>())
			    ).first;
		    
		    // Add this event's details structure to the results
		    MemDetail details;
		    memset(&details, 0, sizeof(MemDetail));

                    details.dm_id.second = 0;
                    std::pair<bool, int> prank = thread.getMPIRank();
                    pid_t processID = thread.getProcessId();
                    if (prank.first) {
                       details.dm_id.first = prank.second;
                    } else {
                       details.dm_id.first = processID;
                    }
		    std::pair<bool, int> threadID = thread.getOpenMPThreadId();
                    if ( threadID.first ) {
                       details.dm_id.second = threadID.second;
                    }

		    details.dm_time = t_intersection / 1000000000.0;
		    details.dm_interval = interval;
		    details.dm_count = data.events.events_val[i].count;

		   if (is_metric_frame) {
		    details.dm_memtype = data.events.events_val[i].mem_type;
		    details.dm_reason = data.events.events_val[i].reason;
		    details.dm_max = data.events.events_val[i].max;
		    details.dm_min = data.events.events_val[i].min;

		    switch (data.events.events_val[i].mem_type) {

			case CBTF_MEM_MALLOC: {
			    details.dm_retval = data.events.events_val[i].retval;
			    details.dm_ptr = data.events.events_val[i].ptr;
			    details.dm_size1 = data.events.events_val[i].size1;
			    details.dm_size2 = 0;
		            details.dm_total_allocation = data.events.events_val[i].total_allocation;
			    l->second.push_back(details);
			break;
			}

			case CBTF_MEM_CALLOC: {
			    details.dm_retval = data.events.events_val[i].retval;
			    details.dm_ptr = 0;
			    details.dm_size1 = data.events.events_val[i].size1;
			    details.dm_size2 = data.events.events_val[i].size2;
		            details.dm_total_allocation = data.events.events_val[i].total_allocation;
			    l->second.push_back(details);
			break;
			}

			case CBTF_MEM_REALLOC: {
			    details.dm_retval = data.events.events_val[i].retval;
			    details.dm_ptr = data.events.events_val[i].ptr;
			    details.dm_size1 = data.events.events_val[i].size1;
			    details.dm_size2 = 0;
		            details.dm_total_allocation = data.events.events_val[i].total_allocation;
			    l->second.push_back(details);
			break;
			}

			case CBTF_MEM_FREE: {
			    details.dm_retval = data.events.events_val[i].retval;
			    details.dm_ptr = data.events.events_val[i].ptr;
			    details.dm_size1 = 0;
			    details.dm_size2 = 0;
		            details.dm_total_allocation = 0;
			    l->second.push_back(details);
			break;
			}

			case CBTF_MEM_MEMALIGN: {
			    details.dm_retval = data.events.events_val[i].retval;
			    details.dm_ptr = 0;
			    details.dm_size1 = data.events.events_val[i].size1;
			    details.dm_size2 = data.events.events_val[i].size2;
			    l->second.push_back(details);
			break;
			}

			case CBTF_MEM_POSIX_MEMALIGN: {
			    details.dm_retval = data.events.events_val[i].retval;
			    details.dm_ptr = data.events.events_val[i].ptr;
			    details.dm_size1 = data.events.events_val[i].size1;
			    details.dm_size2 = data.events.events_val[i].size2;
			    l->second.push_back(details);
			break;
			}

			case CBTF_MEM_UNKNOWN: {
			    details.dm_retval = data.events.events_val[i].retval;
			    details.dm_ptr = 0;
			    details.dm_size1 = 0;
			    details.dm_size2 = 0;
			    l->second.push_back(details);
			break;
			}

			default: {
			    details.dm_retval = data.events.events_val[i].retval;
			    details.dm_ptr = 0;
			    details.dm_size1 = 0;
			    details.dm_size2 = 0;
			    l->second.push_back(details);
			break;
			}
		    }
		  } // is is_metric_frame

#if defined(DEBUG_DETAILS)
std::cerr << "MEM EVENT: mem_type:" << data.events.events_val[i].mem_type
	<< " reason:" << data.events.events_val[i].reason
	<< " retval:" << Address(data.events.events_val[i].retval)
	<< " ptr:" << Address(data.events.events_val[i].ptr)
	<< " size1:" << data.events.events_val[i].size1
	<< " size2:" << data.events.events_val[i].size2
	<< " max_alloc:" << data.events.events_val[i].max
	<< " min_alloc:" << data.events.events_val[i].min
	<< " tot_alloc:" << data.events.events_val[i].total_allocation
	<< " path_cnt:" << data.events.events_val[i].count
	<< std::endl;
#endif
		    
		} // if is_details
	    } // for intersection
	} // for trace
      } // for event
    } // else is_old_mem_data

    // Free the decoded data blob
    if (is_old_mem_data) {
	xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_mem_trace_data),
	     reinterpret_cast<char*>(&olddata));
    } else {
	xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_mem_exttrace_data),
	     reinterpret_cast<char*>(&data));
    }
}


void MemCollector::getUniquePCValues( const Thread& thread,
                                     const Blob& blob,
                                     PCBuffer *buffer) const
{
// noop
}

void MemCollector::getUniquePCValues( const Thread& thread,
                                         const Blob& blob,
                                         std::set<Address>& uaddresses) const
{

    // Decode this data blob
    CBTF_mem_exttrace_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_mem_exttrace_data), &data);

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
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_mem_exttrace_data),
             reinterpret_cast<char*>(&data));
}
