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
 * Declaration of the Collector class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Collector_
#define _OpenSpeedShop_Framework_Collector_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "AddressRange.hxx"
#include "CollectorImpl.hxx"
#include "Metadata.hxx"
#include "SmartPtr.hxx"
#include "TimeInterval.hxx"

#include <set>
#include <string>



namespace OpenSpeedShop { namespace Framework {

    class Database;
    class Experiment;
    class Thread;
    class ThreadGroup;
    
    /**
     * Performance data collector.
     *     
     * Represents a performance data collector. Each collector contains its
     * own metadata, set of parameters, set of generated metrics, and a group
     * of threads for which it is gathering performance data. Member functions
     * for querying and modifying these properties, as well as starting and
     * stopping data collection, are provided here. A static member function
     * is also defined that allows a tool to dynamically determine the
     * available collectors at runtime.
     *
     * @par Example:
     *     Consider a performance data collector plugin that gathers PC sampling
     *     data via the UNIX sprofil() system call. The unique identifier in its
     *     metadata could be "pcsamp", it might have a single integer parameter
     *     that provided the sampling rate and a single floating-point metric
     *     that provided the measured time. If properly installed, this plugin
     *     would appear in the metadata returned by getAvailable(). Using
     *     Experiment::createCollector("pcsamp") a tool could create an instance
     *     of this collector. Then it could set the instance's sampling rate to
     *     10mS, attach a thread to the collector, and begin collection of PC
     *     sampling data by calling startCollecting(). The resulting data would
     *     then be accessed via the time metric.
     * 
     * @ingroup ToolAPI
     */
    class Collector
    {
	friend class Experiment;
	
    public:
	
	static std::set<Metadata> getAvailable();
	
    public:

	Collector(const Collector&);
	~Collector();
	Collector& operator=(const Collector&);

	Metadata getMetadata() const;
	
	std::set<Metadata> getParameters() const;
	
	template <typename T>
	void getParameterValue(const std::string&, T&) const;
	
	template <typename T>
	void setParameterValue(const std::string&, const T&) const;

	std::set<Metadata> getMetrics() const;
		
	template <typename T>
	void getMetricValue(const std::string&, const Thread&,
			    const AddressRange&, const TimeInterval&, T&) const;
	
	ThreadGroup getThreads() const;
	void attachThread(const Thread&) const;
	void detachThread(const Thread&) const;
	
	bool isCollecting() const;
        void startCollecting() const;
        void stopCollecting() const;
	
    private:

	Collector();
	Collector(const SmartPtr<Database>&, const int&);
	
	void validateEntry() const;
	
	/** Database containing this collector. */
        SmartPtr<Database> dm_database;
	
        /** Entry (id) for the collector. */
        int dm_entry;

	/** Collector's implementation. */
	CollectorImpl* dm_impl;
	
    };
    
    

    /**
     * Get a parameter value.
     *
     * Returns one of this collector's parameter values.
     *
     * @pre    Only applies to a collector for which an implementation was
     *         found. An exception of type std::logic_error is thrown if called
     *         for a collector that has no implementation.
     *
     * @pre    Parameters must be declared for the collector before they can be
     *         accessed. An exception of type std::invalid_argument is thrown if
     *         the parameter wasn't previously declared.
     *
     * @pre    Parameter values can only be returned in a value of the same
     *         type as themselves. No implicit type conversion is allowed. An
     *         exception of type std::invalid_argument is thrown if the
     *         parameter is accessed as a type other than its own.
     *
     * @todo    Get the actual parameter value.
     *
     * @param unique_id    Unique identifier of the parameter to get.
     * @retval value       Current value of the parameter.
     */
    template <typename T>
    void Collector::getParameterValue(const std::string& unique_id,
				      T& value) const
    {
	// Check preconditions
	if(dm_impl == NULL)
	    throw std::logic_error("Collector has no implementation.");
	
	// Find this parameter
	std::set<Metadata>::const_iterator i = dm_impl->getParameters().
	    find(Metadata(unique_id, "", "", typeid(T)));
	
	// Check preconditions
	if(i == dm_impl->getParameters().end())
            throw std::invalid_argument("Parameter \"" + unique_id +
                                        "\" hasn't been declared for this "
                                        "collector.");	
        if(!i->isType(typeid(T)))
            throw std::invalid_argument("Parameter \"" + unique_id +
                                        "\" can't be read as a value with a "
                                        "type other than its own.");
    }



    /**
     * Set a parameter value.
     *
     * Sets the specified parameter to a new value.
     *
     * @pre    Only applies to a collector for which an implementation was
     *         found. An exception of type std::logic_error is thrown if called
     *         for a collector that has no implementation.
     *
     * @pre    Parameters must be declared for the collector before they can be
     *         accessed. An exception of type std::invalid_argument is thrown if
     *         the parameter wasn't previously declared.
     *
     * @pre    Parameter values can only be set to a value of the same type as
     *         themselves. No implicit type conversion is allowed. An exception
     *         of type std::invalid_argument is thrown if the parameter is set
     *         to a type other than its own.
     *
     * @todo    Set the actual parameter value.
     *
     * @param unique_id    Unique identifier of the parameter to set.
     * @param value        New value for the parameter.
     */
    template <typename T>
    void Collector::setParameterValue(const std::string& unique_id,
				      const T& value) const
    {
	// Check preconditions
	if(dm_impl == NULL)
	    throw std::logic_error("Collector has no implementation.");
	
	// Find this parameter
	std::set<Metadata>::const_iterator i = dm_impl->getParameters().
	    find(Metadata(unique_id, "", "", typeid(T)));
	
	// Check preconditions
	if(i == dm_impl->getParameters().end())
            throw std::invalid_argument("Parameter \"" + unique_id +
                                        "\" hasn't been declared for this "
                                        "collector.");	
        if(!i->isType(typeid(T)))
            throw std::invalid_argument("Parameter \"" + unique_id +
                                        "\" can't be written as a value with "
                                        "a type other than its own.");
    }



    /**
     * Get a metric value.
     *
     * Returns one of this collector's metric values for a particular
     * thread, over a specific address range and time interval.
     *
     * @pre    Only applies to a collector for which an implementation was
     *         found. An exception of type std::logic_error is thrown if called
     *         for a collector that has no implementation.
     *
     * @pre    Metrics must be declared for the collector before they can be
     *         accessed. An exception of type std::invalid_argument is thrown if
     *         the metric wasn't previously declared.
     *
     * @pre    Metric values can only be returned in a value of the same type as
     *         themselves. No implicit type conversion is allowed. An exception
     *         of type std::invalid_argument is thrown if the metric is accessed
     *         as a type other than its own.
     *
     * @param unique_id   Unique identifier of the metric to get.
     * @param thread      Thread for which to get a value.
     * @param range       Address range over which to get a value.
     * @param interval    Time interval over which to get a value.
     * @retval value      Value of the metric.
     */
    template <typename T>
    void Collector::getMetricValue(const std::string& unique_id,
				   const Thread& thread,
				   const AddressRange& range,
				   const TimeInterval& interval,
				   T& value) const
    {
	// Check preconditions
	if(dm_impl == NULL)
	    throw std::logic_error("Collector has no implementation.");
	
	// Find this metric
	std::set<Metadata>::const_iterator i = dm_impl->getMetrics().
	    find(Metadata(unique_id, "", "", typeid(T)));
	
        // Check preconditions
	if(i == dm_impl->getMetrics().end())
            throw std::invalid_argument("Metric \"" + unique_id +
                                        "\" hasn't been declared for this "
                                        "collector.");	
        if(!i->isType(typeid(T)))
            throw std::invalid_argument("Metric \"" + unique_id +
                                        "\" can't be read as a value with a "
                                        "type other than its own.");

        // Get the metric value
        dm_impl->getMetricValue(unique_id, thread, range, interval, &value);	
    }
    
    
    
} }



#endif
