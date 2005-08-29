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
 * Definition of the GetMetricByFunction functions.
 *
 */

#include "ToolAPI.hxx"
#include "Queries.hxx"

using namespace OpenSpeedShop;
using namespace OpenSpeedShop::Framework;



/**
 * Get metric values by function in a thread.
 *
 * Gets the current list of functions in the specified thread and evalutes the
 * specified metric, in the specified collector, over the entire time interval.
 * The results are returned in a map of functions to metric values. Only those
 * functions with non-zero metric values will be present in the returned map.
 *
 * @pre    The specified collector and thread must be in the same experiment
 *         database. An assertion failure occurs if the collector and thread
 *         are in different databases.
 *
 * @param collector    Collector for which to get a metric.
 * @param metric       Unique identifier of the metric.
 * @param thread       Thread for which to get metric values.
 * @retval result      Smart pointer to a map of the thread's functions to
 *                     their metric values.
 */
void Queries::GetMetricByFunctionInThread(
    const Collector& collector, 
    const std::string& metric,
    const Thread& thread,
    SmartPtr<std::map<Function, double> >& result)
{
    // Check preconditions
    Assert(collector.inSameDatabase(thread));

    // Lock the appropriate database
    collector.lockDatabase();

    // Time interval covering earliest to latest possible time
    const TimeInterval Forever = 
	TimeInterval(Time::TheBeginning(), Time::TheEnd());
    
    // Allocate a new map of functions to doubles
    result = SmartPtr<std::map<Function, double> >(
	new std::map<Function, double>()
	);
    Assert(!result.isNull());
    
    // Get the current list of functions in this thread
    std::set<Function> functions = thread.getFunctions();
    
    // Iterate over each function
    for(std::set<Function>::const_iterator
	    i = functions.begin(); i != functions.end(); ++i) {
	
	// Get the address range of this function
	AddressRange range = i->getAddressRange();
	
	// Evalute the metric over this address range
	double value = 0.0;
	collector.getMetricValue(metric, thread, range, Forever, value);
	
	// Add this function and its metric value to the map
	if(value != 0.0)
	    result->insert(std::make_pair(*i, value));
	
    }    

    // Unlock the apropriate database
    collector.unlockDatabase();
}


void Queries::GetUIntMetricByFunctionInThread(
    const Collector& collector, 
    const std::string& metric,
    const Thread& thread,
    SmartPtr<std::map<Function, unsigned int> >& result)
{
    // Check preconditions
    Assert(collector.inSameDatabase(thread));

    // Lock the appropriate database
    collector.lockDatabase();

    // Time interval covering earliest to latest possible time
    const TimeInterval Forever = 
	TimeInterval(Time::TheBeginning(), Time::TheEnd());
    
    // Allocate a new map of functions to unsigned int
    result = SmartPtr<std::map<Function, unsigned int> >(
	new std::map<Function, unsigned int>()
	);
    Assert(!result.isNull());
    
    // Get the current list of functions in this thread
    std::set<Function> functions = thread.getFunctions();
    
    // Iterate over each function
    for(std::set<Function>::const_iterator
	    i = functions.begin(); i != functions.end(); ++i) {
	
	// Get the address range of this function
	AddressRange range = i->getAddressRange();
	
	// Evalute the metric over this address range
	unsigned int value = 0;
	collector.getMetricValue(metric, thread, range, Forever, value);
	
	// Add this function and its metric value to the map
	if(value != 0.0)
	    result->insert(std::make_pair(*i, value));
	
    }    

    // Unlock the apropriate database
    collector.unlockDatabase();
}

