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

#include "Backend.hxx"
#include "Callbacks.hxx"
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
    // Register callbacks with the backend
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_ATTACH_TO_THREAD,
			      Callbacks::attachToThread);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_CHANGE_THREAD_STATE,
			      Callbacks::changeThreadState);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_CREATE_PROCESS,
			      Callbacks::createProcess);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_DETACH_FROM_THREAD,
			      Callbacks::detachFromThread);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_EXECUTE_NOW,
			      Callbacks::executeNow);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_EXECUTE_AT_ENTRY_OR_EXIT,
			      Callbacks::executeAtEntryOrExit);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_EXECUTE_IN_PLACE_OF,
			      Callbacks::executeInPlaceOf);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_GET_GLOBAL_INTEGER,
			      Callbacks::getGlobalInteger);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_GET_GLOBAL_STRING,
			      Callbacks::getGlobalString);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_GET_MPICH_PROC_TABLE,
			      Callbacks::getMPICHProcTable);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_SET_GLOBAL_INTEGER,
			      Callbacks::setGlobalInteger);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_STOP_AT_ENTRY_OR_EXIT,
			      Callbacks::stopAtEntryOrExit);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_UNINSTRUMENT,
			      Callbacks::uninstrument);
    
    // Start the backend's message pump
    Backend::startMessagePump(argc, argv);

    // TODO: how do we decide when to exit???
    
    // Indicate success to the shell
    return 0;
}
