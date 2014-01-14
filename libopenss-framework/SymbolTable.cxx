////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2008 William Hachfeld. All Rights Reserved.
// Copyright (c) 2012,2014 The Krell Institue. All Rights Reserved.
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
 * Definition of the SymbolTable class.
 *
 */

#include "Address.hxx"
#include "AddressBitmap.hxx"
#include "Blob.hxx"
#include "Database.hxx"
#include "EntrySpy.hxx"
#include "LinkedObject.hxx"
#include "Path.hxx"
#include "SymbolTable.hxx"

#include <deque>

using namespace OpenSpeedShop::Framework;



/**
 * Constructor from address range.
 *
 * Constructs a new, empty, SymbolTable that occupies the specified address
 * range. The address range of the symbol table is used to check the validity
 * of functions, etc. that are later added.
 *
 * @param range    Address range occupied by this symbol table.
 */
SymbolTable::SymbolTable(const AddressRange& range) :
    dm_range(range),
    dm_functions(),
    dm_loops(),
    dm_statements()
{
}



/**
 * Add a function.
 *
 * Adds the specified function with its associated address range to this symbol
 * table. Functions are discarded when they have an invalid address range, are
 * not contained entirely within this symbol table, or overlap previously added
 * functions.
 *
 * @note    In theory a symbol table will never contain invalid address ranges,
 *          overlapping address ranges, or address ranges outside the address
 *          range of the symbol table itself. There are various cases, however,
 *          where bogus symbol information results in this happening. This
 *          function contains sanity checks that help keep the experiment
 *          database, if not 100% correct, at least useable under such 
 *          circumstances.
 *
 * @param begin    Beginning address associated with this function.
 * @param end      Ending address associated with this function.
 * @param name     Name of this function.
 */
void SymbolTable::addFunction(const Address& begin, 
			      const Address& end,
			      const std::string& name)
{
    // Discard functions where (end <= begin)
    if(end <= begin)
	return;
    
    // Construct the address range [begin, end)
    AddressRange range(begin, end);

    // Discard functions not contained entirely within this symbol table
    if(!dm_range.doesContain(range))
	return;
    
    // Discard functions overlapping functions already in this symbol table
    if(dm_functions.find(range) != dm_functions.end())
	return;
    
    // Add this function to the symbol table
    //std::cerr << "SymbolTable::addFunction INSERTS to dm_functions "
    //    << name << std::endl;
    dm_functions.insert(std::make_pair(range, name));
}



/**
 * Add a loop.
 *
 * Adds the specified loop with its associated address range to this symbol
 * table. Loops are discarded when they have an invalid address range, or are
 * not contained entirely within this symbol table.
 *
 * @note    In theory a symbol table will never contain invalid address ranges,
 *          or address ranges outside the address range of the symbol table
 *          itself. There are various cases, however, where bogus symbol
 *          information results in this happening. This function contains
 *          sanity checks that help keep the experiment database, if not 100%
 *          correct, at least useable under such circumstances.
 *
 * @param begin    Beginning address associated with this loop.
 * @param end      Ending address associated with this loop.
 * @param head     Head address of this loop.
 */
void SymbolTable::addLoop(const Address& begin, const Address& end,
                          const Address& head)
{
    // Discard loops where (end <= begin)
    if(end <= begin)
        return;
    
    // Construct the address range [begin, end)
    AddressRange range(begin, end);
    
    // Discard loops not contained entirely within this symbol table
    if(!dm_range.doesContain(range))
        return;
    
    // Add this loop to the symbol table (or find the existing loop)
    std::map<Address, std::vector<AddressRange> >::iterator i = dm_loops.insert(
        std::make_pair(head, std::vector<AddressRange>())
        ).first;
    
    // Add this address range to the loop
    i->second.push_back(range);
}



/**
 * Add a statement.
 *
 * Adds the specified statement with its associated address range to this symbol
 * table. Statements are discarded when they have an invalid address range, or
 * are not contained entirely within this symbol table.
 *
 * @note    In theory a symbol table will never contain invalid address ranges,
 *          or address ranges outside the address range of the symbol table
 *          itself. There are various cases, however, where bogus symbol
 *          information results in this happening. This function contains
 *          sanity checks that help keep the experiment database, if not 100%
 *          correct, at least useable under such circumstances.
 *
 * @param begin     Beginning address associated with this statement.
 * @param end       Ending address associated with this statement.
 * @param path      Full path name of this statement's source file.
 * @param line      Line number of this statement.
 * @param column    Column number of this statement.
 */
void SymbolTable::addStatement(const Address& begin, 
			       const Address& end,
			       const Path& path, 
			       const int& line, 
			       const int& column)
{
    // Discard statements where (end <= begin)
    if(end <= begin)
	return;
    
    // Construct the address range [begin, end)
    AddressRange range(begin, end);

    // Discard statements not contained entirely within this symbol table
    if(!dm_range.doesContain(range))
	return;
    
    // Add this statement to the symbol table (or find the existing statement)
    std::map<StatementEntry, std::vector<AddressRange> >::iterator
	i = dm_statements.insert(
	    std::make_pair(
		StatementEntry(path, line, column),
		std::vector<AddressRange>())
	    ).first;
    
#if 0
    std::cerr << "SymbolTable::addStatment INSERTS to dm_statements " << path
	<< ":" << line
	<< ":" << column
	<< " range: " << range
	 << std::endl;
#endif

    // Add this address range to the statement
    i->second.push_back(range);
}



/**
 * Process and store into a linked object.
 *
 * Process the information previously added to this symbol table and store it
 * for the specified linked object.
 *
 * @param linked_object    Linked object using this symbol table.
 */
void SymbolTable::processAndStore(const LinkedObject& linked_object)
{
    // Begin a transaction on this linked object's database
    SmartPtr<Database> database = EntrySpy(linked_object).getDatabase();
    BEGIN_WRITE_TRANSACTION(database);
    
    // Iterate over each function entry
    for(std::map<AddressRange, std::string>::const_iterator
	    i = dm_functions.begin(); i != dm_functions.end(); ++i) {

	// Get the function range
	Address addr_begin(i->first.getBegin() - dm_range.getBegin());
	Address addr_end(i->first.getEnd() - dm_range.getBegin());
	
#if 0
	std::cerr << "ProcessSTORE: F " << i->second
	<< " LOBJ " << linked_object.getPath()
	<< "\n dm_range begin: " << dm_range
	<< "\n dm_func begin: " << i->first.getBegin()
	<< " dm_func end: " << i->first.getEnd()
	<< "\n addr_begin: " << addr_begin
	<< " addr_end: " << addr_end
	<< std::endl;
#endif

	// Construct a valid bitmap for this (entire) function range
	AddressBitmap valid_bitmap(AddressRange(addr_begin, addr_end));
	for(Address addr = addr_begin; addr < addr_end; ++addr)
	    valid_bitmap.setValue(addr, true);

	// Create the function entry
	database->prepareStatement(
	    "INSERT INTO Functions (linked_object, name) VALUES (?, ?);"
	    );
	database->bindArgument(1, EntrySpy(linked_object).getEntry());
	database->bindArgument(2, i->second);
	while(database->executeStatement());	
	int function = database->getLastInsertedUID();
	
	// Create the function ranges entry
	database->prepareStatement(
	    "INSERT INTO FunctionRanges "
	    "  (function, addr_begin, addr_end, valid_bitmap) "
	    "VALUES (?, ?, ?, ?);"
	    );
	database->bindArgument(1, function);
	database->bindArgument(2, valid_bitmap.getRange().getBegin());
	database->bindArgument(3, valid_bitmap.getRange().getEnd());
	database->bindArgument(4, valid_bitmap.getBlob());
	while(database->executeStatement());

    }

    // Iterate over each loop entry
    for(std::map<Address, std::vector<AddressRange> >::const_iterator
            i = dm_loops.begin(); i != dm_loops.end(); ++i) {
        
        // Create the loop entry
        database->prepareStatement(
            "INSERT INTO Loops (linked_object, addr_head) VALUES (?, ?);"
            );
        database->bindArgument(1, EntrySpy(linked_object).getEntry());
        database->bindArgument(2, Address(i->first - dm_range.getBegin()));
        while(database->executeStatement());
        int loop = database->getLastInsertedUID();
        
        // Construct the set of unique addresses for this loop
        std::set<Address> addresses;
        for(std::vector<AddressRange>::const_iterator
                j = i->second.begin(); j != i->second.end(); ++j)
            for(Address k = j->getBegin(); k != j->getEnd(); ++k)
                addresses.insert(Address(k - dm_range.getBegin()));
        
        // Partition this loop's address set
        std::vector<std::set<Address> > address_sets =
            partitionAddressSet(addresses);
        
        // Iterate over each partitioned address set
        for(std::vector<std::set<Address> >::const_iterator
                j = address_sets.begin(); j != address_sets.end(); ++j) {
            
            // Create and populate an address bitmap for this address set
            AddressBitmap valid_bitmap(AddressRange(*(j->begin()),
                                                    *(j->rbegin()) + 1));
            for(std::set<Address>::const_iterator
                    k = j->begin(); k != j->end(); ++k)
                valid_bitmap.setValue(*k, true);
            
            // Create the loop ranges entry
            database->prepareStatement(
                "INSERT INTO LoopRanges "
                "  (loop, addr_begin, addr_end, valid_bitmap) "
                "VALUES (?, ?, ?, ?);"
                );
            database->bindArgument(1, loop);
            database->bindArgument(2, valid_bitmap.getRange().getBegin());
            database->bindArgument(3, valid_bitmap.getRange().getEnd());
            database->bindArgument(4, valid_bitmap.getBlob());
            while(database->executeStatement());
        }
    }

    // Iterate over each statement entry
    for(std::map<StatementEntry, std::vector<AddressRange> >::const_iterator
	    i = dm_statements.begin(); i != dm_statements.end(); ++i) {

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
	database->bindArgument(1, EntrySpy(linked_object).getEntry());
	database->bindArgument(2, file);
	database->bindArgument(3, i->first.dm_line);
	database->bindArgument(4, i->first.dm_column);
	while(database->executeStatement());	
	int statement = database->getLastInsertedUID();

	// Construct the set of unique addresses for this statement
	std::set<Address> addresses;
	for(std::vector<AddressRange>::const_iterator
		j = i->second.begin(); j != i->second.end(); ++j)
	    for(Address k = j->getBegin(); k != j->getEnd(); ++k)
		addresses.insert(Address(k - dm_range.getBegin()));
	
	// Partition this statement's address set
	std::vector<std::set<Address> > address_sets =
	    partitionAddressSet(addresses);
	
	// Iterate over each partitioned address set
	for(std::vector<std::set<Address> >::const_iterator
		j = address_sets.begin(); j != address_sets.end(); ++j) {
	    
	    // Create and populate an address bitmap for this address set
	    AddressBitmap valid_bitmap(AddressRange(*(j->begin()),
						    *(j->rbegin()) + 1));
	    for(std::set<Address>::const_iterator
		    k = j->begin(); k != j->end(); ++k)
		valid_bitmap.setValue(*k, true);
	    
	    // Create the statement ranges entry
	    database->prepareStatement(
		"INSERT INTO StatementRanges "
		"  (statement, addr_begin, addr_end, valid_bitmap) "
		"VALUES (?, ?, ?, ?);"
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
SymbolTable::partitionAddressSet(const std::set<Address>& address_set)
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
