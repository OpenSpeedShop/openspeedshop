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
 * Definition of the ExampleCollector class.
 *
 */
 
#include "ExampleCollector.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Collector's factory method.
 *
 * Factory method for instantiating a collector implementation. This is the
 * only function that is externally visible from outside the collector plugin.
 *
 * @return    New instance of this collector's implementation.
 */
extern "C" CollectorImpl* example_LTX_CollectorFactory()
{
    return new ExampleCollector();
}



/**
 * Default constructor.
 *
 * Constructs a new example collector with the proper metadata.
 */
ExampleCollector::ExampleCollector() :
    CollectorImpl("example",
		  "Example",
		  "Gather ...")
{
    declareParameter(Metadata("function", "Function",
			      "Function whose execution should be timed.",
			      typeid(Function)));
    
    declareMetric(Metadata("time", "CPU Time",
			   "Inclusive CPU time in seconds.",
			   typeid(double)));
}



/**
 * Get a metric value.
 *
 * Implement getting one of our metric values for a particular thread, over a
 * specific address range and time interval.
 *
 * @param metric      Unique identifier of the metric.
 * @param thread      Thread for which to get a value.
 * @param range       Address range over which to get a value.
 * @param interval    Time interval over which to get a value.
 * @param ptr         Untyped pointer to the return value.
 */
void ExampleCollector::getMetricValue(const std::string& metric,
				      const Thread& thread,
				      const AddressRange& range,
				      const TimeInterval& interval,
				      void* ptr) const
{
    if(metric == "time") {
	double* value = reinterpret_cast<double*>(ptr);
	*value = 123.456;  // Dummy value for now
    }
}
