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
 * Declaration of the Collector class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Collector_
#define _OpenSpeedShop_Framework_Collector_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "AddressRange.hxx"
#include "Assert.hxx"
#include "Blob.hxx"
#include "CollectorImpl.hxx"
#include "Entry.hxx"
#include "Exception.hxx"
#include "ExtentGroup.hxx"
#include "Metadata.hxx"
#include "TimeInterval.hxx"

#include <cxxabi.h>
#include <set>
#include <string>
#include <vector>

namespace OpenSpeedShop { namespace Framework {

    class Database;
    class DataCache;
    class Experiment;
    class Extent;
    template <typename> class SmartPtr;
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
     *     10mS, and begin collection of data by calling startCollecting() for
     *     a thread. The resulting data would then be accessed via the time
     *     metric.
     * 
     * @ingroup CollectorAPI ToolAPI
     */
    class Collector :
	public Entry
    {
	friend class DataCache;
	friend class Experiment;
	friend class Thread;
	
    public:
	
	static std::set<Metadata> getAvailable();
	
	Collector(const Collector&);
	virtual ~Collector();

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
	
	template <typename T>
	void getMetricValues(const std::string&, const Thread&,
			     const ExtentGroup&, std::vector<T >&) const;

	std::set<int> getIdentifiers(const Thread&, const ExtentGroup&) const;
	
	template <typename T>
	void getMetricValues(const std::string&, const Thread&,
			     const ExtentGroup&, const int&,
			     std::vector<T >&) const;
	
	void getUniquePCValues( const Thread&,
				const ExtentGroup&,
				PCBuffer* ) const;

	void getUniquePCValues( const Thread&,
				const ExtentGroup&,
				std::set<Address>& ) const;

	Extent getExtentIn(const Thread&) const;
	
	ThreadGroup getThreads() const;
	ThreadGroup getPostponedThreads() const;

        void startCollecting(const ThreadGroup&) const;	
	void postponeCollecting(const ThreadGroup&) const;
        void stopCollecting(const ThreadGroup&) const;

        void startCollecting(const Thread&) const;	
	void postponeCollecting(const Thread&) const;
        void stopCollecting(const Thread&) const;

    private:

	Collector();
	Collector(const SmartPtr<Database>&, const int&);
	
	void instantiateImpl() const;
	Blob getParameterData() const;
	void setParameterData(const Blob&) const;
	void getMetricValues(const std::string&, const Thread&,
			     const ExtentGroup&, void*) const;
	void getMetricValues(const std::string&, const Thread&,
			     const ExtentGroup&, const int&, void*) const;
	
	/** Collector's implementation. */
	mutable CollectorImpl* dm_impl;
	
    };
    
    

    /**
     * Get a parameter value.
     *
     * Returns one of this collector's parameter values.
     *
     * @pre    Can only be performed on collectors for which an implementation
     *         can be instantiated. A CollectorUnavailable exception is thrown
     *         if the collector's implementation cannot be instantiated.
     *
     * @pre    Parameters must be declared for the collector before they can
     *         be accessed. An assertion failure occurs if the parameter wasn't
     *         previously declared.
     *
     * @pre    Parameter values can only be returned in a value of the same
     *         type as themselves. No implicit type conversion is allowed.
     *         An assertion failure occurs if the parameter is accessed as a
     *         type other than its own.
     *
     * @param unique_id    Unique identifier of the parameter to get.
     * @retval value       Current value of the parameter.
     */
    template <typename T>
    void Collector::getParameterValue(const std::string& unique_id,
				      T& value) const
    {
	// Check preconditions
	if(dm_impl == NULL) {
	    instantiateImpl();
	    if(dm_impl == NULL)
		throw Exception(Exception::CollectorUnavailable,
				getMetadata().getUniqueId());
	}

	// Find this parameter
	std::set<Metadata>::const_iterator i = dm_impl->getParameters().
	    find(Metadata(unique_id, "", "", typeid(T)));
	
	// Check preconditions
	Assert(i != dm_impl->getParameters().end());
#if 0
	Assert(i->isType(typeid(T)));
#else
        if(!i->isType(typeid(T))) {
            int status;
            fprintf(stderr, "Assertion \"%s\" failed in file %s at line %d. ",
                    "!i->isType(typeid(T))", __FILE__, __LINE__);
            fprintf(stderr,
                    "Metric \"%s::%s\" is of type \"%s\" rather than \"%s\".\n",
                    getMetadata().getUniqueId().c_str(), unique_id.c_str(),
                    abi::__cxa_demangle(i->getType().c_str(), NULL, NULL,
                                        &status),
                    abi::__cxa_demangle(typeid(T).name(), NULL, NULL,
                                        &status));
            fflush(stderr);
            abort();
        }
#endif

	
	// Get our parameter data
	Blob blob = getParameterData();
	
	// Extract the parameter value
	dm_impl->getParameterValue(unique_id, blob, &value);
    }
    


    /**
     * Set a parameter value.
     *
     * Sets the specified parameter to a new value.
     *
     * @pre    Can only be performed on collectors for which an implementation
     *         can be instantiated. A CollectorUnavailable exception is thrown
     *         if the collector's implementation cannot be instantiated.
     *
     * @pre    Parameters must be declared for the collector before they can
     *         be accessed. An assertion failure occurs if the parameter wasn't
     *         previously declared.
     *
     * @pre    Parameter values can only be set to a value of the same type
     *         as themselves. No implicit type conversion is allowed. An
     *         assertion failure occurs if the parameter is accessed as a type
     *         other than its own.
     *
     * @param unique_id    Unique identifier of the parameter to set.
     * @param value        New value for the parameter.
     */
    template <typename T>
    void Collector::setParameterValue(const std::string& unique_id,
				      const T& value) const
    {
	// Check preconditions
	if(dm_impl == NULL) {
	    instantiateImpl();
	    if(dm_impl == NULL)
		throw Exception(Exception::CollectorUnavailable,
				getMetadata().getUniqueId());
	}
	
	// Find this parameter
	std::set<Metadata>::const_iterator i = dm_impl->getParameters().
	    find(Metadata(unique_id, "", "", typeid(T)));
	
	// Check preconditions
	Assert(i != dm_impl->getParameters().end());
	Assert(i->isType(typeid(T)));
	
	// Get our parameter data
	Blob blob = getParameterData();

	// Set the parameter value
	dm_impl->setParameterValue(unique_id, &value, blob);
	
	// Set our parameter data
	setParameterData(blob);
    }
    


    /**
     * Get a metric value.
     *
     * Returns one of this collector's metric values for a particular
     * thread, over a specific address range and time interval.
     *
     * @pre    Can only be performed on collectors for which an implementation
     *         can be instantiated. A CollectorUnavailable exception is thrown
     *         if the collector's implementation cannot be instantiated.
     *
     * @pre    Metrics must be declared for the collector before they can
     *         be accessed. An assertion failure occurs if the metric wasn't
     *         previously declared.
     *
     * @pre    Metric values can only be returned in a value of the same type
     *         as themselves. No implicit type conversion is allowed. An
     *         assertion failure occurs if the metric is accessed as a type
     *         other than its own.
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
	// Construct an extent group containing this single subextent
	ExtentGroup subextents;
	subextents.push_back(Extent(interval, range));

	// Evalute the metric value for this subextent
	std::vector<T > values;
	getMetricValues(unique_id, thread, subextents, values);
	
	// Provide the metric value to the caller
	value = values[0];
    }



    /**
     * Get metric values.
     *
     * Returns one of this collector's metric values over all subextents of the
     * specified extent for a particular thread. 
     * 
     * @pre    Can only be performed on collectors for which an implementation
     *         can be instantiated. A CollectorUnavailable exception is thrown
     *         if the collector's implementation cannot be instantiated.
     *
     * @pre    Metrics must be declared for the collector before they can
     *         be accessed. An assertion failure occurs if the metric wasn't
     *         previously declared.
     *
     * @pre    Metric values can only be returned in a value of the same type
     *         as themselves. No implicit type conversion is allowed. An
     *         assertion failure occurs if the metric is accessed as a type
     *         other than its own.
     *
     * @param unique_id     Unique identifier of the metric to get.
     * @param thread        Thread for which to get values.
     * @param subextents    Subextents for which to get values.
     * @retval values       Values of the metric.
     */
    template <typename T>
    void Collector::getMetricValues(const std::string& unique_id,
				    const Thread& thread,
				    const ExtentGroup& subextents,
				    std::vector<T >& values) const
    {
	// Check preconditions
	if(dm_impl == NULL) {
	    instantiateImpl();
	    if(dm_impl == NULL)
		throw Exception(Exception::CollectorUnavailable,
				getMetadata().getUniqueId());
	}
	
	// Find this metric
	std::set<Metadata>::const_iterator i = dm_impl->getMetrics().
	    find(Metadata(unique_id, "", "", typeid(T)));
	
        // Check preconditions
	Assert(i != dm_impl->getMetrics().end());
	if(!i->isType(typeid(T))) {
	    int status;
	    fprintf(stderr, "Assertion \"%s\" failed in file %s at line %d. ",
		    "!i->isType(typeid(T))", __FILE__, __LINE__);
	    fprintf(stderr,
		    "Metric \"%s::%s\" is of type \"%s\" rather than \"%s\".\n",
		    getMetadata().getUniqueId().c_str(), unique_id.c_str(),
		    abi::__cxa_demangle(i->getType().c_str(), NULL, NULL,
					&status),
		    abi::__cxa_demangle(typeid(T).name(), NULL, NULL,
					&status));
	    fflush(stderr);
	    abort();
	}
	
	// Insure vector of values is large enough to contain the results
	if(values.size() < subextents.size())
	    values.resize(subextents.size());
	
	// Get our metric values
	getMetricValues(unique_id, thread, subextents, &values);
    }



    /**
     * Get metric values.
     *
     * Returns one of this collector's metric values over all subextents of the
     * specified extent for a particular thread. The computation is restricted
     * to the specified performance data blob identifier.
     * 
     * @pre    Can only be performed on collectors for which an implementation
     *         can be instantiated. A CollectorUnavailable exception is thrown
     *         if the collector's implementation cannot be instantiated.
     *
     * @pre    Metrics must be declared for the collector before they can
     *         be accessed. An assertion failure occurs if the metric wasn't
     *         previously declared.
     *
     * @pre    Metric values can only be returned in a value of the same type
     *         as themselves. No implicit type conversion is allowed. An
     *         assertion failure occurs if the metric is accessed as a type
     *         other than its own.
     *
     * @param unique_id     Unique identifier of the metric to get.
     * @param thread        Thread for which to get values.
     * @param subextents    Subextents for which to get values.
     * @param identifier    Performance data blob identifier for which to
     *                      get values.
     * @retval values       Values of the metric.
     */
    template <typename T>
    void Collector::getMetricValues(const std::string& unique_id,
				    const Thread& thread,
				    const ExtentGroup& subextents,
				    const int& identifier,
				    std::vector<T >&values) const
    {
	// Check preconditions
	if(dm_impl == NULL) {
	    instantiateImpl();
	    if(dm_impl == NULL)
		throw Exception(Exception::CollectorUnavailable,
				getMetadata().getUniqueId());
	}
	
	// Find this metric
	std::set<Metadata>::const_iterator i = dm_impl->getMetrics().
	    find(Metadata(unique_id, "", "", typeid(T)));
	
        // Check preconditions
	Assert(i != dm_impl->getMetrics().end());
	if(!i->isType(typeid(T))) {
	    int status;
	    fprintf(stderr, "Assertion \"%s\" failed in file %s at line %d. ",
		    "!i->isType(typeid(T))", __FILE__, __LINE__);
	    fprintf(stderr,
		    "Metric \"%s::%s\" is of type \"%s\" rather than \"%s\".\n",
		    getMetadata().getUniqueId().c_str(), unique_id.c_str(),
		    abi::__cxa_demangle(i->getType().c_str(), NULL, NULL,
					&status),
		    abi::__cxa_demangle(typeid(T).name(), NULL, NULL,
					&status));
	    fflush(stderr);
	    abort();
	}
	
	// Insure vector of values is large enough to contain the results
	if(values.size() < subextents.size())
	    values.resize(subextents.size(), T());
	
	// Get our metric values
	getMetricValues(unique_id, thread, subextents, identifier, &values);
    }
    
    
    
} }



#endif
