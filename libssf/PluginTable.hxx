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
 * Declaration and definition of the PluginTable class.
 *
 */

#ifndef _OpenSpeedShop_Framework_PluginTable_
#define _OpenSpeedShop_Framework_PluginTable_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Lockable.hxx"
#include "Metadata.hxx"
#include "Path.hxx"

#include <map>
#include <set>
#include <string>



namespace OpenSpeedShop { namespace Framework {

    class Collector;

    /**
     * Collector plugin table.
     *
     * Table used for keeping track of collector plugins. The table of available
     * collector plugins is built once at object construction. Member functions
     * provide the list of available collectors and the means by which they can
     * be instantiated and destroyed. Actual plugin modules are automatically
     * loaded and unloaded from memory as necessary.
     *
     * @ingroup Implementation
     */    
    class PluginTable :
	private Lockable
    {
	
    public:
	
	PluginTable();
	~PluginTable();
	
	std::set<Metadata> getAvailableCollectors() const;
	Collector* instantiateCollector(const std::string&);
	void destroyCollector(Collector*);
	
	void foreachCallback(const std::string&);
	
    private:

	/**
	 * Plugin entry.
	 *
	 * Structure for an entry in the plugin table describing a single
	 * collector plugin. Contains the path, instance count, and handle for
	 * the plugin.
	 *
	 * @ingroup Implementation
	 */
	struct PluginEntry
	{
	    
	    /** Path of this plugin. */
	    Path path;
	    
	    /** Metadata for this plugin's Collector class. */
	    Metadata metadata;
	    
	    /** Number of instances of this plugin's Collector class. */
	    unsigned instances;
	    
	    /** Handle to this plugin. */
	    void* handle;
	    
	};	
	
	/** Map collector's unique identifiers to their plugin entries. */
	std::map<std::string, PluginEntry> dm_unique_id_to_entry;
	
    };
    
} }



#endif
