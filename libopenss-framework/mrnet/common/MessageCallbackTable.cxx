////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file
 *
 * Definition of the MessageCallbackTable class.
 *
 */

#include "Assert.hxx"
#include "Guard.hxx"
#include "MessageCallbackTable.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Add a callback.
 *
 * Adds the specified message tag and callback pairing to this message
 * callback table.
 *
 * @note    An assertion failure occurs if an attempt is made to add a
 *          message tag and callback pairing that already exists in this
 *          message callback table.
 *
 * @param tag         Message tag for which to add a new callback.
 * @param callback    Callback to be added.
 */
void MessageCallbackTable::addCallback(const int& tag,
				       const MessageCallback callback)
{
    Guard guard_myself(this);

    // Iterate over any existing callbacks for this message tag
    for(MessageCallbackTable::const_iterator
	    i = lower_bound(tag); i != upper_bound(tag); ++i)

	// Check assertions
	Assert(i->second != callback);

    // Add this message tag and callback pairing
    insert(std::make_pair(tag, callback));
}



/**
 * Remove a callback.
 *
 * Removes the specified message tag and callback pairing from this
 * message callback table.
 *
 * @note    An assertion failure occurs if an attempt is made to remove
 *          a message tag and callback pairing that isn't in this message
 *          callback table.
 *
 * @param tag         Message tag for which to remove a callback.
 * @param callback    Callback to be removed.
 */
void MessageCallbackTable::removeCallback(const int& tag,
					  const MessageCallback callback)
{
    Guard guard_myself(this);

    // Iterate over the existing callbacks for this message tag
    for(MessageCallbackTable::iterator
	    i = lower_bound(tag); i != upper_bound(tag); ++i)

	// Erase entry and return immediately if this is the tag/callback
	if (i->second == callback) {
	    erase(i);
	    return;
	}

    // Check assertions
    Assert(false);
}



/**
 * Get message callbacks by their message tag.
 *
 * Returns the message callbacks corresponding to the specified message tag.
 * An empty set is returned if the specified message tagt isn't in this message
 * callback table.
 *
 * @param tag    Query message tag.
 * @return       Messsage callbacks for that message tag.
 */
std::set<MessageCallback>
MessageCallbackTable::getCallbacksByTag(const int& tag) const
{
    Guard guard_myself(this);

    // Assemble the callbacks for this message tag
    std::set<MessageCallback> callbacks;
    for(MessageCallbackTable::const_iterator
	    i = lower_bound(tag); i != upper_bound(tag); ++i)
	callbacks.insert(i->second);

    // Return the callbacks to the caller
    return callbacks;
}
