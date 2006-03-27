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
 * Definition of the GetMetricOfAllInThreadGroup template function.
 *
 */

#ifndef _OpenSpeedShop_Queries_GetMetricOfAllInThreadGroup_
#define _OpenSpeedShop_Queries_GetMetricOfAllInThreadGroup_

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
 * Get metric values of all source objects in a thread group.
 *
 * Evalutes the specified collector's metric, over the specified time interval,
 * for all the source objects, totaled over the specified thread group. Results
 * are provided in a map of source objects to metric values. An initially-empty
 * map is allocated if one is not provided. Source objects with non-zero metric
 * values will then be added into the (new or existing) map.
 *
 * @param collector    Collector for which to get a metric.
 * @param metric       Unique identifier of the metric.
 * @param interval     Time interval over which to get the metric values. 
 * @param threads      Thread group for which to get metric values.
 * @retval result      Smart pointer to a map of the source objects to their
 *                     metric values. A new map is allocated if a null smart
 *                     pointer is passed.
 */
template <typename TS, typename TM>
void Queries::GetMetricOfAllInThreadGroup(
    const Framework::Collector& collector,
    const std::string& metric,
    const Framework::TimeInterval& interval,
    const Framework::ThreadGroup& threads,
    Framework::SmartPtr<std::map<TS, TM > >& result)
{
    // Build the set of all source objects in this thread group
    std::set<TS > objects;
    for(Framework::ThreadGroup::const_iterator
	    i = threads.begin(); i != threads.end(); ++i) {
	std::set<TS > per_thread_objects;
	GetSourceObjects(*i, per_thread_objects);
	objects.insert(per_thread_objects.begin(), per_thread_objects.end());
    }
    
    // Get the metric values for these source objects in the thread group
    GetMetricInThreadGroup(collector, metric, interval, 
			   threads, objects, result);
}



}  // namespace OpenSpeedShop



#endif
