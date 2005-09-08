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
 * Definition of the HWTimeCollector class.
 *
 */
 
#include "HWTimeCollector.hxx"
#include "blobs.h"
#include "PapiAPI.h"

using namespace std;
using namespace OpenSpeedShop::Framework;

/**
 * Collector's factory method.
 *
 * Factory method for instantiating a collector implementation. This is the
 * only function that is externally visible from outside the collector plugin.
 *
 * @return    New instance of this collector's implementation.
 */
extern "C" CollectorImpl* hwctime_LTX_CollectorFactory()
{
    return new HWTimeCollector();
}



/**
 * Default constructor.
 *
 * Constructs a new hardware time sampling collector with the proper metadata.
 */
HWTimeCollector::HWTimeCollector() :
    CollectorImpl("hwctime",
                  "Hardware Time",
                  "Periodically interrupts the running thread, obtains the "
                  "current program stack trace (addresses), stores them, and "
                  "allows the thread to continue execution.")
{
    // Declare our parameters
    declareParameter(Metadata("sampling_rate", "Sampling Rate",
                              "Sampling rate in samples/seconds.",
                              typeid(unsigned)));

    declareParameter(Metadata("event", "Hardware Time Counter Event",
                           "HWCTime event.",
                           typeid(std::string)));

    // Declare our metrics

    declareMetric(Metadata("inclusive_overflows", "Inclusive Overflows",
                           "Inclusive hwc overflow counts.",
                           typeid(uint64_t)));

    declareMetric(Metadata("exclusive_overflows", "Exclusive Overflows",
                           "Exclusive hwc overflow counts.",
                           typeid(uint64_t)));
}



/**
 * Get the default parameter values.
 *
 * Implement getting our default parameter values.
 *
 * @return    Blob containing the default parameter values.
 */
Blob HWTimeCollector::getDefaultParameterValues() const
{
    // Setup an empty parameter structure
    hwctime_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));

    hwc_init_papi();

    // Set the default parameters
    // FIXME: the default threshold below may be too large
    // for some events. May need to calculate a default
    // based on event type...

#if defined(linux)
    parameters.sampling_rate = (unsigned) hw_info->mhz*10000*2;
#else
    parameters.sampling_rate = THRESHOLD*2;
#endif

    parameters.hwctime_event = get_papi_eventcode("PAPI_TOT_CYC");

    // Return the encoded blob to the caller
    return Blob(reinterpret_cast<xdrproc_t>(xdr_hwctime_parameters),
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
void HWTimeCollector::getParameterValue(const std::string& parameter,
					const Blob& data, void* ptr) const
{
    // Decode the blob containing the parameter values
    hwctime_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_hwctime_parameters),
                        &parameters);

    hwc_init_papi();

    // Handle the "sampling_rate" parameter
    if(parameter == "sampling_rate") {
        unsigned* value = reinterpret_cast<unsigned*>(ptr);
        *value = parameters.sampling_rate;
    }

    // Handle the "hwctime_event" parameter
    if(parameter == "event") {
        std::string* value = reinterpret_cast<std::string*>(ptr);
        char* EventCodeStr = get_papi_name(parameters.hwctime_event);
        std::string ecstr(EventCodeStr);
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
void HWTimeCollector::setParameterValue(const std::string& parameter,
					const void* ptr, Blob& data) const
{
    // Decode the blob containing the parameter values
    hwctime_parameters parameters;
    memset(&parameters, 0, sizeof(parameters));
    data.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_hwctime_parameters),
                        &parameters);
    
    // Handle the "sampling_rate" parameter
    if(parameter == "sampling_rate") {
        const unsigned* value = reinterpret_cast<const unsigned*>(ptr);
        parameters.sampling_rate = *value;
    }

    // Handle the "hwctime_event" parameter
    if(parameter == "event") {

        char EventCodeStr[PAPI_MAX_STR_LEN];
        const std::string* papi_event_name =
				reinterpret_cast<const std::string*>(ptr);
        const char *str = papi_event_name->c_str();
        parameters.hwctime_event = get_papi_eventcode((char *)str);
    }
    
    // Re-encode the blob containing the parameter values
    data = Blob(reinterpret_cast<xdrproc_t>(xdr_hwctime_parameters),
                &parameters);
}


/**
 * Start data collection.
 *
 * Implement starting data collection for a particular thread.
 *
 * @param collector    Collector starting data collection.
 * @param thread       Thread for which to start collecting data.
 */
void HWTimeCollector::startCollecting(const Collector& collector,
				      const Thread& thread) const
{
    // Assemble and encode arguments to hwctime_start_sampling()
    hwctime_start_sampling_args args;
    memset(&args, 0, sizeof(args));
    collector.getParameterValue("sampling_rate", args.sampling_rate);

    std::string papi_event_name;
    collector.getParameterValue("event", papi_event_name);

    int EventCode = PAPI_NULL;
    const char* n = papi_event_name.c_str();
    EventCode = get_papi_eventcode((char *) n);
    args.hwctime_event = EventCode;

    getECT(collector, thread, args.experiment, args.collector, args.thread);
    Blob arguments(reinterpret_cast<xdrproc_t>(xdr_hwctime_start_sampling_args),
                   &args);

    // Find exit() in this thread
    std::pair<bool, Function> exit = thread.getFunctionByName("exit");

    // Execute hwctime_stop_sampling() when we enter exit() for the thread
    if(exit.first)
        executeAtEntry(collector, thread, exit.second,
                       "hwctime-rt: hwctime_stop_sampling", Blob());

    // Execute hwctime_start_sampling() in the thread
    executeNow(collector, thread, 
	       "hwctime-rt: hwctime_start_sampling", arguments);
}



/**
 * Stops data collection.
 *
 * Implement stopping data collection for a particular thread.
 *
 * @param collector    Collector stopping data collection.
 * @param thread       Thread for which to stop collecting data.
 */
void HWTimeCollector::stopCollecting(const Collector& collector,
				     const Thread& thread) const
{
    // Execute hwctime_stop_sampling() in the thread
    executeNow(collector, thread,
               "hwctime-rt: hwctime_stop_sampling", Blob());

    // Remove all instrumentation associated with this collector/thread pairing
    uninstrument(collector, thread);
}



/**
 * Get a metric value.
 *
 * Implement getting one of our metric values for a particular thread, over a
 * specific address range and time interval.
 *
 * @param metric       Unique identifier of the metric.
 * @param collector    Collector for which to get a value.
 * @param thread       Thread for which to get a value.
 * @param range        Address range over which to get a value.
 * @param interval     Time interval over which to get a value.
 * @param ptr          Untyped pointer to the return value.
 */
void HWTimeCollector::getMetricValue(const std::string& metric,
				     const Collector& collector,
				     const Thread& thread,
				     const AddressRange& range,
				     const TimeInterval& interval,
				     void* ptr) const
{
    // Handle the "overflows" metric
    if( metric == "inclusive_overflows" ||
	metric == "exclusive_overflows" ) {
        uint64_t* value = reinterpret_cast<uint64_t*>(ptr);

        // Initialize the time metric value to zero
        *value = 0;
	
        // Obtain all the data blobs applicable to the requested metric value
        std::vector<Blob> data_blobs =
            getData(collector, thread, range, interval);
	
        // Iterate over each of the data blobs
	bool top_stack_trace = true;
        for(std::vector<Blob>::const_iterator
                i = data_blobs.begin(); i != data_blobs.end(); ++i) {

            // Decode this data blob
            hwctime_data data;
            memset(&data, 0, sizeof(data));
            i->getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_hwctime_data),
                              &data);
	    
            // Check assertions ???

            // Iterate over each of the samples

            for(unsigned j = 0; j < data.bt.bt_len; j++)
	    {
		if( metric == "inclusive_overflows") {
                    // Is this PC address inside the range?
                    if(range.doesContain(data.bt.bt_val[j])) {
		    
                        // Add this sample time to the overflows metric value
//fprintf(stderr,"HWTC::getMetricValue: pc=%#lx, value=%d range %s\n",data.bt.bt_val[j],*value, std::string(range).c_str());
                        *value += static_cast<uint64_t>(data.interval);
		    }
		} else if ( metric == "exclusive_overflows") {
		    // Loop over each call stack in this BLOB
		    // if "first" PC of call stack inside of requested metric
		    // address range exclusive_time += sample time

		    if (top_stack_trace) {
			if (range.doesContain(data.bt.bt_val[j])) {
                            *value += static_cast<uint64_t>(data.interval);
			}
			top_stack_trace = false;
		    }

		    if ( !top_stack_trace && data.bt.bt_val[j] == 0) {
		        top_stack_trace = true;
		    }
		}
	    }
	    
            // Free the decoded data blob
            xdr_free(reinterpret_cast<xdrproc_t>(xdr_hwctime_data),
                     reinterpret_cast<char*>(&data));
        }
    }
}
