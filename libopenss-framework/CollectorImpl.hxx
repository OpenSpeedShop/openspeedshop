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
 * Declaration of the CollectorImpl class.
 *
 */

#ifndef _OpenSpeedShop_Framework_CollectorImpl_
#define _OpenSpeedShop_Framework_CollectorImpl_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "AddressRange.hxx"
#include "Metadata.hxx"
#include "TimeInterval.hxx"
#include "PCBuffer.hxx"

#include <map>
#include <set>
#include <string>


namespace OpenSpeedShop { namespace Framework {

    class Blob;
    class Collector;
    class Extent;
    class ExtentGroup;
    class Function;
    class Thread;
    class ThreadGroup;
    
    /**
     * Performance data collector implementation.
     *
     * Abstract base class for all performance data collector implementations.
     * Collectors are implemented in a plugin via derived classes that implement
     * a default constructor specifying the collector's metadata, and define the
     * pure virtual member functions declared here.
     * 
     * @ingroup CollectorAPI
     */
    class CollectorImpl :
	public Metadata
    {
	
    public:

	static int getExperimentId(const Collector&);
	static int getCollectorId(const Collector&);
	
	const std::set<Metadata>& getParameters() const;
	const std::set<Metadata>& getMetrics() const;

	/**
	 * Get the default parameter values.
	 *
	 * Pure virtual member function that defines the interface by which a
	 * collector plugin provides its default parameter values. 
	 *
	 * @return    Blob containing the default parameter values.
	 */
	virtual Blob getDefaultParameterValues() const = 0;
	
	/**
	 * Get a parameter value.
	 *
	 * Pure virtual member function that defines the interface by which a
	 * collector plugin provides parameter values. Gets one of this
	 * collector's parameter values from a blob.
	 *
	 * @note    Ideally the actual parameter value type would be used to
	 *          return the value rather than an untyped pointer. However we
	 *          don't know apriori what type the parameter value will be
	 *          since that is entirely determined by the collector plugin,
	 *          and we know nothing about the plugins at compile time. The
	 *          obvious answer would seem to be templating this function by
	 *          the value type as we do for Collector::getParameterValue().
	 *          But then we can't make it virtual (much less pure virtual)
	 *          (and we thus can't defer implementation to the plugin. Using
	 *          an untyped pointer was, unfortunately, the best solution.
	 *
	 * @param parameter    Unique identifier of the parameter.
	 * @param data         Blob containing the parameter values.
	 * @retval ptr         Untyped pointer to the parameter value.
	 */
	virtual void getParameterValue(const std::string& parameter,
				       const Blob& data, void* ptr) const = 0;

	/**
	 * Set a parameter value.
	 *
	 * Pure virtual member function that defines the interface by which a
	 * collector plugin sets parameter values. Sets one of this collector's
	 * parameter values in a blob.
	 *
	 * @note    Ideally the actual parameter value type would be used to
	 *          pass in the value rather than an untyped pointer. However
	 *          we don't know apriori what type the parameter value will be
	 *          since that is entirely determined by the collector plugin,
	 *          and we know nothing about the plugins at compile time. The
	 *          obvious answer would seem to be templating this function by
	 *          the value type as we do for Collector::setParameterValue().
	 *          But then we can't make it virtual (much less pure virtual)
	 *          (and we thus can't defer implementation to the plugin. Using
	 *          an untyped pointer was, unfortunately, the best solution.
	 *
	 * @param parameter    Unique identifier of the parameter.
	 * @param ptr          Untyped pointer to the parameter value.
	 * @retval data        Blob containing the parameter values.
	 */
	virtual void setParameterValue(const std::string& parameter,
				       const void* ptr, Blob& data) const = 0;

	/**
	 * Start data collection.
	 *
	 * Pure virtual member function that defines the interface by which a
	 * collector plugin starts data collection. Instruments the specified
	 * threads as necessary.
	 *
	 * @param collector    Collector starting data collection.
	 * @param threads      Threads for which to start collecting data.
	 */
	virtual void startCollecting(const Collector& collector,
				     const ThreadGroup& threads) const = 0;	

	/**
	 * Stops data collection.
	 *
	 * Pure virtual member function that defines the interface by which a
	 * collector plugin stops data collection. Removes any instrumentation
	 * from the specified threads.
	 *
	 * @param collector    Collector stopping data collection.
	 * @param threads      Threads for which to stop collecting data.
	 */
	virtual void stopCollecting(const Collector& collector,
				    const ThreadGroup& threads) const = 0;

	/**
	 * Get metric values.
	 *
	 * Pure virtual member function that defines the interface by which a
	 * collector plugin provides metric values. Gets one of this collector's
	 * metric values over all subextents of the specified extent for a
	 * particular thread, for one of the collected performance data blobs.
	 *
	 * @note    Ideally the actual metric type would be used to return the
	 *          values rather than an untyped pointer. However we don't know
	 *          apriori what type the metric data will be since that is
	 *          entirely determined by the collector plugin, and we know
	 *          nothing about the plugins at compile time. The obvious
	 *          answer would seem to be templating this function by the
	 *          value type as we do for Collector::getMetricValue(). But
	 *          then we can't make it virtual (much less pure virtual) and
	 *          we thus can't defer implementation to the plugin. Using an
	 *          untyped pointer was, unfortunately, the best solution.
	 *
	 * @param metric        Unique identifier of the metric.
	 * @param collector     Collector for which to get values.
	 * @param thread        Thread for which to get values.
	 * @param extent        Extent of the performance data blob.
	 * @param blob          Blob containing the performance data.
	 * @param subextents    Subextents for which to get values.
	 * @retval ptr          Untyped pointer to the values of the metric.
	 */
	virtual void getMetricValues(const std::string& metric,
				     const Collector& collector,
				     const Thread& thread,
				     const Extent& extent,
				     const Blob& blob,
				     const ExtentGroup& subextents,
				     void* ptr) const = 0;

	virtual void getUniquePCValues( const Thread& thread, const Blob& blob,
		PCBuffer *buf) const = 0;
	virtual void getUniquePCValues( const Thread& thread, const Blob& blob,
		std::set<Address>& uaddress) const = 0;
	
    protected:

	CollectorImpl(const std::string&, const std::string&,
		      const std::string&);
	
	void declareParameter(const Metadata&);
	void declareMetric(const Metadata&);

	void executeNow(const Collector&, const ThreadGroup&,
			const std::string&, const Blob&,
			const bool& disableSaveFPR = false) const;
	void executeAtEntry(const Collector&, const ThreadGroup&,
			    const std::string&, const std::string&,
			    const Blob&) const;
	void executeAtExit(const Collector&, const ThreadGroup&,
			   const std::string&, const std::string&,
			   const Blob&) const;
	void executeInPlaceOf(const Collector&, const ThreadGroup&,
			      const std::string&, const std::string&) const;

	void executeBeforeExit(const Collector&, const ThreadGroup&,
			       const std::string&, const Blob&) const;
	void executeBeforeExitMPI(const Collector&, const ThreadGroup&,
			       const std::string&, const Blob&) const;

	void uninstrument(const Collector&, const ThreadGroup&) const;

	std::string getMPIImplementationName(const Thread&) const;

	/**
	 * Runtime usage map.
	 *
	 * Type defining a mapping from a runtime library name to the group
	 * of threads that use that runtime library.
	 *
	 * @ingroup Implementation
	 */
	typedef std::map<std::string, ThreadGroup> RuntimeUsageMap;

	RuntimeUsageMap getMPIRuntimeUsageMap(const ThreadGroup&) const;

    private:

	/** Set of parameters. */
	std::set<Metadata> dm_parameters;
	
	/** Set of metrics. */
	std::set<Metadata> dm_metrics;
       
        /** Debug flag for tracing mpi job creation and attach */
#ifndef NDEBUG
        static bool is_debug_mpijob_enabled;
#endif
	
    };
    
    
    
} }



#endif
