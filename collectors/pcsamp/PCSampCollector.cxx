////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2004 Silicon Graphics, Inc. All Rights Reserved.
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
 * Definition of the PCSampCollector class.
 *
 */
 
#include "PCSampCollector.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Collector's factory method.
 *
 * Factory method for instantiating a collector implementation. This is the
 * only function that is externally visible from outside the collector plugin.
 *
 * @return    New instance of this collector's implementation.
 */
extern "C" CollectorImpl* pcsamp_LTX_CollectorFactory()
{
    return new PCSampCollector();
}



/**
 * Default constructor.
 *
 * Constructs a new PC sampling collector with the proper metadata.
 */
PCSampCollector::PCSampCollector() :
    CollectorImpl("pcsamp",
		  "PC Sampling",
		  "Periodically interupts the running process, obtains the "
		  "current program counter (PC) value, increments a running "
		  "counter associated with that value, and allows the process "
		  "to continue execution. All of this is implemented directly "
		  "in the kernel to allow for minimal perturbation of the "
		  "process' behavior.")
{
    declareParameter(Metadata("sampling_rate", "Sampling Rate",
			      "Sampling rate in samples/milliseconds.",
			      typeid(unsigned)));

    declareMetric(Metadata("time", "CPU Time",
			   "Exclusive CPU time in seconds.",
			   typeid(double)));
}



/**
 * Start data collection.
 *
 * Implement starting data collection for a particular thread. ...
 *
 * @param collector    Collector starting data collection.
 * @param thread       Thread for which to start collecting data.
 */
void PCSampCollector::startCollecting(const Collector& collector,
				      const Thread& thread) const
{
}



/**
 * Stops data collection.
 *
 * Implement stopping data collection for a particular thread. ...
 *
 * @param collector    Collector stopping data collection.
 * @param thread       Thread for which to stop collecting data.
 */
void PCSampCollector::stopCollecting(const Collector& collector,
				     const Thread& thread) const
{
}



/**
 * Get a metric value.
 *
 * Implement getting one of our metric values for a particular thread, over a
 * specific address range and time interval. ...
 *
 * @param metric      Unique identifier of the metric.
 * @param thread      Thread for which to get a value.
 * @param range       Address range over which to get a value.
 * @param interval    Time interval over which to get a value.
 * @param ptr         Untyped pointer to the return value.
 */
void PCSampCollector::getMetricValue(const std::string& metric,
				     const Thread& thread,
				     const AddressRange& range,
				     const TimeInterval& interval,
				     void* ptr) const
{
    if(metric == "time") {
	double* value = reinterpret_cast<double*>(ptr);
	*value = 0.0;  // Dummy value for now
    }
}
