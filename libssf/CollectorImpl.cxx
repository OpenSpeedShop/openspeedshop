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
 * Definition of the CollectorImpl class.
 *
 */

#include "CollectorImpl.hxx"

#include <stdexcept>

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
 * @pre    Each parameter in a collector must be unique. An exception of type
 *         std::invalid_argument is thrown if an attempt is made to redeclare
 *         a parameter.
 *
 * @param parameter    Parameter to be declareed.
 */
void CollectorImpl::declareParameter(const Metadata& parameter)
{
    // Check preconditions
    if(dm_parameters.find(parameter) != dm_parameters.end())
	throw std::invalid_argument("Parameter \"" + parameter.getUniqueId() +
				    "\" cannot be redeclared.");
    
    // Add the new parameter
    dm_parameters.insert(parameter);
}



/**
 * Declare a metric.
 *
 * Declares a metric by adding it to this collector implementation. Called by
 * derived classes when specifying their metrics.
 *
 * @pre    Each metric in a collector must be unique. An exception of type
 *         std::invalid_argument is thrown if an attempt is made to redeclare
 *         a metric.
 *
 * @param metric    Metric to be declareed.
 */
void CollectorImpl::declareMetric(const Metadata& metric)
{
    // Check preconditions
    if(dm_metrics.find(metric) != dm_metrics.end())
	throw std::invalid_argument("Metric \"" + metric.getUniqueId() +
				    "\" cannot be redeclared.");
    
    // Add the new metric
    dm_metrics.insert(metric);
}
