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
 * Declaration of the Collector class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Collector_
#define _OpenSpeedShop_Framework_Collector_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "AddressRange.hxx"
#include "Guard.hxx"
#include "Lockable.hxx"
#include "Metadata.hxx"
#include "Metric.hxx"
#include "Parameter.hxx"
#include "PluginTable.hxx"
#include "ThreadGroup.hxx"
#include "TimeInterval.hxx"
#include "TypedMetric.hxx"
#include "TypedParameter.hxx"

#include <map>
#include <set>
#include <string>



namespace OpenSpeedShop { namespace Framework {

    class Application;
    class Thread;
    
    /**
     * Performance data collector.
     *
     * Abstract base class for all performance data collectors. Each collector
     * contains its own metadata, set of parameters, set of generated metrics,
     * and a group of threads for which it is gathering performance data. Member
     * functions for querying and modifying these properties are provided here,
     * as are member functions for starting and stopping data collection.
     * Collectors are implemented in a plugin via derived classes that define
     * the pure virtual member functions declared here. A static member function
     * is also defined that allows a tool to dynamically determine the available
     * collectors at runtime.
     *
     * @par Example:
     *     Consider a performance data collector plugin that gathers PC sampling
     *     data via the UNIX sprofil() system call. Its unique identifier could
     *     be "pcsamp", it might have a single integer parameter that provided
     *     the sampling rate and a single floating-point metric that provided
     *     the measured time. If properly installed, "pcsamp" would appear in
     *     the set of unique identifiers returned by getAvailableCollectors().
     *     Using Application::createCollector("pcsamp") a tool could create an
     *     instance of this collector. Then it could set the instance's sampling
     *     rate to 10mS, attach a thread to the collector, and begin collection
     *     of PC sampling data by calling startCollection(). The resulting data
     *     would then be accessed via the time metric.
     * 
     * @ingroup CollectorAPI ToolAPI
     */
    class Collector :
	public Metadata,
	private Lockable
    {
	friend class Application;
	friend class PluginTable;
	
    public:
	
	static std::set<Metadata> getAvailableCollectors();
	
	std::set<Metadata> getParameters() const;
	
	template <typename T>
	void getParameterValue(const std::string&, T&) const;

	template <typename T>
	void setParameterValue(const std::string&, const T&) const;
	
	std::set<Metadata> getMetrics() const;

	template <typename T>
	void getMetricValue(const std::string&, const Thread*,
			    const AddressRange&, const TimeInterval&, T&);
	
	ThreadGroup getThreads() const;
	void attachThread(Thread*);
	void removeThread(Thread*);
	
	bool isCollecting() const;
	void startCollecting();
	void stopCollecting();
	
    protected:
	
	Collector(const std::string&, const std::string&, const std::string&);
	virtual ~Collector();
	
	void declareParameter(Parameter*);
	void declareMetric(Metric*);
	
	/**
	 * Get a metric value.
	 *
	 * Pure virtual member function that defines the interface by which a
	 * collector plugin provides metric values. Gets one of this collector's
	 * metric values for a particular thread, over a specific address range
	 * and time interval.
	 *
	 * @note    Ideally the actual metric type would be used to return the
	 *          value rather than an untyped pointer. However we don't know
	 *          apriori what type the metric data will be since that is
	 *          entirely determined by the collector plugin, and we know
	 *          nothing about the plugins at compile time. The obvious
	 *          answer would seem to be templating this function by the
	 *          value type as we do for getMetricValue(). But then we can't
	 *          make it virtual (much less pure virtual) and we thus can't
	 *          defer implementation to the plugin. Using an untyped pointer
	 *          was, unfortunately, the best solution.
	 *
	 * @param metric      Metric for which to get a value.
	 * @param thread      Thread for which to get a value.
	 * @param range       Address range over which to get a value.
	 * @param interval    Time interval over which to get a value.
	 * @param ptr         Untyped pointer to the return value.
	 */
	virtual void implGetMetricValue(const Metric& metric,
					const Thread& thread,
					const AddressRange& range,
					const TimeInterval& interval,
					void* ptr) = 0;
	
    private:

	/** Table of collector plugins. */
	static PluginTable dm_plugin_table;
	
	/** Map parameter's unique identifiers to their parameter. */
	std::map<std::string, Parameter*> dm_parameters;
	
	/** Map metric's unique identifiers to their metric. */
	std::map<std::string, Metric*> dm_metrics;
	
	/** Group of all threads attached to this collector. */
	ThreadGroup dm_threads;
	
	/** Flag indicating if collector is currently collecting data. */
	bool dm_is_collecting;
	
    };



    /**
     * Get a parameter value.
     *
     * Returns one of this collector's parameter values.
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
     * @pre    A quirk in the GCC implementation of RTTI prevents the type
     *         system from recognizing that two types are "the same" unless the
     *         executable using this library is linked with the -export-dynamic
     *         linker option. An exception of type std::logic_error is thrown
     *         if the executable wasn't linked with this option.
     *
     * @sa    http://gcc.gnu.org/faq.html#dso
     *
     * @param unique_id    Unique identifier of the parameter to get.
     * @retval value       Current value of the parameter.
     */
    template <typename T>
    void Collector::getParameterValue(const std::string& unique_id,
				      T& value) const
    {
	Guard guard_myself(this);
	
	// Find this parameter
	std::map<std::string, Parameter*>::const_iterator i = 
	    dm_parameters.find(unique_id);
	
	// Check preconditions
	
	if(i == dm_parameters.end())
	    throw std::invalid_argument("Parameter \"" + unique_id +
					"\" hasn't been declared for this "
					"collector.");
	
	if(!i->second->isType(typeid(T)))
	    throw std::invalid_argument("Parameter \"" + unique_id +
					"\" can't be read as a value with a "
					"type other than its own.");
	
	// Typecast the parameter
	TypedParameter<T>* typed = dynamic_cast<TypedParameter<T>*>(i->second);
	
	// Check preconditions
	
	if(typed == NULL)
	    throw std::logic_error("Parameter \"" + unique_id + 
				   "\" couldn't be downcasted to the proper "
				   "typed parameter. Insure your tool is "
				   "linked with the \"-export-dynamic\" "
				   "linker option.");
	
	// Get the parameter value
	value = *typed;
    }



    /**
     * Set a parameter value.
     *
     * Sets the specified parameter to a new value.
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
     * @pre    A quirk in the GCC implementation of RTTI prevents the type
     *         system from recognizing that two types are "the same" unless the
     *         executable using this library is linked with the -export-dynamic
     *         linker option. An exception of type std::logic_error is thrown
     *         if the executable wasn't linked with this option.
     *
     * @sa    http://gcc.gnu.org/faq.html#dso
     *
     * @param unique_id    Unique identifier of the parameter to set.
     * @param value        New value for the parameter.
     */
    template <typename T>
    void Collector::setParameterValue(const std::string& unique_id,
				      const T& value) const
    {
	Guard guard_myself(this);
	
	// Find this parameter
	std::map<std::string, Parameter*>::const_iterator i = 
	    dm_parameters.find(unique_id);
	
	// Check preconditions
	
	if(i == dm_parameters.end())
	    throw std::invalid_argument("Parameter \"" + unique_id +
					"\" hasn't been declared for this "
					"collector.");
	
	if(!i->second->isType(typeid(T)))
	    throw std::invalid_argument("Parameter \"" + unique_id +
					"\" can't be read as a value with a "
					"type other than its own.");
	
	// Typecast the parameter
	TypedParameter<T>* typed = dynamic_cast<TypedParameter<T>*>(i->second);
	
	// Check preconditions
	
	if(typed == NULL)
	    throw std::logic_error("Parameter \"" + unique_id +
				   "\" couldn't be downcasted to the proper "
				   "typed parameter. Insure your tool is "
				   "linked with the \"-export-dynamic\" "
				   "linker option.");
	
	// Set the parameter value
	*typed = value;
    }



    /**
     * Get a metric value.
     *
     * Returns one of this collector's metric values for a particular
     * thread, over a specific address range and time interval.
     *
     * @pre    Metrics must be declared for the collector before they can be
     *         accessed. An exception of type std::invalid_argument is thrown if
     *         the metric wasn't previously declared.
     *
     * @pre    Metric values can only be returnd in a value of the same type as
     *         themselves. No implicit type conversion is allowed. An exception
     *         of type std::invalid_argument is thrown if the metric is accessed
     *         as a type other than its own.
     *
     * @pre    A quirk in the GCC implementation of RTTI prevents the type
     *         system from recognizing that two types are "the same" unless the
     *         executable using this library is linked with the -export-dynamic
     *         linker option. An exception of type std::logic_error is thrown
     *         if the executable wasn't linked with this option.
     *
     * @sa    http://gcc.gnu.org/faq.html#dso
     *
     * @param unique_id   Unique identifier of the metric to get.
     * @param thread      Thread for which to get a value.
     * @param range       Address range over which to get a value.
     * @param interval    Time interval over which to get a value.
     * @retval value      Value of the metric.
     */
    template <typename T>
    void Collector::getMetricValue(const std::string& unique_id,
				   const Thread* thread,
				   const AddressRange& range,
				   const TimeInterval& interval,
				   T& value)
    {
	Guard guard_myself(this);

	// Find this metric
	std::map<std::string, Metric*>::const_iterator i =
	    dm_metrics.find(unique_id);

	// Check preconditions

	if(i == dm_metrics.end())
	    throw std::invalid_argument("Metric \"" + unique_id + 
					"\" hasn't been declared for this "
					"collector,");
	
	if(!i->second->isType(typeid(T)))
	    throw std::invalid_argument("Metric \"" + unique_id +
					"\" can't be read as a value with a "
					"type other than its own.");


	// TODO: check validity of thread argument


	// Typecast the metric
	TypedMetric<T>* typed = dynamic_cast<TypedMetric<T>*>(i->second);
	
	// Check preconditions
	
	if(typed == NULL)
	    throw std::logic_error("Metric \"" + unique_id +
				   "\" couldn't be downcasted to the proper "
				   "typed metric. Insure your tool is linked "
				   "with the \"-export-dynamic\" linker "
				   "option.");
	
	// Get the metric value
	implGetMetricValue(*typed, *thread, range, interval, &value);
    }
    
    
    
} }



#endif
