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
 * Definition of the PluginTable class.
 *
 */

#include "Collector.hxx"
#include "Guard.hxx"
#include "PluginTable.hxx"

#include <ltdl.h>
#include <stdexcept>
#include <stdlib.h>

using namespace OpenSpeedShop::Framework;



namespace {



    /**
     * Collector plugin search callback.
     *
     * Simply re-routes callbacks made during a collector plugin search to the
     * appropriate PluginTable object.
     *
     * @param filename    File name of potential collector plugin.
     * @param data        Untyped pointer to the PluginTable.
     * @return            Always returns zero to insure search continues.
     */
    int foreachCallback(const char* filename, lt_ptr data)
    {
	// Re-route the callback to the actual PluginTable object
	PluginTable* table = reinterpret_cast<PluginTable*>(data);
	table->foreachCallback(filename);
	
	// Always return zero to insure we keep searching
	return 0;
    }
    
    
    
}



/**
 * Default constructor.
 *
 * Builds the table of available collector plugins by searching three different
 * directories: the compile-time collector plugin path as passed via a macro
 * from the build system; the home-relative collector plugin path defined by
 * the environment variable OPENSPEEDSHOP_HOME; and a user-specified collector
 * plugin path defined by the environment variable OPENSPEEDSHOP_COLLECTOR_PATH.
 */
PluginTable::PluginTable()
{
    // Initialize libltdl
    Assert(lt_dlinit() == 0);
    
    // Start with an empty user-defined search path
    Assert(lt_dlsetsearchpath("") == 0);
    
    // Add the compile-time collector plugin path
    Assert(lt_dladdsearchdir(COLLECTOR_PATH) == 0);
    
    // Add the home-relative collector plugin path
    if(getenv("OPENSPEEDSHOP_HOME") != NULL) {
	Path home_relative_path =
	    Path(getenv("OPENSPEEDSHOP_HOME")) +
	    Path("/lib/openspeedshop");
	Assert(lt_dladdsearchdir(home_relative_path.c_str()) == 0);	    
    }
    
    // Add the user-specified collector plugin path
    if(getenv("OPENSPEEDSHOP_COLLECTOR_PATH") != NULL) {
	Path user_specified_path = 
	    Path(getenv("OPENSPEEDSHOP_COLLECTOR_PATH"));
	Assert(lt_dladdsearchdir(user_specified_path.c_str()) == 0);
    }
    
    // Now search for plugins in all these paths
    lt_dlforeachfile(lt_dlgetsearchpath(), ::foreachCallback, this);
}



/**
 * Destructor.
 *
 * Destroys the plugin table.
 *
 * @pre    All collector instances associated with plugins in this table must be
 *         destroyed prior to the destruction of the table itself. An exception
 *         of type std::logic_error is thrown if any such instances remain.
 *
 * @note    Normally it is real bad practice to throw an exception from inside
 *          a destructor. PluginTable, however, is only instantiated as a static
 *          data member of the Collector class. As such, its destructor is only
 *          called when the application is shutting down. And it should never be
 *          called during a stack unwind - which is the "real bad case" where a
 *          throw from a destructor actually results in an abort. So throwing
 *          here is relatively safe.
 */
PluginTable::~PluginTable()
{
    Guard guard_myself(this);
    
    // Check preconditions
    
    for(std::map<std::string, PluginEntry>::const_iterator
	    i = dm_unique_id_to_entry.begin();
	i != dm_unique_id_to_entry.end();
	++i)
	if((i->second.instances > 0) || (i->second.handle != NULL))
	    throw std::logic_error("Cannot destroy a plugin table before all "
				   "its collector instances are destroyed.");
    
    // Exit libltdl
    Assert(lt_dlexit() == 0);
}



/**
 * Get all available collectors.
 *
 * Returns to the caller the metadata for all available collectors. An empty set
 * is returned if no collector plugins were found.
 *
 * @return    Metadata for all available collectors.
 */
std::set<Metadata> PluginTable::getAvailableCollectors() const
{
    Guard guard_myself(this);

    // Assemble the metadata for all available collectors
    std::set<Metadata> metadata;
    for(std::map<std::string, PluginEntry>::const_iterator
	    i = dm_unique_id_to_entry.begin();
	i != dm_unique_id_to_entry.end();
	++i)
	metadata.insert(i->second.metadata);
    
    // Return the metadata to the caller
    return metadata;
}



/**
 * Instantiate a collector.
 *
 * Instantiates a collector instance by calling the corresponding plugin's
 * collector factory method and incrementing the corresponding plugin's instance
 * count. If the plugin's instance count was zero, the plugin is automatically
 * loaded into memory first.
 *
 * @pre    Instances can only be created for collectors for which a corresond-
 *         ing plugin was found. An exception of type std::invalid_argument is
 *         thrown if a plugin was not found for the unique identifier. 
 *
 * @note    It is possible that a collector plugin found during the search for
 *          available plugins is subsequently removed before it is loaded for
 *          an actual instantiation. Or that a new module without a collector
 *          factory method was substituted in its place. Or that a new collector
 *          plugin, with different unique identifier, was substituted in its
 *          place. An exception of type std::runtime_error is throw in all of
 *          these unlikely situations.
 *
 * @param unique_id    Unique identifier of the collector to be instantiated.
 * @return             New instance of the named collector.
 */
Collector* PluginTable::instantiateCollector(const std::string& unique_id)
{
    // Find the entry for this unique identifier's plugin
    std::map<std::string, PluginEntry>::iterator
	i = dm_unique_id_to_entry.find(unique_id);
    if(i == dm_unique_id_to_entry.end())
	throw std::runtime_error("Cannot create a collector instance for "
				 "unknown unique identifier \"" + unique_id +
				 "\"");
    
    // Is this the first instance of this collector?
    if(i->second.instances == 0) {
	
	// Load the plugin into memory
	i->second.handle = lt_dlopenext(i->second.path.c_str());
	if(i->second.handle == NULL)
	    throw std::runtime_error("Cannot load previously detected "
				     "collector plugin \"" + i->second.path +
				     "\".");
	
    }
    
    // Find the collector factory method in the plugin
    lt_dlhandle handle = reinterpret_cast<lt_dlhandle>(i->second.handle);
    Assert(handle != NULL);
    Collector* (*factory)() = (Collector* (*)())
	lt_dlsym(handle, "CollectorFactory");
    if(factory == NULL)
	throw std::runtime_error("Cannot locate collector factory method "
				 "in previously detected collector plugin \"" +
				 i->second.path + "\"");
    
    // Create an instance of this collector
    Collector* collector = (*factory)();
    if(i->second.metadata != *collector)
	throw std::runtime_error("Metadata for previously detected "
				 "collector plugin \"" + i->second.path +
				 "\" has changed.");
    
    // Increment the plugin's instance count
    i->second.instances++;    
    
    // Return the collector to the caller
    return collector;
}



/**
 * Destroy a collector instance.
 *
 * Destroys a collector instance by destorying the actual Collector object and
 * decrementing the corresponding plugin's instance count. If the plugin's
 * instance count has reached zero, the plugin is automatically unloaded from
 * memory.
 *
 * @param collector    Collector instance to be destroyed.
 */
void PluginTable::destroyCollector(Collector* collector)
{
    // Find the entry for this collector's plugin
    std::map<std::string, PluginEntry>::iterator
	i = dm_unique_id_to_entry.find(collector->getUniqueId());
    Assert(i != dm_unique_id_to_entry.end());
    
    // Destroy the collector
    delete collector;
    
    // Decrement the plugin's instance count
    i->second.instances--;

    // Was this the last instance of this plugin?
    if(i->second.instances == 0) {

	// Unload the plugin from memory
	lt_dlhandle handle = reinterpret_cast<lt_dlhandle>(i->second.handle);
	Assert(i->second.handle != NULL);
	Assert(lt_dlclose(handle) == 0);
	i->second.handle = NULL;

    }
}



/**
 * Collector plugin search callback.
 *
 * Callback function called (indirectly via ::foreachCallback) by libltdl for
 * each potential collector plugin candidate found in our specified search path.
 * The candidate is examined to determine if it is truly a collector plugin and,
 * if so, is added to our table of available collector plugins.
 *
 * @note    The Collector instances created by each plugin must have a unique
 *          identifier different from that of any other plugin. Once a given
 *          unique identifier has been associated with a given plugin, any
 *          attempt by a subsequent plugin to use that unique identifier will
 *          result in it being silently ignored.
 *
 * @param filename    File name of potential collector plugin.
 */
void PluginTable::foreachCallback(const std::string& filename)
{
    Guard guard_myself(this);
    
    // Can we open this file as a libltdl module?
    lt_dlhandle handle = lt_dlopenext(filename.c_str());
    if(handle == NULL)
	return;
    
    // Is there a collector factory method in this module?
    Collector* (*factory)() = (Collector* (*)())
	lt_dlsym(handle, "CollectorFactory");
    if(factory == NULL) {
	Assert(lt_dlclose(handle) == 0);
	return;
    }
    
    // Create an instance of this collector
    Collector* collector = (*factory)();
    
    // Have we encountered a collector plugin using this unique identifier?
    if(dm_unique_id_to_entry.find(collector->getUniqueId()) !=
       dm_unique_id_to_entry.end()) {
	delete collector;
	Assert(lt_dlclose(handle) == 0);
	return;
    }
    
    // Create an entry for this collector plugin
    PluginEntry entry;
    entry.path = filename;
    entry.metadata = *collector;
    entry.instances = 0;
    entry.handle = NULL;
    
    // Add this entry to the table of collector plugins
    dm_unique_id_to_entry.insert(std::make_pair(collector->getUniqueId(),
						entry));
    
    // Destroy the collector instance
    delete collector;
    
    // Close the module handle
    Assert(lt_dlclose(handle) == 0);
}
