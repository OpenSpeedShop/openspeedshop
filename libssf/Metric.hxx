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
 * Declaration and definition of the Metric class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Metric_
#define _OpenSpeedShop_Framework_Metric_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Metadata.hxx"

#include <string>
#include <typeinfo>



namespace OpenSpeedShop { namespace Framework {

    /**
     * Generic metric with metadata.
     *
     * Base class for all metrics with metadata. Really the only reason such
     * a base class is needed is to allow placement of metrics of mixed types 
     * into STL containers. For example, allowing a std::set<Metric*> to contain
     * both a TypedMetric<unsigned> and a TypedMetric<double>.
     *
     * @ingroup Implementation
     */
    class Metric :
	public Metadata
    {

    public:

	/**
	 * Destructor.
	 *
	 * Does nothing. Defined here only to insure the compiler realizes that
	 * Metric is a polymorphic class.
	 */
	virtual ~Metric()
	{
	}
	
    protected:
	
	/** Constructor from metadata. */
        Metric(const std::string& unique_id, const std::string& short_name,
	       const std::string& description, const std::type_info& type) :
            Metadata(unique_id, short_name, description, type)
	{
        }

    };
    
} }



#endif
