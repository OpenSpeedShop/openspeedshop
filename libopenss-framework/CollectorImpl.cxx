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
 * Definition of the CollectorImpl class.
 *
 */

#include "Collector.hxx"
#include "CollectorImpl.hxx"
#include "DataQueues.hxx"
#include "EntrySpy.hxx"
#include "Instrumentor.hxx"

using namespace OpenSpeedShop::Framework;



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
 * Get experiment, collector, and thread (ECT) identifiers.
 *
 * Gets the experiment, collector, and thread (ECT) identifiers of the specified
 * collector and thread. Called by derived classes when specifying to their
 * runtime library(ies) the ECT for which data is to be collected.
 * 
 * @param collector         Collector to be identified.
 * @param thread            Thread to be identified.
 * @retval experiment_id    Identifier of the experiment.
 * @retval collector_id     Identifier of the collector.
 * @retval thread_id        Identifier of the thread.
 */
void CollectorImpl::getECT(const Collector& collector,
			   const Thread& thread,
			   int& experiment_id,
			   int& collector_id,
			   int& thread_id) const
{
    // Check assertions
    Assert(EntrySpy(collector).getDatabase() == EntrySpy(thread).getDatabase());

    // Get and set the experiment identifier
    experiment_id = 
	DataQueues::getDatabaseIdentifier(EntrySpy(collector).getDatabase());
    
    // Set the collector and thread identifiers
    collector_id = EntrySpy(collector).getEntry();
    thread_id = EntrySpy(thread).getEntry();
}



/**
 * Execute library function now.
 *
 * Immediately execute the specified library function in the specified thread.
 * Called by derived classes to execute functions in their runtime library.
 *
 * @param collector    Collector requesting the execution.
 * @param thread       Thread in which the function should be executed.
 * @param callee       Name of the library function to be executed.
 * @param argument     Blob argument to the function.
 */
void CollectorImpl::executeNow(const Collector& collector,
			       const Thread& thread,
			       const std::string& callee,
			       const Blob& argument) const
{
    Instrumentor::executeNow(thread, collector, callee, argument);
}



/**
 * Execute library function at another function's entry.
 *
 * Executes the specified library function every time another function's entry
 * is executed in the specified thread. Called by derived classes to execute
 * functions in their runtime library.
 *
 * @param collector    Collector requesting the execution.
 * @param thread       Thread in which the function should be executed.
 * @param where        Name of the function at whose entry the library function
 *                     should be executed.
 * @param callee       Name of the library function to be executed.
 * @param argument     Blob argument to the function.
 */
void CollectorImpl::executeAtEntry(const Collector& collector,
				   const Thread& thread,
				   const std::string& where,
				   const std::string& callee, 
				   const Blob& argument) const
{
    Instrumentor::executeAtEntryOrExit(thread, collector, where, true,
				       callee, argument);
}



/**
 * Execute library function at another function's exit.
 *
 * Executes the specified library function every time another function's exit
 * is executed in the specified thread. Called by derived classes to execute
 * functions in their runtime library.
 *
 * @param collector    Collector requesting the execution.
 * @param thread       Thread in which the function should be executed.
 * @param where        Name of the function at whose exit the library function
 *                     should be executed.
 * @param callee       Name of the library function to be executed.
 * @param argument     Blob argument to the function.
 */
void CollectorImpl::executeAtExit(const Collector& collector,
				  const Thread& thread,
				  const std::string& where,
				  const std::string& callee, 
				  const Blob& argument) const
{
    Instrumentor::executeAtEntryOrExit(thread, collector, where, false,
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
 * @param thread       Thread in which the function should be executed.
 * @param where        Name of the function to be replaced with the library
 *                     function.
 * @param callee       Name of the library function to be executed.
 */
void CollectorImpl::executeInPlaceOf(const Collector& collector,
				     const Thread& thread,
				     const std::string& where,
				     const std::string& callee) const
{
    Instrumentor::executeInPlaceOf(thread, collector, where, callee);
}



/**
 * Execute a library function before the thread exits.
 *
 * Executes the specified library function before the instrumented thread exits.
 * Called by derived classes to execute functions in their runtime library.
 *
 * @param collector    Collector requesting the execution.
 * @param thread       Thread in which the function should be executed.
 * @param callee       Name of the library function to be executed.
 * @param argument     Blob argument to the function.
 */
void CollectorImpl::executeBeforeExit(const Collector& collector,
				      const Thread& thread,
				      const std::string& callee, 
				      const Blob& argument) const
{
    Instrumentor::executeAtEntryOrExit(thread, collector, 
				       "exit", 
				       true, callee, argument);
    Instrumentor::executeAtEntryOrExit(thread, collector, 
				       "__libc_thread_freeres", 
				       true, callee, argument);
}



/**
 * Remove instrumentation from a thread.
 *
 * Removes all instrumentation associated with the specified collector from
 * the specified thread. Called by derived classes when they are done using
 * any instrumentation they placed in the thread.
 *
 * @param collector    Collector which is removing instrumentation.
 * @param thread       Thread from which instrumentation should be removed.
 */
void CollectorImpl::uninstrument(const Collector& collector,
				 const Thread& thread) const
{
    Instrumentor::uninstrument(thread, collector);
}



/**
 * Determine the name of the MPI implementation being used by the
 * given thread.  Possibilities are "mpich", "mpt", "openmpi".
 *
 * @param thread            The thread
 *
 * @return    string containing the name of the MPI implementation
 */
std::string CollectorImpl::getMPIImplementationName(const Thread& /*thread*/) const
{
#ifdef HAVE_MPI
    /*
     * The environment variable OPENSS_MPI_IMPLEMENTATION can be set
     * to override the automatic MPI implementation detection process.
     */

    char* env_variable_name = "OPENSS_MPI_IMPLEMENTATION";
    char* value = getenv(env_variable_name);
    if (value) {
	std::string impl_names = ALL_MPI_IMPL_NAMES;
	std::string search_target = std::string(" ") + value + " ";
	if (impl_names.find(search_target) == std::string::npos) {
	    throw Exception(Exception::MPIImplChoiceInvalid,
			    value, impl_names);
	}
	return value;
    }


    /* Automatic MPI Implementation Detection (coming soon) */


    /*
     * The old AC_PKG_MPI code in acinclude.m4 caused the entire
     * OpenSpeedShop build to use the first MPI implementation it
     * found.  This temporary code reproduces that behavior, with the
     * help of the new AC_PKG_MPI code.
     */
    return DEFAULT_MPI_IMPL_NAME;

#else /* ifndef HAVE_MPI */
    Assert(false);
    return "";
#endif /* ifndef HAVE_MPI */
}

