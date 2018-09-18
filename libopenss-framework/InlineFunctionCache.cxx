////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
// Copyright (c) 2018 Krell Institute. All Rights Reserved.
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
 * Definition of the InlineFunctionCache class.
 *
 */

#include "AddressBitmap.hxx"
#include "Blob.hxx"
#include "EntrySpy.hxx"
#include "Guard.hxx"
#include "InlineFunctionCache.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Get intersecting inlines.
 *
 * Returns the inlines within the passed linked object intersecting the
 * passed extent. An empty set is returned if no inlines are found.
 *
 * @param linked_object    Linked object in which to find inlines.
 * @param extent           Extent with which to intersect.
 * @return                 InlineFunctions intersecting this extent.
 */
std::set<InlineFunction>
InlineFunctionCache::getInlineFunctions(const LinkedObject& linked_object,
			      const ExtentGroup& extent)
{
    Guard guard_myself(this);

    // Find this linked object in the cache (adding it if necessary)
    ExtentGroup& cached = dm_cache.getExtents(linked_object);
    if(cached.empty()) {
	addLinkedObject(linked_object);
	cached = dm_cache.getExtents(linked_object);
    }

    // Intersect the extent with this linked object's cached inline functions
    std::set<ExtentGroup::size_type> intersection =
	cached.getIntersectionWith(extent);
    
    // Assemble the intersection results into a statement set
    std::set<InlineFunction> inlines;
    for(std::set<ExtentGroup::size_type>::const_iterator
	    i = intersection.begin(); i != intersection.end(); ++i)
	inlines.insert(dm_cache.getObject(linked_object, *i));
    
    // Return the inline functions to the caller
    return inlines;
}



/**
 * Remove a database.
 *
 * Removes all the linked objects in the passed database from the cache.
 *
 * @param database    Database to be removed from the cache.
 */
void InlineFunctionCache::removeDatabase(const SmartPtr<Database>& database)
{
    Guard guard_myself(this);

    // Find the linked objects in this database
    std::set<LinkedObject> linked_objects;
    BEGIN_TRANSACTION(database);
    database->prepareStatement("SELECT id FROM LinkedObjects;");
    while(database->executeStatement())
	linked_objects.insert(LinkedObject(database,
					   database->getResultAsInteger(1)));
    END_TRANSACTION(database);

    // Remove these linked objects from the cache
    for(std::set<LinkedObject>::const_iterator
	    i = linked_objects.begin(); i != linked_objects.end(); ++i)
	dm_cache.removeExtents(*i);
}



/**
 * Add a linked object.
 *
 * Adds the passed linked object to the cache. All inline functions in the linked
 * object are located and added as a group to the cache. Future queries make
 * use of this cached data.
 *
 * @param linked_object    Linked object to be added to the cache.
 */
void InlineFunctionCache::addLinkedObject(const LinkedObject& linked_object)
{
    SmartPtr<Database> database = EntrySpy(linked_object).getDatabase();

    // Find the extents of all inline functions within the specified linked object
    BEGIN_TRANSACTION(database);
    EntrySpy(linked_object).validate();
    database->prepareStatement(
	"SELECT InlinedFunctions.id, "
	"       InlinedFunctionsRanges.addr_begin, "
	"       InlinedFunctionsRanges.addr_end, "
	"       InlinedFunctionsRanges.valid_bitmap "
	"FROM InlinedFunctionsRanges "
	"  JOIN InlinedFunctions "
	"ON InlinedFunctionsRanges.inline = InlinedFunctions.id "
	"WHERE InlinedFunctions.linked_object = ?;"
	);
    database->bindArgument(1, EntrySpy(linked_object).getEntry());
    while(database->executeStatement()) {

	AddressBitmap bitmap(AddressRange(database->getResultAsAddress(2),
	 				  database->getResultAsAddress(3)),
			     database->getResultAsBlob(4));

	std::set<AddressRange> ranges = bitmap.getContiguousRanges(true);

	for(std::set<AddressRange>::const_iterator
		i = ranges.begin(); i != ranges.end(); ++i)

	    dm_cache.addExtent(
		linked_object,
		InlineFunction(database, database->getResultAsInteger(1)),
		Extent(TimeInterval(Time::TheBeginning(), Time::TheEnd()), *i)
		);

    }
    END_TRANSACTION(database);
}
