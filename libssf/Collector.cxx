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
 * Definition of the Collector class.
 *
 */

#include "Collector.hxx"
#include "Metric.hxx"
#include "Parameter.hxx"

#include <stdexcept>
#include <typeinfo>

using namespace OpenSpeedShop::Framework;



/** Table of collector plugins. */
PluginTable Collector::dm_plugin_table;



/**
 * Get all available collectors.
 *
 * Returns to the caller the metadata for all available collectors. An empty set
 * is returned if no collector plugins were found.
 *
 * @return    Metadata for all available collectors.
 */
std::set<Metadata> Collector::getAvailableCollectors()
{
    // Return the metadata to the caller
    return dm_plugin_table.getAvailableCollectors();
}



/**
 * Get our parameters.
 *
 * Returns to the caller the metadata for all parameters of this collector. An
 * empty set is returned if this collector has no parameters.
 *
 * @return    Metadata for all parameters of this collector.
 */
std::set<Metadata> Collector::getParameters() const
{
    Guard guard_myself(this);
    
    // Assemble the metadata for all parameters of this collector
    std::set<Metadata> metadata;
    for(std::map<std::string, Parameter*>::const_iterator
	    i = dm_parameters.begin(); i != dm_parameters.end(); ++i)
	metadata.insert(*(i->second));

    // Return the metadata to the caller
    return metadata;
}



/**
 * Get our metrics.
 *
 * Returns to the caller the metadata for all metrics of this collector. An
 * empty set is returned if this collector has no metrics (unlikely).
 *
 * @return    Metadata for all metrics of this collector.
 */
std::set<Metadata> Collector::getMetrics() const
{
    Guard guard_myself(this);

    // Assemble the metadata for all metrics of this collector
    std::set<Metadata> metadata;
    for(std::map<std::string, Metric*>::const_iterator
	    i = dm_metrics.begin(); i != dm_metrics.end(); ++i)
	metadata.insert(*(i->second));
    
    // Return the metadata to the caller
    return metadata;
}



/**
 * Get our threads.
 *
 * Returns to the caller all threads currently attached to this collector. An
 * empty thread group is returned if this collector isn't attached to any
 * threads.
 *
 * @return    Threads to which this collector is currently attached.
 */
ThreadGroup Collector::getThreads() const
{
    Guard guard_myself(this);

    // Return the threads to the caller
    return dm_threads;
}



/**
 * Attach to a thread.
 *
 * Attaches the specified thread to this collector. ...
 *
 * @param thread    Thread to be attached.
 */
void Collector::attachThread(Thread* thread)
{
    // TODO: implement
}



/**
 * Remove a thread.
 *
 * Removes the specified thread from this collector. Any performance data that
 * was associated with the thread is destroyed.
 *
 * @param thread    Thread to be removed.
 */
void Collector::removeThread(Thread* thread)
{
    // TODO: implement
}



/**
 * Test if collecting data.
 *
 * Returns a boolean value indicating if the collector is currently collecting
 * performance data.
 *
 * @return    Boolean "true" if the collector is currently collecting data, 
 *            "false" otherwise.
 */
bool Collector::isCollecting() const
{
    return dm_is_collecting;
}



/**
 * Start data collection.
 *
 * Starts data collection for this collector. Data collection can be stopped
 * temporarily or permanently by calling stopCollecting(). All data that is
 * collected is available via the collector's metrics.
 *
 * @pre    Collectors cannot start data collection until they have been attached
 *         to at least one thread. An exception of type std::logic_error is
 *         thrown if an attempt is made to start an unattached collector.
 *
 * @pre    Collectors cannot start data collection if they are currently
 *         collecting data. An exception of type std::logic_error is thrown if
 *         an attempt is made to start a collector that is already collecting.
 */
void Collector::startCollecting()
{
    Guard guard_myself(this);

    // Check preconditions

    if(dm_threads.empty())
	throw std::logic_error("Cannot start a collector that is "
			       "not attached to a thread.");
    
    if(isCollecting())
	throw std::logic_error("Cannot start a collector that is "
			       "already collecting.");

    // TODO: implement

    // Indicate that we are now collecting data
    dm_is_collecting = true;
}



/**
 * Stop data collection.
 *
 * Stops data collection for this collector. Data collection can be resumed by
 * calling startCollecting() again. All data that was collected is available via
 * the collector's metrics.
 *
 * @pre    Collectors cannot stop data collection unless they are currently
 *         collecting data. An exception of type std::logic_error is thrown
 *         if an attempt is made to stop a collector that isn't collecting.
 */
void Collector::stopCollecting()
{
    Guard guard_myself(this);

    // Check preconditions

    if(!isCollecting())
	throw std::logic_error("Cannot stop a collector that is "
			       "not currently collecting.");
    
    // TODO: implement

    // Indicate that we are no longer collecting data
    dm_is_collecting = false;
}



/**
 * Constructor from metadata.
 *
 * Constructs a collector with the given metadata. The created collector has no
 * parameters or metrics, is attached to no threads, and is not collecting any
 * data.
 *
 * @param unique_id      Unique identifier for this collector.
 * @param short_name     Short name of this collector.
 * @param description    More verbose description of this collector.
 */
Collector::Collector(const std::string& unique_id,
		     const std::string& short_name,
		     const std::string& description) :
    Metadata(unique_id, short_name, description, typeid(*this)),
    dm_parameters(),
    dm_metrics(),
    dm_threads(),
    dm_is_collecting(false)
{
}



/**
 * Destructor.
 *
 * Destroys the parameters and metrics associated with this collector.
 */
Collector::~Collector()
{
    // Destroy all parameters
    for(std::map<std::string, Parameter*>::const_iterator
	    i = dm_parameters.begin(); i != dm_parameters.end(); ++i)
	delete i->second;

    // Destroy all metrics
    for(std::map<std::string, Metric*>::const_iterator
	    i = dm_metrics.begin(); i != dm_metrics.end(); ++i)
	delete i->second;
}



/**
 * Declare a parameter.
 *									
 * Declares a parameter by adding it to this collector. Called by derived
 * classes when specifying their parameters.
 *
 * @pre    Null parameters cannot be declared. An exception of type
 *         std::logic_error is thrown if the parameter is null.
 *
 * @pre    Once a collector has been attached to a thread additional parameters
 *         cannot be declared. An exception of type std::logic_error is thrown
 *         if an attempt is made to declare a parameter after the collector has
 *         been attached to one or more threads.
 *
 * @pre    Each parameter in a collector must be unique. An exception of type
 *         std::invalid_argument is thrown if an attempt is made to redeclare
 *         a parameter.
 *
 * @post    Responsibility for destroying the declared parameter is assumed by
 *          this collector's destructor. Derived classes should not dispose of
 *          the declared parameter directly. 
 *
 * @param parameter    Parameter to be declareed.
 */
void Collector::declareParameter(Parameter* parameter)
{
    Guard guard_myself(this);

    // Check preconditions

    if(parameter == NULL)
	throw std::logic_error("Cannot declare a null parameter.");
    
    if(!dm_threads.empty())
	throw std::logic_error("Parameter \"" + parameter->getUniqueId() +
			       "\" cannot be declared after the collector is "
			       "attached to a thread.");
    
    if(dm_parameters.find(parameter->getUniqueId()) != dm_parameters.end())
	throw std::invalid_argument("Parameter \"" + parameter->getUniqueId() +
				    "\" cannot be redeclared.");
    
    // Insert the new parameter
    dm_parameters.insert(std::make_pair(parameter->getUniqueId(), parameter));
}



/**
 * Declare a metric.
 *
 * Declares a metric by adding it to this collector. Called by derived classes
 * when specifying their metrics.
 *
 * @pre    Null metrics cannot be declared. An exception of type
 *         std::logic_error is thrown if the metric is null.
 *
 * @pre    Once a collector has been attached to a thread additional metrics
 *         cannot be declared. An exception of type std::logic_error is thrown
 *         if an attempt is made to declare a metric after the collector has
 *         been attached to one or more threads.
 *
 * @pre    Each metric in a collector must be unique. An exception of type
 *         std::invalid_argument is thrown if an attempt is made to redeclare
 *         a metric.
 *
 * @post    Responsibility for destroying the declared metric is assumed by
 *          this collector's destructor. Derived classes should not dispose of
 *          the declared metric directly.
 * 
 * @param metric    Metric to be declareed.
 */
void Collector::declareMetric(Metric* metric)
{
    Guard guard_myself(this);
    
    // Check preconditions
    
    if(metric == NULL)
	throw std::logic_error("Cannot declare a null metric.");
    
    if(!dm_threads.empty())
	throw std::logic_error("Metric \"" + metric->getUniqueId() +
			       "\" cannot be declared after the collector is "
			       "attached to a thread.");
    
    if(dm_metrics.find(metric->getUniqueId()) != dm_metrics.end())
	throw std::invalid_argument("Metric \"" + metric->getUniqueId() +
				    "\" cannot be redeclared.");
    
    // Insert the new metric
    dm_metrics.insert(std::make_pair(metric->getUniqueId(), metric));
}
