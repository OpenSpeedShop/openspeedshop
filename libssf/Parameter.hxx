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
 * Declaration and definition of the Parameter class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Parameter_
#define _OpenSpeedShop_Framework_Parameter_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Metadata.hxx"

#include <string>
#include <typeinfo>



namespace OpenSpeedShop { namespace Framework {

    /**
     * Generic parameter with metadata.
     *
     * Base class for all parameters with metadata. Really the only reason such
     * a base class is needed is to allow placement of paramters of mixed types 
     * into STL containers. For example, allowing a std::set<Parameter*> to
     * contain both a TypedParameter<unsigned> and a TypedParameter<double>.
     *
     * @ingroup Implementation
     */
    class Parameter :
	public Metadata
    {

    public:

	/**
	 * Destructor.
	 *
	 * Does nothing. Defined here only to insure that the compiler realizes
	 * that Parameter is polymorphic.
	 */
	virtual ~Parameter()
	{
	}

    protected:

	/** Constructor from metadata. */
	Parameter(const std::string& unique_id, const std::string& short_name,
		  const std::string& description, const std::type_info& type) :
	    Metadata(unique_id, short_name, description, type)
	{
	}

    };
    
} }



#endif
