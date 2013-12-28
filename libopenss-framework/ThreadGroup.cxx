////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
// Copyright (c) 2013 The Krell Institute. All Rights Reserved.
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
 * Definition of the ThreadGroup class.
 *
 */

#include "AddressBitmap.hxx"
#include "CollectorGroup.hxx"
#include "EntrySpy.hxx"
#include "ExtentTable.hxx"
#include "Instrumentor.hxx"
#include "Function.hxx"
#include "LinkedObject.hxx"
#include "Loop.hxx"
#include "Statement.hxx"
#include "ThreadGroup.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Obtain the subset of threads in a particular state.
 *
 * Compares the state of every thread in the group against the passed value.
 * Matching threads are added to a new thread group that is returned as the
 * result. Useful for obtaining, for example, the subset of all suspended
 * threads within an existing thread group.
 *
 * @param state    State to compare against.
 * @return         Subset of threads that were in the passed state.
 */
ThreadGroup ThreadGroup::getSubsetWithState(const Thread::State& state) const
{
    ThreadGroup subset;
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	if(i->isState(state))
	    subset.insert(subset.end(), *i);
    return subset;
}



// Used by Experiment::compressDB to prune an OpenSpeedShop database of
// any entries not found in the experiments sampled addresses.
ThreadGroup ThreadGroup::getSubsetWithLinkedObject(const LinkedObject& lo) const
{
    ThreadGroup subset;
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i) {
	int thread_lo_id = EntrySpy(i->getExecutable().second).getEntry();
	if(thread_lo_id == EntrySpy(lo).getEntry())
	    subset.insert(subset.end(), *i);
    }
    return subset;
}



/**
 * Test if any thread is in a particular state.
 *
 * Compares the state of every thread in the group against the passed value.
 * Returns a boolean value indicating if any of the threads in the group were in
 * the passed state.
 *
 * @param state    State to compare against.
 * @return         Boolean "true" if any thread in the group was in the passed
 *                 state, "false" otherwise.
 */
bool ThreadGroup::isAnyState(const Thread::State& state) const
{
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	if(i->isState(state))
	    return true;
    return false;
}



/**
 * Test if all threads are in a particular state.
 *
 * Compares the state of every thread in the group against the passed value.
 * Returns a boolean value indicating if all threads in the group were in the
 * passed state.
 *
 * @param state    State to compare against.
 * @return         Boolean "true" if all threads in the group were in the passed
 *                 state, "false" otherwise.
 */
bool ThreadGroup::areAllState(const Thread::State& state) const
{
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	if(!i->isState(state))
	    return false;
    return true;
}



/**
 * Change all threads to a particular state.
 *
 * Changes the current state of every thread in the group to the passed value.
 * Used to, for example, suspend threads that were previously running. This
 * function does not wait until the threads have actually completed the state
 * change.
 *
 * @param state    Change to this state.
 */
void ThreadGroup::changeState(const Thread::State& state)
{
    Instrumentor::changeState(*this, state);
}



/**
 * Start data collection for all threads.
 *
 * Starts collection of performance data by the specified collector for all
 * threads in the group.
 *
 * @param collector    Collector for which to start data collection.
 */
void ThreadGroup::startCollecting(const Collector& collector) const
{
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	collector.startCollecting(*i);
}



/**
 * Start data collection for all threads.
 *
 * Starts collection of performance data by the specified collectors for all
 * threads in the group.
 *
 * @param collectors    Collectors for which to start data collection.
 */
void ThreadGroup::startCollecting(const CollectorGroup& collectors) const
{
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	for(CollectorGroup::const_iterator
		j = collectors.begin(); j != collectors.end(); ++j)
	    j->startCollecting(*i);
}



/**
 * Postpone data collection for all threads.
 *
 * Postpones collection of performance data by the specified collector for all
 * threads in the group.
 *
 * @param collector    Collector for which to postpone data collection.
 */
void ThreadGroup::postponeCollecting(const Collector& collector) const
{
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	collector.postponeCollecting(*i);
}



/**
 * Postpone data collection for all threads.
 *
 * Postpones collection of performance data by the specified collectors for all
 * threads in the group.
 *
 * @param collectors    Collectors for which to postpone data collection.
 */
void ThreadGroup::postponeCollecting(const CollectorGroup& collectors) const
{
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	for(CollectorGroup::const_iterator
		j = collectors.begin(); j != collectors.end(); ++j)
	    j->postponeCollecting(*i); 
}



/**
 * Stop data collection for all threads.
 *
 * Stops collection of performance data by the specified collector for all
 * threads in the group.
 *
 * @param collector    Collector for which to stop data collection.
 */
void ThreadGroup::stopCollecting(const Collector& collector) const
{
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	collector.stopCollecting(*i);
}



/**
 * Stop data collection for all threads.
 *
 * Stops collection of performance data by the specified collectors for all
 * threads in the group.
 *
 * @param collectors    Collectors for which to stop data collection.
 */
void ThreadGroup::stopCollecting(const CollectorGroup& collectors) const
{
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	for(CollectorGroup::const_iterator
		j = collectors.begin(); j != collectors.end(); ++j)
	    j->stopCollecting(*i); 
}



/**
 * Get linked objects for all threads.
 *
 * Returns the linked objects within the threads in the group. An empty set is
 * returned if no linked objects are found.
 *
 * @pre    All threads in the thread group must be from the same experiment. An
 *         assertion failure occurs if more than one experiment is implied.
 *
 * @return    Linked objects contained within this group.
 */
std::set<LinkedObject> ThreadGroup::getLinkedObjects() const
{
    std::set<LinkedObject> linked_objects;
    
    // Handle special case of an empty thread group
    if(empty())
	return linked_objects;

    // Check preconditions
    SmartPtr<Database> database = EntrySpy(*begin()).getDatabase();
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	Assert(EntrySpy(*i).getDatabase() == database);

    // Find all of the linked objects within any thread of this group
    BEGIN_TRANSACTION(database);
    database->prepareStatement(
	"SELECT thread, "
	"       linked_object "
	"FROM AddressSpaces;"
	);
    while(database->executeStatement())
	if(find(Thread(database, database->getResultAsInteger(1))) != end())
	    linked_objects.insert(
		LinkedObject(database, database->getResultAsInteger(2))
		);
    END_TRANSACTION(database);
    
    // Return the linked objects to the caller
    return linked_objects;
}



/**
 * Get functions for all threads.
 *
 * Returns the functions within the threads in the group. An empty set is
 * returned if no functions are found.
 *
 * @pre    All threads in the thread group must be from the same experiment. An
 *         assertion failure occurs if more than one experiment is implied.
 *
 * @return    Functions contained within this group.
 */
std::set<Function> ThreadGroup::getFunctions() const
{
    std::set<Function> functions;
    
    // Handle special case of an empty thread group
    if(empty())
	return functions;

    // Check preconditions
    SmartPtr<Database> database = EntrySpy(*begin()).getDatabase();
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	Assert(EntrySpy(*i).getDatabase() == database);

    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(database);
    
    // Find all of the linked objects within any thread of this group
    std::set<int> linked_objects;
    database->prepareStatement(
	"SELECT thread, "
	"       linked_object "
	"FROM AddressSpaces;"
	);
    while(database->executeStatement())
	if(find(Thread(database, database->getResultAsInteger(1))) != end())
	    linked_objects.insert(database->getResultAsInteger(2));

    // Find all of the functions in any of these linked objects
    database->prepareStatement(
	"SELECT id, "
	"       linked_object "
	"FROM Functions;"
	);
    while(database->executeStatement())
	if(linked_objects.find(database->getResultAsInteger(2)) !=
	   linked_objects.end())
	    functions.insert(
		Function(database, database->getResultAsInteger(1))
		);
    
    // End this multi-statement transaction
    END_TRANSACTION(database);
    
    // Return the functions to the caller
    return functions;
}



/**
 * Get loops for all threads.
 *
 * Returns the loops within the threads in the group. An empty set is returned
 * if no loops are found.
 *
 * @pre    All threads in the thread group must be from the same experiment. An
 *         assertion failure occurs if more than one experiment is implied.
 *
 * @return    Loops contained within this group.
 */
std::set<Loop> ThreadGroup::getLoops() const
{
    std::set<Loop> loops;
    
    // Handle special case of an empty thread group
    if(empty())
        return loops;

    // Check preconditions
    SmartPtr<Database> database = EntrySpy(*begin()).getDatabase();
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
        Assert(EntrySpy(*i).getDatabase() == database);

    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(database);
    
    // Find all of the linked objects within any thread of this group
    std::set<int> linked_objects;
    database->prepareStatement(
        "SELECT thread, "
        "       linked_object "
        "FROM AddressSpaces;"
        );
    while(database->executeStatement())
        if(find(Thread(database, database->getResultAsInteger(1))) != end())
            linked_objects.insert(database->getResultAsInteger(2));

    // Find all of the loops in any of these linked objects
    database->prepareStatement(
        "SELECT id, "
        "       linked_object "
        "FROM Loops;"
        );
    while(database->executeStatement())
        if(linked_objects.find(database->getResultAsInteger(2)) !=
           linked_objects.end())
            loops.insert(Loop(database, database->getResultAsInteger(1)));
    
    // End this multi-statement transaction
    END_TRANSACTION(database);
    
    // Return the loops to the caller
    return loops;
}



/**
 * Get statements for all threads.
 *
 * Returns the statements within the threads in the group. An empty set is
 * returned if no statements are found.
 *
 * @pre    All threads in the thread group must be from the same experiment. An
 *         assertion failure occurs if more than one experiment is implied.
 *
 * @return    Statements contained within this group.
 */
std::set<Statement> ThreadGroup::getStatements() const
{
    std::set<Statement> statements;
    
    // Handle special case of an empty thread group
    if(empty())
	return statements;

    // Check preconditions
    SmartPtr<Database> database = EntrySpy(*begin()).getDatabase();
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	Assert(EntrySpy(*i).getDatabase() == database);

    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(database);
    
    // Find all of the linked objects within any thread of this group
    std::set<int> linked_objects;
    database->prepareStatement(
	"SELECT thread, "
	"       linked_object "
	"FROM AddressSpaces;"
	);
    while(database->executeStatement())
	if(find(Thread(database, database->getResultAsInteger(1))) != end())
	    linked_objects.insert(database->getResultAsInteger(2));

    // Find all of the statements in any of these linked objects
    database->prepareStatement(
	"SELECT id, "
	"       linked_object "
	"FROM Statements;"
	);
    while(database->executeStatement())
	if(linked_objects.find(database->getResultAsInteger(2)) !=
	   linked_objects.end())
	    statements.insert(
		Statement(database, database->getResultAsInteger(1))
		);
    
    // End this multi-statement transaction
    END_TRANSACTION(database);
    
    // Return the statements to the caller
    return statements;
}



/**
 * Get extents of linked objects for all threads.
 *
 * Returns the extents of all the specified linked objects within the threads
 * in the group. Each extent is intersected with the specified domain extent
 * before being added to the table.
 *
 * @pre    All threads in the thread group must be from the same experiment. An
 *         assertion failure occurs if more than one experiment is implied.
 *
 * @pre    All the specified linked objects must be from the same experiment as
 *         the threads in the thread group. An assertion failure occurs if more
 *         than one experiment is implied.
 *
 * @param objects    Linked objects for which to get extents.
 * @param domain     Extent defining the domain of interest.
 * @return           Table of per-thread extents for the linked objects.
 */
ExtentTable<Thread, LinkedObject>
ThreadGroup::getExtentsOf(const std::set<LinkedObject>& objects, 
			  const Extent& domain) const
{
    ExtentTable<Thread, LinkedObject> table;

    // Handle special case of an empty thread group
    if(empty())
	return table;

    // Check preconditions
    SmartPtr<Database> database = EntrySpy(*begin()).getDatabase();
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	Assert(EntrySpy(*i).getDatabase() == database);
    for(std::set<LinkedObject>::const_iterator
	    i = objects.begin(); i != objects.end(); ++i)
	Assert(EntrySpy(*i).getDatabase() == database);	
    
    // Find the extents of all linked objects within any thread of this group
    BEGIN_TRANSACTION(database);
    database->prepareStatement(
	"SELECT thread, "
	"       linked_object, "
	"       time_begin, "
	"       time_end, "
	"       addr_begin, "
	"       addr_end "
	"FROM AddressSpaces;"
	);
    while(database->executeStatement()) {
	
	Thread thread(database, database->getResultAsInteger(1));
	LinkedObject linked_object(database, database->getResultAsInteger(2));

	// Is this a thread and linked object of interest?
	if((find(thread) != end()) && 
	   (objects.find(linked_object) != objects.end())) {
	    
	    // Intersect this linked object's extent with the domain of interest
	    Extent constrained = 
		Extent(TimeInterval(database->getResultAsTime(3),
				    database->getResultAsTime(4)),
		       AddressRange(database->getResultAsAddress(5),
				    database->getResultAsAddress(6))) & domain;
	    
	    // Add the constrained extent (if it isn't empty) to the table
	    if(!constrained.isEmpty())
		table.addExtent(thread, linked_object, constrained);
	    
	}

    }
    END_TRANSACTION(database);
    
    // Return the table to the caller
    return table;
}



/**
 * Get extents of functions for all threads.
 *
 * Returns the extents of all the specified functions within the threads in
 * the group. Each extent is intersected with the specified domain extent before
 * being added to the table.
 *
 * @pre    All threads in the thread group must be from the same experiment. An
 *         assertion failure occurs if more than one experiment is implied.
 *
 * @pre    All the specified functions must be from the same experiment as the
 *         threads in the thread group. An assertion failure occurs if more than
 *         one experiment is implied.
 *
 * @param objects    Functions for which to get extents.
 * @param domain     Extent defining the domain of interest.
 * @return           Table of per-thread extents for the functions.
 */
ExtentTable<Thread, Function>
ThreadGroup::getExtentsOf(const std::set<Function>& objects, 
			  const Extent& domain) const
{
    ExtentTable<Thread, Function> table;

    // Handle special case of an empty thread group
    if(empty())
	return table;

    // Check preconditions
    SmartPtr<Database> database = EntrySpy(*begin()).getDatabase();
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	Assert(EntrySpy(*i).getDatabase() == database);
    for(std::set<Function>::const_iterator
	    i = objects.begin(); i != objects.end(); ++i)
	Assert(EntrySpy(*i).getDatabase() == database);	
    
    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(database);

    // Find the extents of all linked objects within any thread of this group
    std::multimap<std::pair<int, int>, Extent> linked_objects;
    database->prepareStatement(
	"SELECT thread, "
	"       linked_object, "
	"       time_begin, "
	"       time_end, "
	"       addr_begin, "
	"       addr_end "
	"FROM AddressSpaces;"
	);
    while(database->executeStatement())
	if(find(Thread(database, database->getResultAsInteger(1))) != end())
	    linked_objects.insert(
		std::make_pair(
		    std::make_pair(
			database->getResultAsInteger(1),
			database->getResultAsInteger(2)),
		    Extent(
			TimeInterval(database->getResultAsTime(3),
				     database->getResultAsTime(4)),
			AddressRange(database->getResultAsAddress(5),
				     database->getResultAsAddress(6))
			)
		    )
		);
    
    // Iterate over each of the specified functions
    for(std::set<Function>::const_iterator
	    i = objects.begin(); i != objects.end(); ++i) {

	// Find this function's linked object and address ranges + bitmaps
	EntrySpy(*i).validate();
	database->prepareStatement(
	    "SELECT Functions.linked_object, "
	    "       FunctionRanges.addr_begin, "
	    "       FunctionRanges.addr_end, "
	    "       FunctionRanges.valid_bitmap "
	    "FROM FunctionRanges "
	    "  JOIN Functions "
	    "ON FunctionRanges.function = Functions.id "
	    "WHERE Functions.id = ?;"
	    );
	database->bindArgument(1, EntrySpy(*i).getEntry());
	while(database->executeStatement()) {

	    std::set<AddressRange> ranges = 
		AddressBitmap(AddressRange(database->getResultAsAddress(2),
					   database->getResultAsAddress(3)),
			      database->getResultAsBlob(4)).
		getContiguousRanges(true);

	    // Iterate over each thread of this group
	    for(ThreadGroup::const_iterator j = begin(); j != end(); ++j) {

		// Form a search key from the thread and the linked object
		std::pair<int, int> key = 
		    std::make_pair(EntrySpy(*j).getEntry(), 
				   database->getResultAsInteger(1));

		// Iterate over all occurences of this key
		for(std::multimap<std::pair<int, int>, Extent>::const_iterator
			k = linked_objects.lower_bound(key);
		    k != linked_objects.upper_bound(key);
		    ++k) {

		    // Iterate over the addresss ranges for this function
		    for(std::set<AddressRange>::const_iterator
			    l = ranges.begin(); l != ranges.end(); ++l) {

			// Intersect this extent with domain of interest
			Extent constrained =
			    Extent(k->second.getTimeInterval(),
				   AddressRange(
				       k->second.getAddressRange().getBegin() +
				       l->getBegin(),
				       k->second.getAddressRange().getBegin() +
				       l->getEnd()
				       )
				) & domain;
			
			// Add constrained extent (if non-empty) to table
			if(!constrained.isEmpty())
			    table.addExtent(*j, *i, constrained);
			
		    }
		    
		}
		
	    }
	    
	}

    }

    // End this multi-statement transaction
    END_TRANSACTION(database);

    // Return the table to the caller
    return table;
}



/**
 * Get extents of loops for all threads.
 *
 * Returns the extents of all the specified loops within the threads in the
 * group. Each extent is intersected with the specified domain extent before
 * being added to the table.
 *
 * @pre    All threads in the thread group must be from the same experiment.
 *         An assertion failure occurs if more than one experiment is implied.
 *
 * @pre    All the specified loops must be from the same experiment as the
 *         threads in the thread group. An assertion failure occurs if more
 *         than one experiment is implied.
 *
 * @param objects    Loops for which to get extents.
 * @param domain     Extent defining the domain of interest.
 * @return           Table of per-thread extents for the loops.
 */
ExtentTable<Thread, Loop>
ThreadGroup::getExtentsOf(const std::set<Loop>& objects, 
                          const Extent& domain) const
{
    ExtentTable<Thread, Loop> table;

    // Handle special case of an empty thread group
    if(empty())
        return table;

    // Check preconditions
    SmartPtr<Database> database = EntrySpy(*begin()).getDatabase();
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
        Assert(EntrySpy(*i).getDatabase() == database);
    for(std::set<Loop>::const_iterator
            i = objects.begin(); i != objects.end(); ++i)
        Assert(EntrySpy(*i).getDatabase() == database);	
    
    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(database);
    
    // Find the extents of all linked objects within any thread of this group
    std::multimap<std::pair<int, int>, Extent> linked_objects;
    database->prepareStatement(
        "SELECT thread, "
        "       linked_object, "
        "       time_begin, "
        "       time_end, "
        "       addr_begin, "
        "       addr_end "
        "FROM AddressSpaces;"
        );
    while(database->executeStatement())
        if(find(Thread(database, database->getResultAsInteger(1))) != end())
            linked_objects.insert(
                std::make_pair(
                    std::make_pair(
                        database->getResultAsInteger(1),
                        database->getResultAsInteger(2)),
                    Extent(
                        TimeInterval(database->getResultAsTime(3),
                                     database->getResultAsTime(4)),
                        AddressRange(database->getResultAsAddress(5),
                                     database->getResultAsAddress(6))
                        )
                    )
                );
    
    // Iterate over each of the specified loops
    for(std::set<Loop>::const_iterator
            i = objects.begin(); i != objects.end(); ++i) {

        // Find this loop's linked object and address ranges + bitmaps
        EntrySpy(*i).validate();
        database->prepareStatement(
            "SELECT Loops.linked_object, "
            "       LoopRanges.addr_begin, "
            "       LoopRanges.addr_end, "
            "       LoopRanges.valid_bitmap "
            "FROM LoopRanges "
            "  JOIN Loops "
            "ON LoopRanges.loop = Loops.id "
            "WHERE Loops.id = ?;"
            );
        database->bindArgument(1, EntrySpy(*i).getEntry());
        while(database->executeStatement()) {
            
            std::set<AddressRange> ranges = 
                AddressBitmap(AddressRange(database->getResultAsAddress(2),
                                           database->getResultAsAddress(3)),
                              database->getResultAsBlob(4)).
                getContiguousRanges(true);
            
            // Iterate over each thread of this group
            for(ThreadGroup::const_iterator j = begin(); j != end(); ++j) {
                
                // Form a search key from the thread and the linked object
                std::pair<int, int> key = 
                    std::make_pair(EntrySpy(*j).getEntry(), 
                                   database->getResultAsInteger(1));
                
                // Iterate over all occurences of this key
                for(std::multimap<std::pair<int, int>, Extent>::const_iterator
                        k = linked_objects.lower_bound(key);
                    k != linked_objects.upper_bound(key);
                    ++k) {
                    
                    // Iterate over the addresss ranges for this loop
                    for(std::set<AddressRange>::const_iterator
                            l = ranges.begin(); l != ranges.end(); ++l) {
                        
                        // Intersect this extent with domain of interest
                        Extent constrained =
                            Extent(k->second.getTimeInterval(),
                                   AddressRange(
                                       k->second.getAddressRange().getBegin() +
                                       l->getBegin(),
                                       k->second.getAddressRange().getBegin() +
                                       l->getEnd()
                                       )
                                ) & domain;
                        
                        // Add constrained extent (if non-empty) to table
                        if(!constrained.isEmpty())
                            table.addExtent(*j, *i, constrained);
                        
                    }
                    
                }
                
            }
            
        }
        
    }
    
    // End this multi-statement transaction
    END_TRANSACTION(database);
    
    // Return the table to the caller
    return table;
}



/**
 * Get extents of statements for all threads.
 *
 * Returns the extents of all the specified statements within the threads in
 * the group. Each extent is intersected with the specified domain extent before
 * being added to the table.
 *
 * @pre    All threads in the thread group must be from the same experiment. An
 *         assertion failure occurs if more than one experiment is implied.
 *
 * @pre    All the specified statements must be from the same experiment as the
 *         threads in the thread group. An assertion failure occurs if more than
 *         one experiment is implied.
 *
 * @param objects    Statements for which to get extents.
 * @param domain     Extent defining the domain of interest.
 * @return           Table of per-thread extents for the statements.
 */
ExtentTable<Thread, Statement>
ThreadGroup::getExtentsOf(const std::set<Statement>& objects, 
			  const Extent& domain) const
{
    ExtentTable<Thread, Statement> table;

    // Handle special case of an empty thread group
    if(empty())
	return table;

    // Check preconditions
    SmartPtr<Database> database = EntrySpy(*begin()).getDatabase();
    for(ThreadGroup::const_iterator i = begin(); i != end(); ++i)
	Assert(EntrySpy(*i).getDatabase() == database);
    for(std::set<Statement>::const_iterator
	    i = objects.begin(); i != objects.end(); ++i)
	Assert(EntrySpy(*i).getDatabase() == database);	
    
    // Begin a multi-statement transaction
    BEGIN_TRANSACTION(database);

    // Find the extents of all linked objects within any thread of this group
    std::multimap<std::pair<int, int>, Extent> linked_objects;
    database->prepareStatement(
	"SELECT thread, "
	"       linked_object, "
	"       time_begin, "
	"       time_end, "
	"       addr_begin, "
	"       addr_end "
	"FROM AddressSpaces;"
	);
    while(database->executeStatement())
	if(find(Thread(database, database->getResultAsInteger(1))) != end())
	    linked_objects.insert(
		std::make_pair(
		    std::make_pair(
			database->getResultAsInteger(1),
			database->getResultAsInteger(2)),
		    Extent(
			TimeInterval(database->getResultAsTime(3),
				     database->getResultAsTime(4)),
			AddressRange(database->getResultAsAddress(5),
				     database->getResultAsAddress(6))
			)
		    )
		);
    
    // Iterate over each of the specified statements
    for(std::set<Statement>::const_iterator
	    i = objects.begin(); i != objects.end(); ++i) {

	// Find this statement's linked object and address ranges + bitmaps
	EntrySpy(*i).validate();
	database->prepareStatement(
	    "SELECT Statements.linked_object, "
	    "       StatementRanges.addr_begin, "
	    "       StatementRanges.addr_end, "
	    "       StatementRanges.valid_bitmap "
	    "FROM StatementRanges "
	    "  JOIN Statements "
	    "ON StatementRanges.statement = Statements.id "
	    "WHERE Statements.id = ?;"
	    );
	database->bindArgument(1, EntrySpy(*i).getEntry());
	while(database->executeStatement()) {
	    
	    std::set<AddressRange> ranges = 
		AddressBitmap(AddressRange(database->getResultAsAddress(2),
					   database->getResultAsAddress(3)),
			      database->getResultAsBlob(4)).
		getContiguousRanges(true);

	    // Iterate over each thread of this group
	    for(ThreadGroup::const_iterator j = begin(); j != end(); ++j) {

		// Form a search key from the thread and the linked object
		std::pair<int, int> key = 
		    std::make_pair(EntrySpy(*j).getEntry(), 
				   database->getResultAsInteger(1));

		// Iterate over all occurences of this key
		for(std::multimap<std::pair<int, int>, Extent>::const_iterator
			k = linked_objects.lower_bound(key);
		    k != linked_objects.upper_bound(key);
		    ++k) {

		    // Iterate over the addresss ranges for this statement
		    for(std::set<AddressRange>::const_iterator
			    l = ranges.begin(); l != ranges.end(); ++l) {

			// Intersect this extent with domain of interest
			Extent constrained =
			    Extent(k->second.getTimeInterval(),
				   AddressRange(
				       k->second.getAddressRange().getBegin() +
				       l->getBegin(),
				       k->second.getAddressRange().getBegin() +
				       l->getEnd()
				       )
				) & domain;
			
			// Add constrained extent (if non-empty) to table
			if(!constrained.isEmpty())
			    table.addExtent(*j, *i, constrained);
			
		    }
		    
		}
		
	    }
	    
	}
	
    }
    
    // End this multi-statement transaction
    END_TRANSACTION(database);

    // Return the table to the caller
    return table;
}
