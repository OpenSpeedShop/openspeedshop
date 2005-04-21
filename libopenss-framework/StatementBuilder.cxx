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
 * Definition of the StatementBuilder class.
 *
 */

#include "Address.hxx"
#include "AddressBitmap.hxx"
#include "AddressRange.hxx"
#include "Database.hxx"
#include "Path.hxx"
#include "StatementBuilder.hxx"

#include <deque>

using namespace OpenSpeedShop::Framework;



/**
 * Add a statement entry.
 *
 * Adds the specified statement and associated address to the list of statements
 * for this object.
 *
 * @param path      Full path name of this statement's source file.
 * @param line      Line number of this statement.
 * @param column    Column number of this statement.	    
 * @param addr      Address associated with this statement.
 */
void StatementBuilder::addEntry(const Path& path,
				const int& line,
				const int& column,
				const Address& addr)
{
    // Find or create the entry for this statement
    std::map<Entry, std::set<Address> >::iterator
	i = dm_entry_to_addresses.insert(
	    std::make_pair(Entry(path, line, column),
			   std::set<Address>())
	    ).first;

    // Add this address to the statement's set if not already there
    i->second.insert(addr);
}



/**
 * Process and store statements.
 *
 * Processes the statments previously added to this object and stores them in
 * the specified database for the specified linked object.
 *
 * @param database         Database to contain the stored statements.
 * @param linked_object    Entry identifier for the linked object. 
 */
void StatementBuilder::processAndStore(SmartPtr<Database>& database, 
				       const int& linked_object)
{
    // Begin a transaction on this database
    BEGIN_TRANSACTION(database); 

    // Iterate over each statement entry
    for(std::map<Entry, std::set<Address> >::const_iterator
	    i = dm_entry_to_addresses.begin();
	i != dm_entry_to_addresses.end();
	++i) {

	// Is there an existing file in the database?
	int file = -1;
	database->prepareStatement("SELECT id FROM Files WHERE path = ?;");
	database->bindArgument(1, i->first.dm_path);
	while(database->executeStatement())
	    file = database->getResultAsInteger(1);	    
	
	// Create the file entry if it wasn't present in the database
	if(file == -1) {
	    database->prepareStatement("INSERT INTO Files (path) VALUES (?);");
	    database->bindArgument(1, i->first.dm_path);
	    while(database->executeStatement());
	    file = database->getLastInsertedUID();
	}
	
	// Create the statement entry
	database->prepareStatement(
	    "INSERT INTO Statements "
	    "  (linked_object, file, line, \"column\") "
	    "VALUES (?, ?, ?, ?);"
	    );
	database->bindArgument(1, linked_object);
	database->bindArgument(2, file);
	database->bindArgument(3, i->first.dm_line);
	database->bindArgument(4, i->first.dm_column);
	while(database->executeStatement());	
	int statement = database->getLastInsertedUID();

	// Partition this statement's address set
	std::vector<std::set<Address> > address_sets =
	    partitionAddressSet(i->second);
	
	// Iterate over each partitioned address set
	for(std::vector<std::set<Address> >::const_iterator
		j = address_sets.begin(); j != address_sets.end(); ++j) {

	    // Create and populate an address bitmap for this address set
	    AddressBitmap valid_bitmap(AddressRange(*(j->begin()),
						    *(j->rbegin()) + 1));
	    for(std::set<Address>::const_iterator
		    k = j->begin(); k != j->end(); ++k)
		valid_bitmap.setValue(*k, true);
	    
	    // Create the StatementRanges entry
	    database->prepareStatement(
		"INSERT INTO StatementRanges "
		"  (statement, addr_begin, addr_end, valid_bitmap) "
		"VALUES (?, ?, ?, ?)"
		);
	    database->bindArgument(1, statement);
	    database->bindArgument(2, valid_bitmap.getRange().getBegin());
	    database->bindArgument(3, valid_bitmap.getRange().getEnd());
	    database->bindArgument(4, valid_bitmap.getBlob());
	    while(database->executeStatement());
	    
	}
	
    }
    
    // End the transaction on this database
    END_TRANSACTION(database);    
}



/**
 * Partition an address set.
 *
 * Partitions an address set into one or more subsets. As a comprimise between
 * query speed and database size, the addresses associated with a statement are 
 * stored as an address range and a bitmap - one bit per address in the range -
 * that describes which addresses within the range are associated with the 
 * statement. In the common case where a statement's addresses exhibit a high
 * degree of spatial locality, storing one address range and bitmap is very
 * effective. But there are cases, such as inlined functions, where the degree
 * of spatial locality is minimal. Under such circumstances, the bitmap can
 * grow very large and it is more space efficient to partition the bitmap into
 * subsets that themselves exhibit spatial locality. This function iteratively
 * subdivides the address sets until each subset exhibits a "sufficient" amount
 * of spatial locality.
 *
 * @note    The criteria for subdividing an address set is as follows. The
 *          widest gap (spacing) between two adjacent addresses is found. If
 *          the number of bits required to represent this gap in the bitmap
 *          is greater than the number of bits required to store the initial
 *          header of a StatementRanges table row, then the set is partitioned
 *          at this, widest, gap.
 *
 * @param address_set    Set of addresses to be partitioned.
 * @return               Partitioned sets of addresses.
 */
std::vector<std::set<Address> >
StatementBuilder::partitionAddressSet(const std::set<Address>& address_set)
{
    std::vector<std::set<Address> > result;
    
    // Set the partitioning criteria
    static const Address::difference_type PartitioningCriteria =
	8 * (sizeof(uint32_t) + 2 * sizeof(uint64_t));
    
    // Initialize a queue with the initial, input, set of addresses
    std::deque<std::set<Address> > queue(1, address_set);
    
    // Iterate until the queue is empty
    while(!queue.empty()) {	
	std::set<Address> i = queue.front();
	queue.pop_front();
	
	// Handle special case for empty sets (ignore them)
	if(i.size() == 0)
	    continue;
	
	// Handle special case for single-element sets
	if(i.size() == 1) {
	    result.push_back(i);
	    continue;
	}
	
	// Pair specifying the widest gap in this address set
	std::pair<std::set<Address>::const_iterator, Address::difference_type>
	    widest_gap = std::make_pair(i.begin(), 0);

	// Iterate over each adjacent pair of addresses in this set
	for(std::set<Address>::const_iterator
		prev = i.begin(), current = ++i.begin();
	    current != i.end();
	    ++prev, ++current) {

	    // Calculate the gap between this adjacent address pair
	    Address::difference_type gap = 
		AddressRange(*prev, *current).getWidth() - 1;
	    
	    // Is this gap the widest so far?
	    if(gap > widest_gap.second) {
		widest_gap.first = current;
		widest_gap.second = gap;
	    }
	    
	}
	
	// Is the widest gap greater than our partitioning criteria?
	if(widest_gap.second > PartitioningCriteria) {

	    // Partition the set at this gap
	    queue.push_back(std::set<Address>(i.begin(), widest_gap.first));
	    queue.push_back(std::set<Address>(widest_gap.first, i.end()));
	    
	}
	
	// Otherwise keep this set unpartitioned
	else
	    result.push_back(i);
	
    }

    // Return the results to the caller
    return result;
}
