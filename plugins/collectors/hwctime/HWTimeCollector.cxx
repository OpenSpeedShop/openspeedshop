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
	char EventCodeStr[PAPI_MAX_STR_LEN];
	get_papi_name(parameters.hwctime_event,EventCodeStr);
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

    // Execute hwctime_stop_sampling() when we enter exit() for the thread
    executeAtEntry(collector, thread,
                   "exit", "hwctime-rt: hwctime_stop_sampling", Blob());

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
 * Implement getting one of our metric values over all subextents
 * of the specified extent for a particular thread, for one of the collected
 * performance data blobs.
 *
 * @param metric        Unique identifier of the metric.
 * @param extent        Extent of the performance data blob.
 * @param blob          Blob containing the performance data.
 * @param subextents    Subextents for which to get values.
 * @retval ptr          Untyped pointer to the values of the metric.
 *
 */
void HWTimeCollector::getMetricValues(const std::string& metric,
                                      const Extent& extent,
                                      const Blob& blob,
                                      const ExtentGroup& subextents,
				      void* ptr) const
{
    // Handle the "overflows" metric

    bool incloverflows = (metric == "inclusive_overflows") ? true : false;
    bool excloverflows = (metric == "exclusive_overflows") ? true : false;

    if( incloverflows || excloverflows ) {
        // Cast the untyped pointer into a vector of uint64_t
        std::vector<uint64_t>* values =
                                reinterpret_cast<std::vector<uint64_t>*>(ptr);

        // Check assertions
        Assert(values->size() >= subextents.size());

        // Decode this data blob
        hwctime_data data;
        memset(&data, 0, sizeof(data));
        blob.getXDRDecoding(reinterpret_cast<xdrproc_t>(xdr_hwctime_data),&data
);

        // Check assertions
        // Assert(data.bt.bt_len == data.count.count_len);

        // Calculate time (in nS) of data blob's extent
        uint64_t t_blob =
                static_cast<uint64_t>(extent.getTimeInterval().getWidth());

        // Iterate over each of the samples

        bool top_stack_trace = true;
        bool add_to_values = true;

        for(unsigned i = 0; i < data.bt.bt_len; i++)
        {

            // Find the subextents that contain this sample
            std::set<ExtentGroup::size_type> intersection =
                subextents.getIntersectionWith(
                    Extent(extent.getTimeInterval(),
                       AddressRange(data.bt.bt_val[i])) );

            // Calculate the time (in seconds) attributable to this sample
            uint64_t t_sample = static_cast<uint64_t>(data.interval);

            // The boolean add_to_values is used to determine if we
            // include the computed t_sample in values.
            // incloverflows: always add each t_sample (stack frame) to values.
            // excloverflows: only add t_sample for top stack frame to values.
            if ( excloverflows) {

                if (top_stack_trace) {
                    // "first" PC of call stack, toggle add_to_values to true
                    // and toggle top_stack_trace to false till next new stack.
                    add_to_values = true;
                    top_stack_trace = false;
                } else {
                    // t_sample for this stack frame not added to values.
                    add_to_values = false;
                }

                if ( !top_stack_trace && data.bt.bt_val[i] == 0) {
                    // reached end of stack marker. Next t_sample will
                    // be top of new stack.
                    top_stack_trace = true;
                }
            }

            // add_to_values always true for incloverflows...
            if (add_to_values) {
                // Iterate over each subextent in the intersection
                for(std::set<ExtentGroup::size_type>::const_iterator
                    j = intersection.begin(); j != intersection.end(); ++j) {

                    // Calculate intersection time (in nS) of subextent and
                    // data blob
                    uint64_t t_intersection = static_cast<uint64_t>
                        ((extent.getTimeInterval() &
                          subextents[*j].getTimeInterval()).getWidth());


                    // Add (to the subextent's metric value) the appropriate
                    // fraction of the total time attributable to this sample
                    (*values)[*j] += t_sample * (t_intersection / t_blob);

                } // end for subextent
            }

        } // end for samples

        // Free the decoded data blob
        xdr_free(reinterpret_cast<xdrproc_t>(xdr_hwctime_data),
                 reinterpret_cast<char*>(&data));
    }
}
