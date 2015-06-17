////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
// Copyright (c) 2006-2015 Krell Institute. All Rights Reserved.
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
 * Definition of the HWCCollector class.
 *
 */
 
#include "HWCCollector.hxx"
#include "blobs.h"

#include <stdlib.h>
#include <stdio.h>

#include <vector>
#include <string>
#include <iostream>
#if defined(REGISTER_HWC_EVENTS)
#include "OpenSS_Papi_Events.h"
#endif

static bool we_dun_registered = false;

#include "SS_Message_Element.hxx"
#include "SS_Message_Czar.hxx"

using namespace OpenSpeedShop;
extern SS_Message_Czar& theMessageCzar();

using namespace OpenSpeedShop::Framework;


void
hwc_register_events_for_help(void);

/**
 * Collector's factory method.
 *
 * Factory method for instantiating a collector implementation. This is the
 * only function that is externally visible from outside the collector plugin.
 *
 * @return    New instance of this collector's implementation.
 */
extern "C" CollectorImpl* hwc_LTX_CollectorFactory()
{
    return new HWCCollector();
}



/**
 * Default constructor.
 *
 * Constructs a new HWC sampling collector with the proper metadata.
 */
HWCCollector::HWCCollector() :
    CollectorImpl("hwc",
                  "HWC Sampling",
                  "Periodically interrupts the running thread, obtains the "
                  "current program counter (PC) value, stores it, and allows "
                  "the thread to continue execution.")
{
    // Declare our parameters
    declareParameter(Metadata("sampling_rate", "Sampling Threshhold",
                              "Sampling threshhold in hardware counter events.",
                              typeid(uint64_t)));

    declareParameter(Metadata("event", "Hardware Counter Event",
                           "HWC event.",
                           typeid(std::string)));

    // Declare our metrics
    // FIXME: this really should be an unsigned type. Overflows is
    // rather generic and does not give the user any feedback as to the
    // PAPI event specified (e.g. PAPI_TOT_CYC, PAPI_TOT_INS, etc...).
    declareMetric(Metadata("overflows", "Overflows",
                           "Exclusive overflows in hardware counter events.",
                           typeid(uint64_t)));
    declareMetric(Metadata("threadAverage", "Thread Average Time",
                           "Average Exclusive event counts across threads or ranks.",
                           typeid(uint64_t)));
    declareMetric(Metadata("threadMin", "Thread Minimum Time",
                           "Minimum Exclusive event counts across threads or ranks.",
                           typeid(uint64_t)));
    declareMetric(Metadata("threadMax", "Thread Maximum Time",
                           "Maximum Exclusive event counts across threads or ranks.",
                           typeid(uint64_t)));
    declareMetric(Metadata("percent", "Percent of Exclusive event counts",
                           "Percent of Exclusive event counts.",
                           typeid(double)));


#if defined(REGISTER_HWC_EVENTS)
    // Register the event names in the message facility.
    if (!we_dun_registered) {
    hwc_register_events_for_help();
    }
#endif

}



/**
 * Get the default parameter values.
 *
 * Implement getting our default parameter values.
 *
 * @return    Blob containing the default parameter values.
 */
Blob HWCCollector::getDefaultParameterValues() const
{
    // Setup an empty parameter structure
    hwc_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));

    // THRESHOLD is defined in PapiAPI.h. We do not want any
    // dependency on papi here. We define it anyways to 10000000.
    parameters.sampling_rate = 10000000*2;
    
    strncpy(parameters.hwc_event,"PAPI_TOT_CYC",strlen("PAPI_TOT_CYC"));

    // Return the encoded blob to the caller
    return Blob(reinterpret_cast<xdrproc_t>(xdr_hwc_parameters),
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
void HWCCollector::getParameterValue(const std::string& parameter,
				     const Blob& data, void* ptr) const
{
    // Decode the blob containing the parameter values
    hwc_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_hwc_parameters),
                        &parameters);
    // Handle the "sampling_rate" parameter
    if(parameter == "sampling_rate") {
        uint64_t* value = reinterpret_cast<uint64_t*>(ptr);
        *value = parameters.sampling_rate;
    }

    // Handle the "hwc_event" parameter
    if(parameter == "event") {
        std::string* value = reinterpret_cast<std::string*>(ptr);
	std::string ecstr(parameters.hwc_event);
	*value = ecstr;
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
void HWCCollector::setParameterValue(const std::string& parameter,
					const void* ptr, Blob& data) const
{
    // Decode the blob containing the parameter values
    hwc_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_hwc_parameters),
                        &parameters);
    
    // Handle the "sampling_rate" parameter
    if(parameter == "sampling_rate") {
        const uint64_t* value = reinterpret_cast<const uint64_t*>(ptr);
        parameters.sampling_rate = *value;
	std::ostringstream rate;
	rate << parameters.sampling_rate;
	setenv("OPENSS_HWC_THRESHOLD", rate.str().c_str(), 1);
    }

    // Handle the "hwc_event" parameter
    if(parameter == "event") {

	const std::string* papi_event_name =
				reinterpret_cast<const std::string*>(ptr);
	// clear any previous event preset
	memset(&parameters.hwc_event, 0, sizeof(parameters.hwc_event));
	// copy the noew event preset
	strncpy(parameters.hwc_event,papi_event_name->c_str(),
		strlen(papi_event_name->c_str()));
	setenv("OPENSS_HWC_EVENT", papi_event_name->c_str(), 1);
    }
    
    // Re-encode the blob containing the parameter values
    data = Blob(reinterpret_cast<xdrproc_t>(xdr_hwc_parameters),
                &parameters);
}



/**
 * Start data collection.
 *
 * Implement starting data collection for the specified threads.
 *
 * @param collector    Collector starting data collection.
 * @param threads      Threads for which to start collecting data.
 */
void HWCCollector::startCollecting(const Collector& collector,
				   const ThreadGroup& threads) const
{
    // Assemble and encode arguments to hwc_start_sampling()
    hwc_start_sampling_args args;
    memset(&args, 0, sizeof(args));
    collector.getParameterValue("sampling_rate", args.sampling_rate);

    std::string papi_event_name;
    collector.getParameterValue("event", papi_event_name);
    strncpy(args.hwc_event,papi_event_name.c_str(),strlen(papi_event_name.c_str()));

    args.experiment = getExperimentId(collector);
    args.collector = getCollectorId(collector);
    Blob arguments(reinterpret_cast<xdrproc_t>(xdr_hwc_start_sampling_args),
                   &args);

    // Execute hwc_stop_sampling() before we exit the threads
    executeBeforeExit(collector, threads,
		      "hwc-rt: hwc_stop_sampling", Blob());
    
    // Execute hwc_start_sampling() in the threads
    executeNow(collector, threads, 
	       "hwc-rt: hwc_start_sampling", arguments);
}



/**
 * Stops data collection.
 *
 * Implement stopping data collection for the specified threads.
 *
 * @param collector    Collector stopping data collection.
 * @param threads      Threads for which to stop collecting data.
 */
void HWCCollector::stopCollecting(const Collector& collector,
				  const ThreadGroup& threads) const
{
    // Execute hwc_start_sampling() in the threads
    executeNow(collector, threads,
	       "hwc-rt: hwc_stop_sampling", Blob());

    // Remove instrumentation associated with this collector/threads pairing
    uninstrument(collector, threads);
}



/**
 * Get a metric value.
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
 *
 */
void HWCCollector::getMetricValues(const std::string& metric,
				   const Collector& collector,
				   const Thread& thread,
				   const Extent& extent,
				   const Blob& blob,
				   const ExtentGroup& subextents,
				   void* ptr) const
{
    // Handle the "overflows" metric
    // Check assertions
    Assert(metric == "overflows");

    // Cast the untyped pointer into a vector of doubles
    std::vector<uint64_t>* values =
				reinterpret_cast<std::vector<uint64_t>*>(ptr);
    
    // Check assertions
    Assert(values->size() >= subextents.size());

    // Decode this data blob
    hwc_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_hwc_data), &data);
    
    // Check assertions
    Assert(data.pc.pc_len == data.count.count_len);

    // Calculate time (in nS) of data blob's extent
    // Use a double here same as hwctime does.
    //uint64_t t_blob = static_cast<uint64_t>(extent.getTimeInterval().getWidth());
    double t_blob = static_cast<double>(extent.getTimeInterval().getWidth());

    // Iterate over each of the samples
    for(unsigned i = 0; i < data.pc.pc_len; ++i) {
	
	// Find the subextents that contain this sample
	std::set<ExtentGroup::size_type> intersection = 
	    subextents.getIntersectionWith(
		Extent(extent.getTimeInterval(),
		       AddressRange(data.pc.pc_val[i]))
		);
	
	// Calculate the time (in seconds) attributable to this sample
	uint64_t t_sample = static_cast<uint64_t>(data.count.count_val[i]) *
			    static_cast<uint64_t>(data.interval);
	
	// Iterate over each subextent in the intersection
	for(std::set<ExtentGroup::size_type>::const_iterator
		j = intersection.begin(); j != intersection.end(); ++j) {
	    
	    // Calculate intersection time (in nS) of subextent and data blob
	    double t_intersection = static_cast<double>
		((extent.getTimeInterval() & 
		subextents[*j].getTimeInterval()).getWidth());	    


	    // Add (to the subextent's metric value) the appropriate fraction
	    // of the total counts attributable to this sample.  Never let
	    // this be a value if 0.  If t_sample has a value > 0 then for
	    // hwc, the fraction for t_intersection / t_blob if less than
	    // 1 will be 0 due to the types used here.  Remember, the values
	    // are counts at a threshold and therfore will always be some
	    // integer multiple of that threshold.

	    uint64_t multiplier = static_cast<uint64_t>(t_intersection / t_blob);
	    if (multiplier == 0 ) {
		multiplier = 1;
	    }
	    (*values)[*j] += t_sample * multiplier;
	}
	
    }
    
    // Free the decoded data blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_hwc_data),
	     reinterpret_cast<char*>(&data));

}


#if defined(REGISTER_HWC_EVENTS)
typedef std::pair<std::string, std::string> papi_preset_event;
typedef std::vector<papi_preset_event> papi_available_presets;

/**
 * Method: hwc_register_events_for_help()
 * 
 * We will only know at runtime what events are available
 * for a given platform. This is where we register them
 * with the message facility.
 *     
 * @param   xxx - xxx.
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
void
hwc_register_events_for_help()
{
    std::vector<papi_preset_event> papi_available_presets;

    if (we_dun_registered)
    	return;

  // Send information to the message czar.
    SS_Message_Czar& czar = theMessageCzar();

    SS_Message_Element element;

   // Set keyword. You really need to have a keyword.
    element.set_keyword("hwc_events");

   // Related keywords
    element.add_related("hwc");
    element.add_related("hwc_time");

   // General topic
    element.set_topic("expType");

   // Brief, one line description
    element.set_brief("Available hardware counter events");

   // More than one line description
    element.add_normal(" ");

   // A wordy explaination
    element.add_verbose("Available hardware counter events"":\n\n");

   // Standard example for views.
    element.add_example("\t expsetparam event=PAPI_L1_DCM \n ");


    papi_available_presets.clear();

    std::pair<std::string, std::string> *pe;
    for(unsigned i = 0; OpenSS_Papi_NonDerivedEvents[i]  != NULL; ++i) {
	std::string ev = OpenSS_Papi_NonDerivedEvents[i];

	char delimitor = ':';
	std::string::size_type start = 0;
	std::string::size_type end;

	while ((end =  ev.find_first_of(delimitor,start)) != std::string::npos) {
	    pe = new std::pair<std::string, std::string>
		(std::string(ev,start,end-start),
		std::string(ev,end+1,std::string::npos));
	    papi_available_presets.push_back(*pe);
	    start = end+1;
	}
    }

    
    std::vector<papi_preset_event>::const_iterator it;
    for(    it = papi_available_presets.begin(); 
    	    it != papi_available_presets.end(); 
	    ++it)
    {
    	std::string t_string(it->first);
	t_string+="  ";
	t_string+=it->second;
    	element.add_normal(t_string);
    	element.add_verbose(t_string);

    }
    
   // Submit the message to the database
    czar.Add_Help(element);

    we_dun_registered = true;
}
#endif



void HWCCollector::getUniquePCValues( const Thread& thread,
					 const Blob& blob,
					 PCBuffer *buffer) const
{
    // Decode this data blob
    hwc_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_hwc_data), &data);

    // Check assertions
    // Assert(data.pc.pc_len == data.count.count_len);
    if (data.pc.pc_len != data.count.count_len) {
        std::cerr << "ASSERT getPCValues pc_len "
	    << data.pc.pc_len
	    << " != count_len "
	    << data.count.count_len << std::endl;
    } else {
    }

    // Iterate over each of the samples
    for(unsigned i = 0; i < data.pc.pc_len; ++i) {
        UpdatePCBuffer(data.pc.pc_val[i], buffer);
    }
    
    // Free the decoded data blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_hwc_data),
	     reinterpret_cast<char*>(&data));
}

void HWCCollector::getUniquePCValues( const Thread& thread,
                                         const Blob& blob,
                                         std::set<Address>& uaddresses) const
{

    // Decode this data blob
    hwc_data data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_hwc_data), &data);

    // Check assertions
    // Assert(data.pc.pc_len == data.count.count_len);
    if (data.pc.pc_len != data.count.count_len) {
        std::cerr << "ASSERT getPCValues pc_len "
	    << data.pc.pc_len
	    << " != count_len "
	    << data.count.count_len << std::endl;
    } else {
    }

    // Iterate over each of the samples
    for(unsigned i = 0; i < data.pc.pc_len; ++i) {
	uaddresses.insert(Address(data.pc.pc_val[i]));
    }

    // Free the decoded data blob
    xdr_free(reinterpret_cast<xdrproc_t>(xdr_hwc_data),
             reinterpret_cast<char*>(&data));
}
