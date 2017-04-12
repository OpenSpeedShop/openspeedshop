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
 * Definition of the PthreadsCollector class.
 *
 */
 
#include "PthreadsCollector.hxx"
#include "PthreadsDetail.hxx"

#include "KrellInstitute/Messages/Pthreads.h"
#include "KrellInstitute/Messages/Pthreads_data.h"

using namespace OpenSpeedShop::Framework;



namespace {


    
    /** Type returned for the Pthreads call time metrics. */
    typedef std::map<StackTrace, std::vector<double> > CallTimes;

    /** Type returned for the Pthreads call detail metrics. */
    typedef std::map<StackTrace, std::vector<PthreadsDetail> > CallDetails;
    
    

    /**
     * Traceable function table.
     *
     * Table listing the traceable Pthreads functions. In order for an Pthreads function
     * to actually be traceable, corresponding wrapper(s) must first be written
     * and compiled into this collector's runtime.
     *
     * @note    A function's index position in this table is directly used as
     *          its index position in the CBTF_pthreads_parameters.traced array. Thus
     *          the order the functions are listed here is significant. If it
     *          is changed, users will find that any saved databases suddenly
     *          trace different Pthreads functions than they did previously.
     */
    #include "PthreadsTraceableFunctions.h"

    
}    



/**
 * Collector's factory method.
 *
 * Factory method for instantiating a collector implementation. This is the
 * only function that is externally visible from outside the collector plugin.
 *
 * @return    New instance of this collector's implementation.
 */
extern "C" CollectorImpl* pthreads_LTX_CollectorFactory()
{
    return new PthreadsCollector();
}



/**
 * Default constructor.
 *
 * Constructs a new Pthreads collector with the proper metadata.
 */
PthreadsCollector::PthreadsCollector() :
    CollectorImpl("pthreads",
                  "Pthreads Event Tracing",
		  "Intercepts all calls to Pthreads functions that perform any "
		  "significant amount of work and records for each call, "
		  "the current stack trace and start/end time.")
{
    // Declare our parameters
    declareParameter(Metadata("traced_functions", "Traced Functions",
			      "Set of Pthreads functions to be traced.",
			      typeid(std::map<std::string, bool>)));
    
    // Declare our metrics
    declareMetric(Metadata("time", "Pthreads Call Time",
			   "Exclusive Pthreads call time in seconds.",
			   typeid(double)));
    declareMetric(Metadata("inclusive_times", "Inclusive Times",
			   "Inclusive Pthreads call times in seconds.",
			   typeid(CallTimes)));
    declareMetric(Metadata("exclusive_times", "Exclusive Times",
			   "Exclusive Pthreads call times in seconds.",
			   typeid(CallTimes)));
    declareMetric(Metadata("inclusive_details", "Inclusive Details",
			   "Inclusive Pthreads call details.",
			   typeid(CallDetails)));
    declareMetric(Metadata("exclusive_details", "Exclusive Details",
			   "Exclusive Pthreads call details.",
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
			   "Average Exclusive Pthreads call times in seconds across threads or ranks.",
			   typeid(double)));
    declareMetric(Metadata("threadMin", "Thread Minimum Time",
			   "Minimum Exclusive Pthreads call times in seconds across threads or ranks.",
			   typeid(double)));
    declareMetric(Metadata("threadMax", "Thread Maximum Time",
			   "Maximum Exclusive Pthreads call times in seconds across threads or ranks.",
			   typeid(double)));
    declareMetric(Metadata("stddev", "Standard Deviation Time",
			   "Standard Deviation call times in seconds.",
			   typeid(double)));
    declareMetric(Metadata("count", "Number of Calls",
			   "Number of calls to this function.",
			   typeid(uint64_t)));
}



/**
 * Get the default parameter values.
 *
 * Implement getting our default parameter values.
 *
 * @return    Blob containing the default parameter values.
 */
Blob PthreadsCollector::getDefaultParameterValues() const
{
    // Setup an empty parameter structure    
    CBTF_pthreads_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));

    // Set the default parameters
    for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i)
	parameters.traced[i] = true;
    
    // Return the encoded blob to the caller
    return Blob(reinterpret_cast<xdrproc_t>(xdr_CBTF_pthreads_parameters), &parameters);
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
void PthreadsCollector::getParameterValue(const std::string& parameter,
				    const Blob& data, void* ptr) const
{
    // Decode the blob containing the parameter values
    CBTF_pthreads_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_pthreads_parameters),
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
void PthreadsCollector::setParameterValue(const std::string& parameter,
				    const void* ptr, Blob& data) const
{
    // Decode the blob containing the parameter values
    CBTF_pthreads_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_pthreads_parameters),
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
            setenv("CBTF_PTHREADS_TRACED", (char *)env_param.c_str(), 1);
        }
    }
    
    // Re-encode the blob containing the parameter values
    data = Blob(reinterpret_cast<xdrproc_t>(xdr_CBTF_pthreads_parameters), &parameters);
}



/**
 * Start data collection.
 *
 * Implement starting data collection for the specified threads.
 *
 * @param collector    Collector starting data collection.
 * @param threads      Threads for which to start collecting data.
 */
void PthreadsCollector::startCollecting(const Collector& collector,
				  const ThreadGroup& threads) const
{
#if 0
    // Get the set of traced functions for this collector
    std::map<std::string, bool> traced;
    collector.getParameterValue("traced_functions", traced);
    
    // Assemble and encode arguments to pthreads_start_tracing()
    pthreads_start_tracing_args args;
    memset(&args, 0, sizeof(args));
    args.experiment = getExperimentId(collector);
    args.collector = getCollectorId(collector);
    Blob arguments(reinterpret_cast<xdrproc_t>(xdr_pthreads_start_tracing_args),
                   &args);
    
    // Execute pthreads_stop_tracing() before we exit the threads
    executeBeforeExit(collector, threads,
		      "io-rt: pthreads_stop_tracing", Blob());
    
    // Execute pthreads_start_tracing() in the threads
    executeNow(collector, threads,
               "io-rt: pthreads_start_tracing", arguments);

    // Execute our wrappers in place of the real Pthreads functions
    for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i)	
	if((traced.find(TraceableFunctions[i]) != traced.end()) &&
	   traced.find(TraceableFunctions[i])->second) {
	    // Wrap the Pthreads function
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
void PthreadsCollector::stopCollecting(const Collector& collector,
				 const ThreadGroup& threads) const
{
#if 0
    // Execute pthreads_stop_tracing() in the threads
    executeNow(collector, threads,
               "io-rt: pthreads_stop_tracing", Blob());
    
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

void PthreadsCollector::getMetricValues(const std::string& metric,
				  const Collector& collector,
				  const Thread& thread,
				  const Extent& extent,
				  const Blob& blob,
				  const ExtentGroup& subextents,
				  void* ptr) const
{
    // Determine which metric was specified
    bool is_time = (metric == "time");
    bool is_inclusive_times = (metric == "inclusive_times");
    bool is_exclusive_times = (metric == "exclusive_times");
    bool is_inclusive_details = (metric == "inclusive_details");
    bool is_exclusive_details = (metric == "exclusive_details");

    // Don't return anything if an invalid metric was specified
    if(!is_time &&
       !is_inclusive_times && !is_exclusive_times &&
       !is_inclusive_details && !is_exclusive_details)
	return;
     
    // Check assertions
    if(is_time) {
	Assert(reinterpret_cast<std::vector<double>*>(ptr)->size() >=
	       subextents.size());
    }
    else if(is_inclusive_times || is_exclusive_times) {
	Assert(reinterpret_cast<std::vector<CallTimes>*>(ptr)->size() >=
	       subextents.size());
    }
    else if(is_inclusive_details || is_exclusive_details) {
	Assert(reinterpret_cast<std::vector<CallDetails>*>(ptr)->size() >=
	       subextents.size()); 
    }

    // Decode this data blob
    CBTF_pthreads_exttrace_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_pthreads_exttrace_data), &data);

    // Iterate over each of the events
    for(unsigned i = 0; i < data.events.events_len; ++i) {

#if 0
	// Get the time interval attributable to this event
	TimeInterval interval(Time(data.events.events_val[i].start_time),
			      Time(data.events.events_val[i].stop_time));
#else
        uint64_t start_time = data.events.events_val[i].start_time;
        uint64_t stop_time = data.events.events_val[i].stop_time;
	TimeInterval originterval(Time(data.events.events_val[i].start_time),
			      Time(data.events.events_val[i].stop_time));
#if 0
	std::cerr << "PthreadsCOLLECTOR: start_time " << start_time
		  << " stop_time " << stop_time << std::endl;
#endif
        if (start_time == stop_time) {
            stop_time = start_time + 1;
        } else if (start_time >= stop_time) {
            stop_time = start_time + 1;
        }
        Time start(start_time);
        Time stop(stop_time);
        TimeInterval interval(start, stop);
#endif


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

	    // Stop after the first frame if this is "exclusive" anything
	    if((is_time || is_exclusive_times || is_exclusive_details) &&
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
		if(is_time) {

		    // Add this event's time (in seconds) to the results
		    (*reinterpret_cast<std::vector<double>*>(ptr))[*k] +=
			t_intersection / 1000000000.0;
		    
		}
		else if(is_inclusive_times || is_exclusive_times) {

		    // Find this event's stack trace in the results (or add it)
		    CallTimes::iterator l =
			(*reinterpret_cast<std::vector<CallTimes>*>(ptr))
			[*k].insert(
			    std::make_pair(trace, std::vector<double>())
			    ).first;

		    // Add this event's time (in seconds) to the results
		    l->second.push_back(t_intersection / 1000000000.0);

		}
		else if(is_inclusive_details || is_exclusive_details) {

		    // Find this event's stack trace in the results (or add it)
		    CallDetails::iterator l =
			(*reinterpret_cast<std::vector<CallDetails>*>(ptr))
			[*k].insert(
			    std::make_pair(trace, std::vector<PthreadsDetail>())
			    ).first;
		    
		    // Add this event's details structure to the results
		    PthreadsDetail details;
		    details.dm_interval = interval;
		    details.dm_time = t_intersection / 1000000000.0;
		    details.dm_pthreadtype = data.events.events_val[i].pthread_type;

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

#if 1
		    switch (data.events.events_val[i].pthread_type) {

			case CBTF_PTHREAD_CREATE: {
			    details.dm_retval = data.events.events_val[i].retval;
			    details.dm_ptr1 = data.events.events_val[i].ptr1;
			    details.dm_ptr2 = data.events.events_val[i].ptr2;
			    details.dm_ptr3 = data.events.events_val[i].ptr3;
			    l->second.push_back(details);
			break;
			}

#if 0
			case CBTF_PTHREAD_JOIN: {
			    details.dm_retval = data.events.events_val[i].retval;
			    l->second.push_back(details);
			break;
			}

			case CBTF_PTHREAD_CANCEL: {
			    details.dm_retval = data.events.events_val[i].retval;
			    l->second.push_back(details);
			break;
			}
#endif

			case CBTF_PTHREAD_MUTEX_INIT: {
			    details.dm_retval = data.events.events_val[i].retval;
			    details.dm_ptr1 = data.events.events_val[i].ptr1;
			    details.dm_ptr2 = data.events.events_val[i].ptr2;
			    details.dm_ptr3 = 0;
			    l->second.push_back(details);
			break;
			}

			case CBTF_PTHREAD_MUTEX_DESTROY: {
			    details.dm_retval = data.events.events_val[i].retval;
			    details.dm_ptr1 = data.events.events_val[i].ptr1;
			    details.dm_ptr2 = 0;
			    details.dm_ptr3 = 0;
			    l->second.push_back(details);
			break;
			}
			case CBTF_PTHREAD_MUTEX_LOCK: {
			    details.dm_retval = data.events.events_val[i].retval;
			    details.dm_ptr1 = data.events.events_val[i].ptr1;
			    details.dm_ptr2 = 0;
			    details.dm_ptr3 = 0;
			    l->second.push_back(details);
			break;
			}
			case CBTF_PTHREAD_MUTEX_UNLOCK: {
			    details.dm_retval = data.events.events_val[i].retval;
			    details.dm_ptr1 = data.events.events_val[i].ptr1;
			    details.dm_ptr2 = 0;
			    details.dm_ptr3 = 0;
			    l->second.push_back(details);
			break;
			}
			case CBTF_PTHREAD_MUTEX_TRYLOCK: {
			    details.dm_retval = data.events.events_val[i].retval;
			    details.dm_ptr1 = data.events.events_val[i].ptr1;
			    details.dm_ptr2 = 0;
			    details.dm_ptr3 = 0;
			    l->second.push_back(details);
			break;
			}
			case CBTF_PTHREAD_COND_INIT: {
			    details.dm_retval = data.events.events_val[i].retval;
			    details.dm_ptr1 = data.events.events_val[i].ptr1;
			    details.dm_ptr2 = data.events.events_val[i].ptr2;
			    details.dm_ptr3 = 0;
			    l->second.push_back(details);
			break;
			}
			case CBTF_PTHREAD_COND_DESTROY: {
			    details.dm_retval = data.events.events_val[i].retval;
			    details.dm_ptr1 = data.events.events_val[i].ptr1;
			    details.dm_ptr2 = 0;
			    details.dm_ptr3 = 0;
			    l->second.push_back(details);
			break;
			}
			case CBTF_PTHREAD_COND_SIGNAL: {
			    details.dm_retval = data.events.events_val[i].retval;
			    details.dm_ptr1 = data.events.events_val[i].ptr1;
			    details.dm_ptr2 = 0;
			    details.dm_ptr3 = 0;
			    l->second.push_back(details);
			break;
			}
			case CBTF_PTHREAD_COND_BROADCAST: {
			    details.dm_retval = data.events.events_val[i].retval;
			    details.dm_ptr1 = data.events.events_val[i].ptr1;
			    details.dm_ptr2 = 0;
			    details.dm_ptr3 = 0;
			    l->second.push_back(details);
			break;
			}
			case CBTF_PTHREAD_COND_WAIT: {
			    details.dm_retval = data.events.events_val[i].retval;
			    details.dm_ptr1 = data.events.events_val[i].ptr1;
			    details.dm_ptr2 = data.events.events_val[i].ptr2;
			    details.dm_ptr3 = 0;
			    l->second.push_back(details);
			break;
			}
			case CBTF_PTHREAD_COND_TIMEDWAIT: {
			    details.dm_retval = data.events.events_val[i].retval;
			    details.dm_ptr1 = data.events.events_val[i].ptr1;
			    details.dm_ptr2 = data.events.events_val[i].ptr2;
			    details.dm_ptr3 = 0;
			    l->second.push_back(details);
			break;
			}

			case CBTF_PTHREAD_UNKNOWN: {
			    details.dm_retval = data.events.events_val[i].retval;
			    l->second.push_back(details);
			break;
			}

			default: {
			    details.dm_retval = data.events.events_val[i].retval;
			    l->second.push_back(details);
			break;
			}
		    }
#else
			    details.dm_retval = data.events.events_val[i].retval;
			    details.dm_ptr1 = data.events.events_val[i].ptr1;
			    details.dm_ptr2 = data.events.events_val[i].ptr2;
			    details.dm_ptr3 = data.events.events_val[i].ptr3;
		    l->second.push_back(details);
#endif

		    
		}

	    }
	    
	}

    }

    // Free the decoded data blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_pthreads_exttrace_data),
	     reinterpret_cast<char*>(&data));
}


void PthreadsCollector::getUniquePCValues( const Thread& thread,
                                     const Blob& blob,
                                     PCBuffer *buffer) const
{

#if 0
    // Decode this data blob
    pthreads_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_pthreads_data), &data);

    if (data.stacktraces.stacktraces_len == 0) {
	// todo
    }

    // Iterate over each stack trace in the data blob
    for(unsigned i = 0; i < data.stacktraces.stacktraces_len; ++i) {
	if (data.stacktraces.stacktraces_val[i] != 0) {
	    UpdatePCBuffer(data.stacktraces.stacktraces_val[i], buffer);
	}
    }

    // Free the decoded data blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_pthreads_data),
	     reinterpret_cast<char*>(&data));
#endif
}


void PthreadsCollector::getUniquePCValues( const Thread& thread,
                                         const Blob& blob,
                                         std::set<Address>& uaddresses) const
{

    // Decode this data blob
    CBTF_pthreads_exttrace_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_pthreads_exttrace_data), &data);

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
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_pthreads_exttrace_data),
             reinterpret_cast<char*>(&data));
}
