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
 * Declaration and definition of the TypedParameter template.
 *
 */

#ifndef _OpenSpeedShop_Framework_TypedParameter_
#define _OpenSpeedShop_Framework_TypedParameter_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Parameter.hxx"

#include <string>
#include <typeinfo>



namespace OpenSpeedShop { namespace Framework {

    /**
     * Typed parameter with metadata.
     *
     * Template for a typed parameter with metadata. The parameter value's type
     * is specified by the template instantiation while its metadata and default
     * value are specified at object construction. Used for specifying options
     * to performance data collectors such that runtime introspection of those
     * options is possible.
     *
     * @ingroup CollectorAPI
     */
    template <typename T>
    class TypedParameter :
	public Parameter
    {

    public:

	/** Constructor from metadata and a default value. */
	TypedParameter(const std::string& unique_id,
		       const std::string& short_name,
		       const std::string& description,
		       const T& value = T()) :
	    Parameter(unique_id, short_name, description, typeid(T)),
	    dm_value(value)
	{	    
	}

	/** Operator "=" defined for our value's type. */
        TypedParameter& operator=(const T& value)
	{
            dm_value = value;
            return *this;
        }

	/** Conversion operator to our value's type. */
	operator T() const
	{
	    return dm_value;
	}
	
      private:

	/** Our value. */
	T dm_value;
	
    };
    
} }



#endif
