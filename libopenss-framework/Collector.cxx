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
 * Definition of the Collector class.
 *
 */

#include "Assert.hxx"
#include "Collector.hxx"
#include "Database.hxx"
#include "Guard.hxx"
#include "Lockable.hxx"
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
	 * different locations: the compile-time plugin path as passed via a
	 * macro from the build system; the install plugin path defined by the
	 * environment variable OPENSS_INSTALL_DIR; and user-specified plugin
	 * path(s) defined by the environment variable OPENSS_PLUGIN_PATH.
	 */
	CollectorPluginTable::CollectorPluginTable()
	{
	    // Initialize libltdl
	    Assert(lt_dlinit() == 0);
	    
	    // Start with an empty user-defined search path
	    Assert(lt_dlsetsearchpath("") == 0);
	    
	    // Add the compile-time plugin path
	    Assert(lt_dladdsearchdir(PLUGIN_PATH) == 0);
	    
	    // Add the install plugin path
	    if(getenv("OPENSS_INSTALL_DIR") != NULL) {
		Path install_path = Path(getenv("OPENSS_INSTALL_DIR")) +
		    Path("/lib/openspeedshop");
		Assert(lt_dladdsearchdir(install_path.c_str()) == 0);
	    }
	    
	    // Add the user-specified plugin path
	    if(getenv("OPENSS_PLUGIN_PATH") != NULL) {
		Path user_specified_path = Path(getenv("OPENSS_PLUGIN_PATH"));
		Assert(lt_dladdsearchdir(user_specified_path.c_str()) == 0);
	    }

	    // Now search for collector plugins in all these paths
	    lt_dlforeachfile(lt_dlgetsearchpath(), ::foreachCallback, this);
	}


	
	/**
	 * Destructor.
	 *
	 * Destroys the collector plugin table.
	 *
	 * @pre    All collector implementation instances associated with
	 *         plugins in this table must be destroyed prior to the
	 *         destruction of the table itself. An assertion failure
	 *         occurs if any such instances remain.
	 */
	CollectorPluginTable::~CollectorPluginTable()
	{
	    // Check preconditions    
	    for(std::map<std::string, Entry>::const_iterator
		    i = dm_unique_id_to_entry.begin();
		i != dm_unique_id_to_entry.end();
		++i) {
		Assert(i->second.instances == 0);
		Assert(i->second.handle == NULL);
	    }
	    
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
	 * is automatically loaded into memory first. A null value is returned
	 * if the collector implementation cannot be instantiated for any reason
	 * (plugin wasn't found, plugin was moved, plugin was changed, etc.)
	 *
	 * @param unique_id    Unique identifier of the collector implementation
	 *                     to be instantiated.
	 * @return             New instance of this collector implementation or
	 *                     null if the instantiation failed for any reason.
	 */
	CollectorImpl* instantiate(const std::string& unique_id)
	{
	    CollectorImpl* instance;
    
	    // Find the entry for this unique identifier's plugin
	    std::map<std::string, Entry>::iterator
		i = dm_unique_id_to_entry.find(unique_id);
	    if(i == dm_unique_id_to_entry.end())
		return NULL;
	    
	    // Critical section touching the collector plugin's entry
	    {
		Guard guard_myself(this);
		
		// Is this the first instance of this collector implementation?
		if(i->second.instances == 0) {
		    
		    // Load the plugin into memory
		    i->second.handle = lt_dlopenext(i->second.path.c_str());
		    if(i->second.handle == NULL)
			return NULL;
		    
		}
		
		// Find the factory method in the plugin
		lt_dlhandle handle = 
		    reinterpret_cast<lt_dlhandle>(i->second.handle);
		Assert(handle != NULL);
		CollectorImpl* (*factory)() = (CollectorImpl* (*)())
		    lt_dlsym(handle, "CollectorFactory");
		if(factory == NULL)
		    return NULL;
		
		// Create an instance of this collector
		instance = (*factory)();
		if(i->second.metadata != *instance)
		    return NULL;
		
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
	    // Check assertions
	    Assert(impl != NULL);

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
	    // Create an entry for this possible collector plugin
	    Entry entry;
	    entry.path = filename;
	    
	    // Can we open this file as a libltdl module?
	    lt_dlhandle handle = lt_dlopenext(entry.path.c_str());
	    if(handle == NULL)
		return;
    
	    // Is there a collector factory method in this module?
	    CollectorImpl* (*factory)() = (CollectorImpl* (*)())
		lt_dlsym(handle, "CollectorFactory");
	    if(factory == NULL) {
		Assert(lt_dlclose(handle) == 0);
		return;
	    }

	    // Get the metadata for this collector implementation
	    CollectorImpl* instance = (*factory)();
	    entry.metadata = *instance;
	    delete instance;
	    
	    // Close the module handle
	    Assert(lt_dlclose(handle) == 0);
	    
	    // Have we already used this unique identifier?
	    if(dm_unique_id_to_entry.find(entry.metadata.getUniqueId()) != 
	       dm_unique_id_to_entry.end())
		return;
	    
	    // Add this entry to the table of collector plugins
	    dm_unique_id_to_entry.insert(
		std::make_pair(entry.metadata.getUniqueId(), entry)
		);
	}
	


    private:

	/**
	 * Collector plugin table entry.
	 *
	 * Structure for an entry in the collector plugin table describing
	 * a single collector plugin. Contains the metadata, path, instance
	 * count, and handle for the plugin.
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

	    /** Default constructor. */
	    Entry() :
		metadata(),
		path(""),
		instances(0),
		handle(NULL)
	    {
	    }		
	    
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
    Entry(other.dm_database, other.dm_entry, 0),
    dm_impl(NULL)
{
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
    // Only do the actual assignment if the collectors differ
    if(*this != other) {
	
	// Destroy our current implementation (if any)
	if(dm_impl != NULL)
	    collector_plugin_table.destroy(dm_impl);
	
	// Replace our collector with the new collector
	Entry::operator=(other);
	dm_impl = NULL;
	
    }
    
    // Return ourselves to the caller
    return *this;
}



/**
 * Get our metadata.
 *
 * Returns the metadata of this collector.
 *
 * @note    Information will be limited to the collector's unique identiifer
 *          when the collector's implementation cannot be instantiated.
 *
 * @return    Metadata for this collector.
 */
Metadata Collector::getMetadata() const
{
    // Defer to our implementation (if any)    
    if(dm_impl == NULL)
	instantiateImpl();
    if(dm_impl != NULL)
        return *dm_impl;
    
    // Find our unique identifier
    std::string unique_id;
    BEGIN_TRANSACTION(dm_database);
    validate("Collectors");
    dm_database->prepareStatement(
        "SELECT unique_id FROM Collectors WHERE id = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        unique_id = dm_database->getResultAsString(1);
    END_TRANSACTION(dm_database);
    
    // Return the partial metadata to the caller
    return Metadata(unique_id, "", "", typeid(Collector));
}



/**
 * Get our parameters.
 *
 * Returns the metadata for all parameters of this collector. An empty set is
 * returned if this collector has no parameters.
 *
 * @pre    Can only be performed on collectors for which an implementation can
 *         be instantiated. A CollectorUnavailable exception is thrown if the
 *         collector's implementation cannot be instantiated.
 *
 * @return    Metadata for all parameters of this collector.
 */
std::set<Metadata> Collector::getParameters() const
{
    // Check preconditions
    if(dm_impl == NULL) {
	instantiateImpl();
	if(dm_impl == NULL)
	    throw Exception(Exception::CollectorUnavailable,
			    getMetadata().getUniqueId());
    } 
    
    // Defer to our implementation
    return dm_impl->getParameters();    
}



/**
 * Get our metrics.
 *
 * Returns the metadata for all metrics of this collector. An empty set is
 * returned if this collector has no metrics (unlikely).
 *
 * @pre    Can only be performed on collectors for which an implementation can
 *         be instantiated. A CollectorUnavailable exception is thrown if the
 *         collector's implementation cannot be instantiated.
 *
 * @return    Metadata for all metrics of this collector.
 */
std::set<Metadata> Collector::getMetrics() const
{
    // Check preconditions
    if(dm_impl == NULL) {
	instantiateImpl();
	if(dm_impl == NULL)
	    throw Exception(Exception::CollectorUnavailable,
			    getMetadata().getUniqueId());
    } 

    // Defer to our implementation
    return dm_impl->getMetrics();
}



/**
 * Get our threads.
 *
 * Returns all the threads for which this collector is currently collecting
 * performance data. An empty thread group is returned if this collector isn't
 * collecting performance data for any threads.
 *
 * @return    Threads for which this collector is collecting performance data.
 */
ThreadGroup Collector::getThreads() const
{
    ThreadGroup threads;

    // Find the threads for which we are collecting
    BEGIN_TRANSACTION(dm_database);
    validate("Collectors");
    dm_database->prepareStatement(
        "SELECT thread FROM Collecting WHERE collector = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        threads.insert(Thread(dm_database, dm_database->getResultAsInteger(1)));
    END_TRANSACTION(dm_database);
    
    // Return the threads to the caller
    return threads;
}



/**
 * Start data collection.
 *
 * Starts collection of performance data by this collector for the specified
 * thread. Data collection can be stopped temporarily or permanently by calling
 * stopCollecting() for the specified thread. All data that is collected is
 * available via the collector's metrics.
 *
 * @pre    The thread must be in the same experiment as the collector. An
 *         assertion failure occurs if the thread is in a different experiment
 *         than the collector.
 *
 * @pre    Can only be performed on collectors for which an implementation can
 *         be instantiated. A CollectorUnavailable exception is thrown if the
 *         collector's implementation cannot be instantiated.
 *
 * @note    Any attempt to start collection on a thread for which this collector
 *          is already collecting data will be silently ignored.
 *
 * @param thread    Thread for which to start collecting performance data.
 */
void Collector::startCollecting(const Thread& thread) const
{
    // Check preconditions
    Assert(thread.dm_database == dm_database);
    if(dm_impl == NULL) {
	instantiateImpl();
	if(dm_impl == NULL)
	    throw Exception(Exception::CollectorUnavailable,
			    getMetadata().getUniqueId());
    }
    
    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);
    validate("Collectors");
    thread.validate("Threads");
    
    // Are we collecting for this thread?
    bool is_collecting = false;
    dm_database->prepareStatement(
	"SELECT COUNT(*) FROM Collecting WHERE collector = ? AND thread = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, thread.dm_entry);
    while(dm_database->executeStatement())
	is_collecting = dm_database->getResultAsInteger(1) != 0;

    // Only start collection if we aren't already collecting
    if(!is_collecting) {
	
	// Defer to our implementation
	dm_impl->startCollecting(*this, thread);

	// Note in the database that we are now collecting for this thread
	dm_database->prepareStatement(
	    "INSERT INTO Collecting (collector, thread) VALUES (?, ?);"
	    );
	dm_database->bindArgument(1, dm_entry);
	dm_database->bindArgument(2, thread.dm_entry);
	while(dm_database->executeStatement());
	
    }
    
    // End this multi-statement transaction
    END_TRANSACTION(dm_database);
}



/**
 * Stop data collection.
 *
 * Stops collection of performance data by this collector for the specified
 * thread. Data collection can be resumed by calling startCollecting() for this
 * thread again. All data that was collected is available via the collector's
 * metrics.
 *
 * @pre    The thread must be in the same experiment as the collector. An
 *         assertion failure occurs if the thread is in a different experiment
 *         than the collector.
 *
 * @pre    Can only be performed on collectors for which an implementation can
 *         be instantiated. A CollectorUnavailable exception is thrown if the
 *         collector's implementation cannot be instantiated.
 *
 * @note    Any attempt to stop collection on a thread for which this collector
 *          is not collecting data will be silently ignored.
 *
 * @param thread    Thread for which to stop collecting performance data.
 */
void Collector::stopCollecting(const Thread& thread) const
{
    // Check preconditions
    Assert(thread.dm_database == dm_database);
    if(dm_impl == NULL) {
	instantiateImpl();
	if(dm_impl == NULL)
	    throw Exception(Exception::CollectorUnavailable,
			    getMetadata().getUniqueId());
    }

    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(dm_database);
    validate("Collectors");
    thread.validate("Threads");

    // Are we collecting for this thread?
    bool is_collecting = false;
    dm_database->prepareStatement(
	"SELECT COUNT(*) FROM Collecting WHERE collector = ? AND thread = ?;"
	);
    dm_database->bindArgument(1, dm_entry);
    dm_database->bindArgument(2, thread.dm_entry);
    while(dm_database->executeStatement())
	is_collecting = dm_database->getResultAsInteger(1) != 0;

    // Only stop collection if we are already collecting
    if(is_collecting) {

	// Defer to our implementation
	dm_impl->stopCollecting(*this, thread);

	// Note in the database we are no longer collecting for this thread
	dm_database->prepareStatement(
	    "DELETE FROM Collecting WHERE collector = ? AND thread = ?;"
	    );
	dm_database->bindArgument(1, dm_entry);
	dm_database->bindArgument(2, thread.dm_entry);
	while(dm_database->executeStatement());
	
    }
    
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
    Entry(),
    dm_impl(NULL)
{
}



/**
 * Constructor from a collector entry.
 *
 * Constructs a new Collector for the specified collector entry.
 *
 * @param database    Database containing this collector.
 * @param entry       Identifier for this collector.
 */
Collector::Collector(const SmartPtr<Database>& database, const int& entry) :
    Entry(database, entry, 0),
    dm_impl(NULL)
{
}



/**
 * Instantiate our implementation.
 *
 * Instantiates an implementation for this collector. If the instantiation fails
 * for any reason, the collector's implementation pointer will still be null.
 */
void Collector::instantiateImpl() const
{
    // Check assertions
    Assert(dm_impl == NULL);
    
    // Find our unique identifier
    std::string unique_id;
    BEGIN_TRANSACTION(dm_database);
    validate("Collectors");
    dm_database->prepareStatement(
        "SELECT unique_id FROM Collectors WHERE id = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        unique_id = dm_database->getResultAsString(1);
    END_TRANSACTION(dm_database);
    
    // Attempt to instantiate an implementation for this collector
    dm_impl = collector_plugin_table.instantiate(unique_id);  
}



/**
 * Get our parameter data.
 *
 * Return the parameter data of this collector.
 *
 * @return    Parameter data for this collector.
 */
Blob Collector::getParameterData() const
{
    Blob data;

    // Find our parameter data
    BEGIN_TRANSACTION(dm_database);
    validate("Collectors");
    dm_database->prepareStatement(
        "SELECT parameter_data FROM Collectors WHERE id = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        data = dm_database->getResultAsBlob(1);
    END_TRANSACTION(dm_database);

    // Return the parameter data to the caller
    return data;
}



/**
 * Set our parameter data.
 *
 * Sets the paramter data of this collector.
 *
 * @param data    Parameter data to be set.
 */
void Collector::setParameterData(const Blob& data) const
{
    // Update our parameter data
    BEGIN_TRANSACTION(dm_database);
    validate("Collectors");
    dm_database->prepareStatement(
	"UPDATE Collectors SET parameter_data = ? WHERE id = ?;"
	);
    dm_database->bindArgument(1, data);
    dm_database->bindArgument(2, dm_entry);
    while(dm_database->executeStatement());
    END_TRANSACTION(dm_database);
}
