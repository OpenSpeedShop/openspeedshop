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

#include <set>
#include <string>



namespace OpenSpeedShop { namespace Framework {

    class Collector;
    class Thread;
    
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
	
	const std::set<Metadata>& getParameters() const;
	const std::set<Metadata>& getMetrics() const;

	/**
	 * Start data collection.
	 *
	 * Pure virtual member function that defines the interface by which a
	 * collector plugin starts data collection. Instruments the specified
	 * thread as necessary.
	 *
	 * @param collector    Collector starting data collection.
	 * @param thread       Thread for which to start collecting data.
	 */
	virtual void startCollecting(const Collector& collector,
				     const Thread& thread) const = 0;	

	/**
	 * Stops data collection.
	 *
	 * Pure virtual member function that defines the interface by which a
	 * collector plugin stops data collection. Removes any instrumentation
	 * from the specified thread.
	 *
	 * @param collector    Collector stopping data collection.
	 * @param thread       Thread for which to stop collecting data.
	 */
	virtual void stopCollecting(const Collector& collector,
				    const Thread& thread) const = 0;
	
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
	 * @param metric      Unique identifier of the metric.
	 * @param thread      Thread for which to get a value.
	 * @param range       Address range over which to get a value.
	 * @param interval    Time interval over which to get a value.
	 * @param ptr         Untyped pointer to the return value.
	 */
	virtual void getMetricValue(const std::string& metric,
				    const Thread& thread,
				    const AddressRange& range,
				    const TimeInterval& interval,
				    void* ptr) const = 0;
	
    protected:

	CollectorImpl(const std::string&, const std::string&,
		      const std::string&);
	
	void declareParameter(const Metadata&);
	void declareMetric(const Metadata&);
	
    private:

	/** Set of parameters. */
	std::set<Metadata> dm_parameters;
	
	/** Set of metrics. */
	std::set<Metadata> dm_metrics;
	
    };
    
    
    
} }



#endif
