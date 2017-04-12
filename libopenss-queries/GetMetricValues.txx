////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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
 * Definition of the GetMetricValues template function.
 *
 */

#ifndef _OpenSpeedShop_Queries_GetMetricValues_
#define _OpenSpeedShop_Queries_GetMetricValues_

#include "Queries.hxx"
#include "ToolAPI.hxx"

#ifdef HAVE_OPENMP
#include <omp.h>
#endif



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
 * Get metric values.
 *
 * Evaluates the individual values of the specified collector's metric, over the
 * specified time interval, for the specified source objects, in each thread of
 * the specified thread group. Results are returned in a map of source objects
 * to threads to values. An empty map is allocated if one isn't provided. Non-
 * zero metric values are then added to the (new or existing) map.
 *
 * @pre    The specified collector and all threads in the thread group must be
 *         in the same experiment. An assertion failure occurs if more than one
 *         experiment is implied.
 *
 * @pre    All the specified source objects must be from the same experiment as
 *         the specified collector. An assertion failure occurs if more than one
 *         experiment is implied.
 *
 * @param collector    Collector for which to get a metric.
 * @param metric       Unique identifier of the metric.
 * @param interval     Time interval over which to get the metric values. 
 * @param threads      Thread group for which to get metric values.
 * @param objects      Source objects for which to get metric values.
 * @retval results     Smart pointer to the results map.
 */
template <typename TS, typename TM>
void Queries::GetMetricValues(
    const Framework::Collector& collector,
    const std::string& metric,
    const Framework::TimeInterval& interval,
    const Framework::ThreadGroup& threads,
    const std::set<TS >& objects,
    Framework::SmartPtr<
        std::map<TS, std::map<Framework::Thread, TM > > >& results)
{
    // Check preconditions
    for(Framework::ThreadGroup::const_iterator
	    i = threads.begin(); i != threads.end(); ++i) {
	Assert(collector.inSameDatabase(*i));
    }
    for(typename std::set<TS >::const_iterator
	    i = objects.begin(); i != objects.end(); ++i) {
	Assert(collector.inSameDatabase(*i));
    }

    // Allocate (if necessary) a new results map
    if(results.isNull()) {
	results = 
	    Framework::SmartPtr<
                std::map<TS, std::map<Framework::Thread, TM > > 
	    >(
		new std::map<TS, std::map<Framework::Thread, TM > >()
	     );
    }
    Assert(!results.isNull());
    
    // Construct extent restricting evaluation to the requested time interval
    Framework::Extent restriction(
        interval,
        Framework::AddressRange(Framework::Address::TheLowest(),
                                Framework::Address::TheHighest())
        );
    
    // Lock the appropriate database
    collector.lockDatabase();

    // Get the extent table for the source objects in the thread group
    Framework::ExtentTable<Framework::Thread, TS > extent_table = 
	threads.getExtentsOf(objects, restriction);
    
    // Iterate over each thread in the thread group
    for(Framework::ThreadGroup::const_iterator
	    i = threads.begin(); i != threads.end(); ++i) {

	// Get the extents for the source objects in this thread
	Framework::ExtentGroup& extents = extent_table.getExtents(*i);

	// No need to proceed further with this thread if no extents were found
	if(extents.empty())
	    continue;
	
	// Allocate a vector to hold the evaluated metric values
	std::vector<TM > values(extents.size());

#ifndef HAVE_OPENMP

	// Evaluate the metric values for the necessary extents
	collector.getMetricValues(metric, *i, extents, values);

#else

	// Get the performance data blob identifiers to be evaluated
        std::set<int> temp = collector.getIdentifiers(*i, extents);
	std::vector<int> identifiers(temp.begin(), temp.end());

	// Parallel region to evaluate the metric values
	#pragma omp parallel
	{
	    // Vector holding the evaluated metric values for this thread
	    std::vector<TM > local(extents.size());
	    
            // Make a copy of the extents. This is necessary because
            // the Kd-tree contruction in ExtentGroup isn't thread safe.
            Framework::ExtentGroup copy(extents);

	    // Iterate in parallel over each performance data blob
            #pragma omp for nowait
	    for(int j = 0; j < identifiers.size(); ++j) {
		
		// Evalute the metric values for the necessary extents
		collector.getMetricValues(metric, *i, copy,
					  identifiers[j], local);

	    }
	    
	    //
	    // Partially computed metric values are now distributed between 
	    // the per-thread result vectors ("local"). A reduction of these
	    // per-thread values into the final results vector ("values") is
	    // required. Explaining how this is accomplished is best left to
	    // an example.
	    //
	    // Consider 4 threads processing a 22 element result vector. Four
	    // iterations are performed. Each thread adds a 6 element piece
	    // of its results into the final results during each iteration:
	    // 
	    //     0000000000111111111122
	    //     0123456789012345678901    Element Number
	    //     ----------------------
	    //     0000001111112222223333    Iteration #1  (j=0)
	    //     3333330000001111112222    Iteration #2  (j=1)
	    //     2222223333330000001111    Iteration #3  (j=2)   
	    //     1111112222223333330000    Iteration #4  (j=3)
	    //        ^ 
	    //        +---- Thread number writing this element
	    //              during a given iteration.
	    //

	    // Get the total number of threads and our thread number
	    int num_threads = omp_get_num_threads();
	    int thread_num = omp_get_thread_num();

	    // Compute number of elements to write during each iteration
	    int n = (values.size() + num_threads - 1) / num_threads;

	    // Perform each iteration
	    for(int j = 0; j < num_threads; ++j) {

		// First element reduced by this thread during this iteration
		int first = (n * (j + thread_num)) % (n * num_threads);

		// Perform reduction
		for(int k = 0; (k < n) && ((first + k) < values.size()); ++k)
		    values[first + k] += local[first + k];

		// Wait for all threads to finish their reduction
                #pragma omp barrier

	    }
		
	}
		
#endif
	
	// Iterate over each evaluated extent
	for(Framework::ExtentGroup::size_type j = 0; j < extents.size(); ++j) {
	    
	    // Was this subextent's evaluation a non-empty value?
	    if(values[j] != TM()) {

		// Get the source object corresponding to this evaluated extent
		const TS& object = extent_table.getObject(*i, j);
		
		// Incorporate this value into the results map
		typename std::map<TS, std::map<Framework::Thread, TM > >::
		    iterator k = results->find(object);
		if(k == results->end())
		    k = results->insert(
			std::make_pair(
			    object, std::map<Framework::Thread, TM >()
			    )
			).first;
		typename std::map<Framework::Thread, TM >::iterator
		    l = k->second.find(*i);
		if(l == k->second.end())
		    l = k->second.insert(std::make_pair(*i, TM())).first;
		l->second += values[j];
		
	    }
	    
	}

    }

    // Unlock the appropriate database
    collector.unlockDatabase();
}



}  // namespace OpenSpeedShop



#endif
