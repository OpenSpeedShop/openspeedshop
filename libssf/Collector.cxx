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
 * Definition of the Collector class.
 *
 */

#include "Assert.hxx"
#include "Collector.hxx"
#include "CollectorImpl.hxx"
#include "Database.hxx"
#include "Guard.hxx"
#include "Lockable.hxx"
#include "Optional.hxx"
#include "Path.hxx"
#include "Thread.hxx"
#include "ThreadGroup.hxx"

#include <ltdl.h>
#include <map>
#include <stdexcept>
#include <stdlib.h>
#include <typeinfo>

using namespace OpenSpeedShop::Framework;



namespace {

    // Forward Declaration
    int foreachCallback(const char*, lt_ptr);
	    
    

    /**
     * Collector plugin table.
     *
     * Table used for keeping track of collector plugins. The table of available
     * collector plugins is built once at object construction. Member functions
     * provide the list of available collector implementations and the means by
     * which they can be instantiated and destroyed. Actual plugin modules are
     * automatically loaded and unloaded from memory as necessary.
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
	
	

	/**
	 * Default constructor.
	 *
	 * Builds the table of available collector plugins by searching three
	 * different directories: the compile-time collector plugin path as
	 * passed via a macro from the build system; the home-relative collector
	 * plugin path defined by the environment variable OPENSPEEDSHOP_HOME;
	 * and a user-specified collector plugin path defined by the environment
	 * variable OPENSPEEDSHOP_COLLECTOR_PATH.
	 */
	CollectorPluginTable::CollectorPluginTable()
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
	 * Destroys the collector plugin table.
	 *
	 * @pre    All collector implementation instances associated with
	 *         plugins in this table must be destroyed prior to the
	 *         destruction of the table itself. An exception of type
	 *         std::logic_error is thrown if any such instances remain.
	 *
	 * @note    Normally it is real bad practice to throw an exception from
	 *          inside a destructor. CollectorPluginTable, however, is only
	 *          instantiated as a static object. As such, its destructor is
	 *          only called when the tool is shutting down. And it should
	 *          never be called during a stack unwind - which is the "real
	 *          bad case" where a throw from a destructor actually results
	 *          in an abort. So throwing here is relatively safe.
	 */
	CollectorPluginTable::~CollectorPluginTable()
	{
	    // Check preconditions    
	    for(std::map<std::string, Entry>::const_iterator
		    i = dm_unique_id_to_entry.begin();
		i != dm_unique_id_to_entry.end();
		++i)
		if((i->second.instances > 0) || (i->second.handle != NULL))
		    throw std::logic_error(
			"Cannot destroy the collector plugin table before all "
			"its collector implementation instances are destroyed."
			);
	    
	    // Exit libltdl
	    Assert(lt_dlexit() == 0);
	}
	


	/**
	 * Get all available collectors implementations.
	 *
	 * Returns to the caller the metadata for all available collector
	 * implementations. An empty set is returned if no collector plugins
	 * were found.
	 *
	 * @return    Metadata for all available collector implementations.
	 */
	std::set<Metadata> getAvailable() const
	{
	    // Assemble the metadata for all available collector implementations
	    std::set<Metadata> metadata;
	    for(std::map<std::string, Entry>::const_iterator
		    i = dm_unique_id_to_entry.begin();
		i != dm_unique_id_to_entry.end();
		++i)
		metadata.insert(i->second.metadata);
	    
	    // Return the metadata to the caller
	    return metadata;
	}
	
	

	/**
	 * Instantiate a collector implementation.
	 *
	 * Instantiates a collector implementation by calling the corresponding
	 * plugin's factory method and incrementing the corresponding plugin's
	 * instance count. If the plugin's instance count was zero, the plugin
	 * is automatically loaded into memory first.
	 *
	 * @note    Instances can only be created for collector implementations
	 *          for which a corresonding plugin was found. An exception of
	 *          type std::invalid_argument is thrown if a plugin was not
	 *          found for the specified unique identifier.
	 *
	 * @note    It is possible that a collector plugin found during the
	 *          search for available plugins is subsequently removed before
	 *          it is loaded for an actual instantiation. Or that a new 
	 *          module without a collector factory method was substituted
	 *          in its place. Or that a new collector plugin, with different
	 *          unique identifier, was substituted in its place. An
	 *          exception of type std::runtime_error is throw in all of
	 *          these unlikely situations.
	 *
	 * @param unique_id    Unique identifier of the collector implementation
	 *                     to be instantiated.
	 * @return             New instance of this collector implementation.
	 */
	CollectorImpl* instantiate(const std::string& unique_id)
	{
	    CollectorImpl* instance;
    
	    // Find the entry for this unique identifier's plugin
	    std::map<std::string, Entry>::iterator
		i = dm_unique_id_to_entry.find(unique_id);
	    if(i == dm_unique_id_to_entry.end())
		throw std::invalid_argument(
		    "Cannot create a collector implementation instance for "
		    "unknown unique identifier \"" + unique_id + "\"."
		    );
	    
	    // Critical section touching the collector plugin's entry
	    {
		Guard guard_myself(this);
		
		// Is this the first instance of this collector implementation?
		if(i->second.instances == 0) {
		    
		    // Load the plugin into memory
		    i->second.handle = lt_dlopenext(i->second.path.c_str());
		    if(i->second.handle == NULL)
			throw std::runtime_error(
			    "Cannot load previously detected collector "
			    "plugin \"" + i->second.path + "\"."
			    );
		    
		}
		
		// Find the factory method in the plugin
		lt_dlhandle handle = 
		    reinterpret_cast<lt_dlhandle>(i->second.handle);
		Assert(handle != NULL);
		CollectorImpl* (*factory)() = (CollectorImpl* (*)())
		    lt_dlsym(handle, "CollectorFactory");
		if(factory == NULL)
		    throw std::runtime_error(
			"Cannot locate factory method in previously detected "
			"collector plugin \"" + i->second.path + "\""
			);
		
		// Create an instance of this collector
		instance = (*factory)();
		if(i->second.metadata != *instance)
		    throw std::runtime_error(
			"Metadata for previously detected collector "
			"plugin \"" + i->second.path + "\" has changed.");
		
		// Increment the plugin's instance count
		i->second.instances++;    
		
	    }
	    
	    // Return the instance to the caller
	    return instance;
	}
	
	
	
	/**
	 * Destroy a collector implementation instance.
	 *
	 * Destroys a collector implementation instance by destorying the actual
	 * CollectorImpl object and decrementing the corresponding plugin's
	 * instance count. If the plugin's instance count has reached zero, the
	 * plugin is automatically unloaded from memory.
	 *
	 * @param impl    Collector implementation instance to be destroyed.
	 */
	void destroy(CollectorImpl* impl)
	{
	    // Find the entry for this collector's plugin
	    std::map<std::string, Entry>::iterator
		i = dm_unique_id_to_entry.find(impl->getUniqueId());
	    Assert(i != dm_unique_id_to_entry.end());

	    // Destroy the implementation
	    delete impl;
	    
	    // Critical section touching the collector plugin's entry
	    {
		Guard guard_myself(this);
		
		// Decrement the plugin's instance count
		i->second.instances--;
		
		// Was this the last instance of this plugin?
		if(i->second.instances == 0) {
		    
		    // Unload the plugin from memory
		    lt_dlhandle handle =
			reinterpret_cast<lt_dlhandle>(i->second.handle);
		    Assert(i->second.handle != NULL);
		    Assert(lt_dlclose(handle) == 0);
		    i->second.handle = NULL;
		    
		}
	    }
	}
	

	
	/**
	 * Collector plugin search callback.
	 *
	 * Callback function called (indirectly via ::foreachCallback) by
	 * libltdl for each potential collector plugin candidate found in our
	 * specified search path. The candidate is examined to determine if
	 * it is truly a collector plugin and if so, is added to our table of
	 * available collector plugins.
	 *
	 * @note    The collector implementation instances created by each
	 *          plugin must have a unique identifier different from that
	 *          of any other plugin. Once a given unique identifier has
	 *          been associated with a given plugin, any attempt by a
	 *          subsequent plugin to use that unique identifier will
	 *          result in it being silently ignored.
	 *
	 * @param filename    File name of potential collector plugin.
	 */
	void foreachCallback(const std::string& filename)
	{    
	    // Can we open this file as a libltdl module?
	    lt_dlhandle handle = lt_dlopenext(filename.c_str());
	    if(handle == NULL)
		return;
    
	    // Is there a collector factory method in this module?
	    CollectorImpl* (*factory)() = (CollectorImpl* (*)())
		lt_dlsym(handle, "CollectorFactory");
	    if(factory == NULL) {
		Assert(lt_dlclose(handle) == 0);
		return;
	    }
    
	    // Create a temporary instance of this collector implementation
	    CollectorImpl* instance = (*factory)();
	    
	    // Have we already used this unique identifier?
	    if(dm_unique_id_to_entry.find(instance->getUniqueId()) != 
	       dm_unique_id_to_entry.end()) {
		delete instance;
		Assert(lt_dlclose(handle) == 0);
		return;
	    }
	    
	    // Create an entry for this collector plugin
	    Entry entry;
	    entry.metadata = *instance;
	    entry.path = filename;
	    entry.instances = 0;
	    entry.handle = NULL;
	    
	    // Add this entry to the table of collector plugins
	    dm_unique_id_to_entry.insert(std::make_pair(instance->getUniqueId(),
							entry));
	    
	    // Destroy the temporary instance
	    delete instance;
	    
	    // Close the module handle
	    Assert(lt_dlclose(handle) == 0);
	}
	


    private:

	/**
	 * Collector plugin table entry.
	 *
	 * Structure for an entry in the collector plugin table describing
	 * a single collector plugin. Contains the path, instance count, and
	 * handle for the plugin.
	 *
	 * @ingroup Implementation     
	 */
	struct Entry
	{
	    
	    /** Metadata for this plugin's Collector class. */
	    Metadata metadata;
	    
	    /** Path of this plugin. */
	    Path path;
	    
	    /** Number of instances of this plugin's Collector class. */
	    unsigned instances;
	    
	    /** Handle to this plugin. */
	    void* handle;	
	    
	};	
	
	/** Map collector plugin's unique identifiers to their entries. */
        std::map<std::string, Entry> dm_unique_id_to_entry;
	
    };
    
    

    /**
     * Collector plugin search callback.
     *
     * Simply re-routes callbacks made during a collector plugin search to the
     * appropriate CollectorPluginTable object.
     *
     * @param filename    File name of potential collector plugin.
     * @param data        Untyped pointer to the CollectorPluginTable.
     * @return            Always returns zero to insure search continues.
     */
    int foreachCallback(const char* filename, lt_ptr data)
    {
	// Re-route the callback to the actual PluginTable object
	CollectorPluginTable* table =
	    reinterpret_cast<CollectorPluginTable*>(data);
	table->foreachCallback(filename);
	
	// Always return zero to insure we keep searching
	return 0;
    } 
    
    
    
    /** Table of collector plugins. */
    CollectorPluginTable collector_plugin_table;
    
    
    
}



/**
 * Get all available collectors.
 *
 * Returns the metadata for all available collectors. An empty set is returned
 * if no collector plugins were found.
 *
 * @return    Metadata for all available collectors.
 */
std::set<Metadata> Collector::getAvailable()
{
    // Defer to the collector plugin table
    return collector_plugin_table.getAvailable();
}



/**
 * Copy constructor.
 *
 * Constructs a new Collector by copying the specified collector. The compiler
 * provided default is insufficient here because we need to instantiate another,
 * separate, implementation for the new copy.
 *
 * @param other    Collector to be copied.
 */
Collector::Collector(const Collector& other) :
    dm_database(other.dm_database),
    dm_entry(other.dm_entry),
    dm_impl(NULL)
{
    // Attempt to instantiate an implementation for this collector
    if(other.dm_impl != NULL) {
	try {
	    dm_impl = collector_plugin_table.
		instantiate(other.dm_impl->getUniqueId());
	}
	catch(...) {
	}	
    }
}



/**
 * Destructor.
 *
 * Destroys the collector's implementation if it had one.
 */
Collector::~Collector()
{
    // Destroy the implementation (if any)
    if(dm_impl != NULL)
	collector_plugin_table.destroy(dm_impl);
}



/**
 * Assignment operator.
 *
 * Operator "=" defined for a Collector object. The compiler provided default
 * is insufficient here because we need to instantiate another, separate,
 * implementation for the new copy.
 * 
 * @param other    Collector to be copied.
 */
Collector& Collector::operator=(const Collector& other)
{
    // Only do an assignment if the objects differ
    if((dm_database != other.dm_database) || (dm_entry != other.dm_entry)) {
	
	// Destroy our current implementation (if any)
	if(dm_impl != NULL)
	    collector_plugin_table.destroy(dm_impl);
	
	// Replace our collector with the new collector
	dm_database = other.dm_database;
	dm_entry = other.dm_entry;
	dm_impl = NULL;
	
	// Attempt to instantiate an implementation for this collector
	if(other.dm_impl != NULL) {
	    try {
		dm_impl = collector_plugin_table.
		    instantiate(other.dm_impl->getUniqueId());
	    }
	    catch(...) {
	    }	
	}
	
    }
}



/**
 * Get our metadata.
 *
 * Returns the metadata of this collector.
 *
 * @return    Metadata for this collector.
 */
Metadata Collector::getMetadata() const
{
    // Defer to our implementation (if any)    
    if(dm_impl != NULL)
        return *dm_impl;
    
    // Check assertions
    Assert(!dm_database.isNull());
    
    // Find our unique identifier
    std::string unique_id;
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    dm_database->prepareStatement(
        "SELECT unique_id FROM Collectors WHERE id = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        unique_id = dm_database->getResultAsString(1);
    END_TRANSACTION(dm_database);
    
    // Return the partial metadata to the caller
    return Metadata(unique_id, "", "", typeid(void));
}



/**
 * Get our parameters.
 *
 * Returns the metadata for all parameters of this collector. An empty set is
 * returned if this collector has no parameters.
 *
 * @pre    Only applies to a collector for which an implementation was found.
 *         An exception of type std::logic_error is thrown if called for a
 *         collector that has no implementation.
 *
 * @return    Metadata for all parameters of this collector.
 */
std::set<Metadata> Collector::getParameters() const
{
    // Check assertions
    Assert(!dm_database.isNull());
    
    // Check preconditions
    if(dm_impl == NULL)
        throw std::logic_error("Collector has no implementation.");
    
    // Defer to our implementation
    return dm_impl->getParameters();    
}



/**
 * Get our metrics.
 *
 * Returns the metadata for all metrics of this collector. An empty set is
 * returned if this collector has no metrics (unlikely).
 *
 * @pre    Only applies to a collector for which an implementation was found.
 *         An exception of type std::logic_error is thrown if called for a
 *         collector that has no implementation.
 *
 * @return    Metadata for all metrics of this collector.
 */
std::set<Metadata> Collector::getMetrics() const
{
    // Check assertions
    Assert(!dm_database.isNull());

    // Check preconditions
    if(dm_impl == NULL)
        throw std::logic_error("Collector has no implementation.");

    // Defer to our implementation
    return dm_impl->getMetrics();
}



/**
 * Get our threads.
 *
 * Returns all the threads currently attached to this collector. An empty thread
 * group is returned if this collector isn't attached to any threads.
 *
 * @return    Threads to which this collector is currently attached.
 */
ThreadGroup Collector::getThreads() const
{
    ThreadGroup threads;

    // Check assertions
    Assert(!dm_database.isNull());

    // Find our attached threads
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    dm_database->prepareStatement(
        "SELECT thread FROM Attachments WHERE collector = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        threads.push_back(Thread(dm_database,
                                 dm_database->getResultAsInteger(1)));
    END_TRANSACTION(dm_database);
    
    // Return the threads to the caller
    return threads;
}



/**
 * Attach to a thread.
 *
 * Attaches the specified thread to this collector. If the collector is
 * currently collecting performance data, collection is automatically started
 * for the newly attached thread.
 *
 * @pre    The thread must be in the same experiment as the collector. An
 *         exception of type std::invalid_arugment or Database::Corrupted is
 *         thrown if the thread is in a different experiment than the collector.
 *
 * @pre    A thread that is already attached to this collector cannot be
 *         attached again without first being detached. An exception of type
 *         std::logic_error is thrown if multiple attachments are attempted.
 *
 * @param thread    Thread to be attached.
 */
void Collector::attachThread(const Thread& thread) const
{
    // Check assertions
    Assert(!dm_database.isNull());

    // Check preconditions
    if(thread.dm_database != dm_database)
	throw std::invalid_argument("Cannot attach to a thread that isn't in "
				    "the same experiment as the collector.");
    
    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    thread.validateEntry();

    // Is this attachment already present?
    bool is_attached = false;
    dm_database->prepareStatement(
	"SELECT COUNT(*) FROM Attachments WHERE collector = ? AND thread = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, thread.dm_entry);
    while(dm_database->executeStatement())
	is_attached = dm_database->getResultAsInteger(1) != 0;
    
    // Check preconditions
    if(is_attached)
	throw std::logic_error("Cannot attach a thread to a collector "
			       "more than once.");
    
    // Create the attachment
    dm_database->prepareStatement(
	"INSERT INTO Attachments (collector, thread) VALUES (?, ?);"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, thread.dm_entry);
    while(dm_database->executeStatement());
    
    // Are we collecting?
    bool is_collecting = false;
    dm_database->prepareStatement(
	"SELECT is_collecting FROM Collectors WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	is_collecting = 
	    (dm_database->getResultAsInteger(1) != 0) ? true : false;
    
    // Start data collection for this thread if we are collecting
    if(is_collecting) {
	
	// Check assertions
	Assert(dm_impl != NULL);

	// Defer to our implementation
	dm_impl->startCollecting(*this, thread);
	
    }
    
    // End this multi-statement transaction
    END_TRANSACTION(dm_database);
}



/**
 * Detach a thread.
 *
 * Detaches the specified thread from this collector. If the collector is
 * currently collecting performance data, collection is automatically stopped
 * for the thread being detached.
 *
 * @pre    The thread must be in the same experiment as the collector. An
 *         exception of type std::invalid_argument Database::Corrupted is
 *         thrown if the thread is in a different experiment than the collector.
 *
 * @pre    A thread cannot be detached before it was attached. An exception of
 *         type std::logic_error is thrown if the thread is not attached to this
 *         collector.
 *
 * @param thread    Thread to be detached.
 */
void Collector::detachThread(const Thread& thread) const
{
    // Check assertions
    Assert(!dm_database.isNull());
    
    // Check preconditions
    if(thread.dm_database != dm_database)
	throw std::invalid_argument("Cannot detach a thread that isn't in "
				    "the same experiment as the collector.");
    
    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    thread.validateEntry();
    
    // Is this attachment already present?
    bool is_attached = false;
    dm_database->prepareStatement(
	"SELECT COUNT(*) FROM Attachments WHERE collector = ? AND thread = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, thread.dm_entry);
    while(dm_database->executeStatement())
	is_attached = dm_database->getResultAsInteger(1) != 0;
    
    // Check preconditions
    if(!is_attached)
	throw std::logic_error("Cannot detach a thread from a collector "
			       "that wasn't previously attached.");

    // Are we collecting?
    bool is_collecting = false;
    dm_database->prepareStatement(
	"SELECT is_collecting FROM Collectors WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	is_collecting = 
	    (dm_database->getResultAsInteger(1) != 0) ? true : false;
    
    // Stop data collection for this thread if we are collecting
    if(is_collecting) {
	
	// Check assertions
	Assert(dm_impl != NULL);

	// Defer to our implementation
	dm_impl->stopCollecting(*this, thread);
	
    }

    // Remove the attachment
    dm_database->prepareStatement(
	"DELETE FROM Attachments WHERE collector = ? AND thread = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, thread.dm_entry);
    while(dm_database->executeStatement());
    
    // End this multi-statement transaction
    END_TRANSACTION(dm_database);
}



/**
 * Test if collecting data.
 *
 * Returns a boolean value indicating if the collector is currently collecting
 * performance data.
 *
 * @return    Boolean "true" if the collector is currently collecting data,
 *            "false" otherwise.
 */
bool Collector::isCollecting() const
{
    bool is_collecting = false;

    // Check assertions
    Assert(!dm_database.isNull());
    
    // Are we collecting?
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    dm_database->prepareStatement(
        "SELECT is_collecting FROM Collectors WHERE id = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	is_collecting = 
	    (dm_database->getResultAsInteger(1) != 0) ? true : false;
    END_TRANSACTION(dm_database);
    
    // Return the state to the caller
    return is_collecting;
}



/**
 * Start data collection.
 *
 * Starts performance data collection for this collector. Data collection can
 * be stopped temporarily or permanently by calling stopCollecting(). All data
 * that is collected is available via the collector's metrics.
 *
 * @pre    Collectors cannot start data collection if they are currently
 *         collecting data. An exception of type std::logic_error is thrown if
 *         an attempt is made to start a collector that is already collecting. 
 *
 * @pre    Only applies to a collector for which an implementation was found.
 *         An exception of type std::logic_error is thrown if called for a
 *         collector that has no implementation.
 */
void Collector::startCollecting() const
{
    // Check assertions
    Assert(!dm_database.isNull());
    
    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    
    // Are we collecting?
    bool is_collecting = false;
    dm_database->prepareStatement(
	"SELECT is_collecting FROM Collectors WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	is_collecting = 
	    (dm_database->getResultAsInteger(1) != 0) ? true : false;
    
    // Check preconditions
    if(is_collecting)
	throw std::logic_error("Cannot start a collector that is "
			       "already collecting.");
    if(dm_impl == NULL)
        throw std::logic_error("Collector has no implementation.");
    
    // Defer to our implementation for each attached thread
    ThreadGroup threads = getThreads();
    for(ThreadGroup::const_iterator 
	    i = threads.begin(); i != threads.end(); ++i)
	dm_impl->startCollecting(*this, *i);
    
    // Indicate we are collecting
    dm_database->prepareStatement(
	"UPDATE Collectors SET is_collecting = 1 WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement());
    
    // End this multi-statement transaction
    END_TRANSACTION(dm_database);
}



/**
 * Stop data collection.
 *
 * Stops performance data collection for this collector. Data collection can 
 * be resumed by calling startCollecting() again. All data that was collected
 * is available via the collector's metrics.
 *
 * @pre    Collectors cannot stop data collection unless they are currently
 *         collecting data. An exception of type std::logic_error is thrown
 *         if an attempt is made to stop a collector that isn't collecting.
 */
void Collector::stopCollecting() const
{
    // Check assertions
    Assert(!dm_database.isNull());
    
    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    
    // Are we collecting?
    bool is_collecting = false;
    dm_database->prepareStatement(
	"SELECT is_collecting FROM Collectors WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	is_collecting = 
	    (dm_database->getResultAsInteger(1) != 0) ? true : false;

    // Check preconditions
    if(!is_collecting)
	throw std::logic_error("Cannot stop a collector that is "
                               "not currently collecting.");
	
    // Check assertions
    Assert(dm_impl != NULL);
	
    // Defer to our implementation for each attached thread
    ThreadGroup threads = getThreads();
    for(ThreadGroup::const_iterator 
	    i = threads.begin(); i != threads.end(); ++i)
	dm_impl->stopCollecting(*this, *i);
    
    // Indicate we are no longer collecting
    dm_database->prepareStatement(
	"UPDATE Collectors SET is_collecting = 0 WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement());
    
    // End this multi-statement transaction
    END_TRANSACTION(dm_database);
}



/**
 * Default constructor.
 *
 * Constructs a Collector that refers to a non-existent collector. Any use of
 * a member function on an object constructed in this way will result in an
 * assertion failure.
 */
Collector::Collector() :
    dm_database(NULL),
    dm_entry(0),
    dm_impl(NULL)
{
}



/**
 * Constructor from a collector entry.
 *
 * Constructs a new Collector for the specified collector entry within the
 * passed database. Automatically finds and uses the collector's implementation
 * if one is available.
 *
 * @param database     Database containing the collector.
 * @param entry        Entry (id) for the collector.
 */
Collector::Collector(const SmartPtr<Database>& database, const int& entry) :
    dm_database(database),
    dm_entry(entry),
    dm_impl(NULL)
{
    // Find our unique identifier
    std::string unique_id;
    BEGIN_TRANSACTION(dm_database);
    validateEntry();
    dm_database->prepareStatement(
        "SELECT unique_id FROM Collectors WHERE id = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        unique_id = dm_database->getResultAsString(1);
    END_TRANSACTION(dm_database);

    // Attempt to instantiate an implementation for this collector
    try {
	dm_impl = collector_plugin_table.instantiate(unique_id);
    }
    catch(...) {
    }
}



/**
 * Validate our entry.
 *
 * Validates the existence and uniqueness of our entry within our database. If
 * our entry is found and is unique, this function simply returns. Otherwise
 * an exception of type Database::Corrupted is thrown.
 *
 * @note    Validation may only be performed within the context of an existing
 *          transaction. Any attempt to validate before beginning a transaction
 *          will result in an assertion failure.
 */
void Collector::validateEntry() const
{
    // Find the number of rows matching our entry
    int rows = 0;
    dm_database->prepareStatement(
	"SELECT COUNT(*) FROM Collectors WHERE id = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
	rows = dm_database->getResultAsInteger(1);
    
    // Validate
    if(rows == 0)
	throw Database::Corrupted(*dm_database,
				  "collector entry no longer exists");
    else if(rows > 1)
	throw Database::Corrupted(*dm_database,
				  "collector entry is not unique");
}
