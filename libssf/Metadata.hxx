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
 * Declaration and definition of the Metadata class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Metadata_
#define _OpenSpeedShop_Framework_Metadata_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "TotallyOrdered.hxx"

#include <string>
#include <typeinfo>



namespace OpenSpeedShop { namespace Framework {

    /**
     * Object metadata.
     *
     * Base class for all objects that contain metadata useful in describing
     * themselves. Each such object is assigned a unique identifier, short name,
     * verbose description, and underlying type when the object is constructed.
     * Member functions for querying these properties are also defined. Metadata
     * object are defined as being equivalent when their unique identifiers are
     * identical.
     *
     * @ingroup Implementation
     */
    class Metadata :
	public TotallyOrdered<Metadata>
    {

    public:

	/** Default constructor. */
	Metadata() :
	    dm_unique_id(),
	    dm_short_name(),
	    dm_description(),
	    dm_type()
        {
        }
		
        /** Operator "<" defined for two Metadata objects. */
        bool operator<(const Metadata& other) const
	{
	    return getUniqueId() < other.getUniqueId();
	}

	/** Read-only data member accessor function. */
	const std::string& getUniqueId() const
	{
	    return dm_unique_id;
	}
	
	/** Read-only data member accessor function. */
	const std::string& getShortName() const
	{
	    return dm_short_name;
	}
	
	/** Read-only data member accessor function. */
	const std::string& getDescription() const
	{ 
	    return dm_description; 
	}
	
	/** Test if underlying type is the specified type. */
	bool isType(const std::type_info& type) const
	{
	    return dm_type == type.name();
	}

    protected:

	/** Constructor from metadata. */
	Metadata(const std::string& unique_id, const std::string& short_name,
		 const std::string& description, const std::type_info& type) :
	    dm_unique_id(unique_id),
	    dm_short_name(short_name),
	    dm_description(description),
	    dm_type(type.name())
	{
	}
	
    private:

	/** Unique identifier. */
	std::string dm_unique_id;
	
	/** Short name. */
        std::string dm_short_name;
	
        /** Verbose description. */
        std::string dm_description;

	/** Underlying type. */
	std::string dm_type;
	
    };
    
} }



#endif
