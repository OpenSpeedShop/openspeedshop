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
 * Declaration of the MessageCallbackTable class.
 *
 */

#ifndef _OpenSpeedShop_Framework_MessageCallbackTable_
#define _OpenSpeedShop_Framework_MessageCallbackTable_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Lockable.hxx"
#include "MessageCallback.hxx"

#include <map>
#include <set>



namespace OpenSpeedShop { namespace Framework {

    /**
     * Message callback table.
     *
     * Lockable mapping of message tags to the current callback(s) for the
     * message types corresponding to those tags.
     *
     * @ingroup Implementation
     */
    class MessageCallbackTable :
	private Lockable,
	private std::multimap<int, MessageCallback>
    {

    public:

	void addCallback(const int&, const MessageCallback);
	void removeCallback(const int&, const MessageCallback);

	std::set<MessageCallback> getCallbacksByTag(const int&) const;

    };
	
} }



#endif
