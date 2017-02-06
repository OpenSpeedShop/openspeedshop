////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
// Copyright (c) 2008-2014 The Krell Institute. All Rights Reserved.
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
 * Definition of the IOTCollector class.
 *
 */
 
//#define DEBUG_METRICS 1

#include "IOTCollector.hxx"
#include "IOTDetail.hxx"

#include "blobs.h"

using namespace OpenSpeedShop::Framework;



namespace {


    
    /** Type returned for the I/O call time metrics. */
    typedef std::map<StackTrace, std::vector<double> > CallTimes;

    /** Type returned for the I/O call detail metrics. */
    typedef std::map<StackTrace, std::vector<IOTDetail> > CallDetails;
    
    

    /**
     * Traceable function table.
     *
     * Table listing the traceable I/O functions. In order for an I/O function
     * to actually be traceable, corresponding wrapper(s) must first be written
     * and compiled into this collector's runtime.
     *
     * @note    A function's index position in this table is directly used as
     *          its index position in the iot_parameters.traced array. Thus
     *          the order the functions are listed here is significant. If it
     *          is changed, users will find that any saved databases suddenly
     *          trace different I/O functions than they did previously.
     */

     #include "IOTTraceableFunctions.h"


    
}    


/**
 * Collector's factory method.
 *
 * Factory method for instantiating a collector implementation. This is the
 * only function that is externally visible from outside the collector plugin.
 *
 * @return    New instance of this collector's implementation.
 */
extern "C" CollectorImpl* iot_LTX_CollectorFactory()
{
    return new IOTCollector();
}


/**
 * Default constructor.
 *
 * Constructs a new I/O collector with the proper metadata.
 */
IOTCollector::IOTCollector() :
    CollectorImpl("iot",
                  "I/O Extended Event Tracing",
		  "Intercepts all calls to I/O functions that perform any "
		  "significant amount of work (primarily those that read/write "
		  "data) and for each call records the current stack trace "
		  "and start/end time. Detailed ancillary data is also stored "
		  "stored such as the function's parameters and return value.")
{
    // Allocate a map for pathnames.
    iotPathNames = new std::map<std::string, std::string> ;

    // Declare our parameters
    declareParameter(Metadata("traced_functions", "Traced Functions",
			      "Set of I/O functions to be traced.",
			      typeid(std::map<std::string, bool>)));
    
    // Declare our metrics
    declareMetric(Metadata("time", "I/O Call Time",
			   "Exclusive I/O call time in seconds.",
			   typeid(double)));
    declareMetric(Metadata("inclusive_times", "Inclusive Times",
			   "Inclusive I/O call times in seconds.",
			   typeid(CallTimes)));
    declareMetric(Metadata("exclusive_times", "Exclusive Times",
			   "Exclusive I/O call times in seconds.",
			   typeid(CallTimes)));
    declareMetric(Metadata("inclusive_details", "Inclusive Details",
			   "Inclusive I/O call details.",
			   typeid(CallDetails)));
    declareMetric(Metadata("exclusive_details", "Exclusive Details",
			   "Exclusive I/O call details.",
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
 			   "Average Exclusive I/O call times in seconds across threads or ranks.",
 			   typeid(double)));
    declareMetric(Metadata("threadMin", "Thread Minimum Time",
 			   "Minimum Exclusive I/O call times in seconds across threads or ranks.",
 			   typeid(double)));
    declareMetric(Metadata("threadMax", "Thread Maximum Time",
 			   "Maximum Exclusive I/O call times in seconds across threads or ranks.",
 			   typeid(double)));
    declareMetric(Metadata("stddev", "Standard Deviation Time",
 			   "Standard Deviation call times in seconds.",
 			   typeid(double)));
    declareMetric(Metadata("count", "Number of Calls",
 			   "Number of calls to this function.",
 			   typeid(uint64_t)));
    declareMetric(Metadata("nsysarg", "Number of System Arguments for each I/O Call",
 			   "Number of system arguments for I/O calls to this function.",
 			   typeid(uint64_t)));
    declareMetric(Metadata("retval", "Function dependent return value for each I/O Call",
 			   "Function dependent return value for each I/O call to this function.",
 			   typeid(uint64_t)));
    declareMetric(Metadata("syscallno", "System call value for this I/O Call",
 			   "System call value for this I/O function call.",
 			   typeid(uint64_t)));
    declareMetric(Metadata("pathname", "Pathname used in this I/O Call",
 			   "Pathname used in this I/O function fall.",
 			   typeid(std::string)));

}



/**
 * Default destructor.
 *
 * Reclaim space allocated for removing duplicate pathnames.
 */
IOTCollector::~IOTCollector()
{
    if (iotPathNames != NULL) {
      (*iotPathNames).clear();
      delete iotPathNames;
    }
}



/**
 * Look for duplicate pathnames
 *
 * Save space by reusing duplicate pathnames.
 *
 * @return    std::string
 */
std::string IOTCollector::findPathNameString ( std::string s ) const
{
    std::map<std::string, std::string>::iterator si = (*iotPathNames).find(s);
    if ( si != (*iotPathNames).end() ) {
      return (*si).second;
    } else {
      (*iotPathNames).insert( make_pair(s, s) );
      return s;
    }
}



/**
 * Get the default parameter values.
 *
 * Implement getting our default parameter values.
 *
 * @return    Blob containing the default parameter values.
 */
Blob IOTCollector::getDefaultParameterValues() const
{
    // Setup an empty parameter structure    
    iot_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));

    // Set the default parameters
    for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i)
	parameters.traced[i] = true;
    
    // Return the encoded blob to the caller
    return Blob(reinterpret_cast<xdrproc_t>(xdr_iot_parameters), &parameters);
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
void IOTCollector::getParameterValue(const std::string& parameter,
				      const Blob& data, void* ptr) const
{
    // Decode the blob containing the parameter values
    iot_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_iot_parameters),
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
void IOTCollector::setParameterValue(const std::string& parameter,
				      const void* ptr, Blob& data) const
{
    // Decode the blob containing the parameter values
    iot_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_iot_parameters),
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
            setenv("OPENSS_IOT_TRACED", (char *)env_param.c_str(), 1);
        }
    }
    
    // Re-encode the blob containing the parameter values
    data = Blob(reinterpret_cast<xdrproc_t>(xdr_iot_parameters), &parameters);
}



/**
 * Start data collection.
 *
 * Implement starting data collection for the specified threads.
 *
 * @param collector    Collector starting data collection.
 * @param threads      Threads for which to start collecting data.
 */
void IOTCollector::startCollecting(const Collector& collector,
				   const ThreadGroup& threads) const
{
    // Get the set of traced functions for this collector
    std::map<std::string, bool> traced;
    collector.getParameterValue("traced_functions", traced);
    
    // Assemble and encode arguments to iot_start_tracing()
    iot_start_tracing_args args;
    memset(&args, 0, sizeof(args));
    args.experiment = getExperimentId(collector);
    args.collector = getCollectorId(collector);
    Blob arguments(reinterpret_cast<xdrproc_t>(xdr_iot_start_tracing_args),
                   &args);
    
    // Execute iot_stop_tracing() before we exit the threads
    executeBeforeExit(collector, threads,
		      "iot-rt: iot_stop_tracing", Blob());
    
    // Execute iot_start_tracing() in the threads
    executeNow(collector, threads,
               "iot-rt: iot_start_tracing", arguments);

    // Execute our wrappers in place of the real I/O functions
    for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i)	
	if((traced.find(TraceableFunctions[i]) != traced.end()) &&
	   traced.find(TraceableFunctions[i])->second) {
	    // Wrap the I/O function
	    // What if the traceable function is not found???
	    executeInPlaceOf(
		collector, threads, 
		TraceableFunctions[i],
		std::string("iot-rt: iot") + TraceableFunctions[i]
		);
	}
}



/**
 * Stops data collection.
 *
 * Implement stopping data collection for the specified threads.
 *
 * @param collector    Collector stopping data collection.
 * @param threads      Threads for which to stop collecting data.
 */
void IOTCollector::stopCollecting(const Collector& collector,
				  const ThreadGroup& threads) const
{
    // Execute iot_stop_tracing() in the threads
    executeNow(collector, threads,
               "iot-rt: iot_stop_tracing", Blob());
    
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

void IOTCollector::getMetricValues(const std::string& metric,
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
    iot_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_iot_data), &data);


    if (getenv("OPENSS_DEBUG_IOT_METRICS") != NULL) {
      std::cerr << "IOTCollector::getMetricValues, data.pathnames.pathnames_len=" << data.pathnames.pathnames_len << std::endl;
    }

    // Iterate over each of the events
    for(unsigned i = 0; i < data.events.events_len; ++i) {

#if 0
	// Get the time interval attributable to this event
	TimeInterval interval(Time(data.events.events_val[i].start_time),
			      Time(data.events.events_val[i].stop_time));
#else
        uint64_t start_time = data.events.events_val[i].start_time;
        uint64_t stop_time = data.events.events_val[i].stop_time;
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
	    ++j)
	    trace.push_back(Address(data.stacktraces.stacktraces_val[j]));
	
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
			    std::make_pair(trace, std::vector<IOTDetail>())
			    ).first;
		    
		    // Add this event's details structure to the results
		    IOTDetail details;
		    details.dm_interval = interval;
		    details.dm_time = t_intersection / 1000000000.0;
		    details.dm_retval = data.events.events_val[i].retval;
		    details.dm_nsysargs = data.events.events_val[i].nsysargs;
		    details.dm_syscallno = data.events.events_val[i].syscallno;

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

                    int pidx = data.events.events_val[i].pathindex;

                    if (pidx != 0) {
			// By eliminating duplicates the memory usage is reduced.
			std::string s = std::string(&data.pathnames.pathnames_val[pidx]);
			details.dm_pathname = findPathNameString( s );
                    }

		    for(int sysarg = 0;
			sysarg < data.events.events_val[i].nsysargs;
			sysarg++) {
			details.dm_sysargs[sysarg] =
			    data.events.events_val[i].sysargs[sysarg];
		    }
		    l->second.push_back(details);
		    
		}

	    }
	    
	}

    }

    // Free the decoded data blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_iot_data),
	     reinterpret_cast<char*>(&data));
}


void IOTCollector::getUniquePCValues( const Thread& thread,
                                     const Blob& blob,
                                     PCBuffer *buffer) const
{

    // Decode this data blob
    iot_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_iot_data), &data);

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
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_iot_data),
	     reinterpret_cast<char*>(&data));
}


void IOTCollector::getUniquePCValues( const Thread& thread,
                                         const Blob& blob,
                                         std::set<Address>& uaddresses) const
{

    // Decode this data blob
    iot_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_iot_data), &data);

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
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_iot_data),
             reinterpret_cast<char*>(&data));
}
