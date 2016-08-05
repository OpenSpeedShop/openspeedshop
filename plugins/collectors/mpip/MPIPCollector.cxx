////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 The Krell Institute . All Rights Reserved.
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
 * Definition of the MPIPCollector class.
 *
 */
 
#include "MPIPCollector.hxx"
#include "MPIPDetail.hxx"

#include "KrellInstitute/Messages/Mpi.h"
#include "KrellInstitute/Messages/Mpi_data.h"

using namespace OpenSpeedShop::Framework;



namespace {

    
    /** Type returned for the sample detail metrics. */
    typedef std::map<StackTrace, MPIPDetail> SampleDetail;


    /**
     * Traceable function table.
     *
     * Table listing the traceable MPI functions. In order for an MPI function
     * to actually be traceable, corresponding wrapper(s) must first be written
     * and compiled into this collector's runtime.
     *
     * @note    A function's index position in this table is directly used as
     *          its index position in the CBTF_mpi_parameters.traced array. Thus
     *          the order the functions are listed here is significant. If it
     *          is changed, users will find that any saved databases suddenly
     *          trace different MPI functions than they did previously.
     */

    #include "MPIPTraceableFunctions.h"

}    



/**
 * Collector's factory method.
 *
 * Factory method for instantiating a collector implementation. This is the
 * only function that is externally visible from outside the collector plugin.
 *
 * @return    New instance of this collector's implementation.
 */
extern "C" CollectorImpl* mpip_LTX_CollectorFactory()
{
    return new MPIPCollector();
}



/**
 * Default constructor.
 *
 * Constructs a new MPI collector with the proper metadata.
 */
MPIPCollector::MPIPCollector() :
    CollectorImpl("mpip",
                  "MPI Profiling",
		  "Intercepts calls to MPI functions and records for each call,"
		  " the current stack trace "
		  "and time spent in call.")
{
    // Declare our parameters
    declareParameter(Metadata("traced_functions", "Traced Functions",
			      "Set of MPI functions to be traced.",
			      typeid(std::map<std::string, bool>)));
    
    // Declare our metrics
    declareMetric(Metadata("inclusive_time", "Inclusive Time",
			   "Inclusive MPI call times in seconds.",
			   typeid(double)));
    declareMetric(Metadata("exclusive_time", "Exclusive Time",
			   "Exclusive MPI call times in seconds.",
			   typeid(double)));
    declareMetric(Metadata("inclusive_detail", "Inclusive Detail",
			   "Inclusive MPI call details.",
			   typeid(SampleDetail)));
    declareMetric(Metadata("exclusive_detail", "Exclusive Detail",
			   "Exclusive MPI call details.",
			   typeid(SampleDetail)));
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
    declareMetric(Metadata("count", "Number of Calls",
                           "Number of calls to this MPI function.",
                           typeid(uint64_t)));

}



/**
 * Get the default parameter values.
 *
 * Implement getting our default parameter values.
 *
 * @return    Blob containing the default parameter values.
 */
Blob MPIPCollector::getDefaultParameterValues() const
{
    // Setup an empty parameter structure    
    CBTF_mpi_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));

    // Set the default parameters
    for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i)
	parameters.traced[i] = true;
    
    // Return the encoded blob to the caller
    return Blob(reinterpret_cast<xdrproc_t>(xdr_CBTF_mpi_parameters), &parameters);
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
void MPIPCollector::getCategoryValue(const std::string& parameter,
				      const Blob& data, void* ptr) const
{
    // Decode the blob containing the category values
    CBTF_mpi_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_mpi_parameters),
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
void MPIPCollector::getParameterValue(const std::string& parameter,
				      const Blob& data, void* ptr) const
{
    // Decode the blob containing the parameter values
    CBTF_mpi_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_mpi_parameters),
                        &parameters);

    // Handle the "traced_functions" parameter
    if(parameter == "traced_functions") {
	std::map<std::string, bool>* value =
	    reinterpret_cast<std::map<std::string, bool>*>(ptr);    
        for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i)
	    value->insert(std::make_pair(TraceableFunctions[i], parameters.traced[i]));
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
void MPIPCollector::setParameterValue(const std::string& parameter, const void* ptr, Blob& data) const
{
    // Decode the blob containing the parameter values
    CBTF_mpi_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_mpi_parameters), &parameters);

    
    // Handle the "traced_functions" parameter
    if(parameter == "traced_functions") {
	std::string env_param;
	const std::map<std::string, bool>* value = reinterpret_cast<const std::map<std::string, bool>*>(ptr);
        std::map<std::string, bool> tmp_value;
        tmp_value.clear();

#if DEBUG_PARAM
        std::cerr << "MPIPCollector::setParameterValue, after tmp_value clear()" << std::endl;
#endif

        for (std::map<std::string, bool>::const_iterator im = value->begin(); im != value->end(); im++) {

#if DEBUG_PARAM
           std::cerr << "MPIPCollector::setParameterValue, CREATE VALUE_TMP PRINT LOOP im->first=" << im->first.c_str() << std::endl;
           std::cerr << "MPIPCollector::setParameterValue, CREATE VALUE_TMP PRINT LOOP im->second=" << im->second << std::endl;
#endif

           tmp_value.insert(make_pair(im->first, im->second));
        }

#if DEBUG_PARAM
        for (std::map<std::string, bool>::iterator dbg_tmp_im = tmp_value.begin(); dbg_tmp_im != tmp_value.end(); dbg_tmp_im++) {
           std::cerr << "MPIPCollector::setParameterValue, DEBUG TMP_VALUE PRINT LOOP dbg_tmp_im->first=" << dbg_tmp_im->first.c_str() << std::endl;
           std::cerr << "MPIPCollector::setParameterValue, DEBUG TMP_VALUE PRINT LOOP dbg_tmp_im->second=" << dbg_tmp_im->second << std::endl;
        }

        for (std::map<std::string, bool>::const_iterator im = value->begin(); im != value->end(); im++) {
           std::cerr << "MPIPCollector::setParameterValue, DEBUG PRINT LOOP im->first=" << im->first.c_str() << std::endl;
           std::cerr << "MPIPCollector::setParameterValue, DEBUG PRINT LOOP im->second=" << im->second << std::endl;
        }
#endif

/*  We are looking for these categories in addition to individual MPI function names.
    See MPITraceableFunctions.h for the definitions.
          "all",
          "asynchronous_p2p",
          "collective_com",
          "datatypes",
          "environment",
          "file_io",
          "graphs_contexts_comms",
          "persistent_com",
          "process_topologies",
          "synchronous_p2p",
*/

        if ((tmp_value.find("all") != tmp_value.end()) &&
             tmp_value.find("all")->second) {

#if DEBUG_PARAM
            std::cerr << "MPIPCollector::setParameterValue, tmp_value.find(all) has been FOUND"  << std::endl;
#endif

            for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i) {
//                parameters.traced[i] = true;
                std::map<std::string, bool>::iterator tmp_im = tmp_value.find(TraceableFunctions[i]);
                tmp_im->second = TRUE;
                env_param = env_param + TraceableFunctions[i] + ",";
            }
          
        } 

        if ((tmp_value.find("asynchronous_p2p") != tmp_value.end()) &&
             tmp_value.find("asynchronous_p2p")->second) {

#if DEBUG_PARAM
            std::cerr << "MPIPCollector::setParameterValue, tmp_value.find(asynchronous_p2p) has been FOUND"  << std::endl;
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
            std::cerr << "MPIPCollector::setParameterValue, tmp_value.find(collective_com) has been FOUND"  << std::endl;
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
            std::cerr << "MPIPCollector::setParameterValue, tmp_value.find(datatypes) has been FOUND"  << std::endl;
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
            std::cerr << "MPIPCollector::setParameterValue, tmp_value.find(environment) has been FOUND"  << std::endl;
#endif

            for(unsigned i = 0; TraceableEnvironment[i] != NULL; ++i) {
                std::map<std::string, bool>::iterator tmp_im = tmp_value.find(TraceableEnvironment[i]);
                tmp_im->second = TRUE;
                env_param = env_param + TraceableEnvironment[i] + ",";
            }
          
        } 

        if ((tmp_value.find("file_io") != tmp_value.end()) &&
             tmp_value.find("file_io")->second) {

#if DEBUG_PARAM
            std::cerr << "MPIPCollector::setParameterValue, tmp_value.find(file_io) has been FOUND"  << std::endl;
#endif

            for(unsigned i = 0; TraceableFileIO[i] != NULL; ++i) {
                std::map<std::string, bool>::iterator tmp_im = tmp_value.find(TraceableFileIO[i]);
                tmp_im->second = TRUE;
                env_param = env_param + TraceableFileIO[i] + ",";
            }
          
        } 

        if ((tmp_value.find("graphs_contexts_comms") != tmp_value.end()) &&
             tmp_value.find("graphs_contexts_comms")->second) {

#if DEBUG_PARAM
            std::cerr << "MPIPCollector::setParameterValue, tmp_value.find(graphs_contexts_comms) has been FOUND"  << std::endl;
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
            std::cerr << "MPIPCollector::setParameterValue, tmp_value.find(persistent_com) has been FOUND"  << std::endl;
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
            std::cerr << "MPIPCollector::setParameterValue, tmp_value.find(process_topologies) has been FOUND"  << std::endl;
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
            std::cerr << "MPIPCollector::setParameterValue, tmp_value.find(synchronous_p2p) has been FOUND"  << std::endl;
#endif

            for(unsigned i = 0; TraceableSynchronousP2P[i] != NULL; ++i) {
                std::map<std::string, bool>::iterator tmp_im = tmp_value.find(TraceableSynchronousP2P[i]);
                tmp_im->second = TRUE;
                env_param = env_param + TraceableSynchronousP2P[i] + ",";

            }
          
        } 

        {   // handle individual function names

#if DEBUG_PARAM
           std::cerr << "MPIPCollector::setParameterValue, TraceableFunction loop, HANDLE FUNCTION NAMES INDIVIDUALLY"  << std::endl;
  	   for(unsigned dbgi = 0; TraceableFunctions[dbgi] != NULL; ++dbgi) {
               std::cerr << "MPIPCollector::setParameterValue, DEBUG TraceableFunction loop, TraceableFunctions[dbgi]=" << TraceableFunctions[dbgi] 
                         << "  parameters.traced[dbgi]=" << parameters.traced[dbgi] << std::endl;
           }
#endif

  	   for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i) {
	       parameters.traced[i] = (tmp_value.find(TraceableFunctions[i]) != tmp_value.end()) && tmp_value.find(TraceableFunctions[i])->second;

#if DEBUG_PARAM
               std::cerr << "MPIPCollector::setParameterValue, TraceableFunction loop, TraceableFunctions[i]=" << TraceableFunctions[i] 
                         << "  parameters.traced[i]=" << parameters.traced[i] << std::endl;
#endif

	       if(parameters.traced[i]) {

		   env_param = env_param + TraceableFunctions[i] + ",";
#if DEBUG_PARAM
                   std::cerr << "MPIPCollector::setParameterValue, TraceableFunction loop, env_param=" << env_param << std::endl;
#endif
	       }

	   } // end for TraceableFunctions[]
        } // handle individual function names

	if (env_param.size() > 0) {
	    setenv("CBTF_MPI_TRACED", (char *)env_param.c_str(), 1);
	}

#if DEBUG_PARAM
    std::cerr << "Exit MPIPCollector::setParameterValue, env_param=" << env_param << std::endl;
#endif

    }
    

    // Re-encode the blob containing the parameter values
    data = Blob(reinterpret_cast<xdrproc_t>(xdr_CBTF_mpi_parameters), &parameters);
}



/**
 * Start data collection.
 *
 * Implement starting data collection for the specified threads.
 *
 * @param collector    Collector starting data collection.
 * @param threads      Threads for which to start collecting data.
 */
void MPIPCollector::startCollecting(const Collector& collector,
				   const ThreadGroup& threads) const
{
}



/**
 * Stops data collection.
 *
 * Implement stopping data collection for the specified threads.
 *
 * @param collector    Collector stopping data collection.
 * @param threads      Threads for which to stop collecting data.
 */
void MPIPCollector::stopCollecting(const Collector& collector,
				 const ThreadGroup& threads) const
{
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

void MPIPCollector::getMetricValues(const std::string& metric,
				  const Collector& collector,
				  const Thread& thread,
				  const Extent& extent,
				  const Blob& blob,
				  const ExtentGroup& subextents,
				  void* ptr) const
{

    // Don't return anything if an invalid metric was specified
    if((metric != "inclusive_time") && (metric != "exclusive_time") &&
       (metric != "inclusive_detail") && (metric != "exclusive_detail"))
        return;

    bool is_exclusive =
        (metric == "exclusive_time") || (metric == "exclusive_detail");
    bool is_detail =
        (metric == "inclusive_detail") || (metric == "exclusive_detail");

    if(is_detail) {
        Assert(reinterpret_cast<std::vector<SampleDetail>*>(ptr)->size() >=
               subextents.size());
    } else {
        Assert(reinterpret_cast<std::vector<double>*>(ptr)->size() >=
               subextents.size());
    }

    TimeInterval etimeinterval = extent.getTimeInterval();
    Time ebegintime = etimeinterval.getBegin();
    Extent subextbounds = subextents.getBounds();
    AddressRange subBoundsAR = subextbounds.getAddressRange();

    // Decode this data blob
    CBTF_mpi_profile_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_mpi_profile_data), &data);


    // Check assertions
    Assert(data.stacktraces.stacktraces_len == data.count.count_len);

    // Calculate time (in nS) of data blob's extent
    double t_blob = static_cast<double>(extent.getTimeInterval().getWidth());

    // Iterate over each stack trace in the data blob    
    for(unsigned ib = 0, ie = 0; ie < data.stacktraces.stacktraces_len; ib = ie) {
	
	// Find the end of the current stack trace
	for(ie = ib + 1; ie < data.stacktraces.stacktraces_len; ++ie)
	    if(data.count.count_val[ie] > 0)
		break;
	
	// Calculate the time (in seconds) attributable to this sample
	double t_sample = static_cast<double>(data.time.time_val[ib]/1000000000.0);
	
	// Get the stack trace for this sample
	StackTrace trace(thread, extent.getTimeInterval().getBegin());
	for(unsigned j = ib; j < ie; ++j) {
	   // libunwind can fail to unwind and deliver a bad address.
	   // If that address is equal to or exceeds the highest address
	   // possible then the Address constructor asserts.
	   Address tmp(data.stacktraces.stacktraces_val[j]);
	   if (tmp >= Address::TheHighest()) {
//DEBUG
#if 0
		std::cerr << "TOSSING address " <<  data.stacktraces.stacktraces_len[j]
		<< std::endl; 
#endif
	   } else {
		if (tmp == Address(0)) {
		    // for some reason pthreaded calltrees have an extra
		    // frame with address of 0x0.  Do not pass these on to view
		    // code.  FIXME.  look at unwind code for the real cause...
		    continue;
		}
		trace.push_back(tmp);
	   }
	}
	
	// Iterate over each of the frames in the current stack trace
	for(StackTrace::const_iterator
		j = trace.begin(); j != trace.end(); ++j) {
	    
	    // Stop after first frame if this is "exclusive_[time|detail]"
	    if(is_exclusive && (j != trace.begin()))
		break;
	    
	    // Find the subextents that contain this frame
	    std::set<ExtentGroup::size_type> intersection =
		subextents.getIntersectionWith(
		    Extent(extent.getTimeInterval(), AddressRange(*j))
		    );

	    // Iterate over each subextent in the intersection
	    for(std::set<ExtentGroup::size_type>::const_iterator
		    k = intersection.begin(); k != intersection.end(); ++k) {

		// Calculate intersection time (in nS) of subextent and blob
		double t_intersection = static_cast<double>
		    ((extent.getTimeInterval() &
		      subextents[*k].getTimeInterval()).getWidth());

		// Handle "[inclusive|exclusive]_detail" metric
		if(is_detail) {

		    // Find this stack trace in the subextent's metric value
		    SampleDetail::iterator l =
			(*reinterpret_cast<std::vector<SampleDetail>*>(ptr))[*k]
			.insert(
			    std::make_pair(trace, MPIPDetail())
			    ).first;
		    
		    // Add (to the subextent's metric value) the appropriate
		    // fraction of the count and total time attributable to
		    // this sample
		    l->second.dm_count += static_cast<uint64_t>(
			static_cast<double>(data.count.count_val[ib]) * 
			(t_intersection / t_blob)
			);
		    l->second.dm_time += t_sample * (t_intersection / t_blob);
		    
		}

		// Handle "[inclusive|exclusive]_time" metric		
		else {

		    // Add (to the subextent's metric value) the appropriate
		    // fraction of the total time attributable to this sample
		    (*reinterpret_cast<std::vector<double>*>(ptr))[*k] +=
			t_sample * (t_intersection / t_blob);
		    
		}
		
	    }
	    
	}
	
    }

    // Free the decoded data blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_mpi_profile_data),
	     reinterpret_cast<char*>(&data));
}

void MPIPCollector::getUniquePCValues( const Thread& thread,
                                      const Blob& blob,
                                      PCBuffer *buffer) const
{

    // Decode this data blob
    CBTF_mpi_profile_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_mpi_profile_data), &data);

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
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_mpi_profile_data),
	     reinterpret_cast<char*>(&data));
}

void MPIPCollector::getUniquePCValues( const Thread& thread,
                                         const Blob& blob,
                                         std::set<Address>& uaddresses) const
{

    // Decode this data blob
    CBTF_mpi_profile_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_mpi_profile_data), &data);

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
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_mpi_profile_data),
             reinterpret_cast<char*>(&data));
}
