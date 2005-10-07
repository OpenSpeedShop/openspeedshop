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
 * Definition of the GetMetricInThread template function.
 *
 */

#ifndef _OpenSpeedShop_Queries_GetMetricInThread_
#define _OpenSpeedShop_Queries_GetMetricInThread_

#include "Queries.hxx"
#include "ToolAPI.hxx"



//
// To avoid requiring fully qualified names be used everywhere, something like:
//
//     using namespace OpenSpeedShop;
//     using namespace OpenSpeedShop::Framework;
//
// would normally be placed near the top of this source file. Here, however,
// that particular formulation doesn't work well. This file contains template
// definitions that are usually included directly into other source files. By
// using the above formulation, those source files would be required to have
// the same "using" clauses. And doing so would pretty much negate the whole
// reason for using namespaces in the first place. The solution is to place
// the definitions directly inside the OpenSpeedShop namespace and only
// qualify names with Framework:: where necessary.
//
namespace OpenSpeedShop {



/**
 * Get metric values in a thread.
 *
 * Evalutes the specified collector's metric, over the specified time interval,
 * for the specified source objects in a thread. Results are provided in a map
 * of source objects to metric values. An initially-empty map is allocated if
 * one is not provided. Source objects with non-zero metric values will then be
 * added into the (new or existing) map.
 *
 * @pre    The specified collector and thread must be in the same experiment. An
 *         assertion failure occurs if the collector and thread are in different
 *         experiments.
 *
 * @param collector    Collector for which to get a metric.
 * @param metric       Unique identifier of the metric.
 * @param interval     Time interval over which to get the metric values. 
 * @param thread       Thread for which to get metric values.
 * @param objects      Source objects for which to get metric values.
 * @retval result      Smart pointer to a map of the source objects to their
 *                     metric values. A new map is allocated if a null smart
 *                     pointer is passed.
 */
template <typename TS, typename TM>
void Queries::GetMetricInThread(
    const Framework::Collector& collector,
    const std::string& metric,
    const Framework::TimeInterval& interval,
    const Framework::Thread& thread,
    const std::set<TS >& objects,
    Framework::SmartPtr<std::map<TS, TM > >& result)
{
    // Check preconditions
    Assert(collector.inSameDatabase(thread));
    
    // Allocate (if necessary) a new map of source objects to values
    if(result.isNull())
	result = Framework::SmartPtr<std::map<TS, TM > >(
	    new std::map<TS, TM >()
	    );
    Assert(!result.isNull());
    
    // Construct extent restricting evaluation to requested time interval
    Framework::Extent restriction(
        interval,
        Framework::AddressRange(Framework::Address::TheLowest(),
                                Framework::Address::TheHighest())
        );

    // Extent group containing the subextents being evaluated
    Framework::ExtentGroup subextents;
    
    // Direct-indexed map of evaluated subextents to their source objects
    std::vector<TS > subextent_to_object;
    
    // Lock the appropriate database
    collector.lockDatabase();
    
    // Iterate over each source object
    for(typename std::set<TS >::const_iterator
	    i = objects.begin(); i != objects.end(); ++i) {
	
	// Iterate over each subextent of this source object in this thread
	Framework::ExtentGroup extent = i->getExtentIn(thread);
	for(Framework::ExtentGroup::const_iterator
		j = extent.begin(); j != extent.end(); ++j) {
	    
	    // Restrict this subextent to the requested interval
	    Framework::Extent restricted = *j & restriction;
	    
	    // Add this subextent (if it isn't empty) to the evaluation list
	    if(!restricted.isEmpty()) {
		subextents.push_back(restricted);
		subextent_to_object.push_back(*i);		
	    }
	    
	}

    }

    // Evalute the metric values for the necessary subextents
    std::vector<TM > values;    
    collector.getMetricValues(metric, thread, subextents, values);

    // Iterate over each evaluated subextent
    for(Framework::ExtentGroup::size_type i = 0; i < subextents.size(); ++i) {

	// Was this subextent's evaluation a non-empty value?
	if(values[i] != TM()) {

	    // Add this value into the results
	    if(result->find(subextent_to_object[i]) == result->end())
		result->insert(std::make_pair(subextent_to_object[i],
					      values[i]));
	    else
		(*result)[subextent_to_object[i]] += values[i];
	    
	}

    }
    
    // Unlock the appropriate database
    collector.unlockDatabase();
}



}  // namespace OpenSpeedShop



#endif
