////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
// Copyright (c) 2007-2014 The Krell Institute. All Rights Reserved.
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
 * Definition of the MPITCollector class.
 *
 */
 
#include "MPITCollector.hxx"
#include "MPITDetail.hxx"
#include "EntrySpy.hxx"
#include "blobs.h"

using namespace OpenSpeedShop::Framework;



namespace {

    /** Type returned for the MPIT call time metrics. */
    typedef std::map<StackTrace, std::vector<double> > CallTimes;

    /** Type returned for the MPIT call detail metrics. */
    typedef std::map<StackTrace, std::vector<MPITDetail> > CallDetails;
    
    

    /**
     * Traceable function table.
     *
     * Table listing the traceable MPI functions. In order for an MPI function
     * to actually be traceable, corresponding wrapper(s) must first be written
     * and compiled into this collector's runtime.
     *
     * @note    A function's index position in this table is directly used as
     *          its index position in the mpit_parameters.traced array. Thus
     *          the order the functions are listed here is significant. If it
     *          is changed, users will find that any saved databases suddenly
     *          trace different MPI functions than they did previously.
     */

    #include "MPITTraceableFunctions.h"

}    



/**
 * Collector's factory method.
 *
 * Factory method for instantiating a collector implementation. This is the
 * only function that is externally visible from outside the collector plugin.
 *
 * @return    New instance of this collector's implementation.
 */
extern "C" CollectorImpl* mpit_LTX_CollectorFactory()
{
    return new MPITCollector();
}



/**
 * Default constructor.
 *
 * Constructs a new MPI collector with the proper metadata.
 */
MPITCollector::MPITCollector() :
    CollectorImpl("mpit",
                  "MPI Extended Event Tracing",
		  "Intercepts all calls to MPI functions that perform any "
		  "significant amount of work (primarily those that send "
		  "messages) and for each call records the current stack "
		  "trace and start/end time. Detailed ancillary data is also "
		  "stored such as the source and destination rank, number of "
		  "bytes sent, message tag, communicator used, and message "
		  "data type.")
{
    // Declare our parameters
    declareParameter(Metadata("traced_functions", "Traced Functions",
			      "Set of MPI functions to be traced.",
			      typeid(std::map<std::string, bool>)));
    
    // Declare our metrics
    declareMetric(Metadata("time", "MPI Call Time",
			   "Exclusive MPI call time in seconds.",
			   typeid(double)));
    declareMetric(Metadata("size", "MPI Bytes Sent/Received",
			   "Exclusive MPI call bytes sent/recevied.",
			   typeid(uint64_t)));
    declareMetric(Metadata("inclusive_times", "Inclusive Times",
			   "Inclusive MPI call times in seconds.",
			   typeid(CallTimes)));
    declareMetric(Metadata("exclusive_times", "Exclusive Times",
			   "Exclusive MPI call times in seconds.",
			   typeid(CallTimes)));
    declareMetric(Metadata("inclusive_details", "Inclusive Details",
                           "Inclusive MPI call details.",
                           typeid(CallDetails)));
    declareMetric(Metadata("exclusive_details", "Exclusive Details",
                           "Exclusive MPI call details.",
                           typeid(CallDetails)));
    declareMetric(Metadata("min", "Minimum Time",
                           "Mininum MPI call times in seconds.",
                           typeid(double)));
    declareMetric(Metadata("max", "Maximum Time",
                           "Maximum MPI call times in seconds.",
                           typeid(double)));
    declareMetric(Metadata("average", "Average Time",
                           "Average MPI call times in seconds.",
                           typeid(double)));
    declareMetric(Metadata("threadAverage", "Thread Average Time",
                           "Average Exclusive MPI call times in seconds across threads or ranks.",
                           typeid(double)));
    declareMetric(Metadata("threadMin", "Thread Minimum Time",
                           "Minimum Exclusive MPI call times in seconds across threads or ranks.",
                           typeid(double)));
    declareMetric(Metadata("threadMax", "Thread Maximum Time",
                           "Maximum Exclusive MPI call times in seconds across threads or ranks.",
                           typeid(double)));
    declareMetric(Metadata("stddev", "Standard Deviation Time",
                           "Standard Deviation for MPI call times in seconds.",
                           typeid(double)));
    declareMetric(Metadata("percent", "Percent of Exclusive CPU Time",
                           "Percent of Exclusive CPU Time.",
                           typeid(double)));
    declareMetric(Metadata("count", "Number of Calls",
                           "Number of calls to this MPI function.",
                           typeid(uint64_t)));
    declareMetric(Metadata("dest", "Destination rank value for this particular MPI Call",
                           "Destination rank value for this particular MPI Call.",
                           typeid(uint64_t)));
    declareMetric(Metadata("source", "Source rank value for this particular MPI Call",
                           "Source rank value for this particular MPI Call.",
                           typeid(uint64_t)));
    declareMetric(Metadata("tag", "The tag value for this particular MPI Call",
                           "The tag value for this particular MPI Call.",
                           typeid(uint64_t)));
    declareMetric(Metadata("comm", "The communicator value for this particular MPI Call",
                           "The communicator value for this particular MPI Call.",
                           typeid(uint64_t)));
    declareMetric(Metadata("datatype", "The datatype value for this particular MPI Call",
                           "The datatype value for this particular MPI Call.",
                           typeid(uint64_t)));
    declareMetric(Metadata("retval", "Function dependent return value for each MPI Call",
                           "Function dependent return value for each MPI call to this function.",
                           typeid(uint64_t)));


}



/**
 * Get the default parameter values.
 *
 * Implement getting our default parameter values.
 *
 * @return    Blob containing the default parameter values.
 */
Blob MPITCollector::getDefaultParameterValues() const
{
    // Setup an empty parameter structure    
    mpit_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));

    // Set the default parameters
    for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i)
	parameters.traced[i] = true;
    
    // Return the encoded blob to the caller
    return Blob(reinterpret_cast<xdrproc_t>(xdr_mpit_parameters), &parameters);
}



/**
 * Get a category and parameter value.
 *
 * Implement getting one of our parameter values.
 *
 * @param parameter    Unique identifier of the parameter.
 * @param data         Blob containing the parameter values.
 * @retval ptr         Untyped pointer to the parameter value.
 */
void MPITCollector::getCategoryValue(const std::string& parameter,
				      const Blob& data, void* ptr) const
{
    // Decode the blob containing the category values
    mpit_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_mpit_parameters),
                        &parameters);

    // Handle the "traced_functions" parameter
    if(parameter == "traced_functions") {
	std::map<std::string, bool>* value =
	    reinterpret_cast<std::map<std::string, bool>*>(ptr);    
        for(unsigned i = 0; TraceableCategories[i] != NULL; ++i)
	    value->insert(std::make_pair(TraceableCategories[i], parameters.traced[i]));
    }
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
void MPITCollector::getParameterValue(const std::string& parameter,
				      const Blob& data, void* ptr) const
{
    // Decode the blob containing the parameter values
    mpit_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_mpit_parameters),
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
void MPITCollector::setParameterValue(const std::string& parameter,
				      const void* ptr, Blob& data) const
{
#if DEBUG_PARAM
    std::cerr << "MPITCollector::setParameterValue, parameter=" << parameter << std::endl;
#endif

    // Decode the blob containing the parameter values
    mpit_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_mpit_parameters),
                        &parameters);
    
    // Handle the "traced_functions" parameter
    if(parameter == "traced_functions") {
	std::string env_param;
	const std::map<std::string, bool>* value = reinterpret_cast<const std::map<std::string, bool>*>(ptr);

        std::map<std::string, bool> tmp_value;
        tmp_value.clear();

#if DEBUG_PARAM
        std::cerr << "MPITCollector::setParameterValue, after tmp_value clear()" << std::endl;
#endif

        for (std::map<std::string, bool>::const_iterator im = value->begin(); im != value->end(); im++) {

#if DEBUG_PARAM
           std::cerr << "MPITCollector::setParameterValue, CREATE VALUE_TMP PRINT LOOP im->first=" << im->first.c_str() << std::endl;
           std::cerr << "MPITCollector::setParameterValue, CREATE VALUE_TMP PRINT LOOP im->second=" << im->second << std::endl;
#endif

           tmp_value.insert(make_pair(im->first, im->second));
        }

#if DEBUG_PARAM

        for (std::map<std::string, bool>::iterator dbg_tmp_im = tmp_value.begin(); dbg_tmp_im != tmp_value.end(); dbg_tmp_im++) {
           std::cerr << "MPITCollector::setParameterValue, DEBUG TMP_VALUE PRINT LOOP dbg_tmp_im->first=" << dbg_tmp_im->first.c_str() << std::endl;
           std::cerr << "MPITCollector::setParameterValue, DEBUG TMP_VALUE PRINT LOOP dbg_tmp_im->second=" << dbg_tmp_im->second << std::endl;
        }

        for (std::map<std::string, bool>::const_iterator im = value->begin(); im != value->end(); im++) {
           std::cerr << "MPITCollector::setParameterValue, DEBUG PRINT LOOP im->first=" << im->first.c_str() << std::endl;
           std::cerr << "MPITCollector::setParameterValue, DEBUG PRINT LOOP im->second=" << im->second << std::endl;
        }
#endif

/*  We are looking for these categories in addition to individual MPI function names.
    See MPITraceableFunctions.h for the definitions.
          "all",
          "asynchronous_p2p",
          "collective_com",
          "datatypes",
          "environment",
          "graphs_contexts_comms",
          "persistent_com",
          "process_topologies",
          "synchronous_p2p",
*/

        if ((tmp_value.find("all") != tmp_value.end()) &&
             tmp_value.find("all")->second) {

#if DEBUG_PARAM
            std::cerr << "MPITCollector::setParameterValue, tmp_value.find(all) has been FOUND"  << std::endl;
#endif

            for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i) {
                std::map<std::string, bool>::iterator tmp_im = tmp_value.find(TraceableFunctions[i]);
                tmp_im->second = TRUE;
                env_param = env_param + TraceableFunctions[i] + ",";
            }
          
        } 

        if ((tmp_value.find("asynchronous_p2p") != tmp_value.end()) &&
             tmp_value.find("asynchronous_p2p")->second) {

#if DEBUG_PARAM
            std::cerr << "MPITCollector::setParameterValue, tmp_value.find(asynchronous_p2p) has been FOUND"  << std::endl;
#endif

            for(unsigned i = 0; TraceableAsynchronousP2P[i] != NULL; ++i) {
                std::map<std::string, bool>::iterator tmp_im = tmp_value.find(TraceableAsynchronousP2P[i]);
                tmp_im->second = TRUE;
                env_param = env_param + TraceableAsynchronousP2P[i] + ",";
            }
          
        } 

        if ((tmp_value.find("collective_com") != tmp_value.end()) &&
             tmp_value.find("collective_com")->second) {

#if DEBUG_PARAM
            std::cerr << "MPITCollector::setParameterValue, tmp_value.find(collective_com) has been FOUND"  << std::endl;
#endif

            for(unsigned i = 0; TraceableCollectives[i] != NULL; ++i) {
                std::map<std::string, bool>::iterator tmp_im = tmp_value.find(TraceableCollectives[i]);
                tmp_im->second = TRUE;
                env_param = env_param + TraceableCollectives[i] + ",";
            }
          
        } 

        if ((tmp_value.find("datatypes") != tmp_value.end()) &&
             tmp_value.find("datatypes")->second) {

#if DEBUG_PARAM
            std::cerr << "MPITCollector::setParameterValue, tmp_value.find(datatypes) has been FOUND"  << std::endl;
#endif

            for(unsigned i = 0; TraceableDatatypes[i] != NULL; ++i) {
                std::map<std::string, bool>::iterator tmp_im = tmp_value.find(TraceableDatatypes[i]);
                tmp_im->second = TRUE;
                env_param = env_param + TraceableDatatypes[i] + ",";
            }
          
        } 

        if ((tmp_value.find("environment") != tmp_value.end()) &&
             tmp_value.find("environment")->second) {

#if DEBUG_PARAM
            std::cerr << "MPITCollector::setParameterValue, tmp_value.find(environment) has been FOUND"  << std::endl;
#endif

            for(unsigned i = 0; TraceableEnvironment[i] != NULL; ++i) {
                std::map<std::string, bool>::iterator tmp_im = tmp_value.find(TraceableEnvironment[i]);
                tmp_im->second = TRUE;
                env_param = env_param + TraceableEnvironment[i] + ",";
            }
          
        } 

        if ((tmp_value.find("graphs_contexts_comms") != tmp_value.end()) &&
             tmp_value.find("graphs_contexts_comms")->second) {

#if DEBUG_PARAM
            std::cerr << "MPITCollector::setParameterValue, tmp_value.find(graphs_contexts_comms) has been FOUND"  << std::endl;
#endif

            for(unsigned i = 0; TraceableGraphsContexts[i] != NULL; ++i) {
                std::map<std::string, bool>::iterator tmp_im = tmp_value.find(TraceableGraphsContexts[i]);
                tmp_im->second = TRUE;
                env_param = env_param + TraceableGraphsContexts[i] + ",";
            }
          
        } 

        if ((tmp_value.find("persistent_com") != tmp_value.end()) &&
             tmp_value.find("persistent_com")->second) {

#if DEBUG_PARAM
            std::cerr << "MPITCollector::setParameterValue, tmp_value.find(persistent_com) has been FOUND"  << std::endl;
#endif

            for(unsigned i = 0; TraceablePersistent[i] != NULL; ++i) {
                std::map<std::string, bool>::iterator tmp_im = tmp_value.find(TraceablePersistent[i]);
                tmp_im->second = TRUE;
                env_param = env_param + TraceablePersistent[i] + ",";
            }
          
        } 

        if ((tmp_value.find("process_topologies") != tmp_value.end()) &&
             tmp_value.find("process_topologies")->second) {

#if DEBUG_PARAM
            std::cerr << "MPITCollector::setParameterValue, tmp_value.find(process_topologies) has been FOUND"  << std::endl;
#endif

            for(unsigned i = 0; TraceableProcessTopologies[i] != NULL; ++i) {
                std::map<std::string, bool>::iterator tmp_im = tmp_value.find(TraceableProcessTopologies[i]);
                tmp_im->second = TRUE;
                env_param = env_param + TraceableProcessTopologies[i] + ",";
            }
          
        } 

        if ((tmp_value.find("synchronous_p2p") != tmp_value.end()) &&
             tmp_value.find("synchronous_p2p")->second) {

#if DEBUG_PARAM
            std::cerr << "MPITCollector::setParameterValue, tmp_value.find(synchronous_p2p) has been FOUND"  << std::endl;
#endif

            for(unsigned i = 0; TraceableSynchronousP2P[i] != NULL; ++i) {
                std::map<std::string, bool>::iterator tmp_im = tmp_value.find(TraceableSynchronousP2P[i]);
                tmp_im->second = TRUE;
                env_param = env_param + TraceableSynchronousP2P[i] + ",";
            }
          
        } 

        {   // handle individual function names

#if DEBUG_PARAM
           std::cerr << "MPITCollector::setParameterValue, TraceableFunction loop, HANDLE FUNCTION NAMES INDIVIDUALLY"  << std::endl;
  	   for(unsigned dbgi = 0; TraceableFunctions[dbgi] != NULL; ++dbgi) {
               std::cerr << "MPITCollector::setParameterValue, DEBUG TraceableFunction loop, TraceableFunctions[dbgi]=" << TraceableFunctions[dbgi] 
                         << "  parameters.traced[dbgi]=" << parameters.traced[dbgi] << std::endl;
           }
#endif

  	   for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i) {
	       parameters.traced[i] = (tmp_value.find(TraceableFunctions[i]) != tmp_value.end()) && tmp_value.find(TraceableFunctions[i])->second;

#if DEBUG_PARAM
               std::cerr << "MPITCollector::setParameterValue, TraceableFunction loop, TraceableFunctions[i]=" << TraceableFunctions[i] << "  parameters.traced[i]=" << parameters.traced[i] << std::endl;
#endif
	       if(parameters.traced[i]) {
		   env_param = env_param + TraceableFunctions[i] + ",";
#if DEBUG_PARAM
                   std::cerr << "MPITCollector::setParameterValue, TraceableFunction loop, env_param=" << env_param << std::endl;
#endif
	       }
	   } // end for TraceableFunctions[]

        } // handle individual function names

	if (env_param.size() > 0) {
	    setenv("OPENSS_MPI_TRACED", (char *)env_param.c_str(), 1);
	}

#if DEBUG_PARAM
    std::cerr << "Exit MPITCollector::setParameterValue, env_param=" << env_param << std::endl;
#endif

    }
    
    // Re-encode the blob containing the parameter values
    data = Blob(reinterpret_cast<xdrproc_t>(xdr_mpit_parameters), &parameters);
}



/**
 * Start data collection.
 *
 * Implement starting data collection for the specified threads.
 *
 * @param collector    Collector starting data collection.
 * @param threads      Threads for which to start collecting data.
 */
void MPITCollector::startCollecting(const Collector& collector,
				    const ThreadGroup& threads) const
{
    // Get the set of traced functions for this collector
    std::map<std::string, bool> traced;
    collector.getParameterValue("traced_functions", traced);
    
    // Assemble and encode arguments to mpit_start_tracing()
    mpit_start_tracing_args args;
    memset(&args, 0, sizeof(args));
    args.experiment = getExperimentId(collector);
    args.collector = getCollectorId(collector);
    Blob arguments(reinterpret_cast<xdrproc_t>(xdr_mpit_start_tracing_args),
                   &args);

    // Get the runtime usage map for the specified threads
    RuntimeUsageMap runtime_usage_map = getMPIRuntimeUsageMap(threads);
    
    // Execute mpit_stop_tracing() before we exit the threads
    for(RuntimeUsageMap::const_iterator
	    i = runtime_usage_map.begin(); i != runtime_usage_map.end(); ++i)
	executeBeforeExit(collector, i->second,
			  i->first + ": mpit_stop_tracing", Blob());
    
    // Execute mpit_start_tracing() in the threads
    for(RuntimeUsageMap::const_iterator
	    i = runtime_usage_map.begin(); i != runtime_usage_map.end(); ++i)
	executeNow(collector, i->second,
		   i->first + ": mpit_start_tracing", arguments);

    // Execute our wrappers in place of the real MPI functions
    for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i)	
	if((traced.find(TraceableFunctions[i]) != traced.end()) &&
	   traced.find(TraceableFunctions[i])->second) {
	    
	    std::string P_name = std::string("P") + TraceableFunctions[i];

	    // Wrap the MPI function
	    for(RuntimeUsageMap::const_iterator i = runtime_usage_map.begin();
		i != runtime_usage_map.end();
		++i)
		executeInPlaceOf(collector, i->second, 
				 P_name, i->first + ": mpit_" + P_name);
	    
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
void MPITCollector::stopCollecting(const Collector& collector,
				   const ThreadGroup& threads) const
{
    // Get the runtime usage map for the specified threads
    RuntimeUsageMap runtime_usage_map = getMPIRuntimeUsageMap(threads);

    // Execute mpit_stop_tracing() in the threads
    for(RuntimeUsageMap::const_iterator
	    i = runtime_usage_map.begin(); i != runtime_usage_map.end(); ++i)
	executeNow(collector, i->second,
		   i->first + ": mpit_stop_tracing", Blob());
    
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
void MPITCollector::getMetricValues(const std::string& metric,
				    const Collector& collector,
				    const Thread& thread,
				    const Extent& extent,
				    const Blob& blob,
				    const ExtentGroup& subextents,
				    void* ptr) const
{

    // Determine which metric was specified
    bool is_time = (metric == "time");
    bool is_bytes = (metric == "bytes");
    bool is_inclusive_times = (metric == "inclusive_times");
    bool is_exclusive_times = (metric == "exclusive_times");
    bool is_inclusive_details = (metric == "inclusive_details");
    bool is_exclusive_details = (metric == "exclusive_details");

    // Don't return anything if an invalid metric was specified
    if(!is_time && !is_bytes &&
       !is_inclusive_times && !is_exclusive_times &&
       !is_inclusive_details && !is_exclusive_details)
	return;

    // Check assertions
    if(is_time) {
	Assert(reinterpret_cast<std::vector<double>*>(ptr)->size() >=
	       subextents.size());
    }
    else if(is_bytes) {
	Assert(reinterpret_cast<std::vector<uint64_t>*>(ptr)->size() >=
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
    mpit_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_mpit_data), &data);
    
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
	    if((is_time || is_bytes || 
		is_exclusive_times || is_exclusive_details) &&
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
		else if(is_bytes) {

		    // Add this event's bytes sent/received to the results
		    (*reinterpret_cast<std::vector<double>*>(ptr))[*k] +=
			data.events.events_val[i].size;
		    
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
			    std::make_pair(trace, std::vector<MPITDetail>())
			    ).first;
		    
		    // Add this event's details structure to the results
		    MPITDetail details;
		    details.dm_interval = interval;
		    details.dm_time = t_intersection / 1000000000.0;
		    details.dm_source = data.events.events_val[i].source;
		    details.dm_destination = 
			data.events.events_val[i].destination;
		    details.dm_size = data.events.events_val[i].size;
		    details.dm_tag = data.events.events_val[i].tag;
		    details.dm_communicator = 
			data.events.events_val[i].communicator;
		    details.dm_datatype = data.events.events_val[i].datatype;
		    details.dm_retval = data.events.events_val[i].retval;

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
		    l->second.push_back(details);
		    
		}

	    }
	    
	}

    }

    // Free the decoded data blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_mpit_data),
	     reinterpret_cast<char*>(&data));
}

void MPITCollector::getUniquePCValues( const Thread& thread,
                                      const Blob& blob,
                                      PCBuffer *buffer) const
{

    // Decode this data blob
    mpit_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_mpit_data), &data);

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
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_mpit_data),
	     reinterpret_cast<char*>(&data));
}


void MPITCollector::getUniquePCValues( const Thread& thread,
                                         const Blob& blob,
                                         std::set<Address>& uaddresses) const
{

    // Decode this data blob
    mpit_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_mpit_data), &data);

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
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_mpit_data),
             reinterpret_cast<char*>(&data));
}
