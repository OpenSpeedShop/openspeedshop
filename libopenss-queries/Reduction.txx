////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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
 * Definition of the Reduction namespace.
 *
 */

#ifndef _OpenSpeedShop_Queries_Reduction_
#define _OpenSpeedShop_Queries_Reduction_

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
 * Summation reduction operator.
 *
 * Compute the summation of the specified per-thread individual metric values.
 *
 * @param individual    Per-thread individual metric values.
 * @return              Summation of these individual values.
 */
template <typename TM>
TM Queries::Reduction::Summation(
    const std::map<Framework::Thread, TM >& individual)
{
    TM value = TM();
    for(typename std::map<Framework::Thread, TM >::const_iterator
	    i = individual.begin(); i != individual.end(); ++i)
	value += i->second;   
    return value;
}



/**
 * Arithmetic mean reduction operator.
 *
 * Compute the arithmetic mean (traditional average) of the specified per-thread
 * individual metric values.
 *
 * @param individual    Per-thread individual metric values.
 * @return              Arithmetic mean of these individual values.
 */
template <typename TM>
TM Queries::Reduction::ArithmeticMean(
    const std::map<Framework::Thread, TM >& individual)
{
    Assert(!individual.empty());
    TM value = TM();
    for(typename std::map<Framework::Thread, TM >::const_iterator
	    i = individual.begin(); i != individual.end(); ++i)
	value += i->second;   
    return value / individual.size();
}



/**
 * Minimum reduction operator.
 *
 * Compute the minimum of the specified per-thread individual metric values.
 *
 * @param individual    Per-thread individual metric values.
 * @return              Minimum of these individual values.
 */
template <typename TM>
TM Queries::Reduction::Minimum(
    const std::map<Framework::Thread, TM >& individual)
{
    Assert(!individual.empty());
    TM value = individual.begin()->second;
    for(typename std::map<Framework::Thread, TM >::const_iterator
	    i = individual.begin(); i != individual.end(); ++i)
	value = std::min(value, i->second);
    return value;
}



/**
 * Maximum reduction operator.
 *
 * Compute the maximum of the specified per-thread individual metric values.
 *
 * @param individual    Per-thread individual metric values.
 * @return              Maximum of these individual values.
 */
template <typename TM>
TM Queries::Reduction::Maximum(
    const std::map<Framework::Thread, TM >& individual)
{
    Assert(!individual.empty());
    TM value = individual.begin()->second;
    for(typename std::map<Framework::Thread, TM >::const_iterator
	    i = individual.begin(); i != individual.end(); ++i)
	value = std::max(value, i->second);
    return value;
}



/**
 * Apply reduction.
 *
 * Apply a reduction to the specified individual metric results by computing
 * the specified reduction operator for each source object across all threads
 * and returning the results.
 *
 * @pre    The specified individual metric results smart pointer must be valid.
 *         An assertion failure occurs if this pointer is null.
 *
 * @pre    The reduction operator pointer must be valid. An assertion failure
 *         occurs if this pointer is null.
 *
 * @param individual    Smart pointer to the individual metric results.
 * @param reduction     Pointer to the reduction operator to be applied.
 * @return              Smart pointer to the results map.
 */
template <typename TS, typename TM>
Framework::SmartPtr<std::map<TS, TM > > Queries::Reduction::Apply(
    const Framework::SmartPtr<
        std::map<TS, std::map<Framework::Thread, TM > > >& individual,
    TM (*reduction)(const std::map<Framework::Thread, TM >&))
{
    // Check preconditions
    Assert(!individual.isNull());
    Assert(reduction != NULL);

    // Allocate a new results map
    Framework::SmartPtr<std::map<TS, TM > > results =
	Framework::SmartPtr<std::map<TS, TM > >(new std::map<TS, TM >());
    Assert(!results.isNull());
    
    // Iterate over each source object in the individual results
    for(typename std::map<TS, std::map<Framework::Thread, TM > >::const_iterator
	    i = individual->begin(); i != individual->end(); ++i) {

	// Insert the value of the reduction operator into the results map
	results->insert(std::make_pair(i->first, (*reduction)(i->second)));

    }
    
    // Return the results map to the caller
    return results;
}



/**
 * Get total.
 *
 * Returns the total of all the metric values in the specified results.
 *
 * @pre    The specified individual metric results smart pointer must be valid.
 *         An assertion failure occurs if this pointer is null.
 *
 * @param results    Smart pointer to the results for which to get a total.
 * @return           Total of all metric values in those results.
 */
template <typename TS, typename TM>
TM Queries::Reduction::GetTotal(
    const Framework::SmartPtr<std::map<TS, TM > >& results)
{
    TM total = TM();

    // Check preconditions
    Assert(!results.isNull());  

    // Iterate over each source object in the results
    for(typename std::map<TS, TM >::const_iterator
	    i = results->begin(); i != results->end(); ++i) {

	// Add this value to the total
	total += i->second;   
    
    }
    
    // Return the total to the caller
    return total;
}



/**
 * Get subset.
 *
 * Return from the specified individual metric results the subset of those
 * results that are for a thread within the specified thread group.
 *
 * @pre    The specified individual metric results smart pointer must be valid.
 *         An assertion failure occurs if this pointer is null.
 *
 * @param individual    Smart pointer to the individual metric results.
 * @param threads       Threads for which to get results subset.
 * @return              Smart pointer to the results map.
 */
template <typename TS, typename TM>
Framework::SmartPtr<std::map<TS, std::map<Framework::Thread, TM > > >
Queries::Reduction::GetSubset(
    const Framework::SmartPtr<
        std::map<TS, std::map<Framework::Thread, TM > > >& individual,
    const Framework::ThreadGroup& threads)
{
    // Check preconditions
    Assert(!individual.isNull());

    // Allocate a new results map
    Framework::SmartPtr<std::map<TS, std::map<Framework::Thread, TM > > >
	results =
	Framework::SmartPtr<std::map<TS, std::map<Framework::Thread, TM > > >(
	    new std::map<TS, std::map<Framework::Thread, TM > >()
	    );
    Assert(!results.isNull());

    // Iterate over each source object in the individual results
    for(typename std::map<TS, std::map<Framework::Thread, TM > >::const_iterator
	    i = individual->begin(); i != individual->end(); ++i) {
	
	// Iterate over each per-thread metric value
	for(typename std::map<Framework::Thread, TM >::const_iterator
		j = i->second.begin(); j != i->second.end(); ++j) {

	    // Is this thread in the thread group?
	    if(threads.find(j->first) != threads.end()) {

		// Put this value into the results map
		typename std::map<TS, std::map<Framework::Thread, TM > >::
		    iterator k = results->find(i->first);
		if(k == results->end())
		    k = results->insert(
			std::make_pair(
			    i->first, std::map<Framework::Thread, TM >()
			    )
			).first;
		k->second.insert(std::make_pair(j->first, j->second));
		
	    }
	    
	}

    }

    // Return the results to the caller
    return results;
}



}  // namespace OpenSpeedShop



#endif
