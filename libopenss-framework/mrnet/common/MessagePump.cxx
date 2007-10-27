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
 * Definition of the MessagePump namespace.
 *
 */

#include "MessagePump.hxx"

#include <mrnet/MRNet.h>

using namespace OpenSpeedShop::Framework;



/**
 * Register a callback with the message pump.
 *
 * ...
 *
 * @param tag         ...
 * @param callback    ...
 */
void MessagePump::registerCallback(const int& tag,
				   const MessageCallback callback)
{
}



/**
 * Unregister a callback with the message pump.
 *
 * ...
 *
 * @param tag         ...
 * @param callback    ...
 */
void MessagePump::unregisterCallback(const int& tag,
				     const MessageCallback callback)
{
}



/**
 * Start the backend message pump.
 *
 * ...
 *
 * @param argc    Number of command-line arguments.
 * @param argv    Array of command-line arguments.
 */
void MessagePump::startBackend(int argc, char* argv[])
{
    // Initialize the MRNet library (as a backend)
    MRN::Network network(argc, argv);

    // ...
    while(true) {
	//if(network.recv(&tag, &packet, &stream) != 1) {
	//}	
    }
}
