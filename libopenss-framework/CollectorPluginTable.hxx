////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
 * Declaration of the CollectorPluginTable class.
 *
 */

#ifndef _OpenSpeedShop_Framework_CollectorPluginTable_
#define _OpenSpeedShop_Framework_CollectorPluginTable_

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

    class CollectorImpl;

    /**
     * Collector plugin table.
     *
     * Table used for keeping track of collector plugins. The table is built
     * only once, when it is first required. Member functions provide the list
     * of available collector implementations and the means by which they can
     * be instantiated and destroyed. Actual plugin modules are automatically
     * loaded and unloaded from memory as necessary.
     *
     * @todo    Once built, the table is never modified. However the instance
     *          count and handle for the individual entries are modified. So it
     *          would seem to make sense to have individual entries lockable.
     *          Unfortunately Lockable is currently also NonCopyable, and in
     *          order to place Entry objects into an STL container, Entry must
     *          be copyable. Hence we simply lock the whole table for now. This
     *          should probably be fixed in the future.
     *
     * @ingroup Implementation
     */
    class CollectorPluginTable :
	private Lockable
    {
	
    public:

	static CollectorPluginTable TheTable;
	
	CollectorPluginTable();
	virtual ~CollectorPluginTable();
	
	std::set<Metadata> getAvailable();
	CollectorImpl* instantiate(const std::string&);
	void destroy(CollectorImpl*);

	void foreachCallback(const std::string&);

    private:

	/** Flag indicating if table has been built. */
	bool dm_is_built;
	
	/**
	 * Collector plugin table entry.
	 *
	 * Structure for an entry in the collector plugin table describing a
	 * single collector plugin. Contains the metadata, path, instance count,
	 * and handle for the plugin.
	 */
	struct Entry
	{
	    
	    /** Metadata for this plugin's Collector class. */
	    Metadata dm_metadata;
	    
	    /** Path of this plugin. */
	    Path dm_path;
	    
	    /** Number of instances of this plugin's Collector class. */
	    unsigned dm_instances;
	    
	    /** Handle to this plugin. */
	    void* dm_handle;	
	    
	    /** Default constructor. */
	    Entry() :
		dm_metadata(),
		dm_path(""),
		dm_instances(0),
		dm_handle(NULL)
	     {
	     }		
	    
	};	
	
	/** Map collector plugin's unique identifiers to their entry. */
        std::map<std::string, Entry> dm_unique_id_to_entry;
	
	void build();
	
    };
    
} }



#endif
