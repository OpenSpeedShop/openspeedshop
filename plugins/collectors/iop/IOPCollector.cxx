////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 The Krell Institute. All Rights Reserved.
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
 * Definition of the IOPCollector class.
 *
 */
 
#include "IOPCollector.hxx"
#include "IOPDetail.hxx"

#include "KrellInstitute/Messages/IO.h"
#include "KrellInstitute/Messages/IO_data.h"

using namespace OpenSpeedShop::Framework;



namespace {

    
    /** Type returned for the sample detail metrics. */
    typedef std::map<StackTrace, IOPDetail> SampleDetail;


    /**
     * Traceable function table.
     *
     * Table listing the traceable I/O functions. In order for an I/O function
     * to actually be traceable, corresponding wrapper(s) must first be written
     * and compiled into this collector's runtime.
     *
     * @note    A function's index position in this table is directly used as
     *          its index position in the io_parameters.traced array. Thus
     *          the order the functions are listed here is significant. If it
     *          is changed, users will find that any saved databases suddenly
     *          trace different I/O functions than they did previously.
     */
    #include "IOPTraceableFunctions.h"

    
}    



/**
 * Collector's factory method.
 *
 * Factory method for instantiating a collector implementation. This is the
 * only function that is externally visible from outside the collector plugin.
 *
 * @return    New instance of this collector's implementation.
 */
extern "C" CollectorImpl* iop_LTX_CollectorFactory()
{
    return new IOPCollector();
}



/**
 * Default constructor.
 *
 * Constructs a new I/O collector with the proper metadata.
 */
IOPCollector::IOPCollector() :
    CollectorImpl("iop",
                  "I/O Profiling",
		  "Intercepts all calls to I/O functions that perform any "
		  "significant amount of work (primarily those that read/write "
		  "data) and records for each call, the current stack trace "
		  "and time spent in call.")
{
    // Declare our parameters
    declareParameter(Metadata("traced_functions", "Traced Functions",
			      "Set of I/O functions to be traced.",
			      typeid(std::map<std::string, bool>)));
    
    // Declare our metrics
    declareMetric(Metadata("inclusive_time", "Inclusive Time",
			   "Inclusive I/O call times in seconds.",
			   typeid(double)));
    declareMetric(Metadata("exclusive_time", "Exclusive Time",
			   "Exclusive I/O call times in seconds.",
			   typeid(double)));
    declareMetric(Metadata("inclusive_detail", "Inclusive Detail",
			   "Inclusive I/O call details.",
			   typeid(SampleDetail)));
    declareMetric(Metadata("exclusive_detail", "Exclusive Detail",
			   "Exclusive I/O call details.",
			   typeid(SampleDetail)));
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
}



/**
 * Get the default parameter values.
 *
 * Implement getting our default parameter values.
 *
 * @return    Blob containing the default parameter values.
 */
Blob IOPCollector::getDefaultParameterValues() const
{
    // Setup an empty parameter structure    
    CBTF_io_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));

    // Set the default parameters
    for(unsigned i = 0; TraceableFunctions[i] != NULL; ++i)
	parameters.traced[i] = true;
    
    // Return the encoded blob to the caller
    return Blob(reinterpret_cast<xdrproc_t>(xdr_CBTF_io_parameters), &parameters);
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
void IOPCollector::getParameterValue(const std::string& parameter,
				    const Blob& data, void* ptr) const
{
    // Decode the blob containing the parameter values
    CBTF_io_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_io_parameters),
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
void IOPCollector::setParameterValue(const std::string& parameter,
				    const void* ptr, Blob& data) const
{
    // Decode the blob containing the parameter values
    CBTF_io_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_io_parameters),
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
            setenv("CBTF_IO_TRACED", (char *)env_param.c_str(), 1);
        }
    }
    
    // Re-encode the blob containing the parameter values
    data = Blob(reinterpret_cast<xdrproc_t>(xdr_CBTF_io_parameters), &parameters);
}



/**
 * Start data collection.
 *
 * Implement starting data collection for the specified threads.
 *
 * @param collector    Collector starting data collection.
 * @param threads      Threads for which to start collecting data.
 */
void IOPCollector::startCollecting(const Collector& collector,
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
void IOPCollector::stopCollecting(const Collector& collector,
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

void IOPCollector::getMetricValues(const std::string& metric,
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
    CBTF_io_profile_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_io_profile_data), &data);


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
			    std::make_pair(trace, IOPDetail())
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
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_io_profile_data),
	     reinterpret_cast<char*>(&data));
}


void IOPCollector::getUniquePCValues( const Thread& thread,
                                     const Blob& blob,
                                     PCBuffer *buffer) const
{

    // Decode this data blob
    CBTF_io_profile_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_io_profile_data), &data);

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
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_io_profile_data),
	     reinterpret_cast<char*>(&data));
}


void IOPCollector::getUniquePCValues( const Thread& thread,
                                         const Blob& blob,
                                         std::set<Address>& uaddresses) const
{

    // Decode this data blob
    CBTF_io_profile_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_CBTF_io_profile_data), &data);

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
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_CBTF_io_profile_data),
             reinterpret_cast<char*>(&data));
}
