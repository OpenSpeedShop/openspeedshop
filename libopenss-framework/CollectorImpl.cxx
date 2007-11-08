////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
 * Definition of the CollectorImpl class.
 *
 */

#include "Collector.hxx"
#include "CollectorImpl.hxx"
#include "DataQueues.hxx"
#include "EntrySpy.hxx"
#include "Instrumentor.hxx"
#include "Thread.hxx"
#include "ThreadGroup.hxx"
#include "Function.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Get experiment identifier.
 *
 * Get the experiment identifier for the specified collector. Called by derived
 * classes to provide this information to their runtime library, allowing data
 * to be placed into the correct database.
 *
 * @param collector    Collector to be identified.
 * @return             Identifier of the experiment.
 */
int CollectorImpl::getExperimentId(const Collector& collector)
{
    return DataQueues::getDatabaseIdentifier(EntrySpy(collector).getDatabase());
}



/**
 * Get collector identifier.
 *
 * Get the collector identifier for the specified collector. Called by derived
 * classes to provide this information to their runtime library, allowing data
 * to be placed into the correct database.
 *
 * @param collector    Collector to be identified.
 * @return             Identifier of the collector.
 */
int CollectorImpl::getCollectorId(const Collector& collector)
{
    return EntrySpy(collector).getEntry();
}



/**
 * Get our parameters.
 *
 * Returns the metadata for all parameters of this collector implementation. An
 * empty set is returned if this implementation has no parameters.
 *
 * @return    Metadata for all parameters of this collector implementation.
 */
const std::set<Metadata>& CollectorImpl::getParameters() const
{
    // Return metadata for our parameters to the caller
    return dm_parameters;
}



/**
 * Get our metrics.
 *
 * Returns the metadata for all metrics of this collector implementation. An
 * empty set is returned if this implementation has no metrics (unlikely).
 *
 * @return    Metadata for all metrics of this collector implementation.
 */
const std::set<Metadata>& CollectorImpl::getMetrics() const
{
    // Return metadata for our metrics to the caller
    return dm_metrics;
}



/**
 * Constructor from metadata.
 *
 * Constructs a collector implementation with the given metadata. The created
 * collector implementation has no parameters or metrics.
 *
 * @param unique_id      Unique identifier for this collector.
 * @param short_name     Short name of this collector.
 * @param description    More verbose description of this collector.
 */
CollectorImpl::CollectorImpl(const std::string& unique_id,
			     const std::string& short_name,
			     const std::string& description) :
    Metadata(unique_id, short_name, description, typeid(*this)),
    dm_parameters(),
    dm_metrics()
{
}



/**
 * Declare a parameter.
 *
 * Declares a parameter by adding it to this collector implementation. Called by
 * derived classes when specifying their parameters.
 *
 * @pre    Each parameter in a collector must be unique. An assertion failure
 *         occurs if an attempt is made to redeclare a parameter.
 *
 * @param parameter    Parameter to be declareed.
 */
void CollectorImpl::declareParameter(const Metadata& parameter)
{
    // Check preconditions
    Assert(dm_parameters.find(parameter) == dm_parameters.end());
    
    // Add the new parameter
    dm_parameters.insert(parameter);
}



/**
 * Declare a metric.
 *
 * Declares a metric by adding it to this collector implementation. Called by
 * derived classes when specifying their metrics.
 *
 * @pre    Each metric in a collector must be unique. An assertion failure
 *         occurs if an attempt is made to redeclare a metric.
 *
 * @param metric    Metric to be declareed.
 */
void CollectorImpl::declareMetric(const Metadata& metric)
{
    // Check preconditions
    Assert(dm_metrics.find(metric) == dm_metrics.end());
    
    // Add the new metric
    dm_metrics.insert(metric);
}



/**
 * Execute library function now.
 *
 * Immediately execute the specified library function in the specified threads.
 * Called by derived classes to execute functions in their runtime library.
 *
 * @param collector    Collector requesting the execution.
 * @param threads      Threads in which the function should be executed.
 * @param callee       Name of the library function to be executed.
 * @param argument     Blob argument to the function.
 */
void CollectorImpl::executeNow(const Collector& collector,
			       const ThreadGroup& threads,
			       const std::string& callee,
			       const Blob& argument,
			       const bool& disabelSaveFPR) const
{
    Instrumentor::executeNow(threads, collector, callee, argument,
			     disabelSaveFPR);
}



/**
 * Execute library function at another function's entry.
 *
 * Executes the specified library function every time another function's entry
 * is executed in the specified threads. Called by derived classes to execute
 * functions in their runtime library.
 *
 * @param collector    Collector requesting the execution.
 * @param threads      Threads in which the function should be executed.
 * @param where        Name of the function at whose entry the library function
 *                     should be executed.
 * @param callee       Name of the library function to be executed.
 * @param argument     Blob argument to the function.
 */
void CollectorImpl::executeAtEntry(const Collector& collector,
				   const ThreadGroup& threads,
				   const std::string& where,
				   const std::string& callee, 
				   const Blob& argument) const
{
    Instrumentor::executeAtEntryOrExit(threads, collector, where, true,
				       callee, argument);
}



/**
 * Execute library function at another function's exit.
 *
 * Executes the specified library function every time another function's exit
 * is executed in the specified threads. Called by derived classes to execute
 * functions in their runtime library.
 *
 * @param collector    Collector requesting the execution.
 * @param threads      Threads in which the function should be executed.
 * @param where        Name of the function at whose exit the library function
 *                     should be executed.
 * @param callee       Name of the library function to be executed.
 * @param argument     Blob argument to the function.
 */
void CollectorImpl::executeAtExit(const Collector& collector,
				  const ThreadGroup& threads,
				  const std::string& where,
				  const std::string& callee, 
				  const Blob& argument) const
{
    Instrumentor::executeAtEntryOrExit(threads, collector, where, false,
				       callee, argument);
}



/**
 * Execute a library function in place of another function.
 *
 * Executes the specified library function in place of another function every
 * other time that other function is called. Called by derived classes to create
 * wrappers around functions for the purposes of gathering performance data on
 * their execution.
 *
 * @param collector    Collector requesting the execution.
 * @param threads      Threads in which the function should be executed.
 * @param where        Name of the function to be replaced with the library
 *                     function.
 * @param callee       Name of the library function to be executed.
 */
void CollectorImpl::executeInPlaceOf(const Collector& collector,
				     const ThreadGroup& threads,
				     const std::string& where,
				     const std::string& callee) const
{
    Instrumentor::executeInPlaceOf(threads, collector, where, callee);
}



/**
 * Execute a library function before the thread exits.
 *
 * Executes the specified library function before the instrumented threads exit.
 * Called by derived classes to execute functions in their runtime library.
 *
 * @param collector    Collector requesting the execution.
 * @param threads      Threads in which the function should be executed.
 * @param callee       Name of the library function to be executed.
 * @param argument     Blob argument to the function.
 */
void CollectorImpl::executeBeforeExit(const Collector& collector,
				      const ThreadGroup& threads,
				      const std::string& callee, 
				      const Blob& argument) const
{
    Instrumentor::executeAtEntryOrExit(threads, collector, 
				       "exit", 
				       true, callee, argument);
    Instrumentor::executeAtEntryOrExit(threads, collector, 
				       "__libc_thread_freeres", 
				       true, callee, argument);
}



/**
 * Execute a library function before MPI_Init() exits.
 *
 * Executes the specified library function before MPI_Init() exits. Called by
 * derived classes to execute functions in their runtime library.
 *
 * @param collector    Collector requesting the execution.
 * @param threads      Threads in which the function should be executed.
 * @param callee       Name of the library function to be executed.
 * @param argument     Blob argument to the function.
 */
void CollectorImpl::executeBeforeExitMPI(const Collector& collector,
                                      const ThreadGroup& threads,
                                      const std::string& callee,
                                      const Blob& argument) const
{
    Instrumentor::executeAtEntryOrExit(threads, collector,
                                       "PMPI_Init",
                                       false, callee, argument);
}



/**
 * Remove instrumentation from threads.
 *
 * Removes all instrumentation associated with the specified collector from
 * the specified threads. Called by derived classes when they are done using
 * any instrumentation they placed in the threads.
 *
 * @param collector    Collector which is removing instrumentation.
 * @param threads      Threads from which instrumentation should be removed.
 */
void CollectorImpl::uninstrument(const Collector& collector,
				 const ThreadGroup& threads) const
{
    Instrumentor::uninstrument(threads, collector);
}



/**
 * Get MPI runtime usage map.
 *
 * Returns the MPI runtime usage map for the specified thread group. Called
 * by derived classes to determine which threads should use which versions
 * of their runtime library.
 *
 * @note    A cache could be maintained here to avoid repeated calls to
 *          getMPIImplementationName() for the same threads.
 * 
 * @param threads    Threads for which to get a MPI runtime usage map.
 * @return           MPI runtime usage map for these threads.
 */
CollectorImpl::RuntimeUsageMap
CollectorImpl::getMPIRuntimeUsageMap(const ThreadGroup& threads) const
{
    RuntimeUsageMap runtime_usage_map;

    std::string MPI_IMPL_NAME;
    std::pair<bool, std::string> mpiImplPair;
  


    // Iterate over each thread of this group
    for(ThreadGroup::const_iterator
	    i = threads.begin(); i != threads.end(); ++i) {

        // Get the mpi implementation name out of the database
        mpiImplPair = (*i).getMPIImplementation();
        if(mpiImplPair.first) {
          MPI_IMPL_NAME = mpiImplPair.second;
        } else {
          MPI_IMPL_NAME = "unknown-mpi-implementation";
        } 

	// Get the MPI runtime library name for this thread
	std::string runtime_library_name = 
	    getUniqueId() + "-rt-" + MPI_IMPL_NAME;

	// Find or create the group for this MPI runtime library name
	RuntimeUsageMap::iterator j =
	    runtime_usage_map.find(runtime_library_name);
	if (j == runtime_usage_map.end())
	    j = runtime_usage_map.insert(std::make_pair(
		    runtime_library_name, ThreadGroup()
		    )).first;
	
	// Add this thread to the group for this MPI runtime library name
	j->second.insert(*i);

    }

    // Return the MPI runtime usage map
    return runtime_usage_map;
}
