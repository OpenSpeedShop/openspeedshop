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

#include "Callbacks.hxx"
#include "MessagePump.hxx"
#include "Protocol.h"

using namespace OpenSpeedShop::Framework;



/**
 * Main entry point.
 *
 * Main entry point for the daemon. ...
 *
 * @param argc    Number of command-line arguments.
 * @param argv    Array of command-line arguments.
 */
int main(int argc, char* argv[])
{
    // Register callbacks with the message pump
    MessagePump::registerCallback(OPENSS_PROTOCOL_TAG_ATTACH_TO_THREAD,
				  Callbacks::attachToThread);
    MessagePump::registerCallback(OPENSS_PROTOCOL_TAG_CHANGE_THREAD_STATE,
				  Callbacks::changeThreadState);
    MessagePump::registerCallback(OPENSS_PROTOCOL_TAG_CREATE_PROCESS,
				  Callbacks::createProcess);
    MessagePump::registerCallback(OPENSS_PROTOCOL_TAG_DETACH_FROM_THREAD,
				  Callbacks::detachFromThread);
    MessagePump::registerCallback(OPENSS_PROTOCOL_TAG_EXECUTE_NOW,
				  Callbacks::executeNow);
    MessagePump::registerCallback(OPENSS_PROTOCOL_TAG_EXECUTE_AT_ENTRY_OR_EXIT,
				  Callbacks::executeAtEntryOrExit);
    MessagePump::registerCallback(OPENSS_PROTOCOL_TAG_EXECUTE_IN_PLACE_OF,
				  Callbacks::executeInPlaceOf);
    MessagePump::registerCallback(OPENSS_PROTOCOL_TAG_GET_GLOBAL_INTEGER,
				  Callbacks::getGlobalInteger);
    MessagePump::registerCallback(OPENSS_PROTOCOL_TAG_GET_GLOBAL_STRING,
				  Callbacks::getGlobalString);
    MessagePump::registerCallback(OPENSS_PROTOCOL_TAG_GET_MPICH_PROC_TABLE,
				  Callbacks::getMPICHProcTable);
    MessagePump::registerCallback(OPENSS_PROTOCOL_TAG_SET_GLOBAL_INTEGER,
				  Callbacks::setGlobalInteger);
    MessagePump::registerCallback(OPENSS_PROTOCOL_TAG_STOP_AT_ENTRY_OR_EXIT,
				  Callbacks::stopAtEntryOrExit);
    MessagePump::registerCallback(OPENSS_PROTOCOL_TAG_UNINSTRUMENT,
				  Callbacks::uninstrument);
    
    // Start the (backend) message pump
    MessagePump::startBackend(argc, argv);
    
    // Indicate success to the shell
    return 0;
}
